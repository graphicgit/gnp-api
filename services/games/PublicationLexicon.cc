#include "services/games/PublicationLexicon.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <drogon/drogon.h>
#include <mupdf/fitz.h>

#include "services/games/G3DocumentScanner.h"
#include "services/games/GameTypes.h"

namespace gnp::services {
namespace {

std::mutex gCacheMutex;
struct CacheEntry {
    Corpus corpus;
    std::chrono::steady_clock::time_point loadedAt;
};
std::unordered_map<std::string, CacheEntry> gCache;
constexpr int kCacheTtlSeconds = 1800;
constexpr int kMaxRawChars = 160000;
// Reading stops early once this much text is in hand, so a single long edition is enough.
constexpr int kTargetRawChars = 90000;
// Newest PDFs considered per corpus build, and how many of them must yield text. Scans without a
// text layer are cheap to reject, so the candidate window is wider than the readable target.
constexpr int kMaxCorpusDocuments = 8;
constexpr int kMaxReadableDocuments = 3;
constexpr int kMaxPdfPagesPerDocument = 16;
constexpr int kMaxPdfPagesForReport = 8;
constexpr int kMinPageChars = 40;
constexpr std::size_t kMinLexemes = 8;
constexpr int kHintedDocuments = 5;

const std::unordered_set<std::string> kStopwords = {
    "THE", "AND", "FOR", "ARE", "BUT", "NOT", "YOU", "ALL", "CAN", "HER", "WAS", "ONE", "OUR", "OUT",
    "HAS", "HAVE", "HAD", "HIS", "HERS", "ITS", "THEY", "THEM", "THEIR", "THIS", "THAT", "WITH", "FROM",
    "WERE", "BEEN", "WILL", "WOULD", "COULD", "SHOULD", "ABOUT", "AFTER", "BEFORE", "THERE", "WHERE",
    "WHICH", "WHILE", "WHAT", "WHEN", "YOUR", "YOURS", "INTO", "OVER", "UNDER", "ALSO", "THAN", "THEN",
    "THESE", "THOSE", "SAID", "SAYS", "SAY", "JUST", "LIKE", "MORE", "MOST", "SOME", "SUCH", "ONLY",
    "OTHER", "BEING", "EACH", "BOTH", "MANY", "MUCH", "VERY", "EVEN", "BACK", "DOWN", "UPON", "AMONG",
    "PAGE", "PAGES", "EDITION", "COPYRIGHT", "ADVERTISEMENT", "ADVERT", "CONTINUED", "GRAPHIC", "NEWS",
    "PLUS", "HTTP", "HTTPS", "WWW", "COM", "ORG", "EMAIL", "PHOTO", "PHOTOS", "IMAGE", "IMAGES", "CAPTION",
    "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY", "JANUARY", "FEBRUARY",
    "MARCH", "APRIL", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER",
    "TODAY", "YESTERDAY", "TOMORROW", "WEEK", "WEEKS", "YEAR", "YEARS", "MONTH", "MONTHS",
    "SAYS", "TOLD", "ACCORDING", "REPORT", "REPORTS", "REPORTED", "STORY", "STORIES"
};

bool isStopword(const std::string &word) {
    return kStopwords.count(word) > 0;
}

std::string toUpperAscii(std::string value) {
    for (char &c : value) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc >= 'a' && uc <= 'z') c = static_cast<char>(uc - 'a' + 'A');
    }
    return value;
}

std::string trimCopy(const std::string &value) {
    size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin]))) ++begin;
    size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) --end;
    return value.substr(begin, end - begin);
}

std::string collapseSpace(const std::string &value) {
    std::string out;
    out.reserve(value.size());
    bool pendingSpace = false;
    for (unsigned char c : value) {
        if (c == '\r' || c == '\n' || c == '\t' || std::isspace(c)) {
            pendingSpace = !out.empty();
            continue;
        }
        if (pendingSpace) {
            out.push_back(' ');
            pendingSpace = false;
        }
        out.push_back(static_cast<char>(c));
    }
    return out;
}

bool isAsciiLetter(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool hasVowel(const std::string &word) {
    for (char c : word) {
        if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'Y') return true;
    }
    return false;
}

int scoreWord(const std::string &word, int frequency, bool hasSentence) {
    int score = 0;
    const int n = static_cast<int>(word.size());
    if (n >= 5 && n <= 8) score += 6;
    else if (n >= 4 && n <= 11) score += 3;
    else score += 1;
    if (frequency >= 2 && frequency <= 5) score += 4;
    else if (frequency == 1) score += 2;
    else score += 1;
    if (hasSentence) score += 3;
    return score;
}

std::string snippetAround(const std::string &text, const std::string &word) {
    const std::string upper = toUpperAscii(text);
    const auto pos = upper.find(word);
    if (pos == std::string::npos) {
        if (text.size() <= 160) return text;
        return text.substr(0, 157) + "...";
    }
    const size_t begin = pos > 70 ? pos - 70 : 0;
    size_t end = std::min(text.size(), pos + word.size() + 70);
    std::string snippet = trimCopy(text.substr(begin, end - begin));
    if (begin > 0) snippet = "..." + snippet;
    if (end < text.size()) snippet += "...";
    if (snippet.size() > 180) snippet = snippet.substr(0, 177) + "...";
    return snippet;
}

std::string joinList(const std::vector<std::string> &values, const char *separator) {
    std::string out;
    for (const auto &value : values) {
        if (!out.empty()) out += separator;
        out += value;
    }
    return out;
}

// ---------------------------------------------------------------------------------------------
// G3 storage lookup: puzzle words come from the edition PDFs in the master document bucket.
// ---------------------------------------------------------------------------------------------

std::string resolveBasePath() {
    auto custom = drogon::app().getCustomConfig();
    if (custom.isMember("G3Bucket") && custom["G3Bucket"].isMember("BaseStoragePath")) {
        const std::string configured = custom["G3Bucket"]["BaseStoragePath"].asString();
        if (!configured.empty()) return configured;
    }
    return "./g3-storage";
}

std::vector<std::string> masterBucketNames() {
    std::vector<std::string> buckets;
    auto custom = drogon::app().getCustomConfig();
    if (custom.isMember("G3Bucket") && custom["G3Bucket"].isMember("MasterDocumentBucketName")) {
        const std::string configured = custom["G3Bucket"]["MasterDocumentBucketName"].asString();
        if (!configured.empty()) buckets.push_back(configured);
    }
    if (std::find(buckets.begin(), buckets.end(), "gnp-master-documents") == buckets.end()) {
        buckets.emplace_back("gnp-master-documents");
    }
    return buckets;
}

struct MasterBuckets {
    std::string basePath;
    std::string bucket;                       // bucket that supplied the documents (or the first checked)
    std::string directory;                    // "<basePath>/<bucket>"
    bool directoryExists = false;
    std::vector<std::string> checked;         // bucket names looked at, in order
    std::vector<ScannedDocument> documents;   // newest first; a hinted document is moved to the front
};

MasterBuckets locateMasterDocuments(std::size_t maxDocuments, const std::string &preferredDocumentId) {
    MasterBuckets result;
    result.basePath = resolveBasePath();
    const std::vector<std::string> buckets = masterBucketNames();

    for (const auto &bucket : buckets) {
        const BucketScan scan = scanBucket(result.basePath, bucket, maxDocuments, preferredDocumentId);
        result.checked.push_back(bucket);
        if (result.bucket.empty()) {
            result.bucket = bucket;
            result.directory = scan.directory;
            result.directoryExists = scan.directoryExists;
        }
        if (!scan.documents.empty()) {
            result.bucket = bucket;
            result.directory = scan.directory;
            result.directoryExists = true;
            result.documents = scan.documents;
            break;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------------------------
// Optional database metadata. The newspapers table is used for titles, publication names and for
// resolving an explicit publicationId/newspaperId filter to a document id. It is never required:
// puzzles are built from whatever the bucket holds, even when the database has no matching row.
// ---------------------------------------------------------------------------------------------

struct EditionMeta {
    bool found = false;
    std::string newspaperId;
    std::string title;
    std::string publicationName;
    std::string publicationId;
    std::string publicationDate;
};

drogon::Task<EditionMeta> lookupEditionMeta(const std::string &documentId) {
    EditionMeta meta;
    if (documentId.empty()) co_return meta;
    auto db = drogon::app().getDbClient();
    if (!db) co_return meta;
    try {
        auto rows = co_await db->execSqlCoro(
            "SELECT id::text AS id, COALESCE(title, '') AS title, COALESCE(publication_name, '') AS publication_name, "
            "COALESCE(publication_id::text, '') AS publication_id, COALESCE(publication_date::text, '') AS publication_date "
            "FROM newspapers WHERE document_id = $1::text ORDER BY publication_date DESC NULLS LAST LIMIT 1",
            documentId);
        if (!rows.empty()) {
            const auto &row = rows[0];
            meta.newspaperId = row["id"].isNull() ? "" : row["id"].as<std::string>();
            meta.title = row["title"].isNull() ? "" : row["title"].as<std::string>();
            meta.publicationName = row["publication_name"].isNull() ? "" : row["publication_name"].as<std::string>();
            meta.publicationId = row["publication_id"].isNull() ? "" : row["publication_id"].as<std::string>();
            meta.publicationDate = row["publication_date"].isNull() ? "" : row["publication_date"].as<std::string>();
            meta.found = !meta.newspaperId.empty();
        }
    } catch (const std::exception &e) {
        LOG_DEBUG << "[games] Edition metadata lookup skipped for " << documentId << ": " << e.what();
    }
    co_return meta;
}

drogon::Task<std::vector<std::string>> hintedDocumentIds(const std::string &publicationId,
                                                         const std::string &newspaperId,
                                                         int limit = kHintedDocuments) {
    std::vector<std::string> ids;
    if (publicationId.empty() && newspaperId.empty()) co_return ids;
    auto db = drogon::app().getDbClient();
    if (!db) co_return ids;
    try {
        if (!newspaperId.empty()) {
            auto rows = co_await db->execSqlCoro(
                "SELECT document_id FROM newspapers WHERE id = $1::uuid AND COALESCE(document_id, '') <> '' LIMIT 1",
                newspaperId);
            for (size_t i = 0; i < rows.size(); ++i) {
                if (!rows[i]["document_id"].isNull()) ids.push_back(rows[i]["document_id"].as<std::string>());
            }
        } else {
            auto rows = co_await db->execSqlCoro(
                "SELECT document_id FROM newspapers WHERE publication_id = $1::uuid AND COALESCE(document_id, '') <> '' "
                "ORDER BY publication_date DESC NULLS LAST LIMIT $2",
                publicationId, limit);
            for (size_t i = 0; i < rows.size(); ++i) {
                if (!rows[i]["document_id"].isNull()) ids.push_back(rows[i]["document_id"].as<std::string>());
            }
        }
    } catch (const std::exception &e) {
        LOG_DEBUG << "[games] Edition filter lookup skipped: " << e.what();
    }
    co_return ids;
}

// ---------------------------------------------------------------------------------------------
// PDF text layer extraction.
// ---------------------------------------------------------------------------------------------

std::vector<std::string> extractPdfPages(const std::string &path, int maxPages) {
    std::vector<std::string> pages;
    if (maxPages <= 0) return pages;

    std::error_code pathEc;
    if (!std::filesystem::exists(path, pathEc) || pathEc) return pages;

    fz_context *ctx = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
    if (!ctx) return pages;
    fz_register_document_handlers(ctx);

    fz_document *doc = nullptr;
    fz_var(doc);

    fz_try(ctx) {
        doc = fz_open_document(ctx, path.c_str());
        const int count = fz_count_pages(ctx, doc);
        const int limit = std::min(count, maxPages);
        size_t accumulated = 0;
        for (int i = 0; i < limit; ++i) {
            fz_stext_page *stext = fz_new_stext_page_from_page_number(ctx, doc, i, nullptr);
            fz_buffer *pageBuffer = fz_new_buffer_from_stext_page(ctx, stext);
            unsigned char *data = nullptr;
            const size_t len = fz_buffer_storage(ctx, pageBuffer, &data);
            pages.emplace_back();
            if (data && len > 0) pages.back().assign(reinterpret_cast<const char *>(data), len);
            fz_drop_buffer(ctx, pageBuffer);
            fz_drop_stext_page(ctx, stext);
            accumulated += len;
            if (accumulated > static_cast<size_t>(kMaxRawChars)) break;
        }
    }
    fz_always(ctx) {
        fz_drop_document(ctx, doc);
    }
    fz_catch(ctx) {
        LOG_ERROR << "[games] MuPDF text extraction failed for " << path << ": " << fz_caught_message(ctx);
        pages.clear();
    }
    fz_drop_context(ctx);
    return pages;
}

struct PageBlob {
    std::string text;
    SourceRef source;
    bool fromPdf = false;
};

/**
 * Normalises page text the same way the corpus does, so a page that only carries whitespace (a
 * scan without a text layer) counts as empty and is reported as such.
 */
std::vector<std::string> collapsedPages(const std::vector<std::string> &pageTexts) {
    std::vector<std::string> pages;
    pages.reserve(pageTexts.size());
    for (const auto &pageText : pageTexts) pages.push_back(collapseSpace(pageText));
    return pages;
}

std::vector<PageBlob> pagesFromDocument(const ScannedDocument &document,
                                        const std::vector<std::string> &pageTexts,
                                        const EditionMeta &meta) {
    std::vector<PageBlob> pages;
    SourceRef source;
    source.newspaperId = meta.newspaperId;
    source.publicationId = meta.publicationId;
    source.publicationName = meta.publicationName;
    source.newspaperTitle = meta.title.empty() ? document.stem : meta.title;
    source.publicationDate = meta.publicationDate;
    if (source.publicationDate.empty() && document.modifiedAt.size() >= 10) {
        source.publicationDate = document.modifiedAt.substr(0, 10);
    }

    for (size_t i = 0; i < pageTexts.size(); ++i) {
        if (pageTexts[i].size() < static_cast<size_t>(kMinPageChars)) continue;
        SourceRef pageSource = source;
        pageSource.pageNumber = static_cast<int>(i) + 1;
        pages.push_back(PageBlob{pageTexts[i], pageSource, true});
    }
    return pages;
}

void absorbText(std::unordered_map<std::string, Lexeme> &words,
                 std::string &letterBag,
                 const std::string &raw,
                 const SourceRef &source) {
    const std::string text = collapseSpace(raw);
    if (text.size() < 20) return;

    std::string current;
    auto flush = [&](size_t endIndex) {
        if (current.size() < 4 || current.size() > 12) {
            current.clear();
            return;
        }
        if (!hasVowel(current) || isStopword(current)) {
            current.clear();
            return;
        }
        bool same = true;
        for (char c : current) {
            if (c != current.front()) {
                same = false;
                break;
            }
        }
        if (same) {
            current.clear();
            return;
        }

        const size_t wordStart = endIndex >= current.size() ? endIndex - current.size() : 0;
        std::string sentence = text;
        size_t left = wordStart;
        while (left > 0 && text[left] != '.' && text[left] != '!' && text[left] != '?' && text[left] != '\n') --left;
        if (left > 0 && left < text.size()) ++left;
        size_t right = wordStart;
        while (right < text.size() && text[right] != '.' && text[right] != '!' && text[right] != '?') ++right;
        if (right < text.size()) ++right;
        sentence = trimCopy(text.substr(left, right - left));
        if (sentence.size() > 220) sentence = snippetAround(text, current);
        if (sentence.size() < 20) sentence = snippetAround(text, current);

        auto it = words.find(current);
        if (it == words.end()) {
            Lexeme lexeme;
            lexeme.word = current;
            lexeme.frequency = 1;
            lexeme.sentence = sentence;
            lexeme.snippet = snippetAround(sentence.empty() ? text : sentence, current);
            lexeme.source = source;
            lexeme.score = scoreWord(current, 1, !sentence.empty());
            words.emplace(current, std::move(lexeme));
        } else {
            it->second.frequency += 1;
            if (it->second.sentence.size() < sentence.size()) {
                it->second.sentence = sentence;
                it->second.snippet = snippetAround(sentence, current);
                it->second.source = source;
            }
            it->second.score = scoreWord(it->second.word, it->second.frequency, !it->second.sentence.empty());
        }
        current.clear();
    };

    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (isAsciiLetter(c)) {
            char upper = static_cast<char>(std::toupper(c));
            current.push_back(upper);
            if (letterBag.size() < 4000) letterBag.push_back(upper);
            continue;
        }
        flush(i);
    }
    flush(text.size());
}

Corpus buildCorpus(const std::vector<PageBlob> &pages, const std::vector<std::string> &topics) {
    std::unordered_map<std::string, Lexeme> words;
    std::string letterBag;
    std::vector<SourceRef> sources;
    int rawChars = 0;
    bool fromPdf = false;
    bool fromPageText = false;

    for (const auto &page : pages) {
        if (rawChars > kMaxRawChars) break;
        rawChars += static_cast<int>(page.text.size());
        fromPdf = fromPdf || page.fromPdf;
        fromPageText = fromPageText || !page.fromPdf;
        absorbText(words, letterBag, page.text, page.source);
        if (!page.source.newspaperId.empty() || !page.source.newspaperTitle.empty()) {
            bool seen = false;
            for (const auto &existing : sources) {
                if (existing.newspaperId == page.source.newspaperId && existing.pageNumber == page.source.pageNumber) {
                    seen = true;
                    break;
                }
            }
            if (!seen) sources.push_back(page.source);
        }
    }

    std::vector<std::string> topicNeedles;
    for (const auto &topic : topics) {
        std::string needle = toUpperAscii(trimCopy(topic));
        if (needle.size() >= 3) topicNeedles.push_back(needle);
    }

    Corpus corpus;
    corpus.letterBag = letterBag;
    corpus.sources = sources;
    corpus.pageCount = static_cast<int>(pages.size());
    corpus.rawChars = rawChars;
    corpus.fromPdf = fromPdf;
    corpus.fromPageText = fromPageText;
    corpus.topicsApplied = !topicNeedles.empty();

    for (auto &entry : words) {
        if (!topicNeedles.empty()) {
            const std::string haystack = toUpperAscii(entry.second.sentence + " " + entry.second.snippet + " " + entry.second.word);
            bool matched = false;
            for (const auto &needle : topicNeedles) {
                if (haystack.find(needle) != std::string::npos) {
                    matched = true;
                    break;
                }
            }
            if (!matched) continue;
        }
        corpus.lexemes.push_back(std::move(entry.second));
    }

    std::sort(corpus.lexemes.begin(), corpus.lexemes.end(), [](const Lexeme &a, const Lexeme &b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.frequency != b.frequency) return a.frequency > b.frequency;
        return a.word < b.word;
    });
    if (corpus.lexemes.size() > 400) corpus.lexemes.resize(400);
    return corpus;
}

std::string cacheKey(const std::string &publicationId,
                     const std::string &newspaperId,
                     const std::vector<std::string> &topics) {
    std::ostringstream key;
    key << publicationId << "|" << newspaperId;
    for (const auto &topic : topics) key << "|" << lowerCopy(topic);
    return key.str();
}

} // namespace

void PublicationLexicon::clearCache() {
    std::lock_guard<std::mutex> lock(gCacheMutex);
    gCache.clear();
}

drogon::Task<Corpus> PublicationLexicon::load(const std::string &publicationId,
                                              const std::string &newspaperId,
                                              const std::vector<std::string> &topics) {
    // An explicit publicationId/newspaperId only picks which stored edition is preferred; the words
    // themselves always come from the PDFs in G3 storage.
    const std::vector<std::string> hints = co_await hintedDocumentIds(publicationId, newspaperId);
    const std::string preferred = hints.empty() ? std::string() : hints.front();
    MasterBuckets location = locateMasterDocuments(kMaxCorpusDocuments, preferred);

    if (location.documents.empty()) {
        throw std::runtime_error("LEXICON: No newspaper PDF was found in G3 storage. Looked in " + location.directory +
                                 " (bucket '" + location.bucket + "'). Upload the edition PDF to the master document "
                                 "bucket configured as G3Bucket.MasterDocumentBucketName, then retry.");
    }

    // The stamp keeps a freshly uploaded edition from being masked by a cached corpus.
    const ScannedDocument &newest = location.documents.front();
    const std::string key = cacheKey(publicationId, newspaperId, topics) + "|" + newest.fileName + "@" +
                            std::to_string(newest.modifiedEpochSeconds);
    {
        std::lock_guard<std::mutex> lock(gCacheMutex);
        auto it = gCache.find(key);
        if (it != gCache.end()) {
            const auto age = std::chrono::duration_cast<std::chrono::seconds>(
                                 std::chrono::steady_clock::now() - it->second.loadedAt)
                                 .count();
            if (age < kCacheTtlSeconds && it->second.corpus.lexemes.size() >= kMinLexemes) {
                co_return it->second.corpus;
            }
        }
    }

    std::vector<PageBlob> pages;
    std::vector<std::string> report;
    int opened = 0;
    int readable = 0;
    int totalChars = 0;
    int totalRawChars = 0;   // everything MuPDF returned, before the per-page filter

    for (const auto &document : location.documents) {
        if (readable >= kMaxReadableDocuments || totalChars >= kTargetRawChars) break;
        ++opened;

        const std::vector<std::string> pageTexts = collapsedPages(extractPdfPages(document.path, kMaxPdfPagesPerDocument));
        const EditionMeta meta = co_await lookupEditionMeta(document.stem);
        std::vector<PageBlob> documentPages = pagesFromDocument(document, pageTexts, meta);

        int documentChars = 0;
        int documentRawChars = 0;
        for (const auto &pageText : pageTexts) documentRawChars += static_cast<int>(pageText.size());
        for (const auto &page : documentPages) documentChars += static_cast<int>(page.text.size());
        totalChars += documentChars;
        totalRawChars += documentRawChars;
        if (documentChars > 0) ++readable;

        pages.insert(pages.end(), documentPages.begin(), documentPages.end());
        report.push_back(document.fileName + " (" + std::to_string(documentPages.size()) + " pages, " +
                         std::to_string(documentChars) + " chars)");
        LOG_INFO << "[games] Read " << document.fileName << " from " << location.bucket << ": "
                 << documentPages.size() << " pages, " << documentChars << " chars"
                 << (meta.found ? " (" + meta.title + ")" : "");
    }

    Corpus corpus = buildCorpus(pages, topics);

    if (corpus.lexemes.size() < kMinLexemes && !topics.empty()) {
        throw std::runtime_error("LEXICON: Not enough publication words matched the requested topics.");
    }
    if (corpus.lexemes.size() < kMinLexemes) {
        std::string message = "LEXICON: Not enough text in the newspaper PDFs stored in G3 to build a puzzle. ";
        if (totalRawChars == 0) {
            // A scan without a text layer returns nothing at all, which needs a different fix than
            // an edition that simply carries too little text for a puzzle.
            message += "None of the " + std::to_string(opened) + " newest file(s) in " + location.directory +
                       " contained an extractable text layer (" + joinList(report, ", ") +
                       "). Upload a PDF with selectable text, or OCR the scan before uploading.";
        } else {
            message += "Only " + std::to_string(corpus.lexemes.size()) + " usable words came out of " +
                       joinList(report, ", ") + " in " + location.directory + " (" +
                       std::to_string(totalRawChars) + " characters of extractable text). Upload an edition with more text.";
        }
        if (readable == 0 && totalRawChars > 0) {
            message += " Pages shorter than " + std::to_string(kMinPageChars) + " characters are ignored.";
        }
        LOG_WARN << message;
        throw std::runtime_error(message);
    }

    {
        std::lock_guard<std::mutex> lock(gCacheMutex);
        // Drop corpora whose edition has been replaced so the cache cannot grow across editions.
        const auto now = std::chrono::steady_clock::now();
        for (auto it = gCache.begin(); it != gCache.end();) {
            const auto age = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.loadedAt).count();
            if (age >= kCacheTtlSeconds) {
                it = gCache.erase(it);
            } else {
                ++it;
            }
        }
        gCache[key] = CacheEntry{corpus, now};
    }
    LOG_INFO << "[games] Lexicon ready: " << corpus.lexemes.size() << " words from " << corpus.pageCount
             << " pages in " << location.bucket << " (pdf=" << corpus.fromPdf << ", pageText=" << corpus.fromPageText << ")";
    co_return corpus;
}

drogon::Task<Json::Value> PublicationLexicon::listSources(const std::string &publicationId, int limit) {
    Json::Value data(Json::arrayValue);
    if (limit < 1) limit = 1;
    if (limit > 30) limit = 30;

    std::vector<std::string> filterIds;
    if (!publicationId.empty()) filterIds = co_await hintedDocumentIds(publicationId, "", limit);

    const std::string preferred = filterIds.empty() ? std::string() : filterIds.front();
    MasterBuckets location = locateMasterDocuments(static_cast<size_t>(limit), preferred);
    std::vector<ScannedDocument> documents = location.documents;

    if (!filterIds.empty()) {
        std::vector<ScannedDocument> matched;
        for (const auto &document : documents) {
            for (const auto &id : filterIds) {
                if (documentIdMatches(document, id)) {
                    matched.push_back(document);
                    break;
                }
            }
        }
        // Only narrow the listing when the database actually points at stored files.
        if (!matched.empty()) documents = matched;
    }

    for (const auto &document : documents) {
        const std::vector<std::string> pageTexts = collapsedPages(extractPdfPages(document.path, kMaxPdfPagesForReport));
        const EditionMeta meta = co_await lookupEditionMeta(document.stem);
        const Corpus corpus = buildCorpus(pagesFromDocument(document, pageTexts, meta), {});

        int pagesWithText = 0;
        int textChars = 0;
        int rawTextChars = 0;
        for (const auto &pageText : pageTexts) {
            rawTextChars += static_cast<int>(pageText.size());
            if (pageText.size() < static_cast<size_t>(kMinPageChars)) continue;
            ++pagesWithText;
            textChars += static_cast<int>(pageText.size());
        }

        Json::Value item;
        item["fileName"] = document.fileName;
        item["documentId"] = document.stem;
        item["path"] = document.path;
        item["bucket"] = location.bucket;
        item["sizeBytes"] = static_cast<Json::UInt64>(document.sizeBytes);
        item["modifiedAt"] = document.modifiedAt;
        item["pagesRead"] = static_cast<int>(pageTexts.size());
        item["pagesWithText"] = pagesWithText;
        item["textChars"] = textChars;
        item["rawTextChars"] = rawTextChars;
        item["usableWords"] = static_cast<int>(corpus.lexemes.size());
        item["hasDocument"] = true;
        item["ready"] = corpus.lexemes.size() >= kMinLexemes;
        item["title"] = meta.title.empty() ? document.stem : meta.title;
        item["publicationName"] = meta.publicationName;
        item["publicationId"] = meta.publicationId;
        item["publicationDate"] = meta.publicationDate;
        item["newspaperId"] = meta.newspaperId;
        if (rawTextChars == 0) {
            item["note"] = "No extractable text layer in the first " + std::to_string(kMaxPdfPagesForReport) +
                           " page(s). Upload a PDF with selectable text, or OCR the scan first.";
        } else if (corpus.lexemes.size() < kMinLexemes) {
            item["note"] = "Only " + std::to_string(corpus.lexemes.size()) + " usable words; at least " +
                           std::to_string(kMinLexemes) + " are needed to build a puzzle.";
            if (pagesWithText == 0) {
                item["note"] = item["note"].asString() + " Every page was under " + std::to_string(kMinPageChars) +
                               " characters of extractable text.";
            }
        } else {
            item["note"] = "";
        }
        data.append(item);
    }
    co_return data;
}

Json::Value PublicationLexicon::describeStorage() {
    const MasterBuckets location = locateMasterDocuments(kMaxCorpusDocuments, "");
    Json::Value storage;
    storage["basePath"] = location.basePath;
    storage["bucket"] = location.bucket;
    storage["directory"] = location.directory;
    storage["directoryExists"] = location.directoryExists;
    storage["bucketsChecked"] = Json::Value(Json::arrayValue);
    for (const auto &bucket : location.checked) storage["bucketsChecked"].append(bucket);
    storage["documentsFound"] = static_cast<int>(location.documents.size());
    storage["newestDocument"] = location.documents.empty() ? "" : location.documents.front().fileName;
    storage["newestDocumentModifiedAt"] = location.documents.empty() ? "" : location.documents.front().modifiedAt;
    storage["pagesReadPerDocument"] = kMaxPdfPagesPerDocument;
    storage["documentsScannedPerCorpus"] = kMaxCorpusDocuments;
    storage["readableDocumentsPerCorpus"] = kMaxReadableDocuments;
    storage["minUsableWords"] = static_cast<int>(kMinLexemes);
    return storage;
}

} // namespace gnp::services

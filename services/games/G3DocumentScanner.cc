#include "services/games/G3DocumentScanner.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <system_error>

namespace gnp::services {
namespace {

constexpr int kMaxDirectoryEntries = 20000;

std::string lowerCopy(const std::string &value) {
    std::string out = value;
    for (char &c : out) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc >= 'A' && uc <= 'Z') c = static_cast<char>(uc - 'A' + 'a');
    }
    return out;
}

bool isPdfName(const std::string &name) {
    if (name.size() < 5) return false;
    return lowerCopy(name.substr(name.size() - 4)) == ".pdf";
}

std::string stemOf(const std::string &fileName) {
    const auto dot = fileName.find_last_of('.');
    if (dot == std::string::npos || dot == 0) return fileName;
    return fileName.substr(0, dot);
}

std::string stripPdfSuffix(const std::string &value) {
    if (value.size() > 4 && lowerCopy(value.substr(value.size() - 4)) == ".pdf") {
        return value.substr(0, value.size() - 4);
    }
    return value;
}

/**
 * Converts a file timestamp to unix seconds.
 *
 * The epoch of std::filesystem::file_time_type::clock is implementation defined — libstdc++ does
 * not use the unix epoch — so the two clocks are aligned at the current instant instead of
 * assuming a shared epoch. Only the offset between "now" and the file time is used, which keeps
 * the conversion correct for any epoch.
 */
long long unixSecondsFromFileTime(const std::filesystem::file_time_type &written) {
    using namespace std::chrono;
    using FileClock = std::filesystem::file_time_type::clock;
    const long long fileNow = duration_cast<seconds>(FileClock::now().time_since_epoch()).count();
    const long long systemNow = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    const long long value = duration_cast<seconds>(written.time_since_epoch()).count();
    return value + (systemNow - fileNow);
}

std::string isoUtcFromEpoch(long long epochSeconds) {
    const std::time_t raw = static_cast<std::time_t>(epochSeconds);
    std::tm parts{};
#ifdef _WIN32
    if (gmtime_s(&parts, &raw) != 0) return "";
#else
    if (gmtime_r(&raw, &parts) == nullptr) return "";
#endif
    const int year = parts.tm_year + 1900;
    // Guard against a file clock whose epoch is not the unix epoch.
    if (year < 1970 || year > 2200) return "";
    char buffer[32];
    const int written = std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02dZ",
                                      year, parts.tm_mon + 1, parts.tm_mday,
                                      parts.tm_hour, parts.tm_min, parts.tm_sec);
    if (written <= 0) return "";
    return std::string(buffer, static_cast<std::size_t>(written));
}

} // namespace

bool documentIdMatches(const ScannedDocument &document, const std::string &documentId) {
    if (documentId.empty()) return false;
    const std::string wanted = lowerCopy(stripPdfSuffix(documentId));
    if (wanted.empty()) return false;
    return lowerCopy(document.stem) == wanted || lowerCopy(document.fileName) == lowerCopy(documentId);
}

BucketScan scanBucket(const std::string &basePath,
                      const std::string &bucket,
                      std::size_t maxDocuments,
                      const std::string &preferredStem) {
    BucketScan scan;
    scan.basePath = basePath;
    scan.bucket = bucket;

    const std::filesystem::path directory = std::filesystem::path(basePath) / bucket;
    scan.directory = directory.string();

    std::error_code ec;
    scan.directoryExists = std::filesystem::is_directory(directory, ec) && !ec;
    if (!scan.directoryExists) return scan;

    std::vector<ScannedDocument> found;
    std::filesystem::recursive_directory_iterator it(
        directory, std::filesystem::directory_options::skip_permission_denied, ec);
    const std::filesystem::recursive_directory_iterator end;

    int inspected = 0;
    for (; it != end && inspected < kMaxDirectoryEntries; it.increment(ec)) {
        if (ec) {
            ec.clear();
            continue;
        }
        ++inspected;
        const std::filesystem::directory_entry &entry = *it;
        const std::string name = entry.path().filename().string();
        if (name.empty() || name.front() == '.') {
            std::error_code dirEc;
            if (entry.is_directory(dirEc)) it.disable_recursion_pending();
            continue;
        }

        std::error_code entryEc;
        if (!entry.is_regular_file(entryEc) || entryEc) continue;

        ++scan.filesSeen;
        if (!isPdfName(name)) continue;

        // A zero byte upload is a failed upload; it can never carry a text layer.
        const std::uintmax_t size = entry.file_size(entryEc);
        if (entryEc || size == 0) continue;

        ScannedDocument document;
        document.fileName = name;
        document.stem = stemOf(name);
        document.path = entry.path().string();
        document.sizeBytes = size;

        const std::filesystem::file_time_type written = entry.last_write_time(entryEc);
        if (!entryEc) {
            document.modifiedEpochSeconds = unixSecondsFromFileTime(written);
            document.modifiedAt = isoUtcFromEpoch(document.modifiedEpochSeconds);
        }
        found.push_back(std::move(document));
    }

    std::sort(found.begin(), found.end(), [](const ScannedDocument &a, const ScannedDocument &b) {
        if (a.modifiedEpochSeconds != b.modifiedEpochSeconds) return a.modifiedEpochSeconds > b.modifiedEpochSeconds;
        return a.fileName > b.fileName;
    });

    if (!preferredStem.empty()) {
        std::stable_partition(found.begin(), found.end(), [&preferredStem](const ScannedDocument &document) {
            return documentIdMatches(document, preferredStem);
        });
    }

    scan.documentsFound = static_cast<int>(found.size());
    scan.truncated = maxDocuments > 0 && found.size() > maxDocuments;
    if (scan.truncated) found.resize(maxDocuments);
    scan.documents = std::move(found);
    return scan;
}

} // namespace gnp::services

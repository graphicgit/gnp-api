/**
 * Scans a G3 storage bucket on disk for the newspaper documents the games are built from.
 *
 * Puzzle vocabulary comes from the edition PDFs kept in the master document bucket, so the
 * generator needs to find them without asking the database. This scanner is deliberately free of
 * drogon/mupdf dependencies so the lookup rules (recursive walk, hidden and empty file handling,
 * newest-first ordering, document hints) stay unit testable.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gnp::services {

/** One PDF found inside a G3 bucket directory. */
struct ScannedDocument {
    std::string fileName;             // "9f0c....pdf" — the name the file was stored under
    std::string stem;                 // "9f0c..."     — the resource id, matches newspapers.document_id
    std::string path;                 // full path on disk
    std::uintmax_t sizeBytes = 0;
    std::string modifiedAt;           // ISO-8601 UTC, empty when the timestamp is unusable
    long long modifiedEpochSeconds = 0;
};

/** Outcome of scanning a single bucket. */
struct BucketScan {
    std::string basePath;
    std::string bucket;
    std::string directory;            // "<basePath>/<bucket>"
    bool directoryExists = false;
    int filesSeen = 0;                // regular files inspected, any extension
    int documentsFound = 0;           // usable PDFs before the maxDocuments cap
    bool truncated = false;           // more PDFs exist than the caller asked for
    std::vector<ScannedDocument> documents;   // newest first, capped at maxDocuments
};

/**
 * Lists the newspaper PDFs in "<basePath>/<bucket>", newest first.
 *
 * Hidden entries (leading dot), directories, empty and non-PDF files are skipped; directory
 * symlinks are not followed, so a bucket cannot be walked in a loop. Failures to read a
 * sub-directory are ignored rather than fatal.
 *
 * @param basePath G3 storage root, e.g. "./g3-storage"
 * @param bucket bucket (sub-directory) name, e.g. "gnp-master-documents"
 * @param maxDocuments how many of the newest PDFs to return (0 means no cap)
 * @param preferredStem document id to return first when a matching file exists; the remaining
 *                      files keep their newest-first order behind it
 */
BucketScan scanBucket(const std::string &basePath,
                      const std::string &bucket,
                      std::size_t maxDocuments,
                      const std::string &preferredStem = "");

/** True when the file name looks like a PDF resource id, with or without the extension. */
bool documentIdMatches(const ScannedDocument &document, const std::string &documentId);

} // namespace gnp::services

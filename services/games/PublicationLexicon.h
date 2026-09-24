/**
 * Loads puzzle vocabulary from the newspaper editions stored in G3 storage.
 *
 * The master document bucket (G3Bucket.MasterDocumentBucketName, "gnp-master-documents" by default)
 * is scanned for the newest edition PDFs, their text layer is read with MuPDF and the corpus is
 * built from that text. The newspapers table is consulted only for optional metadata (title,
 * publication name, date) and to resolve an explicit publicationId/newspaperId filter to a stored
 * document — puzzle words never come from the database.
 */
#pragma once

#include <json/json.h>
#include <string>
#include <vector>

#include <drogon/utils/coroutine.h>

#include "services/games/Corpus.h"

namespace gnp::services {

class PublicationLexicon {
public:
    static drogon::Task<Corpus> load(const std::string &publicationId,
                                     const std::string &newspaperId,
                                     const std::vector<std::string> &topics);

    /** Per-file diagnostics for the PDFs found in the master document bucket. */
    static drogon::Task<Json::Value> listSources(const std::string &publicationId, int limit);

    /** Where the generator looks for newspaper PDFs and how much text it reads. */
    static Json::Value describeStorage();

    static void clearCache();
};

} // namespace gnp::services

//
// Created by Emmanuel Addo-Odame on 18/09/2026.
//

#ifndef GNPAPI_JOOMLAAPIARTICLERESPONSE_H
#define GNPAPI_JOOMLAAPIARTICLERESPONSE_H

#include <json/json.h>
#include <optional>
#include <string>
#include <vector>

namespace gnp::dto {

class JoomlaApiArticleResponse {
public:

    // ------------------------------------------------------------------
    //  Pagination links returned by Joomla (JSON:API "links" object)
    // ------------------------------------------------------------------
    struct Links {
        std::string self;
        std::string next;
        std::string last;

        static Links fromJson(const Json::Value &json) {
            Links l;
            if (json.isObject()) {
                if (json.isMember("self")) l.self = json["self"].asString();
                if (json.isMember("next")) l.next = json["next"].asString();
                if (json.isMember("last")) l.last = json["last"].asString();
            }
            return l;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["self"] = self;
            j["next"] = next;
            j["last"] = last;
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  "meta" block (JSON:API meta object)
    // ------------------------------------------------------------------
    struct Meta {
        int totalPages = 0;

        static Meta fromJson(const Json::Value &json) {
            Meta m;
            if (json.isObject() && json.isMember("total-pages") &&
                !json["total-pages"].isNull()) {
                m.totalPages = json["total-pages"].asInt();
            }
            return m;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["total-pages"] = totalPages;
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  Article images (attributes.images)
    // ------------------------------------------------------------------
    struct Images {
        std::string imageIntro;
        std::string imageIntroAlt;
        std::string floatIntro;
        std::string imageIntroCaption;
        std::string imageFulltext;
        std::string imageFulltextAlt;
        std::string floatFulltext;
        std::string imageFulltextCaption;

        static Images fromJson(const Json::Value &json) {
            Images i;
            if (!json.isObject()) return i;

            auto get = [&json](const char *key) -> std::string {
                return (json.isMember(key) && !json[key].isNull())
                           ? json[key].asString()
                           : std::string{};
            };

            i.imageIntro          = get("image_intro");
            i.imageIntroAlt       = get("image_intro_alt");
            i.floatIntro          = get("float_intro");
            i.imageIntroCaption   = get("image_intro_caption");
            i.imageFulltext       = get("image_fulltext");
            i.imageFulltextAlt    = get("image_fulltext_alt");
            i.floatFulltext       = get("float_fulltext");
            i.imageFulltextCaption = get("image_fulltext_caption");
            return i;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["image_intro"]           = imageIntro;
            j["image_intro_alt"]       = imageIntroAlt;
            j["float_intro"]           = floatIntro;
            j["image_intro_caption"]   = imageIntroCaption;
            j["image_fulltext"]        = imageFulltext;
            j["image_fulltext_alt"]    = imageFulltextAlt;
            j["float_fulltext"]        = floatFulltext;
            j["image_fulltext_caption"] = imageFulltextCaption;
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  Article metadata (attributes.metadata)
    // ------------------------------------------------------------------
    struct Metadata {
        std::string robots;
        std::string author;
        std::string rights;

        static Metadata fromJson(const Json::Value &json) {
            Metadata m;
            if (!json.isObject()) return m;
            if (json.isMember("robots") && !json["robots"].isNull()) m.robots = json["robots"].asString();
            if (json.isMember("author") && !json["author"].isNull()) m.author = json["author"].asString();
            if (json.isMember("rights") && !json["rights"].isNull()) m.rights = json["rights"].asString();
            return m;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["robots"] = robots;
            j["author"] = author;
            j["rights"] = rights;
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  A single relationship entry ({ "type": ..., "id": ... })
    // ------------------------------------------------------------------
    struct RelationshipRef {
        std::string type;
        std::string id;

        static RelationshipRef fromJson(const Json::Value &json) {
            RelationshipRef r;
            if (json.isObject()) {
                if (json.isMember("type") && !json["type"].isNull()) r.type = json["type"].asString();
                if (json.isMember("id")   && !json["id"].isNull())   r.id   = json["id"].asString();
            }
            return r;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["type"] = type;
            j["id"]   = id;
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  "relationships" block for one article
    // ------------------------------------------------------------------
    struct Relationships {
        RelationshipRef category;    // data is an object
        RelationshipRef createdBy;   // data is an object
        // tags: data is an array (empty in the sample) — kept as a vector
        // of RelationshipRef for completeness.
        std::vector<RelationshipRef> tags;

        static Relationships fromJson(const Json::Value &json) {
            Relationships r;
            if (!json.isObject()) return r;

            if (json.isMember("category") && json["category"].isObject() &&
                json["category"].isMember("data")) {
                r.category = RelationshipRef::fromJson(json["category"]["data"]);
            }
            if (json.isMember("created_by") && json["created_by"].isObject() &&
                json["created_by"].isMember("data")) {
                r.createdBy = RelationshipRef::fromJson(json["created_by"]["data"]);
            }
            if (json.isMember("tags") && json["tags"].isObject() &&
                json["tags"].isMember("data") && json["tags"]["data"].isArray()) {
                for (const auto &t : json["tags"]["data"]) {
                    r.tags.push_back(RelationshipRef::fromJson(t));
                }
            }
            return r;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;

            Json::Value cat;
            cat["data"] = category.toJson();
            j["category"] = cat;

            Json::Value cb;
            cb["data"] = createdBy.toJson();
            j["created_by"] = cb;

            Json::Value tagsArr(Json::arrayValue);
            for (const auto &t : tags) tagsArr.append(t.toJson());
            Json::Value tags;
            tags["data"] = tagsArr;
            j["tags"] = tags;

            return j;
        }
    };

    // ------------------------------------------------------------------
    //  attributes  (the bulk of an article)
    // ------------------------------------------------------------------
    struct Attributes {
        int         id            = 0;
        int         assetId       = 0;
        std::string title;
        std::string alias;
        int         state         = 0;
        int         access        = 0;
        std::string created;
        int         createdBy     = 0;
        std::string createdByAlias;
        std::string modified;
        int         featured      = 0;
        std::string language;
        int         hits          = 0;
        std::string publishUp;
        std::optional<std::string> publishDown;   // can be null
        std::string note;
        Images      images;
        std::string metakey;
        std::string metadesc;
        Metadata    metadata;
        int         version       = 0;
        std::optional<std::string> featuredUp;    // can be null
        std::optional<std::string> featuredDown;  // can be null
        std::string typeAlias;
        std::string text;
        // tags: [] — array of strings (empty in the sample). Kept for parity.
        std::vector<std::string> tags;

        static Attributes fromJson(const Json::Value &json) {
            Attributes a;
            if (!json.isObject()) return a;

            auto getStr = [&json](const char *key) -> std::string {
                return (json.isMember(key) && !json[key].isNull())
                           ? json[key].asString()
                           : std::string{};
            };

            auto getInt = [&json](const char *key) -> int {
                return (json.isMember(key) && !json[key].isNull())
                           ? json[key].asInt()
                           : 0;
            };

            auto getOptStr = [&json](const char *key) -> std::optional<std::string> {
                if (json.isMember(key) && !json[key].isNull()) {
                    return json[key].asString();
                }
                return std::nullopt;
            };

            a.id             = getInt("id");
            a.assetId        = getInt("asset_id");
            a.title          = getStr("title");
            a.alias          = getStr("alias");
            a.state          = getInt("state");
            a.access         = getInt("access");
            a.created        = getStr("created");
            a.createdBy      = getInt("created_by");
            a.createdByAlias = getStr("created_by_alias");
            a.modified       = getStr("modified");
            a.featured       = getInt("featured");
            a.language       = getStr("language");
            a.hits           = getInt("hits");
            a.publishUp      = getStr("publish_up");
            a.publishDown    = getOptStr("publish_down");
            a.note           = getStr("note");
            a.metakey        = getStr("metakey");
            a.metadesc       = getStr("metadesc");
            a.version        = getInt("version");
            a.featuredUp     = getOptStr("featured_up");
            a.featuredDown   = getOptStr("featured_down");
            a.typeAlias      = getStr("typeAlias");
            a.text           = getStr("text");

            if (json.isMember("images") && json["images"].isObject()) {
                a.images = Images::fromJson(json["images"]);
            }
            if (json.isMember("metadata") && json["metadata"].isObject()) {
                a.metadata = Metadata::fromJson(json["metadata"]);
            }
            if (json.isMember("tags") && json["tags"].isArray()) {
                for (const auto &t : json["tags"]) {
                    if (!t.isNull()) a.tags.push_back(t.asString());
                }
            }
            return a;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["id"]              = id;
            j["asset_id"]        = assetId;
            j["title"]           = title;
            j["alias"]           = alias;
            j["state"]           = state;
            j["access"]          = access;
            j["created"]         = created;
            j["created_by"]      = createdBy;
            j["created_by_alias"] = createdByAlias;
            j["modified"]        = modified;
            j["featured"]        = featured;
            j["language"]        = language;
            j["hits"]            = hits;
            j["publish_up"]      = publishUp;
            j["publish_down"]    = publishDown ? Json::Value(*publishDown) : Json::Value(Json::nullValue);
            j["note"]            = note;
            j["images"]          = images.toJson();
            j["metakey"]         = metakey;
            j["metadesc"]        = metadesc;
            j["metadata"]        = metadata.toJson();
            j["version"]         = version;
            j["featured_up"]     = featuredUp ? Json::Value(*featuredUp) : Json::Value(Json::nullValue);
            j["featured_down"]   = featuredDown ? Json::Value(*featuredDown) : Json::Value(Json::nullValue);
            j["typeAlias"]       = typeAlias;
            j["text"]            = text;

            Json::Value tagsArr(Json::arrayValue);
            for (const auto &t : tags) tagsArr.append(t);
            j["tags"] = tagsArr;

            return j;
        }
    };

    // ------------------------------------------------------------------
    //  A single article entry in data[]
    // ------------------------------------------------------------------
    struct Article {
        std::string  type;          // e.g. "articles"
        std::string  id;            // JSON:API resource id (string)
        Attributes   attributes;
        Relationships relationships;

        static Article fromJson(const Json::Value &json) {
            Article a;
            if (!json.isObject()) return a;

            if (json.isMember("type") && !json["type"].isNull()) a.type = json["type"].asString();
            if (json.isMember("id")   && !json["id"].isNull())   a.id   = json["id"].asString();
            if (json.isMember("attributes") && json["attributes"].isObject()) {
                a.attributes = Attributes::fromJson(json["attributes"]);
            }
            if (json.isMember("relationships") && json["relationships"].isObject()) {
                a.relationships = Relationships::fromJson(json["relationships"]);
            }
            return a;
        }

        [[nodiscard]] Json::Value toJson() const {
            Json::Value j;
            j["type"]          = type;
            j["id"]            = id;
            j["attributes"]    = attributes.toJson();
            j["relationships"] = relationships.toJson();
            return j;
        }
    };

    // ------------------------------------------------------------------
    //  Top-level response object
    // ------------------------------------------------------------------
    Links                links;
    std::vector<Article> data;
    Meta                 meta;

    static JoomlaApiArticleResponse fromJson(const Json::Value &json) {
        JoomlaApiArticleResponse res;
        if (!json.isObject()) return res;

        if (json.isMember("links") && json["links"].isObject()) {
            res.links = Links::fromJson(json["links"]);
        }

        if (json.isMember("data") && json["data"].isArray()) {
            for (const auto &item : json["data"]) {
                res.data.push_back(Article::fromJson(item));
            }
        }

        if (json.isMember("meta") && json["meta"].isObject()) {
            res.meta = Meta::fromJson(json["meta"]);
        }

        return res;
    }

    [[nodiscard]] Json::Value toJson() const {
        Json::Value j;
        j["links"] = links.toJson();

        Json::Value dataArr(Json::arrayValue);
        for (const auto &a : data) dataArr.append(a.toJson());
        j["data"] = dataArr;

        j["meta"] = meta.toJson();
        return j;
    }
};

} // namespace gnp::services

#endif //GNPAPI_JOOMLAAPIARTICLERESPONSE_H
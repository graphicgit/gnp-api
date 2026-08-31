//
// Created by Emmanuel Addo-Odame on 18/08/2026.
//

#ifndef GNPAPI_ARTICLEDTO_H
#define GNPAPI_ARTICLEDTO_H
#include <json/json.h>
#include <string>

namespace gnp::dto {

    struct ImageDto {
        std::string imageIntro;
        std::string imageIntroAlt;
        std::string floatIntro;
        std::string imageIntroCaption;
        std::string imageFulltext;
        std::string imageFulltextAlt;
        std::string floatFulltext;
        std::string imageFulltextCaption;

        void fromJson(const Json::Value& json) {
            if (json.isObject()) {
                imageIntro = json.get("image_intro", "").asString();
                imageIntroAlt = json.get("image_intro_alt", "").asString();
                floatIntro = json.get("float_intro", "").asString();
                imageIntroCaption = json.get("image_intro_caption", "").asString();
                imageFulltext = json.get("image_fulltext", "").asString();
                imageFulltextAlt = json.get("image_fulltext_alt", "").asString();
                floatFulltext = json.get("float_fulltext", "").asString();
                imageFulltextCaption = json.get("image_fulltext_caption", "").asString();
            }
        }
    };

    class ArticleDto {

    public:
        ArticleDto() = default;
        explicit ArticleDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        // Getters
        [[nodiscard]] const std::string& getExternalId() const { return external_id_; }
        [[nodiscard]] const std::string& getExternalAssetId() const { return external_asset_id_; }
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getAlias() const { return alias_; }
        [[nodiscard]] const std::string& getCreatedByAlias() const { return createdByAlias_; }
        [[nodiscard]] const std::string& getModified() const { return modified_; }
        [[nodiscard]] const std::string& getPublishUp() const { return publishUp_; }
        [[nodiscard]] const ImageDto& getImages() const { return images_; }
        [[nodiscard]] const std::string& getMetakey() const { return metakey_; }
        [[nodiscard]] const std::string& getMetadesc() const { return metadesc_; }
        [[nodiscard]] const std::string& getText() const { return text_; }
        [[nodiscard]] const std::vector<std::string>& getTags() const { return tags_; }


        // Setters
        void setId(const std::string& id) { external_id_ = id; }
        void setAssetId(const std::string& assetId) { external_asset_id_ = assetId; }
        void setTitle(const std::string& title) { title_ = title; }
        void setAlias(const std::string& alias) { alias_ = alias; }
        void setCreatedByAlias(const std::string& createdByAlias) { createdByAlias_ = createdByAlias; }
        void setModified(const std::string& modified) { modified_ = modified; }
        void setPublishUp(const std::string& publishUp) { publishUp_ = publishUp; }
        void setImages(const ImageDto& images) { images_ = images; }
        void setMetakey(const std::string& metakey) { metakey_ = metakey; }
        void setMetadesc(const std::string& metadesc) { metadesc_ = metadesc; }
        void setText(const std::string& text) { text_ = text; }
        void setTags(const std::vector<std::string>& tags) { tags_ = tags; }


    private:
        std::string external_id_ ;
        std::string external_asset_id_;
        std::string title_;
        std::string alias_;
        std::string createdByAlias_;
        std::string modified_;
        std::string publishUp_;
        ImageDto images_;
        std::string metakey_;
        std::string metadesc_;
        std::string text_;
        std::vector<std::string> tags_;

    };

    inline void ArticleDto::fromJson(const Json::Value& json) {

        if (!json.isObject()) return;


        // Flatten directly from attributes object if present
        const Json::Value& attrs = json.isMember("attributes") ? json["attributes"] : json;

        if (attrs.isObject()) {
            external_id_ = attrs.get("id", 0).asInt();
            external_asset_id_ = attrs.get("asset_id", 0).asInt();
            title_ = attrs.get("title", "").asString();
            alias_ = attrs.get("alias", "").asString();
            createdByAlias_ = attrs.get("created_by_alias", "").asString();
            modified_ = attrs.get("modified", "").asString();
            publishUp_ = attrs.get("publish_up", "").asString();
            images_.fromJson(attrs["images"]);
            metakey_ = attrs.get("metakey", "").asString();
            metadesc_ = attrs.get("metadesc", "").asString();
            text_ = attrs.get("text", "").asString();

            tags_.clear();
            if (attrs.isMember("tags") && attrs["tags"].isArray()) {
                for (const auto& tag : attrs["tags"]) {
                    tags_.push_back(tag.asString());
                }
            }
        }



    }

}
#endif //GNPAPI_ARTICLEDTO_H

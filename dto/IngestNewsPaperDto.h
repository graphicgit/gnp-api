//
// Created by Emmanuel Addo-Odame on 14/11/2025.
//

#ifndef INGESTNEWSPAPERDTO_H
#define INGESTNEWSPAPERDTO_H
#include <json/json.h>

namespace gnp::dto {

    class IngestNewsPaperDto {

    public:

        IngestNewsPaperDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getSlug() const { return slug_; }
        [[nodiscard]] double getPrice() const { return price_; }
        [[nodiscard]] bool isFree() const { return is_free_; }
        [[nodiscard]] const std::string& getCategoryId() const { return category_id_; }
        [[nodiscard]] const std::string& getCategoryName() const { return category_name_; }
        [[nodiscard]] const std::string& getPublicationId() const { return publication_id_; }
        [[nodiscard]] const std::string& getPublicationName() const { return publication_name_; }
        [[nodiscard]] const std::string& getCopyrightOwner() const { return copyright_owner_; }
        [[nodiscard]] const std::string& getEditionNumber() const { return edition_number_; }
        [[nodiscard]] bool getIsPopular() const { return is_popular_; }
        [[nodiscard]] const std::string& getShortDescription() const { return short_description_; }
        [[nodiscard]] const std::string& getFullDescription() const { return full_description_; }
        [[nodiscard]] const std::string& getThumbnailId() const { return thumbnail_id_; }
        [[nodiscard]] const std::string& getFileType() const { return file_type_; }
        [[nodiscard]] const std::string& getStorageType() const { return storage_type_; }
        [[nodiscard]] const std::string& getDocumentId() const { return document_id_; }
        [[nodiscard]] bool isPublished() const { return is_published_; }
        [[nodiscard]] const std::string& getPublishedDate() const { return published_date_; }
        [[nodiscard]] const std::string& getFeaturedStories() const { return featured_stories_; }

        // Setters
        void setTitle(const std::string& v) { title_ = v; }
        void setSlug(const std::string& v) { slug_ = v; }
        void setPrice(double v) { price_ = v; }
        void setIsFree(bool v) { is_free_ = v; }
        void setCategoryId(const std::string& v) { category_id_ = v; }
        void setCategoryName(const std::string& v) { category_name_ = v; }
        void setPublicationId(const std::string& v) { publication_id_ = v; }
        void setPublicationName(const std::string& v) { publication_name_ = v; }
        void setCopyrightOwner(const std::string& v) { copyright_owner_ = v; }
        void setEditionNumber(const std::string& v) { edition_number_ = v; }
        void setIsPopular(bool v) { is_popular_ = v; }
        void setShortDescription(const std::string& v) { short_description_ = v; }
        void setFullDescription(const std::string& v) { full_description_ = v; }
        void setThumbnailId(const std::string& v) { thumbnail_id_ = v; }
        void setFileType(const std::string& v) { file_type_ = v; }
        void setStorageType(const std::string& v) { storage_type_ = v; }
        void setDocumentId(const std::string& v) { document_id_ = v; }
        void setIsPublished(bool v) { is_published_ = v; }
        void setPublishedDate(const std::string& v) { published_date_ = v; }
        void setFeaturedStories(const std::string& v) { featured_stories_ = v; }


    private:

        std::string title_;
        std::string slug_;
        double price_ = 0.00;
        bool is_free_ = false;
        std::string category_id_;
        std::string category_name_;
        std::string publication_id_;
        std::string publication_name_;
        std::string copyright_owner_;
        std::string edition_number_;
        bool is_popular_ = false;
        std::string short_description_;
        std::string full_description_;
        std::string thumbnail_id_;
        std::string file_type_;
        std::string featured_stories_;
        std::string storage_type_;
        std::string document_id_;
        bool is_published_ = false;
        std::string published_date_;

    };

    inline void IngestNewsPaperDto::fromJson(const Json::Value& json) {

        if (json.isMember("title")) title_ = json["title"].asString();
        if (json.isMember("slug")) slug_ = json["slug"].asString();
        if (json.isMember("price")) price_ = json["price"].asDouble();
        if (json.isMember("isFree")) is_free_ = json["isFree"].asBool();
        if (json.isMember("categoryId")) category_id_ = json["categoryId"].asString();
        if (json.isMember("categoryName")) category_name_ = json["categoryName"].asString();
        if (json.isMember("publicationId")) publication_id_ = json["publicationId"].asString();
        if (json.isMember("publicationName")) publication_name_ = json["publicationName"].asString();
        if (json.isMember("copyrightOwner")) copyright_owner_ = json["copyrightOwner"].asString();
        if (json.isMember("editionNumber")) edition_number_ = json["editionNumber"].asString();
        if (json.isMember("isPopular")) is_popular_ = json["isPopular"].asBool();
        if (json.isMember("shortDescription")) short_description_ = json["shortDescription"].asString();
        if (json.isMember("fullDescription")) full_description_ = json["fullDescription"].asString();
        if (json.isMember("thumbnailId")) thumbnail_id_ = json["thumbnailId"].asString();
        if (json.isMember("fileType")) file_type_ = json["fileType"].asString();
        if (json.isMember("storageType")) storage_type_ = json["storageType"].asString();
        if (json.isMember("documentId")) document_id_ = json["documentId"].asString();
        if (json.isMember("isPublished")) is_published_ = json["isPublished"].asBool();
        if (json.isMember("publishedDate")) published_date_ = json["publishedDate"].asString();

        if (json.isMember("featuredStories") && json["featuredStories"].isArray()) {
            Json::StreamWriterBuilder builder;
            builder["commentStyle"] = "None";
            builder["indentation"] = "";  // Compact JSON
            featured_stories_ = Json::writeString(builder, json["featuredStories"]);
        } else {
            featured_stories_ = "[]";  // Default empty array
        }

    }


}


#endif //INGESTNEWSPAPERDTO_H

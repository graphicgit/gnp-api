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
        [[nodiscard]] int getPopular() const { return popular_; }
        [[nodiscard]] const std::string& getShortDescription() const { return short_description_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getThumbnailImage() const { return thumbnail_image_; }
        [[nodiscard]] const std::string& getCoverImage() const { return cover_image_; }
        [[nodiscard]] const std::string& getFileConverted() const { return file_converted_; }
        [[nodiscard]] const std::string& getFileType() const { return file_type_; }
        [[nodiscard]] const std::string& getPreviewUrl() const { return preview_url_; }
        [[nodiscard]] const std::string& getDocumentUrl() const { return document_url_; }
        [[nodiscard]] bool isPublished() const { return is_published_; }
        [[nodiscard]] const std::string& getPublishedDate() const { return published_date_; }

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
        void setPopular(int v) { popular_ = v; }
        void setShortDescription(const std::string& v) { short_description_ = v; }
        void setDescription(const std::string& v) { description_ = v; }
        void setThumbnailImage(const std::string& v) { thumbnail_image_ = v; }
        void setCoverImage(const std::string& v) { cover_image_ = v; }
        void setFileConverted(const std::string& v) { file_converted_ = v; }
        void setFileType(const std::string& v) { file_type_ = v; }
        void setPreviewUrl(const std::string& v) { preview_url_ = v; }
        void setDocumentUrl(const std::string& v) { document_url_ = v; }
        void setIsPublished(bool v) { is_published_ = v; }
        void setPublishedDate(const std::string& v) { published_date_ = v; }


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
        int popular_ = 0;
        std::string short_description_;
        std::string description_;
        std::string thumbnail_image_;
        std::string cover_image_;
        std::string file_converted_;
        std::string file_type_;
        std::string preview_url_;
        std::string document_url_;
        bool is_published_ = false;
        std::string published_date_;


    };

    inline void IngestNewsPaperDto::fromJson(const Json::Value& json) {

        if (json.isMember("title")) title_ = json["title"].asString();
        if (json.isMember("slug")) slug_ = json["slug"].asString();
        if (json.isMember("price")) price_ = json["price"].asDouble();
        if (json.isMember("isFree")) is_free_ = json["isFree"].asBool();
        if (json.isMember("categoryId")) category_id_ = json["categoryId"].asString();
        if (json.isMember("publicationId")) publication_id_ = json["publicationId"].asString();
        if (json.isMember("copyrightOwner")) copyright_owner_ = json["copyrightOwner"].asString();
        if (json.isMember("editionNumber")) edition_number_ = json["editionNumber"].asString();
        if (json.isMember("popular")) popular_ = json["popular"].asInt();
        if (json.isMember("shortDescription")) short_description_ = json["shortDescription"].asString();
        if (json.isMember("description")) description_ = json["description"].asString();
        if (json.isMember("thumbnailImage")) thumbnail_image_ = json["thumbnailImage"].asString();
        if (json.isMember("coverImage")) cover_image_ = json["coverImage"].asString();
        if (json.isMember("fileConverted")) file_converted_ = json["fileConverted"].asString();
        if (json.isMember("fileType")) file_type_ = json["fileType"].asString();
        if (json.isMember("previewUrl")) preview_url_ = json["previewUrl"].asString();
        if (json.isMember("documentUrl")) document_url_ = json["documentUrl"].asString();
        if (json.isMember("isPublished")) is_published_ = json["isPublished"].asBool();
        if (json.isMember("publishedDate")) published_date_ = json["publishedDate"].asString();

    }


}


#endif //INGESTNEWSPAPERDTO_H

//
// Created by Emmanuel Addo-Odame on 24/06/2026.
//

#ifndef GNPAPI_G3STORAGESERVICE_H
#define GNPAPI_G3STORAGESERVICE_H

#include <string>
#include <optional>
#include <drogon/utils/coroutine.h>

namespace gnp::services {

    class G3StorageService {
    public:
        G3StorageService();

        /**
         * @brief Saves file data to a specific bucket (directory).
         * @param bucketName The name of the bucket
         * @param fileName The name of the file to save
         * @param fileData The binary or text data of the file
         * @return true if successful, false otherwise
         */
        bool saveFile(const std::string& bucketName, const std::string& fileName, const std::string& fileData) const;

        /**
         * @brief Deletes a file from a specific bucket.
         * @param bucketName The name of the bucket
         * @param fileName The name of the file to delete
         * @return true if deleted or doesn't exist, false if deletion failed
         */
        bool deleteFile(const std::string& bucketName, const std::string& fileName) const;

        /**
         * @brief Retrieves the full path to a file in a bucket.
         * @param bucketName The name of the bucket
         * @param fileName The name of the file
         * @return The absolute or relative path if it exists, std::nullopt otherwise
         */
        std::optional<std::string> getFilePath(const std::string& bucketName, const std::string& fileName) const;

        /**
         * @brief Retrieves the binary/text contents of a file in a bucket.
         * @param bucketName The name of the bucket
         * @param fileName The name of the file
         * @return The file contents if it exists, std::nullopt otherwise
         */
        std::optional<std::string> getFileContent(const std::string& bucketName, const std::string& fileName) const;

        /**
         * @brief Extracts the first page of a PDF file as a PNG thumbnail and uploads to Cloudinary.
         * @param bucketName The name of the bucket
         * @param fileName The name of the PDF file
         * @param thumbnailFileName The name to save the generated PNG thumbnail
         * @return The URL of the uploaded thumbnail if successful, empty string otherwise
         */
        drogon::Task<std::string> extractThumbnail(const std::string& bucketName, const std::string& fileName, const std::string& thumbnailFileName) const;

    private:
        void ensureBucketExists(const std::string& bucketName) const;
        std::string buildFilePath(const std::string& bucketName, const std::string& fileName) const;
        bool generateLocalThumbnail(const std::string& pdfFilePath, const std::string& thumbnailFilePath) const;

        std::string baseStoragePath_;
    };

}
#endif //GNPAPI_G3STORAGESERVICE_H
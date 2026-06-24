//
// Created by Emmanuel Addo-Odame on 24/06/2026.
//

#include "G3StorageService.h"
#include <drogon/drogon.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace gnp::services {

G3StorageService::G3StorageService() {
  // You can customize the base storage path via config or use a default
  auto customConfig = drogon::app().getCustomConfig();
  if (customConfig.isMember("G3Bucket") &&
      customConfig["G3Bucket"].isMember("BaseStoragePath")) {
    baseStoragePath_ = customConfig["G3Bucket"]["BaseStoragePath"].asString();
  } else {
    baseStoragePath_ = "/var/g3-storage";
  }

  // Ensure base directory exists
  if (!std::filesystem::exists(baseStoragePath_)) {
    std::filesystem::create_directories(baseStoragePath_);
  }
}

void G3StorageService::ensureBucketExists(const std::string &bucketName) const {
  std::filesystem::path bucketPath =
      std::filesystem::path(baseStoragePath_) / bucketName;
  if (!std::filesystem::exists(bucketPath)) {
    std::filesystem::create_directories(bucketPath);
  }
}

std::string G3StorageService::buildFilePath(const std::string &bucketName,
                                           const std::string &fileName) const {
  return (std::filesystem::path(baseStoragePath_) / bucketName / fileName)
      .string();
}

bool G3StorageService::saveFile(const std::string &bucketName,
                               const std::string &fileName,
                               const std::string &fileData) const {
  try {
    ensureBucketExists(bucketName);
    std::string filePath = buildFilePath(bucketName, fileName);

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
      LOG_ERROR << "G3BucketService: Failed to open file for writing: "
                << filePath;
      return false;
    }
    outFile.write(fileData.data(), fileData.size());
    outFile.close();
    return true;
  } catch (const std::exception &e) {
    LOG_ERROR << "G3BucketService: Exception saving file " << fileName
              << " to bucket " << bucketName << ": " << e.what();
    return false;
  }
}

bool G3StorageService::deleteFile(const std::string &bucketName,
                                 const std::string &fileName) const {
  try {
    std::string filePath = buildFilePath(bucketName, fileName);
    if (std::filesystem::exists(filePath)) {
      return std::filesystem::remove(filePath);
    }
    return true; // Return true if it already doesn't exist
  } catch (const std::exception &e) {
    LOG_ERROR << "G3BucketService: Exception deleting file " << fileName
              << " from bucket " << bucketName << ": " << e.what();
    return false;
  }
}

std::optional<std::string>
G3StorageService::getFilePath(const std::string &bucketName,
                             const std::string &fileName) const {
  std::string filePath = buildFilePath(bucketName, fileName);
  if (std::filesystem::exists(filePath) &&
      std::filesystem::is_regular_file(filePath)) {
    return filePath;
  }
  return std::nullopt;
}

std::optional<std::string> G3StorageService::getFileContent(const std::string &bucketName,
                                const std::string &fileName) const {
  try {
    std::string filePath = buildFilePath(bucketName, fileName);
    if (!std::filesystem::exists(filePath) ||
        !std::filesystem::is_regular_file(filePath)) {
      return std::nullopt;
    }

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
      LOG_ERROR << "G3BucketService: Failed to open file for reading: "
                << filePath;
      return std::nullopt;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
  } catch (const std::exception &e) {
    LOG_ERROR << "G3BucketService: Exception reading file " << fileName
              << " from bucket " << bucketName << ": " << e.what();
    return std::nullopt;
  }
}

} // namespace gnp::services
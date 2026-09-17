//
// Created by Emmanuel Addo-Odame on 24/06/2026.
//

#include "G3StorageService.h"
#include <chrono>
#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <filesystem>
#include <fstream>
#include <mupdf/fitz.h>
#include <sstream>
namespace gnp::services {

G3StorageService::G3StorageService() {

  // You can customize the base storage path via config or use a default
  auto customConfig = drogon::app().getCustomConfig();
  if (customConfig.isMember("G3Bucket") &&
      customConfig["G3Bucket"].isMember("BaseStoragePath")) {
    baseStoragePath_ = customConfig["G3Bucket"]["BaseStoragePath"].asString();
  } else {
    baseStoragePath_ = "./g3-storage";
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
    LOG_DEBUG << "[saveFile] Starting — bucket: '" << bucketName << "', file: '"
              << fileName << "', size: " << fileData.size() << " bytes";

    ensureBucketExists(bucketName);
    LOG_DEBUG << "[saveFile] Bucket ensured: '" << bucketName << "'";

    std::string filePath = buildFilePath(bucketName, fileName);
    LOG_DEBUG << "[saveFile] Resolved file path: '" << filePath << "'";

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
      LOG_ERROR << "[saveFile] Failed to open file for writing: '" << filePath
                << "' — check permissions or path validity";
      return false;
    }
    LOG_DEBUG << "[saveFile] File opened successfully, writing "
              << fileData.size() << " bytes";

    outFile.write(fileData.data(),
                  static_cast<std::streamsize>(fileData.size()));

    if (!outFile.good()) {
      LOG_ERROR << "[saveFile] Stream error after write to '" << filePath
                << "' — badbit=" << outFile.bad()
                << ", failbit=" << outFile.fail();
      return false;
    }

    outFile.close();
    LOG_DEBUG << "[saveFile] File written and closed successfully: '"
              << filePath << "'";
    return true;
  } catch (const std::filesystem::filesystem_error &e) {
    LOG_ERROR << "[saveFile] Filesystem error saving '" << fileName
              << "' to bucket '" << bucketName << "': " << e.what()
              << " (path1: '" << e.path1() << "', path2: '" << e.path2()
              << "')";
    return false;
  } catch (const std::exception &e) {
    LOG_ERROR << "[saveFile] Exception saving '" << fileName << "' to bucket '"
              << bucketName << "': " << e.what();
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

std::optional<std::string>
G3StorageService::getFileContent(const std::string &bucketName,
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

drogon::Task<std::string> G3StorageService::extractThumbnail(
    const std::string &bucketName, const std::string &fileName,
    const std::string &thumbnailFileName) const {
  std::string pdfFilePath = buildFilePath(bucketName, fileName);

  auto customConfig = drogon::app().getCustomConfig();
  std::string thumbnailBucketName =
      customConfig["G3Bucket"]["ThumbnailBucketName"].asString();

  if (thumbnailBucketName.empty()) {
    // Fallback to a default bucket name (or use the same bucket)
    thumbnailBucketName = "thumbnails";
    LOG_WARN
        << "[extractThumbnail] ThumbnailBucketName not set, using default: "
        << thumbnailBucketName;
  }

  ensureBucketExists(thumbnailBucketName);

  std::string thumbnailFilePath =
      buildFilePath(thumbnailBucketName, thumbnailFileName);

  if (!std::filesystem::exists(pdfFilePath)) {
    LOG_ERROR << "[extractThumbnail] PDF file not found: " << pdfFilePath;
    co_return "";
  }

  fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if (!ctx) {
    LOG_ERROR << "[extractThumbnail] Failed to create mupdf context";
    co_return "";
  }

  // Register document handlers to be able to open PDFs
  fz_register_document_handlers(ctx);

  fz_document *doc = NULL;
  fz_pixmap *pix = NULL;
  bool success = false;

  fz_try(ctx) {
    // Open the PDF document
    doc = fz_open_document(ctx, pdfFilePath.c_str());

    // Calculate a transform to render the page at 72 dpi (scale 1.0)
    fz_matrix ctm = fz_scale(1.0f, 1.0f);

    // Render the page to a pixmap
    pix =
        fz_new_pixmap_from_page_number(ctx, doc, 0, ctm, fz_device_rgb(ctx), 0);

    // Save the pixmap as a PNG image
    fz_save_pixmap_as_png(ctx, pix, thumbnailFilePath.c_str());

    success = true;
  }
  fz_always(ctx) {
    fz_drop_pixmap(ctx, pix);
    fz_drop_document(ctx, doc);
  }
  fz_catch(ctx) {
    LOG_ERROR << "[extractThumbnail] mupdf error: " << fz_caught_message(ctx);
  }

  fz_drop_context(ctx);

  if (!success) {
    co_return "";
  }
  LOG_DEBUG << "[extractThumbnail] Thumbnail saved successfully locally: "
            << thumbnailFilePath;

  // upload the thumbnail to cloudinary ...
  std::string cloud = customConfig["CloudinarySettings"]["Cloud"].asString();
  std::string apiKey = customConfig["CloudinarySettings"]["ApiKey"].asString();
  std::string apiSecret = customConfig["CloudinarySettings"]["ApiSecret"].asString();

  auto now = std::chrono::system_clock::now();
  auto timestamp = std::to_string(
      std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
          .count());

  // parameters to sign must be sorted alphabetically: folder, timestamp
  std::string stringToSign = "folder=thumbnails&timestamp=" + timestamp + apiSecret;
  std::string signature = drogon::utils::getSha1(stringToSign);

  auto req = drogon::HttpRequest::newFileUploadRequest({drogon::UploadFile(thumbnailFilePath, "", "file")});
  req->setPath("/v1_1/" + cloud + "/image/upload");
  req->setMethod(drogon::Post);
  req->setParameter("api_key", apiKey);
  req->setParameter("folder", "thumbnails");
  req->setParameter("timestamp", timestamp);
  req->setParameter("signature", signature);
  req->setContentTypeCode(drogon::CT_MULTIPART_FORM_DATA);

  auto client = drogon::HttpClient::newHttpClient("https://api.cloudinary.com");
  
  try {
    drogon::HttpResponsePtr resp = co_await client->sendRequestCoro(req);

    if (resp && resp->getStatusCode() == 200) {
      auto json = resp->getJsonObject();
      if (json && json->isMember("secure_url")) {
        std::string url = (*json)["secure_url"].asString();
        LOG_DEBUG << "[extractThumbnail] Cloudinary upload successful, URL: "
                  << url;
        co_return url;
      }
    }

    // Any non-200 response or missing secure_url field falls through to here
    LOG_ERROR << "[extractThumbnail] Cloudinary upload failed for: "
              << thumbnailFilePath;
    if (resp) {
      LOG_ERROR << "[extractThumbnail] Status: " << resp->getStatusCode()
                << ", Response: " << std::string(resp->getBody());
    }
  } catch (const std::exception &e) {
    // sendRequestCoro throws on transport-level failures (DNS, timeout, TLS, etc.)
    LOG_ERROR << "[extractThumbnail] Cloudinary request threw an exception: "
              << e.what();
  }

  co_return "";
}

} // namespace gnp::services
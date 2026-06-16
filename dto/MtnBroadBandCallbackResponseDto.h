//
// Created by Emmanuel Addo-Odame on 16/06/2026.
//

#ifndef GNPAPI_MTNBROADBANDCALLBACKRESPONSEDTO_H
#define GNPAPI_MTNBROADBANDCALLBACKRESPONSEDTO_H

#include <json/json.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace gnp::dto {

    struct MtnRequestParamData {
        std::string name;
        std::string value;
    };

    class MtnRequestParam {
    public:
        void fromJson(const Json::Value &json) {
            if (json.isMember("planId") && !json["planId"].isNull()) {
                planId_ = json["planId"].asString();
            }
            if (json.isMember("command") && !json["command"].isNull()) {
                command_ = json["command"].asString();
            }
            if (json.isMember("data") && json["data"].isArray()) {
                for (const auto& item : json["data"]) {
                    MtnRequestParamData dataItem;
                    if (item.isMember("name") && !item["name"].isNull()) {
                        dataItem.name = item["name"].asString();
                    }
                    if (item.isMember("value") && !item["value"].isNull()) {
                        dataItem.value = item["value"].asString();
                    }
                    data_.push_back(dataItem);
                    
                    dataMap_[dataItem.name] = dataItem.value;
                }
            }
        }

        [[nodiscard]] const std::string& getPlanId() const { return planId_; }
        [[nodiscard]] const std::string& getCommand() const { return command_; }
        [[nodiscard]] const std::vector<MtnRequestParamData>& getData() const { return data_; }
        
        [[nodiscard]] std::string getDataValue(const std::string& name) const {
            auto it = dataMap_.find(name);
            if (it != dataMap_.end()) {
                return it->second;
            }
            return "";
        }

    private:
        std::string planId_;
        std::string command_;
        std::vector<MtnRequestParamData> data_;
        std::unordered_map<std::string, std::string> dataMap_;
    };

    class MtnBroadBandCallbackResponseDto {
    public:
        void fromJson(const Json::Value &json) {
            if (json.isMember("externalServiceId") && !json["externalServiceId"].isNull()) {
                externalServiceId_ = json["externalServiceId"].asString();
            }
            if (json.isMember("requestId") && !json["requestId"].isNull()) {
                requestId_ = json["requestId"].asString();
            }
            if (json.isMember("requestTimeStamp") && !json["requestTimeStamp"].isNull()) {
                requestTimeStamp_ = json["requestTimeStamp"].asString();
            }
            if (json.isMember("channel") && !json["channel"].isNull()) {
                channel_ = json["channel"].asString();
            }
            if (json.isMember("featureId") && !json["featureId"].isNull()) {
                featureId_ = json["featureId"].asString();
            }
            if (json.isMember("requestParam") && json["requestParam"].isObject()) {
                requestParam_.fromJson(json["requestParam"]);
            }
        }

        [[nodiscard]] const std::string& getExternalServiceId() const { return externalServiceId_; }
        [[nodiscard]] const std::string& getRequestId() const { return requestId_; }
        [[nodiscard]] const std::string& getRequestTimeStamp() const { return requestTimeStamp_; }
        [[nodiscard]] const std::string& getChannel() const { return channel_; }
        [[nodiscard]] const std::string& getFeatureId() const { return featureId_; }
        [[nodiscard]] const MtnRequestParam& getRequestParam() const { return requestParam_; }

    private:
        std::string externalServiceId_;
        std::string requestId_;
        std::string requestTimeStamp_;
        std::string channel_;
        std::string featureId_;
        MtnRequestParam requestParam_;
    };

}

#endif //GNPAPI_MTNBROADBANDCALLBACKRESPONSEDTO_H
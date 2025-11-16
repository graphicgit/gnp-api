//
// Created by Emmanuel Addo-Odame on 16/11/2025.
//

#ifndef SENDEMAILDTO_H
#define SENDEMAILDTO_H
namespace gnp::dto {

    class SendEmailDto {

    public:

        SendEmailDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getTo() const { return to_; }
        [[nodiscard]] const std::string& getSubject() const { return subject_; }
        [[nodiscard]] const std::string& getBody() const { return body_; }
        [[nodiscard]] const std::string& getHost() const { return host_; }
        [[nodiscard]] int getPort() const { return port_; }
        [[nodiscard]] bool getEnableSsl() const { return enableSsl_; }
        [[nodiscard]] const std::string& getUsername() const { return userName_; }
        [[nodiscard]] const std::string& getPassword() const { return password_; }
        [[nodiscard]] const std::string& getSenderName() const { return senderName_; }
        [[nodiscard]] bool getIsBodyHtml() const { return isBodyHtml_; }

        // Setters
        void setTo(const std::string& to) { to_ = to; }
        void setSubject(const std::string& subject) { subject_ = subject; }
        void setBody(const std::string& body) { body_ = body; }

    private:

        std::string to_;
        std::string subject_;
        std::string body_;
        std::string host_;
        int port_;
        bool enableSsl_;
        std::string userName_;
        std::string password_;
        std::string senderName_;
        bool isBodyHtml_;

    };


    inline void SendEmailDto::fromJson(const Json::Value& json) {

        if (json.isMember("To") && !json["To"].isNull()) {
            to_ = json["To"].asString();
        }

        if (json.isMember("Subject") && !json["Subject"].isNull()) {
            subject_ = json["Subject"].asString();
        }

        if (json.isMember("Body") && !json["Body"].isNull()) {
            body_ = json["Body"].asString();
        }

    }

}
#endif //SENDEMAILDTO_H

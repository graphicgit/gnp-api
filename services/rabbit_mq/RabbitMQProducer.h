//
// Created by Emmanuel Addo-Odame on 13/11/2025.
//

#ifndef RABBITMQPRODUCER_H
#define RABBITMQPRODUCER_H

#include <drogon/drogon.h>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <string>
#include <memory>

namespace gnp::services {

    class RabbitMQProducer {

        public:

            RabbitMQProducer(const std::string& url, const std::string& queue);
            ~RabbitMQProducer();

            bool connect();
            void disconnect();
            bool sendMessage(const std::string& message, const std::string& routingKey = "");
            bool sendJsonMessage(const Json::Value& json, const std::string& routingKey = "");

    private:
        std::string url_;
        std::string queue_;
        amqp_connection_state_t conn_;
        bool connected_;

        bool parseUrl(const std::string& url, std::string& host, int& port, std::string& vhost, std::string& user, std::string& password);
        void handleError(const std::string& context, int status);

    };


}
#endif //RABBITMQPRODUCER_H

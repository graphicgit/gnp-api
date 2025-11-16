//
// Created by Emmanuel Addo-Odame on 13/11/2025.
//

#ifndef RABBITMQPRODUCER_H
#define RABBITMQPRODUCER_H

#include <drogon/drogon.h>
#include <string>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/ssl_socket.h>

namespace gnp::services {

    class RabbitMqClient {
    public:
        explicit RabbitMqClient(const std::string &uri);

        bool produce(const std::string &exchange,
                     const std::string &queue,
                     const std::string &jsonMessage);

    private:
        std::string _uri;

        struct ConnectionParts {
            std::string host;
            int port;
            std::string vhost;
            std::string username;
            std::string password;
            bool ssl;
        };

        ConnectionParts parseUri(const std::string &uri);

        void dieOnError(int x, const std::string &msg);
    };


}
#endif //RABBITMQPRODUCER_H

//
// Created by Emmanuel Addo-Odame on 16/11/2025.
//

#include "RabbitMqClient.h"
#include <iostream>
#include <regex>
#include <cstring>

namespace gnp::services {

RabbitMqClient::RabbitMqClient(const std::string &uri)
    : _uri(uri) {}

// Parse AMQPS URI
RabbitMqClient::ConnectionParts RabbitMqClient::parseUri(const std::string &uri) {
    // Example: amqps://user:pass@host/vhost
    std::regex re(R"(amq(p|ps)://([^:]+):([^@]+)@([^/]+)/(.+))");
    std::smatch m;

    if (!std::regex_match(uri, m, re)) {
        throw std::runtime_error("Invalid RabbitMQ URI");
    }

    ConnectionParts p;
    p.ssl      = (m[1] == "ps");
    p.username = m[2];
    p.password = m[3];
    p.host     = m[4];
    p.vhost = "/" + m[5].str();
    p.port     = p.ssl ? 5671 : 5672;

    return p;
}

void RabbitMqClient::dieOnError(int x, const std::string &msg) {
    if (x < 0) {
        throw std::runtime_error(msg + " (amqp error)");
    }
}

bool RabbitMqClient::produce(const std::string &exchange,
                             const std::string &queue,
                             const std::string &jsonMessage)
{
    try {
        auto cfg = parseUri(_uri);

        amqp_connection_state_t conn = amqp_new_connection();

        amqp_socket_t *socket = nullptr;

        if (cfg.ssl) {
            socket = amqp_ssl_socket_new(conn);
            if (!socket) {
                throw std::runtime_error("Failed to create SSL socket");
            }
            amqp_ssl_socket_set_verify_peer(socket, 0);
            amqp_ssl_socket_set_verify_hostname(socket, 0);
        } else {
            socket = amqp_tcp_socket_new(conn);
            if (!socket) {
                throw std::runtime_error("Failed to create TCP socket");
            }
        }

        dieOnError(amqp_socket_open(socket, cfg.host.c_str(), cfg.port),
                   "Opening socket");

        // Login
        amqp_rpc_reply_t loginReply = amqp_login(
            conn,
            cfg.vhost.c_str(),
            0,
            131072,
            0,
            AMQP_SASL_METHOD_PLAIN,
            cfg.username.c_str(),
            cfg.password.c_str()
        );

        if (loginReply.reply_type != AMQP_RESPONSE_NORMAL) {
            throw std::runtime_error("RabbitMQ login failed");
        }

        amqp_channel_open(conn, 1);
        amqp_rpc_reply_t channelReply = amqp_get_rpc_reply(conn);
        if (channelReply.reply_type != AMQP_RESPONSE_NORMAL) {
            throw std::runtime_error("Opening channel failed");
        }

        // Queue declare
        amqp_queue_declare(conn, 1,
            amqp_cstring_bytes(queue.c_str()),
            1, 0, 0, 0,
            amqp_empty_table);

        if (amqp_get_rpc_reply(conn).reply_type != AMQP_RESPONSE_NORMAL) {
            throw std::runtime_error("Queue declare failed");
        }

        // Publish
        amqp_bytes_t messageBytes;
        messageBytes.len  = jsonMessage.size();
        messageBytes.bytes = (void*)jsonMessage.c_str();

        int status = amqp_basic_publish(
            conn,
            1,
            amqp_cstring_bytes(exchange.c_str()),
            amqp_cstring_bytes(queue.c_str()),
            0,
            0,
            NULL,
            messageBytes
        );

        if (status != AMQP_STATUS_OK) {
            throw std::runtime_error("Publish failed");
        }

        // Close cleanly
        amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(conn);

        std::cout << "published => " << jsonMessage << std::endl;
        return true;
    }
    catch (const std::exception &ex) {
        std::cerr << "RabbitMQ produce error: " << ex.what() << std::endl;
        return false;
    }
}

}


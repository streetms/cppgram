//
// Created by streetms on 13.02.23.
//

#ifndef CPPGRAM_BOT_H
#define CPPGRAM_BOT_H
#include <boost/asio/io_service.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <optional>

class Bot {
public:
    Bot (const Bot& bot) = delete;
    bool operator=(const  Bot& bot) = delete;
    explicit Bot(std::string token);
    ~Bot();
    void send_message(uint64_t chat_id, std::string_view text);
    void send_message(std::string_view chat_id, std::string_view text);
    std::optional<boost::property_tree::ptree> getUpdates();
private:
    boost::beast::http::response<boost::beast::http::string_body> get(std::string_view url);
    const std::string token;
    const char* port;
    boost::asio::io_service service;
    uint64_t last_update_id;
    boost::asio::ssl::context* ctx;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* socket;
};


#endif //CPPGRAM_BOT_H

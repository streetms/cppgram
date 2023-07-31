//
// Created by streetms on 13.02.23.
//
#include <iostream>
#include "Bot.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::asio;
using namespace boost::beast;
using namespace boost::property_tree;

Bot::Bot(const std::string&  token) : Bot() {
    this->token = token;
}

Bot::Bot(std::string &&token) : Bot() {
    this->token = std::move(token);
}
void Bot::send_text(uint64_t chat_id, std::string_view text) {
    std::string request(std::format("api.telegram.org/bot{}/sendMessage?chat_id={}&text={}",token,chat_id,text));
    get(request);
}

std::vector<Update> Bot::getUpdates() {
    std::string request;
    std::vector<Update> updates;
    if (last_update_id  == 0) {
        request = std::format("api.telegram.org/bot{}/getUpdates",token);
    } else {
        request = std::format("api.telegram.org/bot{}/getUpdates?offset={}",token,last_update_id+1);
    }
    std::cout <<request << "\n";
    ptree json;
    std::stringstream st(get(request).body());
    read_json(st,json);
    auto children = json.get_child("result");
    if (children.empty()) {
        return updates;
    }
    if (last_update_id == 0) {
        last_update_id = children.back().second.get<uint64_t>("update_id");
        return updates;
    }
    last_update_id = children.back().second.get<uint64_t>("update_id");

    updates.reserve(children.size());
    for (auto& [text,tree] : children){
        updates.emplace_back(tree);
    }
    return updates;
}

boost::beast::http::response<boost::beast::http::string_body> Bot::get(std::string_view url) {
    size_t path_pos = url.find('/');
    std::string host(url.begin(),url.begin()+path_pos);
    std::string path(url.begin()+path_pos,url.end());
    http::request<http::string_body> req{ http::verb::get, path, 11 };
    req.set(http::field::host, host);
    http::write(*socket, req);
    http::response<http::string_body> res;
    flat_buffer buffer;
    http::read(*socket, buffer, res);
    return res;
}

Bot::~Bot() {
    delete ctx;
    delete socket;
}

Bot::Bot() {
    last_update_id = 0;
    ctx = new ssl::context(ssl::context::sslv23_client);
    socket = new ssl::stream<ip::tcp::socket>(service,*ctx);
    boost::asio::ip::tcp::resolver resolver(service);
    auto it = resolver.resolve("api.telegram.org", "https");
    connect(socket->lowest_layer(), it);
    socket->handshake(ssl::stream_base::handshake_type::client);
}

void Bot::send_message(uint64_t chat_id,const Message& message) {
    switch (message.type) {
        case Message::Type::Text:
            this->send_text(chat_id,message.get<Text>());
            break;
        case Message::Type::Photo:
            this->send_photo(chat_id,message.get<Photo>());
            break;
    }
}

void Bot::send_photo(uint64_t chat_id, const Photo &photo) {
    std::string request(std::format("api.telegram.org/bot{}/sendPhoto?chat_id={}&photo={}",token,chat_id,photo.file_id));
    get(request);
}


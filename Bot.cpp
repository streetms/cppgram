//
// Created by streetms on 13.02.23.
//

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

Bot::Bot(std::string  token) : token(std::move(token)) {
    last_update_id = 0;
    port = "443";
    ctx = new ssl::context(ssl::context::sslv23_client);
    socket = new ssl::stream<ip::tcp::socket>(service,*ctx);
    boost::asio::ip::tcp::resolver resolver(service);
    auto it = resolver.resolve("api.telegram.org", port);
    connect(socket->lowest_layer(), it);
    socket->handshake(ssl::stream_base::handshake_type::client);
}

void Bot::send_message(uint64_t chat_id, std::string_view text) {
    char request[4096];
    sprintf(request,"api.telegram.org/bot%s/sendMessage?chat_id=%lu&text=%s",token.data(),chat_id,text.data());
    get(request);
}

void Bot::send_message(std::string_view chat_id, std::string_view text) {
    char request[4096];
    sprintf(request,"api.telegram.org/bot%s/sendMessage?chat_id=@%s&text=%s",token.data(),chat_id.data(),text.data());
    get(request);
}

std::optional<boost::property_tree::ptree> Bot::getUpdates() {
    char request[4096];
    if (last_update_id  == 0) {
        sprintf(request,"api.telegram.org/bot%s/getUpdates",token.data());
    } else {
        sprintf(request,"api.telegram.org/bot%s/getUpdates?offset=%lu",token.data(),last_update_id+1);
    }

    ptree json;
    std::stringstream st(get(request).body());
    read_json(st,json);
    if (json.get_child("result").empty()) {
        return {};
    }
    if (last_update_id == 0) {
        last_update_id = json.get_child("result").back().second.get<uint64_t>("update_id");
        return {};
    }
    last_update_id = json.get_child("result").back().second.get<uint64_t>("update_id");
    return json;
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

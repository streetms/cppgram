//
// Created by konstantin on 09.03.23.
//

#ifndef CPPGRAM_MESSAGE_H
#define CPPGRAM_MESSAGE_H
#include <cstdint>
#include <string>
#include <any>
#include <optional>
#include <boost/property_tree/ptree.hpp>
using Text=std::string;
struct Message{
private:
    uint64_t message_id;
    struct From{
        uint64_t id;
        bool is_bot;
        std::string first_name;
        std::string username;
        std::string language_code;
    };
    struct Chat{
        uint64_t id;
        std::string first_name;
        std::string username;
        std::string type;
    };

    std::any data;
public:
    struct Entities{
        uint64_t offset;
        uint64_t length;
        enum class Type {bot_command};
        Type type;
    };
    Message(const Message& other) = delete;
    Message& operator=(const Message& other) = delete;
    enum class Type{Text};
    Message(const boost::property_tree::ptree& json);
    template <typename T>
    T get(){
        return std::any_cast<T>(data);
    }
    uint64_t date;
    std::optional<Entities> entities;
    Type type;
    From from;
    Chat chat;
};
#endif //CPPGRAM_MESSAGE_H

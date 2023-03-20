//
// Created by konstantin on 09.03.23.
//
#include "Message.h"
#include "Photo.h"
Message::Message(const boost::property_tree::ptree &json) {
    this->date = json.get<uint64_t>("date");
    this->message_id = json.get<uint64_t>("message_id");

    auto child = json.get_child("from");
    from.id = child.get<uint64_t>("id");
    from.first_name = child.get<std::string>("first_name");
    from.is_bot = child.get<bool>("is_bot");
    from.username = child.get<std::string>("username");
    from.language_code = child.get<std::string>("language_code");

    child = json.get_child("chat");
    chat.id =  child.get<uint64_t>("id");
    chat.username = child.get<std::string>("username");
    chat.first_name = child.get<std::string>("first_name");
    try{
        data = json.get<std::string>("text");
        type = Type::Text;
    } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>& ex) {
        try {
            Photo photo;

            auto t = json.get_child("photo");
            photo.file_id = t.front().second.get<std::string>("file_id");
            for (Photo::PhotoSize Size;auto& el : json.get_child("photo")){
                Size.file_unique_id = el.second.get<std::string>("file_unique_id");
                Size.width = el.second.get<uint64_t>("width");
                Size.height =  el.second.get<uint64_t>("height");
                Size.file_size =  el.second.get<uint64_t>("file_size");
                photo.array.push_back(std::move(Size));
            }
            type = Type::Photo;
            data = std::move(photo);
        } catch (...){
            throw std::runtime_error("unknown type message");
        }
    }
    try {
        child = json.get_child("entities");
        Entities ent{};
        ent.offset = child.front().second.get<uint64_t>("offset");
        ent.length = child.front().second.get<uint64_t>("length");
        auto t = child.front().second.get<std::string>("type");
        if (t == "bot_command"){
           ent.type = Entities::Type::bot_command;
        }
        entities = ent;
    } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>& ex) {
        entities = {};
    }
}
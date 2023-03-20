#include <fstream>
#include <iostream>
#include <queue>
#include <ranges>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include "Bot.h"
#include "Update/Message/Message.h"

std::string_view available_command[] = {"/start"};

int main(){
    std::ifstream fin(".config");
    std::string token;
    uint64_t admin_id;
    try {
        fin >> token;
        fin >> admin_id;
    } catch(...) {
        std::cout << "не найден .config файл";
    }
    Bot bot(std::move(token));
    boost::bimap<boost::bimaps::unordered_set_of<uint64_t>, boost::bimaps::unordered_set_of<uint64_t>> chat;
    std::queue<uint64_t> queue;
    while (true) {
        try {
            auto updates = bot.getUpdates();
            for (auto &update: updates) {
                auto message = update.get<Message>();
                uint64_t id = message.from.id;
                if (message.entities) {
                    switch (message.entities->type) {
                        case Message::Entities::Type::bot_command:
                            if (std::ranges::find(available_command, message.get<Text>()) ==
                            std::end(available_command)) {
                                bot.send_text(id, "неизвестная команда для бота");
                                continue;
                            }
                            if (message.get<Text>() == "/start") {
                                if (chat.right.find(id) == chat.right.end() &&
                                chat.left.find(id) == chat.left.end()) {
                                    if (queue.empty()) {
                                        queue.emplace(id);
                                        bot.send_text(id, "поиск собеседника");
                                    } else {
                                        chat.insert({id, queue.back()});
                                        bot.send_text(queue.back(), "собеседник найден");
                                        bot.send_text(id, "собеседник найден");
                                        queue.pop();
                                    }
                                } else {
                                    bot.send_text(id, "вы уже находитесь в диалоге");
                                }
                            }
                            break;
                    }
                } else {
                    auto it1 = chat.right.find(id);
                    auto it2 = chat.left.find(id);
                    if (it1 != chat.right.end() || it2 != chat.left.end()) {
                        uint64_t target_id = (it1 != chat.right.end()) ? it1->second : it2->second;
                        bot.send_message(target_id, message);
                    } else {
                        bot.send_text(id, "вы не в диалоге. Введите команду /start для поиска собеседника");
                    }
                }
            }
        } catch(std::exception& ex){
            bot.send_text(admin_id,ex.what());
        }
    }
}
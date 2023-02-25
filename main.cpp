#include "Bot.h"

int main() {
    Bot bot(getenv("BOT_TOKEN"));

    auto json = bot.getUpdates();
    while(true) {
        if (json.has_value()) {
            for (auto update: json->get_child("result")) {
                auto chat_id = update.second.get_child("message").get_child("from").get<uint64_t>("id");
                auto text = update.second.get_child("message").get<std::string>("text");
                bot.send_message(chat_id, text);
            }
        }
        json = bot.getUpdates();
    }

}
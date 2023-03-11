#include <fstream>
#include <iostream>
#include "Bot.h"
#include "Update/Message/Message.h"

int main() {
    std::ifstream fin(".bot_token");
    std::string token;
    fin >> token;
    Bot bot(std::move(token));

    while(true) {
        auto updates = bot.getUpdates();
        for (auto& update : updates) {
            Message m = update.get<Message>();
            if (m.type == Message::Type::Text) {
                bot.send_message(m.from.id,m.get<std::string>());
            }
        }
    }

}
#include <fstream>






#include <iostream>
#include <queue>







#include <ranges>





#include <boost/graph/adjacency_list.hpp>
#include "Bot.h"
#include "Update/Message/Message.h"

constexpr std::string_view available_command[] = {"/start","/new","/stop"};
namespace rng = std::ranges;
typedef uint64_t vertex_t;
typedef boost::adjacency_list<boost::hash_setS,boost::hash_setS,boost::undirectedS,vertex_t> graph_t;
typedef typename boost::graph_traits<graph_t>::vertex_iterator vertex_it;
typedef typename boost::graph_traits<graph_t>::vertex_descriptor vertex_desc;

std::queue<uint64_t> queue;

std::optional<vertex_desc> find_vertex(const graph_t& graph, vertex_t name){
    auto [begin,end] = boost::vertices(graph);
    std::optional<vertex_desc> res = {};
    for (auto it = begin; it != end; it++){
        vertex_desc desc = *it;
        if (boost::get(boost::vertex_bundle,graph)[desc] == name){
            res = desc;
            return res;
        }
    }
    return res;
}

vertex_t get_adjacent_name(graph_t& graph, vertex_desc& desc){
    auto neighbour = boost::adjacent_vertices(desc,graph).first;
    return boost::get(boost::vertex_bundle,graph)[*neighbour];
}


void start(graph_t& graph,Bot& bot, uint64_t id){
    if (not find_vertex(graph,id).has_value()) {
        if (queue.empty()) {
            queue.emplace(id);
            bot.send_text(id, "поиск собеседника");
        } else {
            if (id != queue.back()) {
                vertex_desc id1 = boost::add_vertex(id,graph);
                vertex_desc id2 = boost::add_vertex(queue.back(),graph);
                boost::add_edge(id1,id2,graph);
                bot.send_text(queue.back(), "собеседник найден");
                bot.send_text(id, "собеседник найден");
                queue.pop();
            }
        }
    } else {
        bot.send_text(id, "вы уже находитесь в диалоге");
    }
}

void stop(graph_t& graph,Bot& bot,uint64_t id) {
    std::optional<vertex_desc> desc =  find_vertex(graph,id);
    if (not desc.has_value()){
        return ;
    }
    vertex_desc neighbour = *boost::adjacent_vertices(*desc,graph).first;
    vertex_t companion_id = boost::get(boost::vertex_bundle,graph)[neighbour];
    bot.send_text(id,"диалог завершен");
    bot.send_text(companion_id,"диалог завершен");
    boost::remove_edge(*desc,neighbour,graph);
    boost::remove_vertex(neighbour,graph);
    boost::remove_vertex(*desc,graph);
}

void new_(graph_t& graph,Bot& bot,uint64_t id){
    stop(graph,bot,id);
    start(graph,bot,id);
}
void execute_command(Bot& bot, const Message& message, graph_t& graph){
    auto text = message.get<Text>();
    uint64_t id = message.from.id;
    if (rng::find(available_command,text) != std::end(available_command)) {
        if (text == "/start") {
            start(graph,bot,id);
        }
        if (text == "/stop") {
            stop(graph,bot,id);
        }
        if (text == "/new"){
            new_(graph,bot,id);
        }
    } else{
        bot.send_text(id,"неизвестная команда");
    }
}

int main(){
    std::ifstream fin(".config");
    std::string token;
    uint64_t admin_id;
    if (fin.is_open()) {
        fin >> token;
        fin >> admin_id;
    } else {
        std::cout << "cannot open .config";
        std::exit(1);
    }
    Bot bot(std::move(token));
    graph_t graph;
    while (true) {
        auto updates = bot.getUpdates();
        for (auto &update: updates) {
            auto message = update.get<Message>();
            uint64_t id = message.from.id;
            if (message.entities.has_value()) {
                switch (message.entities->type) {
                    case Message::Entities::Type::bot_command:
                        execute_command(bot,message,graph);
                        break;
                }
            } else {
                std::optional<vertex_desc> desc = find_vertex(graph,id);
                if (desc.has_value()) {
                    bot.send_message(get_adjacent_name(graph, *desc), message);
                } else {
                    bot.send_text(id, "вы не в диалоге. Введите команду /start для поиска собеседника");
                }
            }
        }
    }
}

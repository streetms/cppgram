//
// Created by konstantin on 09.03.23.
//

#ifndef CPPGRAM_UPDATE_H
#define CPPGRAM_UPDATE_H

#include "boost/property_tree/ptree.hpp"

class Update {
public:
    explicit Update(const boost::property_tree::ptree& tree){
        json = tree;
    }
    template <typename T>
    T get(std::string_view name){
        return (json.get_child(name.data()));
    }
private:
    boost::property_tree::ptree json;
};


#endif //CPPGRAM_UPDATE_H

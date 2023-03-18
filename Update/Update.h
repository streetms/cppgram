//
// Created by konstantin on 09.03.23.
//

#ifndef CPPGRAM_UPDATE_H
#define CPPGRAM_UPDATE_H

#include "boost/property_tree/ptree.hpp"
#include <boost/type_index.hpp>
class Update {
public:
    explicit Update(const boost::property_tree::ptree& tree) : json(tree){
    }
    template <typename T>
    T get() {
        std::string type_name = boost::typeindex::type_id<T>().pretty_name();
        type_name[0] = char(std::tolower(type_name[0]));
        return json.get_child(type_name);
    }
private:
    boost::property_tree::ptree json;
};


#endif //CPPGRAM_UPDATE_H

//
// Created by konstantin on 09.03.23.
//

#ifndef CPPGRAM_PHOTO_H
#define CPPGRAM_PHOTO_H
#include <string>
#include <boost/container/static_vector.hpp>
struct Photo{
public:
    struct PhotoSize{
        std::string file_unique_id;
        uint64_t file_size;
        uint64_t width;
        uint64_t height;
    };
    std::string file_id;
    boost::container::static_vector<PhotoSize,4> array;
};
#endif //CPPGRAM_PHOTO_H

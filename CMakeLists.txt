cmake_minimum_required(VERSION 3.24)
project(Bot)
find_package(OpenSSL REQUIRED)
set(CMAKE_CXX_STANDARD 23)
set(SOURCE  Bot.cpp Update/Message/Message.cpp)
add_compile_options(-Wall -Wextra -Werror -Wconversion)
add_executable(Bot main.cpp ${SOURCE} Update/Update.h Update/Message/Message.h Update/Message/Photo.h)
target_link_libraries(Bot OpenSSL::SSL)

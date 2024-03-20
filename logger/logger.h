//
// Created by Twelvee on 05.09.2023.
//

#include <iostream>
#include <memory>
#ifndef RESOURCEMOD_LOGGER_H
#define RESOURCEMOD_LOGGER_H

#include <string>
#include <stdexcept>
#include <fstream>

const std::string LOG_PREFIX = "ResourceMod | ";
enum LOG_LEVEL {
    LEVEL_DEBUG, LEVEL_INFO, LEVEL_ERROR
};

class logger {
public:
    static void fatal() {
        std::cout<<LOG_PREFIX<<" FATAL ERROR" <<std::endl;
        std::cout<<LOG_PREFIX<<" FATAL ERROR" <<std::endl;
        std::cout<<LOG_PREFIX<<" FATAL ERROR" <<std::endl;
    }
    static void log(std::string message, LOG_LEVEL level = LEVEL_INFO) {
        std::cout << LOG_PREFIX << message << std::endl;
    }

    template<typename ... Args>
    static std::string format(const std::string &format, Args ...args) {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

    static void fileLog(std::string message) {
        std::ofstream myfile;
        myfile.open("resourcemod.log.txt", std::ios_base::app);
        myfile << message << std::endl;
        myfile.close();
    }
};


#endif //RESOURCEMOD_LOGGER_H

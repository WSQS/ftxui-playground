//
// Created by sophomore on 12/28/24.
//

#ifndef FILESYSTEM_WRAPPER_H
#define FILESYSTEM_WRAPPER_H

#include "boost/asio.hpp"
#include "boost/process/v2.hpp"
namespace bp = boost::process;
namespace asio = boost::asio;
using boost::system::error_code;

namespace filesystem {
    namespace command {
        inline auto execute_once(const std::string &command) {
            asio::io_context io_context;
            asio::readable_pipe proc(io_context);
            FILE *pipe = popen(command.c_str(), "r");
            proc.assign(fileno(pipe));
            std::string result{};
            while (true) {
                error_code ec{};
                std::string line{};
                read_until(proc, asio::dynamic_buffer(line), '\n', ec);
                if (ec == asio::error::eof) {
                    break;
                }
                result += line;
                line.clear();
            }
            pclose(pipe);
            return result;
        }

        inline auto get_directory_content(std::string path) {
            std::istringstream iss{execute_once("ls " + path)};
            std::vector<std::string> result;
            std::string line;
            while (iss >> line) {
                result.emplace_back(std::move(line));
            }
            return result;
        }
    }

    namespace stander {
    }
}
#endif //FILESYSTEM_WRAPPER_H

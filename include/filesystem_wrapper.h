//
// Created by sophomore on 12/28/24.
//

#ifndef FILESYSTEM_WRAPPER_H
#define FILESYSTEM_WRAPPER_H

#include "boost/asio.hpp"
#include "boost/process/v2.hpp"
#include <cstdlib>
#include <filesystem>
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
        if (ec == asio::error::eof)
            break;
        result += line;
        line.clear();
    }
    pclose(pipe);
    return result;
}

inline auto get_directory_content(const std::string &path) {
    std::istringstream iss{execute_once("ls -a " + path)};
    std::vector<std::string> result;
    std::string line;
    while (iss >> line) {
        if (line == ".")
            continue;
        result.emplace_back(std::move(line));
    }
    return result;
}
inline auto exists(const std::string &path) { return std::system(("test -e " + path).c_str()) == 0; }
} // namespace command

namespace stander {
inline auto check_parent_sign(const std::string &file_path) {
    std::vector<std::string> result{};
    if (std::filesystem::path(file_path).root_directory() != file_path)
        result.emplace_back("..");
    return result;
}

inline auto get_directory_content(const std::string &file_path) {
    std::vector<std::string> result = {".."};
    for (const auto &entry : std::filesystem::directory_iterator(file_path))
        result.emplace_back(entry.path().filename().string());
    return result;
};

inline auto exists(const std::string &path) { return std::filesystem::exists(path); }

} // namespace stander
} // namespace filesystem
#endif // FILESYSTEM_WRAPPER_H

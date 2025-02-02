//
// Created by sophomore on 12/28/24.
//

#ifndef FILESYSTEM_WRAPPER_H
#define FILESYSTEM_WRAPPER_H

#include <algorithm> // transform
#include <cstdlib>
#include <cstring> // strerror
#include <filesystem>
#include <iostream>
#include <vector> // vector

#include <sys/types.h>
#include <sys/wait.h> // wait
#include <unistd.h>   // fork, dup, execvp

// namespace bp = boost::process;
// namespace asio = boost::asio;
// using boost::system::error_code;
using uint8 = uint_fast8_t;
using int32 = int_fast32_t;
namespace filesystem {
namespace command {
class pipe_wrapper {
    constexpr static auto READ_END = 0;
    constexpr static auto WRITE_END = 1;
    enum class pipe_statues : uint_fast8_t { unavailable, available, read, write } statues{};
    int pipe_des[2]{};

public:
    pipe_wrapper() {
        if (pipe(pipe_des)) {
            std::cerr << "ERROR: pipe failed: " << strerror(errno) << std::endl;
            statues = pipe_statues::unavailable;
        } else {
            statues = pipe_statues::available;
        }
    }
    ~pipe_wrapper() {
        switch (statues) {
        case pipe_statues::available:
            close(pipe_des[READ_END]);
            close(pipe_des[WRITE_END]);
            break;
        case pipe_statues::read:
            close(pipe_des[READ_END]);
            break;
        case pipe_statues::write:
            close(pipe_des[WRITE_END]);
            break;
        default:
            break;
        }
    }
    bool valid() const { return statues != pipe_statues::unavailable; }
    auto read_end() {
        if (statues == pipe_statues::available) {
            statues = pipe_statues::read;
            close(pipe_des[WRITE_END]);
        }
        if (statues == pipe_statues::read)
            return pipe_des[READ_END];
        return -1;
    }
    auto write_end() {
        if (statues == pipe_statues::available) {
            statues = pipe_statues::write;
            close(pipe_des[READ_END]);
        }
        if (statues == pipe_statues::write)
            return pipe_des[WRITE_END];
        return -1;
    }
};

inline auto execute_once(std::vector<std::string> input_args) -> std::string {
    constexpr auto buffer_size = 1024;
    pipe_wrapper pipe_des{};
    if (!pipe_des.valid())
        return {};
    auto pid = fork();
    if (!pid) {
        if (!dup2(pipe_des.write_end(), STDOUT_FILENO)) {
            std::cerr << "ERROR: dup2 failed:  " << strerror(errno) << std::endl;
            return {};
        }
        // Build argv for execvp
        std::vector<char *> args;
        std::transform(input_args.begin(), input_args.end(), back_inserter(args),
            [](std::string &s) { return s.data(); });
        args.push_back(nullptr);
        if (execvp(args.data()[0], args.data()) != 0) {
            std::cerr << "ERROR: execvp failed: " << strerror(errno) << std::endl;
            return {};
        }
    }
    wait(nullptr);
    std::string result;
    while (true) {
        char buffer[buffer_size];
        const auto size = read(pipe_des.read_end(), buffer, sizeof(char) * buffer_size);
        if (size < 0) {
            std::cerr << "ERROR: read failed: " << strerror(errno) << std::endl;
            return result;
        }
        if (size == 0)
            break;
        result.append(buffer, size);
    }
    return result;
}

// #include "boost/asio.hpp"
// #include "boost/process/v2.hpp"
// inline auto execute_once(const std::string &command) {
//     asio::io_context io_context;
//     asio::readable_pipe proc(io_context);
//     FILE *pipe = popen(command.c_str(), "r");
//     proc.assign(fileno(pipe));
//     std::string result{};
//     while (true) {
//         error_code ec{};
//         std::string line{};
//         read_until(proc, asio::dynamic_buffer(line), '\n', ec);
//         if (ec == asio::error::eof)
//             break;
//         result += line;
//         line.clear();
//     }
//     pclose(pipe);
//     return result;
// }

inline auto get_directory_content(const std::string &path) {
    std::istringstream iss{execute_once({"ls", "-a", path})};
    std::vector<std::string> result;
    std::string line;
    while (iss >> line) {
        if (line == ".")
            continue;
        if (line.empty())
            continue;
        result.emplace_back(std::move(line));
    }
    return result;
}

inline auto exists(const std::string &path) {
    return std::system(("test -e " + path).c_str()) == 0;
}

inline auto get_parent_directory(std::string &file_path) {
    file_path = execute_once({"realpath", execute_once({"dirname", file_path})});
    // remove the last character
    size_t pos = file_path.find('\n');
    if (pos != std::string::npos)
        file_path.erase(pos);
}

inline auto get_file_name(const std::string &file_path) {
    auto result = execute_once({"basename", file_path});
    // remove the last character
    size_t pos = result.find('\n');
    if (pos != std::string::npos)
        result.erase(pos);
    return result;
}

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
}

inline auto exists(const std::string &path) { return std::filesystem::exists(path); }

inline auto get_parent_directory(std::string &file_path) {
    std::filesystem::path temp_directory{file_path};
    file_path = temp_directory.append("..").lexically_normal().string();
}

inline auto get_file_name(const std::string &file_path) {
    return std::filesystem::path(file_path).filename().string();
}

} // namespace stander
} // namespace filesystem
#endif // FILESYSTEM_WRAPPER_H

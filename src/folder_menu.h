//
// Created by sophomore on 12/15/24.
//

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H
#include <vector>
#include <filesystem>
#include <thread>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
using namespace ftxui;

namespace playground {
    struct menu_data {
        Ref<std::vector<std::string>> entries{};
        std::shared_ptr<int> selected{};
        MenuOption option{};
        Component instantiate(){
            return Menu(&*entries,selected.get(),option);
        }
    };

    struct input_data{
        std::string content{};
        InputOption option{};
        Component instantiate() {
            return Input(content,option);
        }
    };

    struct path_data{
        input_data input{};
        menu_data menu{};
        std::string log{};
        int selected = 1;
        Components instantiate() {
            return {input.instantiate(),menu.instantiate()};
        }
    };
    inline auto check_parent_sign(path_data &input_path_data) {
        if (std::filesystem::path(input_path_data.input.content).root_directory() == input_path_data.input.content)
            (*input_path_data.menu.entries).clear();
        else
            *input_path_data.menu.entries = {".."};
    }
    inline auto handle_path_existence(path_data &input_path_data) {
        if (!std::filesystem::exists(input_path_data.input.content)) {
            input_path_data.input.content = "/";
            input_path_data.log = "Unavailable path";
        }
    }

    inline auto get_directory_content(path_data &input_path_data) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data.input.content)) {
            (*input_path_data.menu.entries).push_back(entry.path().filename().string());
        }
    };
    inline auto get_parent_directory(path_data &input_path_data) {
        std::filesystem::path temp_directory{input_path_data.input.content};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data.input.content = temp_directory.string();
    };
    inline auto run_command(path_data &input_path_data) {
        std::thread commandThread{
            [command = std::string("code ") + input_path_data.input.content]() {
                return std::system(command.c_str());
            }
        };
        commandThread.detach();
    };
    inline auto handel_file(path_data &input_path_data) {
        run_command(input_path_data);
        get_parent_directory(input_path_data);
        get_directory_content(input_path_data);
    };
    inline auto handel_file_type(path_data &input_path_data) {
        std::filesystem::path directory{input_path_data.input.content};
        switch (status(directory).type()) {
            case std::filesystem::file_type::directory:
                get_directory_content(input_path_data);
            break;
            case std::filesystem::file_type::regular:
                handel_file(input_path_data);
            break;
            default:
                input_path_data.log = "Unsupported file type";
        }
    };
    inline auto build_log(const path_data &input_path_data) {
        if (input_path_data.log.size() != 0)
            return text(input_path_data.log) | border;
        else
            return std::make_shared<Node>();
    };
    Component folder_menu();

    Component folder_menu(path_data data);
}


#endif //FOLDER_MENU_H

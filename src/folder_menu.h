//
// Created by sophomore on 12/15/24.
//

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H
#include <filesystem>
#include <thread>
#include <vector>

#include "enhanced_menu.h"
#include "menu_util.h"
#include "multiselect_menu.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

namespace playground {
    struct menu_data {
        std::vector<reference<std::string> > entries{};
        std::shared_ptr<int> selected{};
    };

    struct path_data {
        std::string file_path;
        menu_data menu{};
        int selected = 1;
    };

    inline auto check_parent_sign(const std::shared_ptr<path_data> &input_path_data) {
        if (std::filesystem::path(input_path_data->file_path).root_directory() != input_path_data->
            file_path)
            input_path_data->menu.entries.emplace_back("..");
    }

    inline auto handle_path_existence(std::string &file_path) {
        if (!std::filesystem::exists(file_path)) {
            log("Unavailable path:" + file_path);
            file_path = "/";
        }
    }

    inline auto get_directory_content(const std::shared_ptr<path_data> &input_path_data) {
        input_path_data->menu.entries.clear();
        check_parent_sign(input_path_data);
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data->file_path)) {
            input_path_data->menu.entries.emplace_back(entry.path().filename().string());
        }
    };

    inline auto get_parent_directory(const std::shared_ptr<path_data> &input_path_data) {
        std::filesystem::path temp_directory{input_path_data->file_path};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data->file_path = temp_directory.string();
    };

    inline auto run_command(const std::shared_ptr<path_data> &input_path_data) {
        std::thread commandThread{
            [command = std::string("code ") + input_path_data->file_path]() {
                return std::system(command.c_str());
            }
        };
        commandThread.detach();
    };

    inline auto handel_file(const std::shared_ptr<path_data> &input_path_data) {
        run_command(input_path_data);
        get_parent_directory(input_path_data);
        get_directory_content(input_path_data);
    };

    inline auto handel_file_type(const std::shared_ptr<path_data> &input_path_data) {
        std::filesystem::path directory{input_path_data->file_path};
        switch (status(directory).type()) {
            case std::filesystem::file_type::directory:
                get_directory_content(input_path_data);
                break;
            case std::filesystem::file_type::regular:
                handel_file(input_path_data);
                break;
            default:
                log("Unsupported file type");
        }
    };

    inline auto build_log(const std::string &log) {
        if (log.size() != 0)
            return text(log) | border;
        else
            return std::make_shared<Node>();
    };

    inline auto get_menu(const std::shared_ptr<path_data> &input_path_data) {
        multiselect_menu_option menu_option{multiselect_menu_option::Vertical()};
        // handel menu enter
        menu_option.on_enter = [input_data=input_path_data]() {
            handle_path_existence(input_data->file_path);
            std::filesystem::path directory{input_data->file_path};
            directory = directory.append(input_data->menu.entries[*input_data->menu.selected]()).
                    lexically_normal();
            input_data->file_path = directory.string();
            handel_file_type(input_data);
        };
        return multiselect_menu(&input_path_data->menu.entries, input_path_data->menu.selected.get(),
                                menu_option);
    }

    inline auto input_transform(InputState state) {
        state.element |= color(Color::White);
        if (state.is_placeholder) {
            state.element |= dim;
        }
        if (state.focused) {
            state.element |= inverted;
        } else {
            state.element |= hcenter;
        }
        if (state.hovered) {
            state.element |= bgcolor(Color::GrayDark);
        }
        return state.element;
    };

    inline auto get_input(const std::shared_ptr<path_data> &input_path_data) {
        InputOption input_option{};
        input_option.multiline = false;
        input_option.on_enter = [input_data=input_path_data]() mutable {
            handle_path_existence(input_data->file_path);
            std::filesystem::path directory{input_data->file_path};
            handel_file_type(input_data);
        };
        input_option.transform = input_transform;
        return Input(&input_path_data->file_path, input_option);
    }

    inline auto handle_input(const Event &event) {
        if (event.is_character()) {
            switch (event.character()[0]) {
                case 'q':
                    get_screen().Exit();
                    return true;
                case 'c':
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

    inline auto FileMenu(std::shared_ptr<path_data> &input_data) {
        get_directory_content(input_data);
        auto menu = get_menu(input_data);
        auto input = get_input(input_data);
        auto container = Container::Vertical({input, menu}) | CatchEvent(handle_input);
        return Renderer(container, [input,menu,input_data] {
            return vbox({
                       input->Render(),
                       separator(),
                       menu->Render() | yframe | yflex, // the flex is necessary for log to display
                       // build_log(input_data)
                   }) | flex | border;
        });
    }

    inline auto build_tab_value(std::vector<std::shared_ptr<path_data> > &path_datas) {
        std::vector<reference<std::string>> data;
        for (const auto &path_data: path_datas) {
            data.emplace_back(&path_data->file_path);
        }
        return data;
    }

    inline auto add_folder_menu(std::vector<std::shared_ptr<path_data> > &path_datas, Component &tab_container,
                                int index) {
        std::shared_ptr<path_data> input_data;
        if (path_datas.empty())
            input_data = Make<path_data>(path_data{
                "/home", {{}, {Make<int>()}}
            });
        else
            input_data = Make<path_data>(path_data{
                path_datas[index]->file_path, {{}, {Make<int>()}}
            });
        path_datas.push_back(input_data);
        tab_container->Add(FileMenu(input_data));
    }

    inline auto remove_folder_menu(std::vector<std::shared_ptr<path_data> > &path_datas, Component &tab_container,
                                   int index) {
        if (path_datas.empty()) {
            log("There is not tab anymore");
            return;
        }
        path_datas.erase(path_datas.begin() + index);
        tab_container->ChildAt(index)->Detach();
    }
}


#endif //FOLDER_MENU_H

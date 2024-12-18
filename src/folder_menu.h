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

namespace playground
{
    struct menu_data
    {
        Ref<std::vector<std::string>> entries{};
        std::shared_ptr<int> selected{};
    };

    struct path_data
    {
        std::string file_path;
        menu_data menu{};
        std::string log{};
        int selected = 1;
    };



    inline auto check_parent_sign(const std::shared_ptr<path_data>& input_path_data)
    {
        if (std::filesystem::path(input_path_data->file_path).root_directory() == input_path_data->
            file_path)
            input_path_data->menu.entries->clear();
        else
            *input_path_data->menu.entries = {".."};
    }

    inline auto handle_path_existence(const std::shared_ptr<path_data>& input_path_data)
    {
        if (!std::filesystem::exists(input_path_data->file_path))
        {
            input_path_data->file_path = "/";
            input_path_data->log = "Unavailable path";
        }
    }

    inline auto get_directory_content(const std::shared_ptr<path_data>& input_path_data)
    {
        for (const auto& entry : std::filesystem::directory_iterator(input_path_data->file_path))
        {
            input_path_data->menu.entries->push_back(entry.path().filename().string());
        }
    };

    inline auto get_parent_directory(const std::shared_ptr<path_data>& input_path_data)
    {
        std::filesystem::path temp_directory{input_path_data->file_path};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data->file_path = temp_directory.string();
    };

    inline auto run_command(const std::shared_ptr<path_data>& input_path_data)
    {
        std::thread commandThread{
            [command = std::string("code ") + input_path_data->file_path]()
            {
                return std::system(command.c_str());
            }
        };
        commandThread.detach();
    };

    inline auto handel_file(const std::shared_ptr<path_data>& input_path_data)
    {
        run_command(input_path_data);
        get_parent_directory(input_path_data);
        get_directory_content(input_path_data);
    };

    inline auto handel_file_type(const std::shared_ptr<path_data>& input_path_data)
    {
        std::filesystem::path directory{input_path_data->file_path};
        switch (status(directory).type())
        {
        case std::filesystem::file_type::directory:
            get_directory_content(input_path_data);
            break;
        case std::filesystem::file_type::regular:
            handel_file(input_path_data);
            break;
        default:
            input_path_data->log = "Unsupported file type";
        }
    };

    inline auto build_log(const std::shared_ptr<path_data>& input_path_data)
    {
        if (input_path_data->log.size() != 0)
            return text(input_path_data->log) | border;
        else
            return std::make_shared<Node>();
    };

    inline auto get_menu(const std::shared_ptr<path_data>& input_path_data)
    {
        MenuOption menu_option{MenuOption::Vertical()};
        // handel menu enter
        menu_option.on_enter = [input_data=input_path_data]()
        {
            handle_path_existence(input_data);
            std::filesystem::path directory{input_data->file_path};
            directory = directory.append((*input_data->menu.entries)[*input_data->menu.selected]).
                                  lexically_normal();
            input_data->file_path = directory.string();
            check_parent_sign(input_data);
            handel_file_type(input_data);
        };
        return Menu(&*input_path_data->menu.entries, input_path_data->menu.selected.get(),
                    menu_option);
    }

    inline auto input_transform(InputState state)
    {
        state.element |= color(Color::White);
        if (state.is_placeholder)
        {
            state.element |= dim;
        }
        if (state.focused)
        {
            state.element |= inverted;
        }
        else
        {
            state.element |= hcenter;
        }
        if (state.hovered)
        {
            state.element |= bgcolor(Color::GrayDark);
        }
        return state.element;
    };

    inline auto get_input(const std::shared_ptr<path_data>& input_path_data)
    {
        InputOption input_option{};
        input_option.multiline = false;
        input_option.on_enter = [input_data=input_path_data]() mutable
        {
            handle_path_existence(input_data);
            check_parent_sign(input_data);
            std::filesystem::path directory{input_data->file_path};
            handel_file_type(input_data);
        };
        input_option.transform = input_transform;
        return Input(&input_path_data->file_path, input_option);
    }

    inline auto handle_input(const Event& event)
    {
        if (event.is_character())
        {
            switch (event.character()[0])
            {
            case 'q':
                // screen.Exit();
                return true;
            case 'c':
                return true;
            default:
                break;
            }
        }
        return false;
    }

    inline auto FileMenu(std::shared_ptr<path_data>& input_data)
    {
        get_directory_content(input_data);
        auto menu = get_menu(input_data);
        auto input = get_input(input_data);
        auto container = Container::Vertical({input, menu}) | CatchEvent(handle_input);
        return Renderer(container, [input,menu,input_data]
        {
            return vbox({
                input->Render(),
                separator(),
                menu->Render() | yframe | yflex, // the flex is necessary for log to display
                build_log(input_data)
            }) | flex | border;
        });
    }

    Component folder_menu();

    Component folder_menu(path_data data);
}


#endif //FOLDER_MENU_H

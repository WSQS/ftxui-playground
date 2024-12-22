//
// Created by sophomore on 12/15/24.
//

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H
#include <filesystem>
#include <thread>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
using namespace ftxui;

namespace playground {
    /// @brief Option for the Menu component.
    /// @ingroup component
    struct enhanced_menu_option {
        // Standard constructors:
        static enhanced_menu_option Horizontal() {
            enhanced_menu_option option;
            option.direction = Direction::Right;
            option.entries_option.transform = [](const EntryState &state) {
                Element e = text(state.label);
                if (state.focused) {
                    e |= inverted;
                }
                if (state.active) {
                    e |= bold;
                }
                if (!state.focused && !state.active) {
                    e |= dim;
                }
                return e;
            };
            option.elements_infix = [] { return text(" "); };

            return option;
        };

        static enhanced_menu_option HorizontalAnimated();

        static enhanced_menu_option Vertical() {
            enhanced_menu_option option;
            option.entries_option.transform = [](const EntryState &state) {
                Element e = text((state.active ? "> " : "  ") + state.label); // NOLINT
                if (state.focused) {
                    e |= inverted;
                }
                if (state.active) {
                    e |= bold;
                }
                if (!state.focused && !state.active) {
                    e |= dim;
                }
                return e;
            };
            return option;
        };

        static enhanced_menu_option VerticalAnimated();

        static enhanced_menu_option Toggle() {
            auto option = enhanced_menu_option::Horizontal();
            option.elements_infix = [] { return text("│") | automerge; };
            return option;
        };

        std::variant<ConstStringListRef, std::vector<std::string *> *> entries; ///> The list of entries.
        Ref<int> selected = 0; ///> The index of the selected entry.

        // Style:
        UnderlineOption underline;
        MenuEntryOption entries_option;
        Direction direction = Direction::Down;
        std::function<Element()> elements_prefix;
        std::function<Element()> elements_infix;
        std::function<Element()> elements_postfix;

        // Observers:
        std::function<void()> on_change; ///> Called when the selected entry changes.
        std::function<void()> on_enter; ///> Called when the user presses enter.
        Ref<int> focused_entry = 0;
    };

    Component enhanced_menu(enhanced_menu_option options);

    Component enhanced_menu(ConstStringListRef entries,
                            int *selected_,
                            playground::enhanced_menu_option options = enhanced_menu_option::Vertical());

    Component MenuEntry(MenuEntryOption options);

    Component MenuEntry(ConstStringRef label, MenuEntryOption options = {});

    struct menu_data {
        Ref<std::vector<std::string> > entries{};
        std::shared_ptr<int> selected{};
    };

    struct path_data {
        std::string file_path;
        menu_data menu{};
        int selected = 1;
    };

    inline auto &get_screen() {
        static auto screen = ScreenInteractive::Fullscreen();
        return screen;
    }

    inline void log(std::string log_message) {
        get_screen().PostEvent(Event::Special("log" + log_message));
    }

    inline auto check_parent_sign(const std::shared_ptr<path_data> &input_path_data) {
        if (std::filesystem::path(input_path_data->file_path).root_directory() == input_path_data->
            file_path)
            input_path_data->menu.entries->clear();
        else
            *input_path_data->menu.entries = {".."};
    }

    inline auto handle_path_existence(const std::shared_ptr<path_data> &input_path_data) {
        if (!std::filesystem::exists(input_path_data->file_path)) {
            log("Unavailable path:" + input_path_data->file_path);
            input_path_data->file_path = "/";
        }
    }

    inline auto get_directory_content(const std::shared_ptr<path_data> &input_path_data) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data->file_path)) {
            input_path_data->menu.entries->push_back(entry.path().filename().string());
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
        enhanced_menu_option menu_option{enhanced_menu_option::Vertical()};
        // handel menu enter
        menu_option.on_enter = [input_data=input_path_data]() {
            handle_path_existence(input_data);
            std::filesystem::path directory{input_data->file_path};
            directory = directory.append((*input_data->menu.entries)[*input_data->menu.selected]).
                    lexically_normal();
            input_data->file_path = directory.string();
            check_parent_sign(input_data);
            handel_file_type(input_data);
        };
        return enhanced_menu(&*input_path_data->menu.entries, input_path_data->menu.selected.get(),
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
            handle_path_existence(input_data);
            check_parent_sign(input_data);
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
        std::vector<std::string *> data;
        for (const auto &path_data: path_datas) {
            data.push_back(&path_data->file_path);
        }
        return data;
    }
}


#endif //FOLDER_MENU_H

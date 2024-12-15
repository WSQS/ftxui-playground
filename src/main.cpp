#include <filesystem>
#include <iostream>
#include "folder_menu.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;
using namespace playground;
//
// ButtonOption Style() {
//     auto option = ButtonOption::Animated(Color::Red, Color::Black, Color::Black, Color::White);
//     option.transform = [](const EntryState &s) {
//         auto element = text(s.label);
//         if (s.focused) {
//             element |= bold;
//         }
//         return element | center | borderEmpty | flex;
//     };
//     return option;
// }

int main() {
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    constexpr auto check_parent_sign = [](path_data &input_path_data) {
        if (std::filesystem::path(input_path_data.input.content).root_directory() == input_path_data.input.content)
            input_path_data.menu.entries.clear();
        else
            input_path_data.menu.entries = {".."};
    };
    constexpr auto handle_path_existence = [](path_data &input_path_data) {
        if (!std::filesystem::exists(input_path_data.input.content)) {
            input_path_data.input.content = "/";
            input_path_data.log  = "Unavailable path";
        }
    };
    constexpr auto get_directory_content = [](path_data &input_path_data) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data.input.content)) {
            input_path_data.menu.entries.push_back(entry.path().filename().string());
        }
    };
    constexpr auto get_parent_directory = [](path_data &input_path_data) {
        std::filesystem::path temp_directory{input_path_data.input.content};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data.input.content = temp_directory.string();
    };
    constexpr auto run_command = [](path_data &input_path_data) {
        std::thread commandThread{
            [command = std::string("code ") + input_path_data.input.content]() {
                return std::system(command.c_str());
            }
        };
        commandThread.detach();
    };
    constexpr auto handel_file = [run_command,get_parent_directory,get_directory_content](path_data &input_path_data) {
        run_command(input_path_data);
        get_parent_directory(input_path_data);
        get_directory_content(input_path_data);
    };
    constexpr auto handel_file_type = [get_directory_content,handel_file](path_data &input_path_data) {
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
    constexpr auto build_log = [](path_data &input_path_data) {
        if (input_path_data.log.size() != 0)
            return text(input_path_data.log) | border;
        else
            return std::make_shared<Node>();
    };
    path_data input_data{{"/home/sophomore"}, {{".."}, {}, MenuOption::Vertical()}, };
    MenuOption &menu_option = input_data.menu.option;
    InputOption &input_option = input_data.input.option;
    // handel menu enter
    menu_option.on_enter = [&]() {
        handle_path_existence(input_data);
        std::filesystem::path directory{input_data.input.content};
        directory = directory.append(input_data.menu.entries[input_data.menu.selected]).lexically_normal();
        input_data.input.content = directory.string();
        check_parent_sign(input_data);
        handel_file_type(input_data);
    };
    auto menu = input_data.menu.instantiate();
    get_directory_content(input_data);
    input_option.multiline = false;
    input_option.on_enter = [&] {
        handle_path_existence(input_data);
        check_parent_sign(input_data);
        std::filesystem::path directory{input_data.input.content};
        handel_file_type(input_data);
    };
    input_option.transform = [](InputState state) {
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
    auto input = input_data.input.instantiate();
    auto container = Container::Vertical({input, menu});
    auto component = Renderer(container, [&] {
        return vbox({
                   input->Render(),
                   separator(),
                   menu->Render() | yframe | yflex, // the flex is necessary for log to display
                   build_log(input_data)
               }) | flex | border;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    screen.Loop(component);
    // auto foldermenu = folder_menu(input_data);
    // screen.Loop(foldermenu);
    return EXIT_SUCCESS;
}

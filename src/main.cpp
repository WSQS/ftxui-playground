#include <filesystem>
#include <iostream>
#include "folder_menu.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;
using namespace playground;

int main() {
    auto screen = ScreenInteractive::Fullscreen();

    path_data input_data{{"/home/sophomore"}, {{{".."}}, {Make<int>()}, MenuOption::Vertical()},};
    MenuOption &menu_option = input_data.menu.option;
    InputOption &input_option = input_data.input.option;
    // handel menu enter
    menu_option.on_enter = [&]() {
        handle_path_existence(input_data);
        std::filesystem::path directory{*input_data.input.content};
        directory = directory.append((*input_data.menu.entries)[*input_data.menu.selected]).lexically_normal();
        input_data.input.content = directory.string();
        check_parent_sign(input_data);
        handel_file_type(input_data);
    };
    auto menu = input_data.menu.instantiate();
    // auto menu = Menu(input_data.menu.option);
    get_directory_content(input_data);
    input_option.multiline = false;
    input_option.on_enter = [input_data]() mutable {
        handle_path_existence(input_data);
        check_parent_sign(input_data);
        std::filesystem::path directory{*input_data.input.content};
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
    // auto input = Input(input_data.input.option);
    auto container = Container::Vertical({input, menu}) | CatchEvent([&screen](const Event &event) {
        if (event.is_character()) {
            switch (event.character()[0]) {
                case 'q':
                    screen.Exit();
                    return true;
                case 'c':
                    return true;
                default:
                    break;
            }
        }
        return false;
    });
    auto component = Renderer(container, [input,menu,input_data] {
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

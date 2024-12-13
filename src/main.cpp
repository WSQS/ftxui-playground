#include <iostream>
#include <filesystem>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/screen_interactive.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"      // for ComponentBase


using namespace ftxui;

ButtonOption Style() {
    auto option = ButtonOption::Animated(Color::Red, Color::Black, Color::Black, Color::White);
    option.transform = [](const EntryState &s) {
        auto element = text(s.label);
        if (s.focused) {
            element |= bold;
        }
        return element | center | borderEmpty | flex;
    };
    return option;
}

struct path_data {
    std::string directory_path{};
    std::vector<std::string> entries{};
    int selected = 0;
};

int main() {
    using namespace ftxui;
    path_data input_data{"/home/sophomore", {".."}};
    auto screen = ScreenInteractive::Fullscreen();
    MenuOption menu_option = MenuOption::Vertical();
    auto check_parent_sign = [](path_data &input_path_data) {
        if (std::filesystem::path(input_path_data.directory_path).root_directory() == input_path_data.directory_path)
            input_path_data.entries.clear();
        else
            input_path_data.entries = {".."};
    };
    auto handle_path_existence = [](path_data &input_path_data) {
        if (!std::filesystem::exists(input_path_data.directory_path)) {
            input_path_data.directory_path = "/";
        }
    };
    auto get_directory_content = [](path_data &input_path_data) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data.directory_path)) {
            input_path_data.entries.push_back(entry.path().filename());
        }
    };
    auto get_parent_directory = [](path_data &input_path_data) {
        std::filesystem::path temp_directory{input_path_data.directory_path};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data.directory_path = temp_directory;
    };
    // handel menu enter
    menu_option.on_enter = [&] {
        handle_path_existence(input_data);
        std::filesystem::path directory{input_data.directory_path};
        directory = directory.append(input_data.entries[input_data.selected]).lexically_normal();
        input_data.directory_path = directory;
        check_parent_sign(input_data);
        if (status(directory).type() == std::filesystem::file_type::directory)
            get_directory_content(input_data);
        else if (status(directory).type() == std::filesystem::file_type::regular) {
            auto command = std::string("code ") + directory.string();
            get_parent_directory(input_data);
            get_directory_content(input_data);
            std::thread commandThread{
                [command]() {
                    auto _ = std::system(command.c_str());
                    return _;
                }
            };
            commandThread.detach();
        }
    };
    auto menu = Menu(&input_data.entries, &input_data.selected, menu_option);
    for (const auto &entry: std::filesystem::directory_iterator(input_data.directory_path)) {
        input_data.entries.push_back(entry.path().filename());
    }
    InputOption input_option{};
    input_option.multiline = false;
    input_option.on_enter = [&] {
        handle_path_existence(input_data);
        check_parent_sign(input_data);
        std::filesystem::path directory{input_data.directory_path};
        if (status(directory).type() == std::filesystem::file_type::directory)
            get_directory_content(input_data);
        else if (status(directory).type() == std::filesystem::file_type::regular) {
            auto _ = std::system((std::string("code ") + input_data.directory_path).c_str());
            get_parent_directory(input_data);
            get_directory_content(input_data);
            return _;
        }
        return 0;
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
    auto input = Input(&input_data.directory_path, input_option);
    auto container = Container::Vertical({input, menu});
    auto component = Renderer(container, [&] {
        return vbox({
                   input->Render(),
                   separator(),
                   // buttons->Render () | flex,
                   menu->Render() | yframe,
               }) |
               flex | border;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    screen.Loop(component);
    return EXIT_SUCCESS;
}

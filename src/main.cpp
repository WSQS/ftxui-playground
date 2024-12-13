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

struct path_input_data {
    std::string directory_path;
    std::vector<std::string> entries;
};

int main() {
    using namespace ftxui;
    path_input_data input_data{"/home/sophomore", {".."}};
    int selected = 0;
    MenuOption option = MenuOption::Vertical();
    auto screen = ScreenInteractive::Fullscreen();
    auto func = [&] {
        std::filesystem::path directory{input_data.directory_path};
        directory = directory.append(input_data.entries[selected]).lexically_normal();
        input_data.directory_path = directory;
        if (directory.root_directory() == input_data.directory_path) {
            input_data.entries.clear();
        } else {
            input_data.entries = {".."};
        }
        if (is_directory(directory)) {
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                input_data.entries.push_back(entry.path().filename());
            }
        } else {
            auto command = std::string("code ") + directory.string();
            std::filesystem::path temp_directory{input_data.directory_path};
            directory = directory.append("..").lexically_normal();
            input_data.directory_path = directory;
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                input_data.entries.push_back(entry.path().filename());
            }
            std::thread commandThread{
                [command]() {
                    auto _ = std::system(command.c_str());
                    return _;
                }
            };
            commandThread.detach();
        }
    };
    option.on_enter = func;
    auto menu = Menu(&input_data.entries, &selected, option);
    for (const auto &entry: std::filesystem::directory_iterator(input_data.directory_path)) {
        input_data.entries.push_back(entry.path().filename());
    }
    InputOption input_option{};
    input_option.multiline = false;
    input_option.on_enter = [&] {
        if (!std::filesystem::exists(input_data.directory_path)) {
            input_data.directory_path = "/";
        }
        std::filesystem::path directory{input_data.directory_path};
        if (directory.root_directory() == input_data.directory_path) {
            input_data.entries.clear();
        } else {
            input_data.entries = {".."};
        }
        if (is_regular_file(directory)) {
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                input_data.entries.push_back(entry.path().filename());
            }
        } else {
            input_data.directory_path = std::string("code ") + directory.string();
            input_data.entries.push_back(input_data.directory_path);
            auto _ = std::system(input_data.directory_path.c_str());
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

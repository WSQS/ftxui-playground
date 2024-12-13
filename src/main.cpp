#include <iostream>
#include <filesystem>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/screen_interactive.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"      // for ComponentBase


using namespace ftxui;

ButtonOption
Style() {
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

int
main() {
    using namespace ftxui;
    std::vector<std::string> entries{".."};
    int selected = 0;
    MenuOption option = MenuOption::Vertical();
    auto screen = ScreenInteractive::Fullscreen();
    std::string directoryPath = "/home/sophomore";
    auto func = [&] {
        std::filesystem::path directory{directoryPath};
        directory = directory.append(entries[selected]).lexically_normal();
        directoryPath = directory;
        if (directory.root_directory() == directoryPath) {
            entries.clear();
        } else {
            entries = {".."};
        }
        if (is_directory(directory)) {
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                entries.push_back(entry.path().filename());
            }
        } else {
            auto command = std::string("code ") + directory.string();
            std::filesystem::path temp_directory{directoryPath};
            directory = directory.append("..").lexically_normal();
            directoryPath = directory;
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                entries.push_back(entry.path().filename());
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
    auto menu = Menu(&entries, &selected, option);
    for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
        entries.push_back(entry.path().filename());
    }
    InputOption input_option{};
    input_option.multiline = false;
    input_option.on_enter = [&] {
        if (!std::filesystem::exists(directoryPath)) {
            directoryPath = "/";
        }
        std::filesystem::path directory{directoryPath};
        if (directory.root_directory() == directoryPath) {
            entries.clear();
        } else {
            entries = {".."};
        }
        if (is_regular_file(directory)) {
            for (const auto &entry: std::filesystem::directory_iterator(directory)) {
                entries.push_back(entry.path().filename());
            }
        } else {
            directoryPath = std::string("code ") + directory.string();
            entries.push_back(directoryPath);
            auto _ = std::system(directoryPath.c_str());
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
    auto input = Input(&directoryPath, input_option);
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

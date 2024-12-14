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
    std::string log{};
    int selected = 0;
};

int main() {
    using namespace ftxui;
    path_data input_data{"/home/sophomore", {".."}};
    auto screen = ScreenInteractive::Fullscreen();
    MenuOption menu_option = MenuOption::Vertical();
    constexpr auto check_parent_sign = [](path_data &input_path_data) {
        if (std::filesystem::path(input_path_data.directory_path).root_directory() == input_path_data.directory_path)
            input_path_data.entries.clear();
        else
            input_path_data.entries = {".."};
    };
    constexpr auto handle_path_existence = [](path_data &input_path_data) {
        if (!std::filesystem::exists(input_path_data.directory_path)) {
            input_path_data.directory_path = "/";
        }
    };
    constexpr auto get_directory_content = [](path_data &input_path_data) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path_data.directory_path)) {
            input_path_data.entries.push_back(entry.path().filename().string());
        }
    };
    constexpr auto get_parent_directory = [](path_data &input_path_data) {
        std::filesystem::path temp_directory{input_path_data.directory_path};
        temp_directory = temp_directory.append("..").lexically_normal();
        input_path_data.directory_path = temp_directory.string();
    };
    constexpr auto run_command = [](path_data &input_path_data) {
        std::thread commandThread{
            [command = std::string("code ") + input_path_data.directory_path]() {
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
        std::filesystem::path directory{input_path_data.directory_path};
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
    // handel menu enter
    menu_option.on_enter = [&]() {
        handle_path_existence(input_data);
        std::filesystem::path directory{input_data.directory_path};
        directory = directory.append(input_data.entries[input_data.selected]).lexically_normal();
        input_data.directory_path = directory.string();
        check_parent_sign(input_data);
        handel_file_type(input_data);
    };
    auto menu = Menu(&input_data.entries, &input_data.selected, menu_option);
    for (const auto &entry: std::filesystem::directory_iterator(input_data.directory_path)) {
        input_data.entries.push_back(entry.path().filename().string());
    }
    InputOption input_option{};
    input_option.multiline = false;
    input_option.on_enter = [&] {
        handle_path_existence(input_data);
        check_parent_sign(input_data);
        std::filesystem::path directory{input_data.directory_path};
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
    auto input = Input(&input_data.directory_path, input_option);
    auto container = Container::Vertical({input, menu});
    auto component = Renderer(container, [&] {
        return vbox({
                   input->Render(),
                   separator(),
                   // buttons->Render () | flex,
                   menu->Render() | yframe | yflex, // the flex is necessary for log to display
                   text(input_data.log) | border
                   // input_data.log.size() ? text(input_data.log) : std::make_shared<Node>()
               }) | flex | border;
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

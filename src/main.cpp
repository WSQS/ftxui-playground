#include <iostream>
#include <filesystem>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include "ftxui/component/screen_interactive.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, text, Element, operator|, vbox, border


using namespace ftxui;

ButtonOption
Style()
{
    auto option = ButtonOption::Animated(Color::Red, Color::Black, Color::Black, Color::White);
    option.transform = [](const EntryState& s)
    {
        auto element = text(s.label);
        if (s.focused)
        {
            element |= bold;
        }
        return element | center | borderEmpty | flex;
    };
    return option;
}

int
main()
{
    using namespace ftxui;

    auto summary = [&]
    {
        auto content = vbox({
            hbox({text(L"- done:   "), text(L"3") | bold}) | color(Color::Green),
            hbox({text(L"- active: "), text(L"2") | bold}) | color(Color::RedLight),
            hbox({text(L"- queue:  "), text(L"9") | bold}) | color(Color::Red),
        });
        return window(text(L" Summary "), content);
    };
    auto value = 50;
    std::vector<std::string> entries{".."};
    int selected = 0;

    MenuOption option = MenuOption::Vertical();
    auto screen = ScreenInteractive::Fullscreen();
    std::filesystem::path directoryPath = "/mnt/c";
    option.on_enter = [&]
    {
        std::cout << '\0' << std::endl;
        directoryPath.append(entries[selected]);
        // entries.clear();
        entries = {".."};
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
        {
            entries.push_back(entry.path().filename());
        }
    };
    auto menu = Menu(&entries, &selected, option);
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        entries.push_back(entry.path().filename());
    }
    auto btn_dec_01 = Button("-1", [&value]
    {
        value = rand() % 50;
    }, Style());
    auto btn_dec_02 = Button("-1", [&value,&entries]
    {
        value = rand() % 50;
        entries.push_back(std::to_string(value));
        // menu->Add(Button("-2",[]{}));
    }, Style());
    // auto btn_dec_02 = Button("+1", [&value] { value += 1; }, Style());
    auto document = //
        vbox({
            hbox({
                summary(),
                summary(),
                summary() | flex,
            }),
            summary(),
            summary(),
        });
    // auto buttons = Container::Horizontal ({ btn_dec_02 });

    auto component = Renderer(menu, [&]
    {
        return vbox({
                text(entries[selected]),
                separator(),
                // buttons->Render () | flex,
                menu->Render() | flex,
            }) |
            flex | border;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    screen.Loop(menu);
    return EXIT_SUCCESS;
}

#include <iostream>

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

int main() {
    using namespace ftxui;

    auto summary = [&] {
        auto content = vbox({
            hbox({text(L"- done:   "), text(L"3") | bold}) | color(Color::Green),
            hbox({text(L"- active: "), text(L"2") | bold}) | color(Color::RedLight),
            hbox({text(L"- queue:  "), text(L"9") | bold}) | color(Color::Red),
        });
        return window(text(L" Summary "), content);
    };
    auto value = 50;
    std::vector<std::string> entries = {
        "entry 1",
        "entry 2",
        "entry 3",
    };
    int selected = 0;

    MenuOption option;
    // option.on_enter = screen.ExitLoopClosure();
    auto menu = Menu(&entries, &selected, MenuOption::Horizontal());
    auto btn_dec_01 = Button("-1", [&value,&menu] {
        value = rand() % 50;
    }, Style());
    auto btn_dec_02 = Button("-1", [&value,&menu,&entries] {
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
    auto buttons = Container::Horizontal({btn_dec_02});

    auto component = Renderer(buttons, [&value,&buttons,&menu] {
        return vbox({
                   hbox(text(std::to_string(value)), separator(), gauge(value / 50.f)),
                   separator(),
                   buttons->Render() | flex,
                    menu->Render() | flex,
               }) |
               flex | border;
    });
    // Limit the size of the document to 80 char.
    document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(component);
    return EXIT_SUCCESS;
}

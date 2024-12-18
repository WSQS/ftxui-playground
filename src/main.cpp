#include <filesystem>
#include <iostream>
#include "folder_menu.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;
using namespace playground;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();
    std::vector<std::shared_ptr<path_data>> path_datas;
    Components menus;
    menus.push_back(FileMenu(path_datas));
    menus.push_back(FileMenu(path_datas));
    int select = 0;
    std::vector<std::string> tab_values{
        "tab_1",
        "tab_2",
        "tab_3",
    };
    auto tab_toggle = Toggle(&tab_values, &select);
    auto tab_container = Container::Tab(menus, &select);
    auto container = Container::Vertical({
        tab_toggle,
        tab_container,
    });
    auto renderer = Renderer(container, [&] {
  return vbox({
             tab_toggle->Render(),
             separator(),
             tab_container->Render(),
         }) |
         border;
});
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    screen.Loop(renderer);
    // auto foldermenu = folder_menu(input_data);
    // screen.Loop(foldermenu);
    return EXIT_SUCCESS;
}

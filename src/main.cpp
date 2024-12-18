#include <filesystem>
#include <iostream>
#include "folder_menu.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;
using namespace playground;

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    std::vector<std::shared_ptr<path_data>> path_datas;
    auto component = FileMenu(path_datas);
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

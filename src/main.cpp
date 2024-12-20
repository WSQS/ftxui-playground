#include <filesystem>
#include <iostream>
#include "folder_menu.h"
#include "ftxui/component/component.hpp"


using namespace ftxui;
using namespace playground;



auto add_folder_menu(std::vector<std::string*> &tab_values, std::vector<std::shared_ptr<path_data> > &path_datas,
                     Component &tab_container) {
    static int i = 0;
    auto input_data = Make<path_data>(path_data{
        "/home", {{{".."}}, {Make<int>()}},{std::to_string(i)}
    });
    i++;
    tab_values = build_tab_value(path_datas);
    path_datas.push_back(input_data);
    tab_container->Add(FileMenu(input_data));
}

int main() {
    std::vector<std::shared_ptr<path_data> > path_datas;
    int select = 0;
    std::vector<std::string*> tab_values{};
    auto tab_container = Container::Tab({}, &select);
    add_folder_menu(tab_values, path_datas, tab_container);
    add_folder_menu(tab_values, path_datas, tab_container);
    auto tab_toggle = Toggle(&tab_values, &select);
    auto container = Container::Vertical({
        tab_toggle,
        tab_container,
    });
    auto renderer = Renderer(container, [&] {
        return vbox({
                   tab_toggle->Render(),
                   separator(),
                   tab_container->Render(),
               }) | border;
    }) | CatchEvent([&](const Event &event) {
        if (event == Event::Character('a')) {
            // tab_values.push_back("aaa");
            add_folder_menu(tab_values, path_datas, tab_container);
            // move focus to the last tab
            select = static_cast<int>(tab_values.size()) - 1;
            return true;
        }
        // if (event == Event::Return) {
        //     tab_values = build_tab_value(path_datas);
        //     return false;
        // }
        return false;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    get_screen().Loop(renderer);
    // auto foldermenu = folder_menu(input_data);
    // screen.Loop(foldermenu);
    return EXIT_SUCCESS;
}

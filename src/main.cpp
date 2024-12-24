#include <filesystem>
#include "folder_menu.h"

using namespace ftxui;
using namespace playground;

int main() {
    std::vector<std::shared_ptr<path_data> > path_datas;
    int select = 0;
    std::vector<reference<std::string> > tab_values{};
    auto tab_container = Container::Tab({}, &select);
    add_folder_menu(path_datas, tab_container, select);
    add_folder_menu(path_datas, tab_container, select);
    tab_values = build_tab_value(path_datas);
    auto tab_toggle = playground::Toggle(&tab_values, &select);
    auto container = Container::Vertical({tab_toggle, tab_container});
    std::string log{};
    auto renderer = Renderer(container, [&] {
        return vbox({
                   tab_toggle->Render(),
                   separator(),
                   tab_container->Render() | flex,
                   build_log(log),
               }) | border;
    }) | CatchEvent([&](const Event &event) {
        // add tab
        if (event == Event::Character('a')) {
            add_folder_menu(path_datas, tab_container, select);
            tab_values = build_tab_value(path_datas);
            // move focus to the last tab
            select = static_cast<int>(tab_values.size()) - 1;
            return true;
        }
        // remove current tab
        if (event == Event::Character('d')) {
            remove_folder_menu(path_datas, tab_container, select);
            tab_values = build_tab_value(path_datas);
            // move focus to the last tab
            select = static_cast<int>(tab_values.size()) - 1;
            return true;
        }
        // handel log
        if (event.input().size() >= 3 && event.input().substr(0, 3) == "log") {
            log = event.input().substr(3, event.input().size() - 3);
            return true;
        }
        return false;
    });
    // Limit the size of the document to 80 char.
    // document = document; //| size(WIDTH, LESS_THAN, 80);

    // auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    // Render(screen, document);
    //
    // std::cout << screen.ToString() << '\0' << std::endl;

    get_screen().Loop(renderer);
    return EXIT_SUCCESS;
}

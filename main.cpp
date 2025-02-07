#include "folder_menu.h"
using namespace ftxui;
using namespace playground;

int main() {
    std::vector<std::shared_ptr<path_data>> path_datas;
    int select = 0;
    std::vector<reference<std::string>> tab_values{};
    auto tab_container = Container::Tab({}, &select);
    add_folder_menu(path_datas, tab_container, select);
    add_folder_menu(path_datas, tab_container, select);
    tab_values = build_tab_value(path_datas);
    auto tab_toggle = playground::Toggle(&tab_values, &select);
    auto search_bar = Input();
    bool enable_search = false;
    auto container_with_tab =
        Container::Vertical({tab_toggle, tab_container}) | CatchEvent([&](const Event &event) {
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
            return false;
        });
    auto full = Container::Vertical({container_with_tab, search_bar});
    std::string log{};
    auto renderer = Renderer(full, [&] {
        return vbox({
                   tab_toggle->Render(),
                   separator(),
                   tab_container->Render() | flex,
                   enable_search ? search_bar->Render() | border : Make<Node>(),
                   build_box(log),
               }) |
               border;
    }) | CatchEvent([&](const Event &event) {
        if (event == Event::Character('q'))
            get_screen().Exit();
        if (event == Event::Character('/')) {
            // playground::log("searching");
            enable_search = !enable_search;
            search_bar->TakeFocus();
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

    get_screen().Loop(renderer);
    return EXIT_SUCCESS;
}

//
// Created by sophomore on 12/15/24.
//

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H
#include <vector>
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
using namespace ftxui;

namespace playground {
    struct menu_data {
        std::vector<std::string> entries{};
        int selected{};
        MenuOption option{};
        Component instantiate(){
            return Menu(&entries,&selected,option);
        }
    };

    struct input_data{
        std::string content{};
        InputOption option{};
        Component instantiate() {
            return Input(&content,option);
        }
    };

    struct path_data{
        input_data input{};
        menu_data menu{};
        std::string log{};
        int selected = 1;
        Components instantiate() {
            return {input.instantiate(),menu.instantiate()};
        }
    };

    Component folder_menu();

    Component folder_menu(path_data data);
}


#endif //FOLDER_MENU_H

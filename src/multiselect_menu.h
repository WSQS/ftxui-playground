//
// Created by sophomore on 12/22/24.
//

#ifndef MULTISELECT_MENU_H
#define MULTISELECT_MENU_H

#include <set>

#include "ftxui/component/component.hpp"
using namespace ftxui;

namespace playground {
    struct multiselect_menu_option {
        // Standard constructors:
        static multiselect_menu_option Horizontal() {
            multiselect_menu_option option;
            option.direction = ftxui::Direction::Right;
            option.entries_option.transform = [](const EntryState &state) {
                Element e = text(state.label);
                if (state.focused) {
                    e |= inverted;
                }
                if (state.active) {
                    e |= bold;
                }
                if (!state.focused && !state.active) {
                    e |= dim;
                }
                return e;
            };
            option.elements_infix = [] { return text(" "); };

            return option;
        }

        static multiselect_menu_option Vertical() {
            multiselect_menu_option option;
            option.entries_option.transform = [](const EntryState &state) {
                Element e = text((state.active ? "> " : "  ") + state.label); // NOLINT
                if (state.focused) {
                    e |= inverted;
                }
                if (state.active) {
                    e |= bold;
                }
                if (!state.focused && !state.active) {
                    e |= dim;
                }
                return e;
            };
            return option;
        }

        std::variant<ConstStringListRef, std::vector<std::string *> *> entries; ///> The list of entries.
        Ref<int> selected = 0; ///> The index of the selected entry.
        Ref<std::set<int>> toggled{};

        // Style:
        UnderlineOption underline;
        MenuEntryOption entries_option;
        Direction direction = Direction::Down;
        std::function<Element()> elements_prefix;
        std::function<Element()> elements_infix;
        std::function<Element()> elements_postfix;

        // Observers:
        std::function<void()> on_change; ///> Called when the selected entry changes.
        std::function<void()> on_enter; ///> Called when the user presses enter.
        Ref<int> focused_entry = 0;
    };

    Component multiselect_menu(multiselect_menu_option options);

    Component multiselect_menu(ConstStringListRef entries,
                               int *selected_,
                               multiselect_menu_option options = multiselect_menu_option::Vertical());
}


#endif //MULTISELECT_MENU_H

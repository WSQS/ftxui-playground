//
// Created by sophomore on 12/22/24.
//

#ifndef ENHANCED_MENU_H
#define ENHANCED_MENU_H
#include "ftxui/component/component.hpp"

using namespace ftxui;

namespace playground {
    struct enhanced_menu_option {
        // Standard constructors:
        static enhanced_menu_option Horizontal() {
            enhanced_menu_option option;
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
        };

        static enhanced_menu_option HorizontalAnimated();

        static enhanced_menu_option Vertical() {
            enhanced_menu_option option;
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
        };

        static enhanced_menu_option VerticalAnimated();

        static enhanced_menu_option Toggle() {
            auto option = enhanced_menu_option::Horizontal();
            option.elements_infix = [] { return text("│") | automerge; };
            return option;
        };

        std::variant<ConstStringListRef, std::vector<std::string *> *> entries; ///> The list of entries.
        Ref<int> selected = 0; ///> The index of the selected entry.

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

    Component enhanced_menu(enhanced_menu_option options);

    Component enhanced_menu(ConstStringListRef entries,
                            int *selected_,
                            enhanced_menu_option options = enhanced_menu_option::Vertical());

    Component Toggle(ConstStringListRef entries, int *selected);

    Component Toggle(std::vector<std::string *> *entries, int *selected);
} // playground

#endif //ENHANCED_MENU_H

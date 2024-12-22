//
// Created by sophomore on 12/22/24.
//

#ifndef MENU_UTIL_H
#define MENU_UTIL_H
#include "ftxui/component/component.hpp"
using namespace ftxui;

namespace playground {
    inline Element DefaultOptionTransform(const EntryState &state) {
        std::string label = (state.active ? "> " : "  ") + state.label; // NOLINT
        Element e = text(std::move(label));
        if (state.focused) {
            e = e | inverted;
        }
        if (state.active) {
            e = e | bold;
        }
        return e;
    }

    inline bool IsInverted(Direction direction) {
        switch (direction) {
            case Direction::Up:
            case Direction::Left:
                return true;
            case Direction::Down:
            case Direction::Right:
                return false;
        }
        return false; // NOT_REACHED()
    }

    inline bool IsHorizontal(Direction direction) {
        switch (direction) {
            case Direction::Left:
            case Direction::Right:
                return true;
            case Direction::Down:
            case Direction::Up:
                return false;
        }
        return false; // NOT_REACHED()
    }

    // Similar to std::clamp, but allow hi to be lower than lo.
    template<class T>
    constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
        return v < lo ? lo : hi < v ? hi : v;
    }
}
#endif //MENU_UTIL_H

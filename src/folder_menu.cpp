//
// Created by sophomore on 12/15/24.
//
#include "folder_menu.h"
#include <algorithm>                // for max, fill_n, reverse
#include <chrono>                   // for milliseconds
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <functional>               // for function
#include <string>                   // for operator+, string
#include <utility>                  // for move
#include <vector>                   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/animation.hpp"  // for Animator, Linear
#include "ftxui/component/component.hpp"  // for Make, Menu, MenuEntry, Toggle
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, MenuEntryOption, UnderlineOption, AnimatedColorOption, AnimatedColorsOption, EntryState



#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp, Event::End, Event::Home, Event::PageDown, Event::PageUp, Event::Return, Event::Tab, Event::TabReverse



#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released, Mouse::WheelDown, Mouse::WheelUp, Mouse::None



#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, Decorator, nothing, Elements, bgcolor, color, hbox, separatorHSelector, separatorVSelector, vbox, xflex, yflex, text, bold, focus, inverted, select



#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color
#include "ftxui/util/ref.hpp"  // for Ref, ConstStringListRef, ConstStringRef

using namespace ftxui;

namespace playground {

} // namespace ftxui

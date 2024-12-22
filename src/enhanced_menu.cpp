//
// Created by sophomore on 12/22/24.
//

#include "enhanced_menu.h"

#include "ftxui/component/event.hpp"

namespace playground {
    Element DefaultOptionTransform(const EntryState &state) {
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

    bool IsInverted(Direction direction) {
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

    bool IsHorizontal(Direction direction) {
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

    /// @brief A list of items. The user can navigate through them.
    /// @ingroup component
    class enhanced_menu_base : public ComponentBase, public enhanced_menu_option {
    public:
        explicit enhanced_menu_base(const enhanced_menu_option &option) : enhanced_menu_option(option) {
        }

        bool IsHorizontal() { return playground::IsHorizontal(direction); }

        void OnChange() {
            if (on_change) {
                on_change();
            }
        }

        void OnEnter() {
            if (on_enter) {
                on_enter();
            }
        }

        void Clamp() {
            if (selected() != selected_previous_) {
                SelectedTakeFocus();
            }
            boxes_.resize(size());
            selected() = clamp(selected(), 0, size() - 1);
            selected_previous_ = clamp(selected_previous_, 0, size() - 1);
            selected_focus_ = clamp(selected_focus_, 0, size() - 1);
            focused_entry() = clamp(focused_entry(), 0, size() - 1);
        }

        void OnAnimation(animation::Params &params) override {
            animator_first_.OnAnimation(params);
            animator_second_.OnAnimation(params);
            for (auto &animator: animator_background_) {
                animator.OnAnimation(params);
            }
            for (auto &animator: animator_foreground_) {
                animator.OnAnimation(params);
            }
        }

        Element Render() override {
            Clamp();
            UpdateAnimationTarget();

            Elements elements;
            const bool is_menu_focused = Focused();
            if (elements_prefix) {
                elements.push_back(elements_prefix());
            }
            elements.reserve(size());
            for (int i = 0; i < size(); ++i) {
                if (i != 0 && elements_infix) {
                    elements.push_back(elements_infix());
                }
                const bool is_focused = (focused_entry() == i) && is_menu_focused;
                const bool is_selected = (selected() == i);

                const EntryState state = {
                    std::holds_alternative<ConstStringListRef>(entries)
                        ? std::get<ConstStringListRef>(entries)[i]
                        : *(*std::get<std::vector<std::string *> *>(entries))[i],
                    false, is_selected, is_focused, i,
                };

                auto focus_management = (selected_focus_ != i)
                                            ? nothing
                                            : is_menu_focused
                                                  ? focus
                                                  : ftxui::select;

                const Element element =
                        (entries_option.transform
                             ? entries_option.transform
                             : DefaultOptionTransform) //
                        (state);
                elements.push_back(element | AnimatedColorStyle(i) | reflect(boxes_[i]) |
                                   focus_management);
            }
            if (elements_postfix) {
                elements.push_back(elements_postfix());
            }

            if (IsInverted(direction)) {
                std::reverse(elements.begin(), elements.end());
            }

            const Element bar =
                    IsHorizontal() ? hbox(std::move(elements)) : vbox(std::move(elements));

            if (!underline.enabled) {
                return bar | reflect(box_);
            }

            if (IsHorizontal()) {
                return vbox({
                           bar | xflex,
                           separatorHSelector(first_, second_, //
                                              underline.color_active,
                                              underline.color_inactive),
                       }) |
                       reflect(box_);
            } else {
                return hbox({
                           separatorVSelector(first_, second_, //
                                              underline.color_active,
                                              underline.color_inactive),
                           bar | yflex,
                       }) |
                       reflect(box_);
            }
        }

        void SelectedTakeFocus() {
            selected_previous_ = selected();
            selected_focus_ = selected();
        }

        void OnUp() {
            switch (direction) {
                case Direction::Up:
                    selected()++;
                    break;
                case Direction::Down:
                    selected()--;
                    break;
                case Direction::Left:
                case Direction::Right:
                    break;
            }
        }

        void OnDown() {
            switch (direction) {
                case Direction::Up:
                    selected()--;
                    break;
                case Direction::Down:
                    selected()++;
                    break;
                case Direction::Left:
                case Direction::Right:
                    break;
            }
        }

        void OnLeft() {
            switch (direction) {
                case Direction::Left:
                    selected()++;
                    break;
                case Direction::Right:
                    selected()--;
                    break;
                case Direction::Down:
                case Direction::Up:
                    break;
            }
        }

        void OnRight() {
            switch (direction) {
                case Direction::Left:
                    selected()--;
                    break;
                case Direction::Right:
                    selected()++;
                    break;
                case Direction::Down:
                case Direction::Up:
                    break;
            }
        }

        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        bool OnEvent(Event event) override {
            Clamp();
            if (!CaptureMouse(event)) {
                return false;
            }

            if (event.is_mouse()) {
                return OnMouseEvent(event);
            }

            if (Focused()) {
                const int old_selected = selected();
                if (event == Event::ArrowUp || event == Event::Character('k')) {
                    OnUp();
                }
                if (event == Event::ArrowDown || event == Event::Character('j')) {
                    OnDown();
                }
                if (event == Event::ArrowLeft || event == Event::Character('h')) {
                    OnLeft();
                }
                if (event == Event::ArrowRight || event == Event::Character('l')) {
                    OnRight();
                }
                if (event == Event::PageUp) {
                    selected() -= box_.y_max - box_.y_min;
                }
                if (event == Event::PageDown) {
                    selected() += box_.y_max - box_.y_min;
                }
                if (event == Event::Home) {
                    selected() = 0;
                }
                if (event == Event::End) {
                    selected() = size() - 1;
                }
                if (event == Event::Tab && size()) {
                    selected() = (selected() + 1) % size();
                }
                if (event == Event::TabReverse && size()) {
                    selected() = (selected() + size() - 1) % size();
                }

                selected() = clamp(selected(), 0, size() - 1);

                if (selected() != old_selected) {
                    focused_entry() = selected();
                    SelectedTakeFocus();
                    OnChange();
                    return true;
                }
            }

            if (event == Event::Return) {
                OnEnter();
                return true;
            }

            return false;
        }

        bool OnMouseEvent(Event event) {
            if (event.mouse().button == Mouse::WheelDown ||
                event.mouse().button == Mouse::WheelUp) {
                return OnMouseWheel(event);
            }

            if (event.mouse().button != Mouse::None &&
                event.mouse().button != Mouse::Left) {
                return false;
            }
            if (!CaptureMouse(event)) {
                return false;
            }
            for (int i = 0; i < size(); ++i) {
                if (!boxes_[i].Contain(event.mouse().x, event.mouse().y)) {
                    continue;
                }

                TakeFocus();
                focused_entry() = i;

                if (event.mouse().button == Mouse::Left &&
                    event.mouse().motion == Mouse::Pressed) {
                    if (selected() != i) {
                        selected() = i;
                        selected_previous_ = selected();
                        OnChange();
                    }
                    return true;
                }
            }
            return false;
        }

        bool OnMouseWheel(Event event) {
            if (!box_.Contain(event.mouse().x, event.mouse().y)) {
                return false;
            }
            const int old_selected = selected();

            if (event.mouse().button == Mouse::WheelUp) {
                selected()--;
            }
            if (event.mouse().button == Mouse::WheelDown) {
                selected()++;
            }

            selected() = clamp(selected(), 0, size() - 1);

            if (selected() != old_selected) {
                SelectedTakeFocus();
                OnChange();
            }
            return true;
        }

        void UpdateAnimationTarget() {
            UpdateColorTarget();
            UpdateUnderlineTarget();
        }

        void UpdateColorTarget() {
            if (size() != int(animation_background_.size())) {
                animation_background_.resize(size());
                animation_foreground_.resize(size());
                animator_background_.clear();
                animator_foreground_.clear();

                const int len = size();
                animator_background_.reserve(len);
                animator_foreground_.reserve(len);
                for (int i = 0; i < len; ++i) {
                    animation_background_[i] = 0.F;
                    animation_foreground_[i] = 0.F;
                    animator_background_.emplace_back(&animation_background_[i], 0.F,
                                                      std::chrono::milliseconds(0),
                                                      animation::easing::Linear);
                    animator_foreground_.emplace_back(&animation_foreground_[i], 0.F,
                                                      std::chrono::milliseconds(0),
                                                      animation::easing::Linear);
                }
            }

            const bool is_menu_focused = Focused();
            for (int i = 0; i < size(); ++i) {
                const bool is_focused = (focused_entry() == i) && is_menu_focused;
                const bool is_selected = (selected() == i);
                float target = is_selected ? 1.F : is_focused ? 0.5F : 0.F; // NOLINT
                if (animator_background_[i].to() != target) {
                    animator_background_[i] = animation::Animator(
                        &animation_background_[i], target,
                        entries_option.animated_colors.background.duration,
                        entries_option.animated_colors.background.function);
                    animator_foreground_[i] = animation::Animator(
                        &animation_foreground_[i], target,
                        entries_option.animated_colors.foreground.duration,
                        entries_option.animated_colors.foreground.function);
                }
            }
        }

        Decorator AnimatedColorStyle(int i) {
            Decorator style = nothing;
            if (entries_option.animated_colors.foreground.enabled) {
                style = style | color(Color::Interpolate(
                            animation_foreground_[i],
                            entries_option.animated_colors.foreground.inactive,
                            entries_option.animated_colors.foreground.active));
            }

            if (entries_option.animated_colors.background.enabled) {
                style = style | bgcolor(Color::Interpolate(
                            animation_background_[i],
                            entries_option.animated_colors.background.inactive,
                            entries_option.animated_colors.background.active));
            }
            return style;
        }

        void UpdateUnderlineTarget() {
            if (!underline.enabled) {
                return;
            }

            if (FirstTarget() == animator_first_.to() &&
                SecondTarget() == animator_second_.to()) {
                return;
            }

            if (FirstTarget() >= animator_first_.to()) {
                animator_first_ = animation::Animator(
                    &first_, FirstTarget(), underline.follower_duration,
                    underline.follower_function, underline.follower_delay);

                animator_second_ = animation::Animator(
                    &second_, SecondTarget(), underline.leader_duration,
                    underline.leader_function, underline.leader_delay);
            } else {
                animator_first_ = animation::Animator(
                    &first_, FirstTarget(), underline.leader_duration,
                    underline.leader_function, underline.leader_delay);

                animator_second_ = animation::Animator(
                    &second_, SecondTarget(), underline.follower_duration,
                    underline.follower_function, underline.follower_delay);
            }
        }

        bool Focusable() const final {
            return std::holds_alternative<ConstStringListRef>(entries)
                       ? std::get<ConstStringListRef>(entries).size()
                       : std::get<std::vector<std::string *> *>(entries)->size();
        }

        int size() const {
            return static_cast<int>(std::holds_alternative<ConstStringListRef>(entries)
                                        ? std::get<ConstStringListRef>(entries).size()
                                        : std::get<std::vector<std::string *> *>(entries)->size());
        }

        float FirstTarget() {
            if (boxes_.empty()) {
                return 0.F;
            }
            const int value = IsHorizontal()
                                  ? boxes_[selected()].x_min - box_.x_min
                                  : boxes_[selected()].y_min - box_.y_min;
            return float(value);
        }

        float SecondTarget() {
            if (boxes_.empty()) {
                return 0.F;
            }
            const int value = IsHorizontal()
                                  ? boxes_[selected()].x_max - box_.x_min
                                  : boxes_[selected()].y_max - box_.y_min;
            return float(value);
        }

    protected:
        int selected_previous_ = selected();
        int selected_focus_ = selected();

        // Mouse click support:
        std::vector<Box> boxes_;
        Box box_;

        // Animation support:
        float first_ = 0.F;
        float second_ = 0.F;
        animation::Animator animator_first_ = animation::Animator(&first_, 0.F);
        animation::Animator animator_second_ = animation::Animator(&second_, 0.F);
        std::vector<animation::Animator> animator_background_;
        std::vector<animation::Animator> animator_foreground_;
        std::vector<float> animation_background_;
        std::vector<float> animation_foreground_;
    };

    // NOLINTNEXTLINE
    Component enhanced_menu(enhanced_menu_option option) {
        return Make<enhanced_menu_base>(std::move(option));
    }

    Component enhanced_menu(ConstStringListRef entries, int *selected, enhanced_menu_option option) {
        option.entries = std::move(entries);
        option.selected = selected;
        return enhanced_menu(option);
    }

    Component Toggle(ConstStringListRef entries, int *selected) {
        return enhanced_menu(std::move(entries), selected, enhanced_menu_option::Toggle());
    }

    Component Toggle(std::vector<std::string *> *entries, int *selected) {
        auto option = enhanced_menu_option::Toggle();
        option.entries = std::move(entries);
        option.selected = selected;
        return enhanced_menu(option);
    }
} // playground

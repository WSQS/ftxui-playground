//
// Created by sophomore on 12/15/24.
//

#include "folder_menu.h"

using namespace ftxui;

namespace playground {
    class folder_menu_base : public ComponentBase{
        path_data data{};
    public:
        folder_menu_base(path_data data): data(std::move(data)) {
            // children_ = data.instantiate();
            // children_ = {{},{}};
        }

    private:
        Element Render() override {
            return vbox({
                       // children_[0]->Render(),
                       separator(),
                       // children_[1]->Render() | yframe | yflex, // the flex is necessary for log to display
                       build_log(data)
                   }) | flex | border;
        }

        bool OnEvent(Event event) override {
            data.log = event.input();
            // if (event == Event::Return)
            //     log = "return";
            // for (Component& child : children_) {      // NOLINT
            //     if (child->OnEvent(event)) {
            //         return true;
            //     }
            // }
            if (children_[data.selected]->OnEvent(event)) {
                // log += " handled";
                return true;
            }
            if (event == Event::ArrowUp) {
                data.selected = std::max(data.selected-1,0);
                data.log += std::to_string(data.selected);
                return true;
            }
            if (event == Event::ArrowDown) {
                data.selected = std::min(data.selected+1,1);
                data.log += std::to_string(data.selected);
                return true;
            }
            return false;
        }

        static Element build_log(const path_data &input_path_data) {
            if (!input_path_data.log.empty())
                return text(input_path_data.log) | border;
            else
                return std::make_shared<Node>();
        };
        // Component ActiveChild() override {
        //     if (children_.empty()) {
        //         return nullptr;
        //     }
        //     return children_[static_cast<size_t>(data.selected) % children_.size()];
        // }
    };

    Component folder_menu() {
        return Make<folder_menu_base>(path_data{});
    }

    Component folder_menu(path_data data) {
        return Make<folder_menu_base>(std::move(data));
    }
}

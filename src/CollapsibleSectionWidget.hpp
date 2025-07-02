// CollapsibleSectionWidget.hpp
#pragma once
#include "Menu.hpp"
#include <string>
#include <vector>

namespace GUI {

class CollapsibleSectionWidget : public Widget {
public:
    CollapsibleSectionWidget(const std::string& title) : title(title), is_open(true) {}

    void AddLine(const std::string& line) {
        lines.push_back(line);
    }

    void Render() override {
        if (ImGui::CollapsingHeader(title.c_str(), &is_open)) {
            for (const auto& line : lines) {
                ImGui::TextUnformatted(line.c_str());
            }
        }
    }

    void Clear() {
        lines.clear();
    }

private:
    std::string title;
    bool is_open;
    std::vector<std::string> lines;
};

} // namespace GUI

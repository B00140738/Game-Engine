// ConsoleWidget.hpp
#pragma once
#include "Menu.hpp"
#include <string>
#include <vector>

namespace GUI {

class ConsoleWidget : public Widget {
public:
    void AddLog(const std::string& log) {
        logs.push_back(log);
        scroll_to_bottom = true;
    }

    void Render() override {
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 150), ImGuiCond_Always);

        ImGui::Begin("Console", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);

        if (ImGui::Button("Clear")) logs.clear();

        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto& log : logs) {
            ImGui::TextUnformatted(log.c_str());
        }

        if (scroll_to_bottom) {
            ImGui::SetScrollHereY(1.0f);
            scroll_to_bottom = false;
        }

        ImGui::EndChild();
        ImGui::End();
    }

private:
    std::vector<std::string> logs;
    bool scroll_to_bottom = false;
};

} // namespace GUI

#pragma once

#include "../lib/imgui/imgui.h"
#include <memory>
#include <string>
#include <vector>

namespace GUI {

class Widget {
public:
  virtual ~Widget() = default;
  virtual void Render() = 0;
};

class Menu {
public:
  Menu(const std::string &title, bool *open = nullptr)
      : m_Title(title), m_Open(open ? open : &m_DefaultOpen) {}

  void AddWidget(std::shared_ptr<Widget> widget) {
    m_Widgets.push_back(widget);
  }

  void Render() {
    if (*m_Open) {
      ImGuiIO &io = ImGui::GetIO();
      float topBarHeight = ImGui::GetFrameHeightWithSpacing(); // use this instead of GetMainMenuBarHeight

      ImVec2 windowPos = ImVec2(0, topBarHeight);
      ImVec2 windowSize = ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y - topBarHeight);

      ImGui::SetNextWindowPos(windowPos);
      ImGui::SetNextWindowSize(windowSize);

      ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoBringToFrontOnFocus;

      ImGui::Begin(m_Title.c_str(), m_Open, window_flags);

      for (const auto &widget : m_Widgets) {
        if (widget) {
          widget->Render();
        }
      }

      ImGui::End();
    }
  }

  void SetOpen(bool open) { *m_Open = open; }

private:
  std::string m_Title;
  std::vector<std::shared_ptr<Widget>> m_Widgets;
  bool m_DefaultOpen = true;
  bool *m_Open;
};

} // namespace GUI

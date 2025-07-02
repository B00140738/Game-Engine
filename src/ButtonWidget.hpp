#pragma once

#include "Menu.hpp"
#include <functional>

namespace GUI {

    class ButtonWidget : public Widget {
    public:
        ButtonWidget(const std::string& label, std::function<void()> onClick)
            : m_Label(label), m_OnClick(onClick) {}

        void Render() override {
            if (ImGui::Button(m_Label.c_str())) {
                if (m_OnClick) m_OnClick();
            }
        }

    private:
        std::string m_Label;
        std::function<void()> m_OnClick;
    };

}

#pragma once

namespace geode {
    enum class MouseButton {
        None        = -1,
        Left        = 0,
        Right       = 1,
        Middle      = 2,
        Prev        = 3,
        Next        = 4,
        DoubleClick = 19,
        ScrollUp    = 20,
        ScrollDown  = 21,
    };

    constexpr const char* mouseToString(MouseButton btn) {
        switch (btn) {
            case MouseButton::None: return "";
            case MouseButton::Left: return "Left Click";
            case MouseButton::DoubleClick: return "Double Click";
            case MouseButton::Right: return "Right Click";
            case MouseButton::Middle: return "Middle Click";
            case MouseButton::Next: return "Mouse Forward";
            case MouseButton::Prev: return "Mouse Back";
            case MouseButton::ScrollUp: return "Scroll Up";
            case MouseButton::ScrollDown: return "Scroll Down";
        }
        return "Mouse Unk";
    }
}


#pragma once

#include <Geode.hpp>

namespace geode::api {
	enum class MouseEvent {
	    Left,
	    Right,
	    Middle,
	    Prev,
	    Next,
	    DoubleClick,
	    ScrollUp,
	    ScrollDown,
	};

	constexpr const char* mouseToString(MouseEvent btn) {
	    switch (btn) {
	        case MouseEvent::Left: return "Left Click";
	        case MouseEvent::DoubleClick: return "Double Click";
	        case MouseEvent::Right: return "Right Click";
	        case MouseEvent::Middle: return "Middle Click";
	        case MouseEvent::Next: return "Mouse Forward";
	        case MouseEvent::Prev: return "Mouse Back";
	        case MouseEvent::ScrollUp: return "Scroll Up";
	        case MouseEvent::ScrollDown: return "Scroll Down";
	    }
	    return "Mouse Unk";
	}
}
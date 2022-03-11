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
}
#pragma once

#include <Geode.hpp>

namespace geode {
	/**
	 * Do NOT change order! 
	 * These are same as GLFW
	 */
	enum class MouseEvent {
	    Left = 0,
	    Right = 1,
	    Middle = 2,
	    Prev = 3,
	    Next = 4,
	    DoubleClick = 8,
	    ScrollUp,
	    ScrollDown,
	};
}

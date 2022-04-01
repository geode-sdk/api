#pragma once

#include <Geode.hpp>
#include <variant>

#include "MouseEvent.hpp"

namespace geode::api {
	using KeyboardEvent = cocos2d::enumKeyCodes;

	enum class KeyModifiers : int {
	    None      = 0b0000,
	    Control   = 0b0001,
	    Shift     = 0b0010,
	    Alt       = 0b0100,
	    Command   = 0b1000,
	};

	inline KeyModifiers operator|(KeyModifiers const& a, KeyModifiers const& b) {
		return (KeyModifiers)((int)a | (int)b);
	}

	using ShortcutEvent = std::variant<KeyboardEvent, MouseEvent, std::monostate>;

	struct Shortcut {
		KeyModifiers modifiers;
		ShortcutEvent input;

		Shortcut() : Shortcut(std::monostate()) {}
		Shortcut(std::variant<KeyboardEvent, MouseEvent, std::monostate> ev, KeyModifiers m)
			: modifiers(m), input(ev) {}
		Shortcut(std::variant<KeyboardEvent, MouseEvent, std::monostate> ev)
			: Shortcut(ev, KeyModifiers::None) {}

		bool accepts(Shortcut const&) const;

		std::string toString() const;
	};
}
#pragma once

#include <Geode.hpp>
#include <Shortcut.hpp>
#include <ShortcutAction.hpp>
#include <string_view>

namespace geode::api {

	class ShortcutManager {
	 protected:
	 	std::unordered_map<std::string_view, ShortcutAction*> m_actions;
	 	std::unordered_map<std::string_view, bool> m_actionState;
	 	static ShortcutManager* shared;
	  public:
	  	static ShortcutManager* get();

	  	void registerShortcut(ShortcutAction sa);
	  	void unregisterShortcut(std::string_view sel);

	  	bool resetToPreferred(std::string_view sel);
	  	bool resetAllToPreferred();

	  	bool remapShortcut(std::string_view sel, Shortcut const& event);

	  	void dispatchEvent(Shortcut const& sc);
	};
}

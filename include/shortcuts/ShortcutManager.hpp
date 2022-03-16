#pragma once

#include "../sdk/include/Geode.hpp"
#include <Shortcut.hpp>
#include <ShortcutAction.hpp>
#include <string>
#include <map>
#include <optional>

namespace geode::api {
	using shortcut_action_id = std::pair<std::string, Mod*>;
	inline bool operator<(shortcut_action_id const& a, shortcut_action_id const& b) {
		return a.first < b.first && a.second < b.second;
	}

	class ShortcutManager {
	 protected:

	 	std::map<shortcut_action_id, ShortcutAction*> m_actions;
	 	std::map<shortcut_action_id, bool> m_actionStates;
	 	static ShortcutManager* shared;

	  public:
	  	static ShortcutManager* get();

	  	Result<shortcut_action_id> registerShortcut(ShortcutAction&& sa);

	  	void unregisterShortcut(shortcut_action_id const& id);
	  	inline void unregisterShortcut(std::string const& sel) {
	  		unregisterShortcut({sel, Interface::mod()});
	  	}

	  	void resetToDefault(shortcut_action_id const& id);
	  	inline void resetToDefault(std::string const& sel) {
	  		resetToDefault({sel, Interface::mod()});
	  	}

	  	void resetAllToDefault();

	  	bool remapShortcut(shortcut_action_id const& id, Shortcut&& event);
	  	inline bool remapShortcut(std::string const& sel, Shortcut&& event) {
	  		remapShortcut({sel, Interface::mod()}, std::move(event));
	  	}

	  	void dispatchEvent(Shortcut const& sc);
	};
}
#pragma once

#include "../sdk/include/Geode.hpp"
#include <Shortcut.hpp>
#include <ShortcutAction.hpp>
#include <string>
#include <map>
#include <optional>

namespace geode {
	using shortcut_action_id = std::pair<std::string, Mod*>;
	inline bool operator<(shortcut_action_id const& a, shortcut_action_id const& b) {
		return a.first < b.first && a.second < b.second;
	}
}

namespace std {
    template<>
    struct hash<shortcut_action_id> {
        inline std::size_t operator()(shortcut_action_id const& action) const {
			return std::hash<std::string>()(std::get<0>(action));
		}
    };
}

namespace geode {
	class ShortcutManager {
	 protected:

	 	std::unordered_map<shortcut_action_id, ShortcutAction*> m_actions;
	 	std::unordered_map<shortcut_action_id, ShortcutActionState> m_actionStates;

	 	static ShortcutManager* shared;

	  public:
	  	static ShortcutManager* get();

	  	Result<shortcut_action_id> registerShortcut(ShortcutAction&& sa);

	  	void unregisterShortcut(shortcut_action_id const& id);
	  	inline void unregisterShortcut(std::string const& sel) {
	  		unregisterShortcut({sel, Mod::get()});
	  	}

	  	void resetToDefault(shortcut_action_id const& id);
	  	inline void resetToDefault(std::string const& sel) {
	  		resetToDefault({sel, Mod::get()});
	  	}

	  	void resetAllToDefault();

	  	bool remapShortcut(shortcut_action_id const& id, Shortcut&& event);
	  	inline bool remapShortcut(std::string const& sel, Shortcut&& event) {
	  		remapShortcut({sel, Mod::get()}, std::move(event));
	  	}

	  	void dispatchEvent(ShortcutEvent const& sc, bool down);
	  	void update(float dt);
	};
}

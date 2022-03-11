#pragma once

#include <Geode.hpp>
#include <Shortcut.hpp>
#include <string>

namespace geode::api {
	struct ShortcutActionSettings {
		Shortcut binding;
		float rapidDelay;
		float rapidRate;
		bool enabled;
		bool rapidEnabled;
	};

	class ShortcutManager;

	class ShortcutAction {
	 protected:
	 	std::string m_selector;
	 	std::string m_sceneTypeID;
	 	ShortcutActionSettings m_currentSettings;
	 	ShortcutActionSettings m_defaultSettings;
	 	Mod* m_owner;

	 	ShortcutAction(Mod* m) : m_owner(m) {}
	 	ShortcutAction(ShortcutAction&& sa) : 
	 	  m_owner(sa.m_owner), 
	 	  m_selector(sa.m_selector),
	 	  m_sceneTypeID(sa.m_sceneTypeID),
	 	  m_currentSettings(sa.m_currentSettings),
	 	  m_defaultSettings(sa.m_defaultSettings) {}

	 public:
	 	template <typename T>
	 	inline static ShortcutAction localShortcut(std::string_view sel, Shortcut&& preferredBind) {
	 		ShortcutAction ret(Interface::mod());
	 		ret.m_selector = sel;
	 		ret.m_sceneTypeID = typeid(T).name();
	 		ret.m_defaultSettings.enabled = true;
	 		ret.m_defaultSettings.binding = preferredBind;
	 		ret.m_defaultSettings.rapidEnabled = false;

	 		ret.m_currentSettings = ret.m_defaultSettings;
	 		return ret;
	 	}

	 	inline static ShortcutAction globalShortcut(std::string_view sel, Shortcut&& preferredBind, float rapidDelay = -1, float rapidRate = -1) {
	 		ShortcutAction ret(Interface::mod());
	 		ret.m_selector = sel;
	 		ret.m_sceneTypeID = "";
	 		ret.m_defaultSettings.enabled = true;
	 		ret.m_defaultSettings.binding = preferredBind;
	 		ret.m_defaultSettings.rapidEnabled = false;

	 		ret.m_currentSettings = ret.m_defaultSettings;
	 		return ret;
	 	}

	 	inline ShortcutActionSettings& getSettings() {return m_currentSettings;}

	 	friend class ShortcutManager;
	};
}
#pragma once

#include <Geode.hpp>
#include <Event.hpp>
#include <DispatchEvent.hpp>

namespace geode {
	class Shortcut;
	class ShortcutSettings;

	class GEODE_API_DLL ShortcutReceptor {
	 protected:
		virtual void activate(Shortcut*) = 0;
		virtual void deactivate(Shortcut*) = 0;
		virtual ~ShortcutReceptor() = default;

		virtual void loadJSON(nlohmann::json&);
		virtual nlohmann::json saveJSON();

		friend ShortcutSettings;
		friend Shortcut;
	};

	struct GEODE_API_DLL ShortcutSettings {
		bool rapidFire;
		int rapidFireRate;
		float rapidFireDelay;
	 protected:
		ShortcutReceptor* receptor;
	 public:
		ShortcutSettings(ShortcutReceptor* receptor = nullptr, int rapidRate = -1, float rapidDelay = 0.0); // -1 will disable rapidFire
		friend Shortcut;
	};

	class GEODE_API_DLL Shortcut {
		static std::vector<Shortcut*> shortcuts;
	 protected:
		ShortcutSettings m_defaultSettings;
		ShortcutSettings m_settings;
		std::string m_name;
		std::function<void(bool)> m_callback;

		std::atomic<bool> m_isFired;

		Shortcut(std::string name, ShortcutSettings defaultSettings, std::function<void(bool)> callback);
	 public:
		static Shortcut* create(std::string name, ShortcutSettings defaultSettings, std::function<void(bool)> callback);

		inline static Shortcut* create(std::string name, std::function<void(bool)> callback) {
			return Shortcut::create(name, ShortcutSettings(), callback);
		}

		ShortcutSettings& currentSettings();
		ShortcutSettings const& defaultSettings() const;
		std::string const& name() const;
		void changeReceptor(ShortcutReceptor* receptor);

		~Shortcut() = delete;

		static std::vector<Shortcut*> const& allShortcuts();

		virtual void fireOn();
		virtual void fireOff();
	};
}

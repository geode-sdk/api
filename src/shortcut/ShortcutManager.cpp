#include <ShortcutManager.hpp>

USE_GEODE_NAMESPACE();

ShortcutManager* ShortcutManager::shared = nullptr;

ShortcutManager* ShortcutManager::get() {
	if (!ShortcutManager::shared)
		ShortcutManager::shared = new ShortcutManager;
	return ShortcutManager::shared;
}

Result<shortcut_action_id> ShortcutManager::registerShortcut(ShortcutAction&& sa) {
	shortcut_action_id id = { sa.m_selector, sa.m_owner };
	if (m_actions.count(id)) {
		return Err<>("Shortcut Action already exists");
	} else {
		auto actionPtr = new ShortcutAction(std::move(sa));
		m_actions[id] = actionPtr;
		m_actionStates[id].eventActive = false;

		return Ok<shortcut_action_id>(id);
	}
}

void ShortcutManager::unregisterShortcut(shortcut_action_id const& id) {
	m_actions.erase(id);
	m_actionStates.erase(id);
}

void ShortcutManager::resetToDefault(shortcut_action_id const& id) {
	ShortcutAction* a = m_actions[id];
	a->m_currentSettings = a->m_defaultSettings;
}

void ShortcutManager::resetAllToDefault() {
	for (auto& a : m_actions) {
		resetToDefault(a.first);
	}
}

bool ShortcutManager::remapShortcut(shortcut_action_id const& id, Shortcut&& event) {
	m_actions[id]->m_currentSettings.binding = event; 
	return true;
}


void ShortcutManager::dispatchEvent(ShortcutEvent const& sc, bool enabled) {

	for (auto [id, action] : m_actions) {
		auto settings = action->m_currentSettings;
		auto& state = m_actionStates[id];
		if (settings.binding.input == sc && state.eventEnabled != enabled) {
			state.timeSinceRapid = 0;
			state.eventActive = enabled;
			state.eventEnabled = enabled;
			state.firstFire = true;
		}
	}
}

void ShortcutManager::update(float dt) {
	for (auto& [id, state] : m_actionStates) {
		if (state.eventActive) {
			Log::get() << "shortcut caught";

			auto action = m_actions[id];
			auto actionSettings = action->m_currentSettings;

			auto dispatcher = CCDirector::sharedDirector()->getKeyboardDispatcher();

			auto modifiers = KeyModifiers::None;
			if (dispatcher->getShiftKeyPressed())
				modifiers = modifiers | KeyModifiers::Shift;
			if (dispatcher->getControlKeyPressed())
				modifiers = modifiers | KeyModifiers::Control;
			if (dispatcher->getCommandKeyPressed())
				modifiers = modifiers | KeyModifiers::Command;
			if (dispatcher->getAltKeyPressed())
				modifiers = modifiers | KeyModifiers::Alt;

			if (actionSettings.binding.modifiers == modifiers) {
				state.timeSinceRapid += dt;
				if (!actionSettings.rapidEnabled || state.timeSinceRapid >= actionSettings.rapidRate || state.firstFire) {
					state.timeSinceRapid = 0;
					state.firstFire = false;

					if (!actionSettings.rapidEnabled) {
						Log::get() << "no more...";
						state.eventActive = false;
					}

					EventCenter::get()->send(
						Event<bool>(
							action->m_selector,
							true,
							Mod::get()
						),
						action->m_owner
					);
				}
			} else {
				if (!state.firstFire) {
					EventCenter::get()->send(
						Event<bool>(
							action->m_selector,
							false,
							Mod::get()
						),
						action->m_owner
					);
				}
				state.timeSinceRapid = 0;
				state.eventActive = false;
			}
		}
	}
}



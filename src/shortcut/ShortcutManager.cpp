#include <ShortcutManager.hpp>

USE_GEODE_NAMESPACE();
using namespace api;

ShortcutManager* ShortcutManager::shared = nullptr;

ShortcutManager* ShortcutManager::get() {
	if (!ShortcutManager::shared)
		ShortcutManager::shared = new ShortcutManager;
	return ShortcutManager::shared;
}

Result<shortcut_action_id> ShortcutManager::registerShortcut(ShortcutAction&& sa) {
	shortcut_action_id id = { sa.m_selector, sa.m_owner };
	if (m_actions.count(id) > 0) {
		return Err<>("Shortcut Action already exists");
	} else {
		auto actionPtr = new ShortcutAction(std::move(sa));
		m_actions[id] = actionPtr;
		m_actionStates[id] = false;

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
#include <settings/Setting.hpp>
#include <unordered_map>

USE_GEODE_NAMESPACE();

bool SettingManager::hasSettings() {
	return !this->m_settings.empty();
}

SettingManager* SettingManager::with(Mod* m) {
    static std::unordered_map<Mod*, SettingManager*> managers;
    if (!managers.count(m))
        managers[m] = new SettingManager(m);
    return managers[m];
}

SettingManager::SettingManager(Mod* m) {
	this->m_mod = m;
	auto root = this->m_mod->getDataStore()["settings"];

	if (!root.is_object()) {
		return;
	}

	for (auto [id, setting] : root.items()) {
		if (!setting.is_object()) {
			FLAlertLayer::create(
				"Failed to load settings",
				std::string("JSON error: '") + id + "' key is not an object!",
				"OK"
			)->show();
			return;
		}

		auto ctrl = setting["control"];
		if (!ctrl.is_string()) {
			FLAlertLayer::create(
				"Failed to load settings",
				"JSON error: 'control' key is not a string (or doesn't exist)!",
				"OK"
			)->show();
			return;
		}
		std::string control = ctrl;

		Setting* out = nullptr;
		EventCenter::get()->broadcast(Event(
			// events::getSetting(id),
			id,
			&out,
			Mod::get()
		));

		if (out == nullptr) {
			FLAlertLayer::create(
				"Failed to load settings",
				std::string("No known setting control '") + control + "'",
				"OK"
			)->show();
			return;
		}
		this->m_settings[id] = out;
	}
}

Setting* SettingManager::getSetting(std::string id) {
	return this->m_settings[id];
}

void SettingManager::updateSetting(std::string id) {
	this->m_mod->getDataStore()["settings"][id] = this->getSetting(id)->saveJSON();
}

std::vector<CCNode*> SettingManager::generateSettingNodes() {
	std::vector<CCNode*> out;
	for (auto [k, v] : this->m_settings) {
		out.push_back(v->createControl());
	}
	return out;
}

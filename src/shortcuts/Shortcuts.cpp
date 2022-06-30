#include <Shortcuts.hpp>
#include <thread>
#include <chrono>

USE_GEODE_NAMESPACE();

void ShortcutReceptor::loadJSON(nlohmann::json& j) {}
nlohmann::json ShortcutReceptor::saveJSON() {
	return nlohmann::json::object();
}

ShortcutSettings::ShortcutSettings(ShortcutReceptor* receptor, int rapidRate, float rapidDelay) {
	this->receptor = receptor ;
	this->rapidFireRate = rapidRate;
	this->rapidFireDelay = rapidDelay;
	this->rapidFire = rapidRate != -1;
}

std::vector<Shortcut*> Shortcut::shortcuts = {};

Shortcut::Shortcut(std::string name, ShortcutSettings defaultSettings, std::function<void(bool)> callback) : m_name(name), m_defaultSettings(defaultSettings), m_callback(callback) {
	// shit that populates m_settings
	Shortcut::shortcuts.push_back(this);
}

Shortcut* Shortcut::create(std::string name, ShortcutSettings defaultSettings, std::function<void(bool)> callback) {
	return new Shortcut(name, defaultSettings, callback);
}

ShortcutSettings& Shortcut::currentSettings() {
	return m_settings;
}

ShortcutSettings const& Shortcut::defaultSettings() const {
	return m_defaultSettings;
}

std::string const& Shortcut::name() const {
	return m_name;
}

void Shortcut::changeReceptor(ShortcutReceptor* newReceptor) {
	if (m_settings.receptor) {
		m_settings.receptor->deactivate(this);
		delete m_settings.receptor;
	}

	m_settings.receptor = newReceptor;
	newReceptor->activate(this);
}

std::vector<Shortcut*> const& Shortcut::allShortcuts() {
	return Shortcut::shortcuts;
}

void Shortcut::fireOn() {
	if (m_isFired) return;

	m_callback(true);
	m_isFired = true;

	if (m_settings.rapidFire) {
		std::thread([this] {
			std::this_thread::sleep_for(std::chrono::nanoseconds((long long)(1000000000 * m_settings.rapidFireDelay)));

			while (m_isFired) {
				std::this_thread::sleep_for(std::chrono::nanoseconds((long long)(1000000000 * m_settings.rapidFireRate)));
				Loader::get()->queueInGDThread(std::bind(m_callback, true));
			}
		}).detach();
	}
}

void Shortcut::fireOff() {
	if (!m_isFired) return;

	m_callback(false);
	m_isFired = false;
}

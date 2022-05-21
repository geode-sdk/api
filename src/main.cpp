#include <Geode.hpp>
#include "APIInternal.hpp"
#include <Events.hpp>
#include <shortcuts/ShortcutManager.hpp>
#include <index/Index.hpp>
#include <general/Notification.hpp>
#include <thread>

USE_GEODE_NAMESPACE();

$observe("test-keybind") {
	Notification::build()
		.title("Gay sex")
		.text("Have you ever like uhh and the uhhh yeah uhhhh like that yeah")
		.time(1)
		.location(static_cast<NotificationLocation>(rand() % 6))
		.show();
}

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
	Interface::get()->init(mod);

    ExtMouseDispatcher::get()->registerDispatcher();

    // if (!Index::get()->isIndexUpdated()) {
    //     Index::get()->updateIndex(); 
    // }

    ShortcutManager::get()->registerShortcut(ShortcutAction::globalShortcut(
        "test-keybind",
        Shortcut(
            KEY_T,
            KeyModifiers::Control
        )
    ));

    EventCenter::get()->registerObserver<ghc::filesystem::path>(
       "dragdrop.geode", [](auto const& data) {
            auto path = data.object();
            auto to_file = Loader::get()->getGeodeDirectory() / geodeModDirectory / path.filename();

            if (to_file == path) {
                FLAlertLayer::create("Oops!", "<cr>" + path.stem().u8string() + "</c> is already installed!", "OK")->show();
            } else if (ghc::filesystem::copy_file(path, to_file, ghc::filesystem::copy_options::overwrite_existing)) {
                FLAlertLayer::create("Success!", "<cg>" + path.stem().u8string() + "</c> successfully installed!", "OK")->show();
            } else {
                FLAlertLayer::create("Oops!", "<cr>" + path.stem().u8string() + "</c> couldn't be installed!", "OK")->show();
            }
        }
    );

    return true;
}

GEODE_API bool GEODE_CALL geode_unload() {
    ExtMouseDispatcher::get()->unregisterDispatcher();
    return true;
}

GEODE_API bool GEODE_CALL geode_load_data(const char* path) {
    auto settingsPath = ghc::filesystem::path(path) / "api.json";
    if (ghc::filesystem::exists(settingsPath)) {
        try {
            auto data = file_utils::readString(settingsPath);
            if (!data) return false;
            if (!APIInternal::get()->load(nlohmann::json::parse(data.value())))
                return false;
        } catch(...) {
            return false;
        }
    }
    return true;
} 

GEODE_API bool GEODE_CALL geode_save_data(const char* path) {
    auto settingsPath = ghc::filesystem::path(path) / "api.json";
    auto json = nlohmann::json::object();
    if (!APIInternal::get()->save(json))
        return false;
    return file_utils::writeString(settingsPath, json.dump(4)).is_value();
} 

#include <Geode.hpp>
#include <GeodeAPI.hpp>
#include "APIInternal.hpp"

USE_GEODE_NAMESPACE();

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
	Interface::get()->init(mod);
    
    #ifdef GEODE_IS_WINDOWS
    mod->with<GeodeAPI>()->addKeybindAction(TriggerableAction {
        "Keybind Test",
        "test_keybind",
        KB_GLOBAL_CATEGORY,
        [](CCNode* node, bool down) -> bool {
            if (down) {
                FLAlertLayer::create("Hey", "Keybinds work", "OK")->show();
            }
            return false;
        }
    }, {{ KEY_T, Keybind::Modifiers::Control | Keybind::Modifiers::Alt }});

    mod->with<GeodeAPI>()->addDragDropHandler("geode_mod_installer", [](Path path) -> bool {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        std::string newloc = std::string(buffer).substr(0, pos) + "/geode/mods/" + path.file_name + ".geode";

        if (!MoveFile(path.absolute_path.c_str(), newloc.c_str()) && !CopyFile(path.absolute_path.c_str(), newloc.c_str(), false)) {
            FLAlertLayer::create("Oops!", path.file_name + " couldn't be installed!", "OK")->show();
        } else {
            FLAlertLayer::create("Success!", path.file_name + " successfully installed!", "OK")->show();
        }
        return true;
    }, ".geode");
    #endif

    return true;
}

GEODE_API bool GEODE_CALL geode_load_data(const char* path) {
    Interface::mod()->log() << __FUNCTION__ << geode::endl;
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

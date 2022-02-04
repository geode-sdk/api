#include <Geode>
#include <GeodeAPI>

USE_GEODE_NAMESPACE();

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
    Interface::get()->init(mod);

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
    }, {{ Keybind::Modifiers::Control | Keybind::Modifiers::Alt, KEY_T }});

    return true;
}

GEODE_API void GEODE_CALL geode_unload() {}

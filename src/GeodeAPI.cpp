#include <GeodeAPI.hpp>
#include <settings/SettingNodeManager.hpp>

USE_GEODE_NAMESPACE();

bool GeodeAPI::addKeybindAction(
    KeybindAction     const& action,
    KeybindList       const& defaults,
    keybind_action_id const& insertAfter
) {
    return KeybindManager::get()->addKeybindAction(
        Mod::get(), action, defaults, insertAfter
    );
}

bool GeodeAPI::removeKeybindAction(keybind_action_id const& id) {
    return KeybindManager::get()->removeKeybindAction(Mod::get(), id);
}

Result<> GeodeAPI::setCustomSettingNode(
    std::string const& key,
    CustomSettingNodeGenerator generator
) {
    if (SettingNodeManager::get()->registerCustomNode(Mod::get(), key, generator)) {
        return Ok<>();
    } else {
        return Err<>("Unknown error");
    }
}

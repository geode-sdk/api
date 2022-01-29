#include <GeodeAPI.hpp>

bool GeodeAPI::addKeybindAction(
    KeybindAction     const& action,
    KeybindList       const& defaults,
    keybind_action_id const& insertAfter
) {
    return KeybindManager::get()->addKeybindAction(
        this, action, defaults, insertAfter
    );
}

bool GeodeAPI::removeKeybindAction(keybind_action_id const& id) {
    return KeybindManager::get()->removeKeybindAction(this, id);
}


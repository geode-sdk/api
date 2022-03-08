#pragma once

#include <Geode.hpp>
#include "keybinds/KeybindManager.hpp"
#include "settings/CustomSettingNode.hpp"
#include "nodes/BasedButtonSprite.hpp"
#include "nodes/BasedButton.hpp"


#define EXPORT_NAME GeodeAPI
#include <API.hpp>

namespace geode {
    class GEODE_API_DLL GeodeAPI : public ModAPI {
    public:
        API_INIT("com.geode.api");

        /**
         * Add a new keybind action, i.e. a 
         * function that can be bound to a keybind.
         * @param action A KeybindAction; either 
         * TriggerableAction, ModifierAction or 
         * RepeatableAction.
         * @param defaults Default keybinds for 
         * this action.
         * @param insertAfter Where to insert 
         * this action in the in-game list. 
         * `nullptr` means to insert at the end.
         * @returns True if the action was added, 
         * false if not. If the function returns 
         * false, it's probably the action's ID 
         * being invalid / colliding with another 
         * action's ID.
         */
        static bool addKeybindAction(
            KeybindAction     const& action,
            KeybindList       const& defaults,
            keybind_action_id const& insertAfter = nullptr
        ) API_DECL(&GeodeAPI::addKeybindAction, action, defaults, insertAfter);
        
        /**
         * Remove a keybind action.
         * @param id ID of the action.
         * @returns True if the action was 
         * removed, false if not.
         */
        static bool removeKeybindAction(keybind_action_id const& id)
        API_DECL(&GeodeAPI::removeKeybindAction, id);

        Result<> setCustomSettingNode(
            std::string const& key,
            CustomSettingNodeGenerator generator
        ) API_DECL(&GeodeAPI::setCustomSettingNode, key, generator);
    };
}

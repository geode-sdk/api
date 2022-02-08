#pragma once

#include <Geode.hpp>
#include "keybinds/KeybindManager.hpp"
#include "settings/CustomSettingNode.hpp"
#include "nodes/BasedButtonSprite.hpp"

namespace geode {
    class GEODE_API_DLL GeodeAPI : public APIMod {
    public:
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
        bool addKeybindAction(
            KeybindAction     const& action,
            KeybindList       const& defaults,
            keybind_action_id const& insertAfter = nullptr
        );
        
        /**
         * Remove a keybind action.
         * @param id ID of the action.
         * @returns True if the action was 
         * removed, false if not.
         */
        bool removeKeybindAction(keybind_action_id const& id);

        Result<> setCustomSettingNode(
            std::string const& key,
            CustomSettingNodeGenerator generator
        );
    };
}

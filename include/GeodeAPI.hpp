#pragma once

#include <Geode.hpp>
#include "keybinds/KeybindManager.hpp"
#include "dragdrop/DragDropManager.hpp"
#include "settings/CustomSettingNode.hpp"
#include "nodes/BasedButtonSprite.hpp"
#include "nodes/BasedButton.hpp"

namespace geode {
    class GEODE_API_DLL GeodeAPI : public Mod {
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

        /**
         * Add a new file drag and drop handler
         * @param handler_id A unique identifier 
         * for the handler.
         * @param handler A function that takes a 
         * ghc::filesystem::path object and returns a bool that
         * signifies whether you'd like to stop
         * further handlers from being triggered.
         * @returns True if the action was added, 
         * false if not. If the function returns 
         * false, it's probably the action's ID 
         * being invalid / colliding with another 
         * action's ID.
         */
        bool addDragDropHandler(
            std::string const& handler_id,
            std::function<bool(ghc::filesystem::path const&)> handler
        );

        /**
         * Add a new file drag and drop handler
         * @param handler_id A unique identifier 
         * for the handler.
         * @param handler A function that takes a 
         * ghc::filesystem::path object and returns a bool that
         * signifies whether you'd like to stop
         * @param extension The file extension you'd
         * like the handler to listen for.
         * @returns True if the action was added, 
         * false if not. If the function returns 
         * false, it's probably the action's ID 
         * being invalid / colliding with another 
         * action's ID.
         */
        bool addDragDropHandler(
            std::string const& handler_id,
            std::function<bool(ghc::filesystem::path const&)> handler,
            std::string const& extension
        );

        /**
         * Remove a Drag+Drop handler.
         * @param id ID of the handler.
         * @returns True if the handler was 
         * removed, false if not.
         */
        bool removeDragDropHandler(std::string const& handler_id);

        Result<> setCustomSettingNode(
            std::string const& key,
            CustomSettingNodeGenerator generator
        );
    };
}

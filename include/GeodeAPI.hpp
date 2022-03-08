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
        static bool addDragDropHandler(
            std::string const& handler_id,
            std::function<bool(ghc::filesystem::path)> handler
        ) API_DECL( 
            (bool(*)(
                std::string const&,
                std::function<bool(ghc::filesystem::path)>
            ))&GeodeAPI::addDragDropHandler,
            handler_id,
            handler
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
        static bool addDragDropHandler(
            std::string const& handler_id,
            std::function<bool(ghc::filesystem::path)> handler,
            std::string const& extension
        ) API_DECL( 
            (bool(*)(
                std::string const&,
                std::function<bool(ghc::filesystem::path)>,
                std::string const&
            ))&GeodeAPI::addDragDropHandler,
            handler_id,
            handler
        );

        /**
         * Remove a Drag+Drop handler.
         * @param id ID of the handler.
         * @returns True if the handler was 
         * removed, false if not.
         */
        static bool removeDragDropHandler(std::string const& handler_id)
        API_DECL(&GeodeAPI::removeDragDropHandler, handler_id);

        Result<> setCustomSettingNode(
            std::string const& key,
            CustomSettingNodeGenerator generator
        ) API_DECL(&GeodeAPI::setCustomSettingNode, key, generator);
    };
}

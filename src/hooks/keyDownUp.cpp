#include "hook.hpp"

/*doesnt work on macos */
#ifdef GEODE_IS_WINDOWS

// class $modify(UILayer) {
//     void keyDown(enumKeyCodes key) {
//         KeybindManager::get()->handleKeyEvent(
//             KB_PLAY_CATEGORY, Keybind(key), PlayLayer::get(), true
//         );
//     }

//     void keyUp(enumKeyCodes key) {
//         KeybindManager::get()->handleKeyEvent(
//             KB_PLAY_CATEGORY, Keybind(key), PlayLayer::get(), false
//         );
//     }
// };

// class $modify(EditorUI) {
//     void keyDown(enumKeyCodes key) {
//         KeybindManager::get()->handleKeyEvent(
//             KB_EDITOR_CATEGORY, Keybind(key), this, true
//         );
//     }

//     void keyUp(enumKeyCodes key) {
//         KeybindManager::get()->handleKeyEvent(
//             KB_EDITOR_CATEGORY, Keybind(key), this, false
//         );
//     }
// };

#endif

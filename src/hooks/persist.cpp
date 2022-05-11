#include "hook.hpp"
#include <general/SceneManager.hpp>

class $modify(AppDelegate) {
    void willSwitchToScene(CCScene* scene) {
        AppDelegate::willSwitchToScene(scene);
        SceneManager::get()->willSwitchToScene(scene);
    }
};

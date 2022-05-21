#include "hook.hpp"
#include <dispatch/ExtMouseDispatcher.hpp>
#include <shortcuts/ShortcutManager.hpp>

class $modify(CCTouchDispatcher) {
    void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int touchType) {
        return ExtMouseDispatcher::get()->touches(touches, event, touchType);
    }
};

#ifdef GEODE_IS_DESKTOP
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        ShortcutManager::get()->dispatchEvent(key, down);
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};

class $modify(CCMouseDispatcher) {
    bool dispatchScrollMSG(float x, float y) {
        return ExtMouseDispatcher::get()->dispatchScrollMSG(x, y);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        ExtMouseDispatcher::get()->update();
        ShortcutManager::get()->update(dt);
        return CCScheduler::update(dt);
    }
};
#endif

#ifdef GEODE_IS_WINDOWS
class $modify(CCEGLView) {
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        if (action == 1) {
            ExtMouseDispatcher::get()->mouseDown(
                static_cast<MouseEvent>(button)
            );
        } else {
            ExtMouseDispatcher::get()->mouseUp(
                static_cast<MouseEvent>(button)
            );
        }
        return CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    }
};
#endif

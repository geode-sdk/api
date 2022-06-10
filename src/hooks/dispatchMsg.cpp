#include "hook.hpp"
#include <dispatch/ExtMouseDispatcher.hpp>
#include <shortcuts/ShortcutManager.hpp>

// override CCTouchDispatcher with ExtMouseDispatcher
class $modify(CCTouchDispatcher) {
    void touches(CCSet* touches, CCEvent* event, unsigned int touchType) {
        #ifdef GEODE_IS_MOBILE
        if (touchType == CCTOUCHMOVED) {
            ExtMouseDispatcher::get()->update();
        }
        #endif
        ExtMouseDispatcher::get()->touches(touches, event, touchType);
    }

    // fixes for touch dispatcher discrepancies, by alk

    void addTargetedDelegate(CCTouchDelegate* delegate, int priority, bool swallowsTouches) {
        m_bForcePrio = false;
        if (m_pTargetedHandlers->count() > 0) {
            auto handler = static_cast<CCTouchHandler*>(m_pTargetedHandlers->objectAtIndex(0));
            priority = handler->getPriority() - 2;
        }
        CCTouchDispatcher::addTargetedDelegate(delegate, priority, swallowsTouches);
    }

    void incrementForcePrio(int num) {
        m_bForcePrio = false;
    }

    void decrementForcePrio(int num) {
        m_bForcePrio = false;
    }
};

// pop ExtMouseDispatcher delegates.
// CCTouchDispatcher etc. pop their delegates in 
// CCLayer::onExit aswell
class $modify(CCNode) {
    void onExit() {
        if (auto d = dynamic_cast<ExtMouseDelegate*>(static_cast<CCNode*>(this))) {
            ExtMouseDispatcher::get()->popDelegate(d);
        }
        CCNode::onExit();
    }
};

#ifdef GEODE_IS_DESKTOP
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        ShortcutManager::get()->dispatchEvent(key, down);
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};

// override CCMouseDispatcher with ExtMouseDispatcher
class $modify(CCMouseDispatcher) {
    bool dispatchScrollMSG(float x, float y) {
        return ExtMouseDispatcher::get()->dispatchScrollMSG(x, y);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        // dispatch mouse move events
        ExtMouseDispatcher::get()->update();
        // handle shortcut repeating
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
        if (!ExtMouseDispatcher::get()->dispatchMouseEvent(
            static_cast<MouseEvent>(button),
            action == 1, 
            ExtMouseDispatcher::getMousePosition()
        )) {
            CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
        }
    }
};
#endif

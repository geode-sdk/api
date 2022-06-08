#include "hook.hpp"
#include <dispatch/ExtMouseDispatcher.hpp>
#include <shortcuts/ShortcutManager.hpp>

class $modify(CCTouchDispatcher) {
    void touches(CCSet* touches, CCEvent* event, unsigned int touchType) {
        return ExtMouseDispatcher::get()->touches(touches, event, touchType);
    }

    void removeDelegate(CCTouchDelegate* delegate) {
        // this is to check if we have replaced 
        // ExtMouseDispatcher
        std::cout << "CCTouchDispatcher::removeDelegate\n";
        if (as<CCTouchDispatcher*>(this) == ExtMouseDispatcher::get()) {
            std::cout << "-> is ExtMouseDispatcher\n";
            auto d = static_cast<ExtMouseDelegate*>(delegate);
            if (d) {
                std::cout << "   -> is ExtMouseDelegate\n";
                if (ExtMouseDispatcher::get()->isPopLocked(d)) {
                    std::cout << "     -> is pop locked\n";
                    return;
                }
                std::cout << "     -> popping delegate\n";
                return ExtMouseDispatcher::get()->popDelegate(d);
            }
        }
        return CCTouchDispatcher::removeDelegate(delegate);
    }
};

class $modify(CCMouseDispatcher) {
    void removeDelegate(CCMouseDelegate* delegate) {
        // this symbol is shared with CCKeyboardDispatcher and 
        // CCKeypadDispatcher
        std::cout << "CCMouseDispatcher::removeDelegate\n";
        if (as<CCMouseDispatcher*>(this) == static_cast<CCMouseDispatcher*>(
            ExtMouseDispatcher::get()
        )) {
            std::cout << " -> is ExtMouseDispatcher\n";
            auto d = static_cast<ExtMouseDelegate*>(delegate);
            if (d) {
                std::cout << "   -> is ExtMouseDelegate\n";
                if (ExtMouseDispatcher::get()->isPopLocked(d)) {
                    std::cout << "     -> is pop locked\n";
                    return;
                }
                std::cout << "     -> popping delegate\n";
                return ExtMouseDispatcher::get()->popDelegate(d);
            }
        }
        return CCMouseDispatcher::removeDelegate(delegate);
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

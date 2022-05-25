#include "hook.hpp"
#include <dispatch/ExtMouseManager.hpp>

#ifdef GEODE_IS_DESKTOP
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        //ShortcutManager::get()->dispatchEvent(key, down);
        #warning no shortcut
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};

class $modify(CCMouseDispatcher) {
    bool dispatchScrollMSG(float x, float y) {
        ExtMouseManager::get()->dispatchScrollEvent(
            y, x, ExtMouseManager::getMousePosition()
        );
        return CCMouseDispatcher::dispatchScrollMSG(x, y);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        ExtMouseManager::get()->dispatchMoveEvent(
            ExtMouseManager::getMousePosition()
        );
        //ShortcutManager::get()->update(dt);
        #warning no shortcut
        return CCScheduler::update(dt);
    }
};
#endif

#if defined(GEODE_IS_MOBILE)
class $modify(CCTouchDispatcher) {
    void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int touchType) {
        auto touch = touches->anyObject();
        if (touch) {
            switch (touchType) {
                case CCTOUCHBEGAN: {
                    if (ExtMouseManager::get()->dispatchClickEvent(
                        MouseEvent::Left, true, as<CCTouch*>(touch)->getLocation()
                    )) return;
                } break;

                case CCTOUCHENDED: case CCTOUCHCANCELLED: {
                    if (ExtMouseManager::get()->dispatchClickEvent(
                        MouseEvent::Left, false, as<CCTouch*>(touch)->getLocation()
                    )) return;
                } break;

                case CCTOUCHMOVED: {
                    ExtMouseManager::get()->dispatchMoveEvent(
                        as<CCTouch*>(touch)->getLocation()
                    );
                } break;

                default: break;
            }
        }
        return CCTouchDispatcher::touches(touches, event, touchType);
    }
};
#endif

#ifdef GEODE_IS_WINDOWS
class $modify(CCEGLView) {
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        if (ExtMouseManager::get()->dispatchClickEvent(
            static_cast<MouseEvent>(button), action,
            ExtMouseManager::getMousePosition()
        )) return;
        return CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    }
    
    void pollEvents() {
        // MSG msg;
		// while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        //     TranslateMessage(&msg);
        //     if (msg.message == WM_DROPFILES) {
        //         UINT buffsize = MAX_PATH;
        //         char* buf = new char[MAX_PATH];
        //         HDROP hDropInfo = (HDROP) msg.wParam;
        //         DragQueryFile(hDropInfo, 0, buf, buffsize);

        //         ghc::filesystem::path p(buf);

        //         std::string fileExtension = p.extension().u8string();
        //         if (fileExtension.at(0) == '.') {
        //             fileExtension = fileExtension.substr(1);
        //         }

        //         NotificationCenter::get()->broadcast(Notification(
        //             "dragdrop",
        //             p,
        //             Mod::get()
        //         ));

        //         NotificationCenter::get()->broadcast(Notification(
        //             std::string("dragdrop.") + fileExtension,
        //             p,
        //             Mod::get()            
        //         ));

        //         delete[] buf;
        //     }
        //     DispatchMessage(&msg);
        // }
        CCEGLView::pollEvents();
    }
};
#endif

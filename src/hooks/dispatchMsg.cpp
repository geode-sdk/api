#include "hook.hpp"

class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        // KeybindManager::get()->registerKeyPress(key, down);
        // if (!KeybindManager::keyIsModifier(key)) {
        //     if (KeybindManager::get()->handleKeyEvent(
        //         KB_GLOBAL_CATEGORY,
        //         Keybind(key),
        //         CCDirector::sharedDirector()->getRunningScene(),
        //         down
        //     )) return true;
        // }


        geode::api::ShortcutManager::get()->dispatchEvent(key, down);
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        // KeybindManager::get()->handleRepeats(dt);
        geode::api::ShortcutManager::get()->update(dt);
        return CCScheduler::update(dt);
    }
};

#ifdef GEODE_IS_WINDOWS

class $modify(CCEGLView) {
    void onGLFWMouseCallBack(GLFWwindow* wnd, int btn, int pressed, int z) {
        // KeybindManager::get()->registerMousePress(
        //     static_cast<MouseButton>(btn), pressed
        // );
        return CCEGLView::onGLFWMouseCallBack(wnd, btn, pressed, z);
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

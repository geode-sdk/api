#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "../dispatch/ExtMouseManager.hpp"
#include "SceneManager.hpp"
#include <chrono>

namespace geode {
    enum class NotificationLocation {
        TopLeft,
        TopCenter,
        TopRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
    };

    static constexpr const float g_defaultNotificationTime = 3.f;
    static constexpr const NotificationLocation g_platformNotificationLocation = 
    #ifdef GEODE_IS_DESKTOP 
        NotificationLocation::BottomRight;
    #else
        NotificationLocation::TopCenter;
    #endif;

    class Notification;
    class NotificationManager;

    struct GEODE_API_DLL NotificationBuilder {
        std::string m_title = "";
        std::string m_text = "";
        std::string m_icon = "GJ_infoIcon_001.png";
        cocos2d::CCNode* m_iconNode = nullptr;
        std::string m_bg = "GJ_square02.png";
        float m_time = g_defaultNotificationTime;
        NotificationLocation m_location = g_platformNotificationLocation;

        inline NotificationBuilder& title(std::string const& title) {
            m_title = title;
            return *this;
        }
        inline NotificationBuilder& text(std::string const& text) {
            m_text = text;
            return *this;
        }
        inline NotificationBuilder& icon(std::string const& icon) {
            m_icon = icon;
            m_iconNode = nullptr;
            return *this;
        }
        inline NotificationBuilder& icon(cocos2d::CCNode* icon) {
            m_icon = "";
            m_iconNode = icon;
            return *this;
        }
        inline NotificationBuilder& loading() {
            return this->icon(LoadingCircle::create());
        }
        inline NotificationBuilder& bg(std::string const& bg) {
            m_bg = bg;
            return *this;
        }
        inline NotificationBuilder& location(NotificationLocation location) {
            m_location = location;
            return *this;
        }
        inline NotificationBuilder& time(float time) {
            m_time = time;
            return *this;
        }
        Notification* show();
    };

    class GEODE_API_DLL Notification :
        public cocos2d::CCNode,
        public ExtMouseDelegate
    {
    protected:
        cocos2d::CCObject* m_target = nullptr;
        cocos2d::SEL_MenuHandler m_selector = nullptr;
        cocos2d::extension::CCScale9Sprite* m_bg;
        cocos2d::CCNode* m_icon = nullptr;
        cocos2d::CCLabelBMFont* m_title = nullptr;
        cocos2d::CCArray* m_labels = nullptr;
        cocos2d::CCPoint m_showDest;
        cocos2d::CCPoint m_hideDest;
        cocos2d::CCPoint m_touchStart;
        cocos2d::CCPoint m_posAtTouchStart;
        std::chrono::system_clock::time_point m_touchTime;
        NotificationLocation m_location;
        float m_time;
        bool m_hiding;

        bool init(
            std::string const& title,
            std::string const& text,
            cocos2d::CCNode* icon,
            const char* bg
        );

        Notification();
        virtual ~Notification();

        void mouseEnterExt(cocos2d::CCPoint const& pos) override;
        void mouseLeaveExt(cocos2d::CCPoint const& pos) override;
        void mouseMoveExt(cocos2d::CCPoint const& pos) override;
        bool mouseUpExt(MouseEvent button, cocos2d::CCPoint const& pos) override;
        bool mouseDownExt(MouseEvent button, cocos2d::CCPoint const& pos) override;

        void clicked();

        void animateIn();
        void animateOut();

        void hidden();
        void showForReal();

        friend class NotificationManager;

    public:
        static Notification* create(
            std::string const& title,
            std::string const& text,
            cocos2d::CCNode* icon,
            const char* bg
        );
        static NotificationBuilder build();

        void show(
            NotificationLocation = g_platformNotificationLocation,
            float time = g_defaultNotificationTime
        );
        void hide();
    };

    class NotificationManager {
    protected:
        std::unordered_map<NotificationLocation, std::vector<Notification*>> m_notifications;

        void push(Notification*);
        void pop(Notification*);

        friend class Notification;
    
    public:
        static NotificationManager* get();
    };
}

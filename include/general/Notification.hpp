#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "../dispatch/ExtMouseManager.hpp"
#include "SceneManager.hpp"

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
        struct Button {
            std::string m_text = "";
            cocos2d::CCObject* m_sender = nullptr;
            cocos2d::SEL_MenuHandler m_handler = nullptr;
        };
        std::string m_title = "";
        std::string m_text = "";
        std::string m_icon = "GJ_infoIcon_001.png";
        cocos2d::CCNode* m_iconNode = nullptr;
        std::string m_bg = "GJ_square02.png";
        float m_time = g_defaultNotificationTime;
        NotificationLocation m_location = g_platformNotificationLocation;
        std::vector<Button> m_buttons = {};

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
        inline NotificationBuilder& button(
            std::string const& text,
            cocos2d::CCObject* sender,
            cocos2d::SEL_MenuHandler handler
        ) {
            m_buttons.push_back({ text, sender, handler });
            return *this;
        }
        Notification* show();
    };

    class GEODE_API_DLL Notification :
        public cocos2d::CCNode
        // public ExtMouseDelegate
    {
    protected:
        cocos2d::extension::CCScale9Sprite* m_bg;
        cocos2d::CCNode* m_icon = nullptr;
        cocos2d::CCLabelBMFont* m_title = nullptr;
        cocos2d::CCArray* m_labels = nullptr;
        CCMenuItemSpriteExtra* m_closeBtn = nullptr;
        cocos2d::CCMenu* m_menu;
        cocos2d::CCArray* m_buttons = nullptr;
        cocos2d::CCPoint m_hideDest;
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

        // void mouseEnterExt(cocos2d::CCPoint const& pos) override;
        // void mouseLeaveExt(cocos2d::CCPoint const& pos) override;
        // bool mouseUpExt(MouseEvent button, cocos2d::CCPoint const& pos) override;

        void clicked();

        void onHide(CCObject*);
        void hide();
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

        void addButton(
            std::string const& title,
            cocos2d::CCObject* target,
            cocos2d::SEL_MenuHandler handler
        );

        void show(
            NotificationLocation = g_platformNotificationLocation,
            float time = g_defaultNotificationTime
        );
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

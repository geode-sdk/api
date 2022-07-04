#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "../dispatch/ExtMouseDispatcher.hpp"
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

    static constexpr const float DEFAULT_NOTIFICATION_TIME = 4.f;
    static constexpr const NotificationLocation PLATFORM_NOTIFICATION_LOCATION = 
    #ifdef GEODE_IS_DESKTOP 
        NotificationLocation::BottomRight;
    #else
        NotificationLocation::TopCenter;
    #endif;

    class Notification;
    class NotificationManager;

    struct GEODE_API_DLL NotificationBuilder {
        Mod* m_owner = Mod::get();
        std::string m_title = "";
        std::string m_text = "";
        std::string m_icon = "GJ_infoIcon_001.png";
        cocos2d::CCNode* m_iconNode = nullptr;
        std::string m_bg = "GJ_square02.png";
        float m_time = DEFAULT_NOTIFICATION_TIME;
        NotificationLocation m_location = PLATFORM_NOTIFICATION_LOCATION;

        inline NotificationBuilder& from(Mod* owner) {
            m_owner = owner;
            return *this;
        }
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
            auto loadingCircle = cocos2d::CCSprite::create("loadingCircle.png");
            loadingCircle->runAction(cocos2d::CCRepeat::create(
                cocos2d::CCRotateBy::create(1.f, 360.f), 40000
            ));
            return this->icon(loadingCircle);
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
        inline NotificationBuilder& stay() {
            m_time = .0f;
            return *this;
        }
        Notification* show();
    };

    class GEODE_API_DLL Notification : public cocos2d::CCLayer {
    protected:
        Mod* m_owner;
        cocos2d::CCObject* m_target = nullptr;
        cocos2d::SEL_MenuHandler m_selector = nullptr;
        cocos2d::extension::CCScale9Sprite* m_bg;
        cocos2d::CCNode* m_icon = nullptr;
        cocos2d::CCLabelBMFont* m_title = nullptr;
        cocos2d::CCArray* m_labels = nullptr;
        cocos2d::CCPoint m_showDest;
        cocos2d::CCPoint m_hideDest;
        cocos2d::CCPoint m_posAtTouchStart;
        std::chrono::system_clock::time_point m_touchTime;
        NotificationLocation m_location;
        float m_time;
        bool m_hiding;

        bool init(
            Mod* owner,
            std::string const& title,
            std::string const& text,
            cocos2d::CCNode* icon,
            const char* bg
        );

        Notification();
        virtual ~Notification();

        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void registerWithTouchDispatcher() override;

        void clicked();

        void animateIn();
        void animateOut();

        void hidden();
        void showForReal();

        friend class NotificationManager;

    public:
        static Notification* create(
            Mod* owner,
            std::string const& title,
            std::string const& text,
            cocos2d::CCNode* icon,
            const char* bg
        );
        static NotificationBuilder build();

        void show(
            NotificationLocation = PLATFORM_NOTIFICATION_LOCATION,
            float time = DEFAULT_NOTIFICATION_TIME
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

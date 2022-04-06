#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "FontRenderer.hpp"

struct MDParser;

namespace geode {
    class GEODE_API_DLL MDTextArea :
        public cocos2d::CCLayer,
        public cocos2d::CCLabelProtocol,
        public FLAlertLayerProtocol
    {
    protected:
        std::string m_text;
        cocos2d::CCSize m_size;
        cocos2d::extension::CCScale9Sprite* m_bgSprite = nullptr;
        cocos2d::CCMenu* m_content = nullptr;
        CCScrollLayerExt* m_scrollLayer = nullptr;
        FontRenderer* m_renderer = nullptr;

        bool init(
            std::string const& str,
            cocos2d::CCSize const& size
        );

        virtual ~MDTextArea();

        void onLink(CCObject*);
        void onGDProfile(CCObject*);
        void FLAlert_Clicked(FLAlertLayer*, bool btn) override;
    
        friend struct ::MDParser;

    public:
        static MDTextArea* create(
            std::string const& str,
            cocos2d::CCSize const& size
        );

        void updateLabel();

        void scrollWheel(float y, float) override;

        void setString(const char* text) override;
        const char* getString() override;
    };
}

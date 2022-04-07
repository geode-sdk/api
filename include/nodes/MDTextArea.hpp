#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "FontRenderer.hpp"

struct MDParser;

namespace geode {
    class TextLinkButton :
        public cocos2d::CCMenuItemSprite,
        public cocos2d::CCLabelProtocol
    {
    protected:
        cocos2d::CCLabelProtocol* m_label;
        cocos2d::CCRGBAProtocol* m_rgba;
        GLubyte m_opacity;
        cocos2d::ccColor3B m_color;
        std::vector<TextLinkButton*> m_linked;

        bool init(
            FontRenderer::Label const& label,
            cocos2d::CCObject* target,
            cocos2d::SEL_MenuHandler handler
        );
    
    public:
        static TextLinkButton* create(
            FontRenderer::Label const& label,
            cocos2d::CCObject* target,
            cocos2d::SEL_MenuHandler handler
        );

        void link(TextLinkButton* other);

        void selectedWithoutPropagation(bool selected);
        void selected() override;
        void unselected() override;

        void setString(const char* text) override;
        const char* getString() override;
    };

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

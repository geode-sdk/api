#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    enum class TextAlignment {
        Begin, Center, End,
    };

    enum class TextWrap {
        NoWrap,
        CharacterWrap,
        WordWrap,
    };

    class BMFontRenderer : public cocos2d::CCObject {
    public:
        using Char = unsigned int;
        using FontID = size_t;
        static constexpr const FontID s_invalidFontID = -1;

        struct Font {
            cocos2d::CCBMFontConfiguration* m_configuration = nullptr;
            cocos2d::CCTexture2D* m_texture = nullptr;
            cocos2d::CCSpriteBatchNode* m_batchNode = nullptr;
            float m_scale;
            float m_spacing;
            cocos2d::ccColor4B m_color;

            int kerningAmountForFirst(unsigned short first, unsigned short second);

            bool load(const char* bmFont);
            ~Font();
        };

    protected:
        std::vector<Font*> m_fonts;
        cocos2d::CCPoint m_origin = cocos2d::CCPointZero;
        cocos2d::CCSize m_size = cocos2d::CCSizeZero;
        cocos2d::CCPoint m_cursor = cocos2d::CCPointZero;
        cocos2d::CCNode* m_target = nullptr;
        Font* m_selectedFont = nullptr;
        TextWrap m_selectedWrap = TextWrap::WordWrap;
        Char m_lastRendered = -1;

        bool init();

    public:
        static BMFontRenderer* create();
        virtual ~BMFontRenderer();

        void begin(
            cocos2d::CCNode* target,
            cocos2d::CCPoint const& pos = cocos2d::CCPointZero,
            cocos2d::CCSize const& size = cocos2d::CCSizeZero
        );
        cocos2d::CCNode* end();

        FontID addFont(
            const char* font,
            float scale = .0f,
            float spacing = .0f,
            cocos2d::ccColor4B color = { 255, 255, 255, 255 }
        );

        void selectFont(FontID font);
        void selectWrap(TextWrap wrap);
        void renderChar(Char character);
        void renderString(std::string const& str);
        void moveCursor(cocos2d::CCPoint const& pos);

        Font* getFont(FontID font) const;
    };
}

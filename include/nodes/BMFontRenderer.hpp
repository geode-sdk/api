#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    enum class TextAlignment {
        Begin,
        Center,
        End,
    };

    enum class TextWrap {
        NoWrap,
        CharacterWrap,
        WordWrap,
    };

    enum class TextCapitalization {
        Normal,
        AllUpper,
        AllLower,
    };

    // enum only as these are flags
    enum TextStyle {
        TextStyleRegular      = 0,
        TextStyleBold         = 0b1,
        TextStyleItalic       = 0b10,
        TextStyleUnderline    = 0b100,
        TextStyleStrikethrough= 0b1000,
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
            TextCapitalization m_caps = TextCapitalization::Normal;
            int m_style = TextStyleRegular;
            float m_italicSkew;

            int kerningAmountForFirst(unsigned short first, unsigned short second);

            bool load(const char* bmFont);
            ~Font();
        };

        template<typename T>
        struct OverrideFontProp {
            bool m_do = false;
            T m_value;
        };

    protected:
        std::vector<Font*> m_fonts;
        cocos2d::CCPoint m_origin = cocos2d::CCPointZero;
        cocos2d::CCSize m_size = cocos2d::CCSizeZero;
        cocos2d::CCPoint m_cursor = cocos2d::CCPointZero;
        cocos2d::CCNode* m_target = nullptr;
        Font* m_selectedFont = nullptr;
        OverrideFontProp<int> m_overrideStyle;
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
            int style = TextStyleRegular,
            cocos2d::ccColor4B color = { 255, 255, 255, 255 },
            float spacing = .0f,
            TextCapitalization caps = TextCapitalization::Normal,
            float italicSkew = 15.f
        );

        void selectFont(FontID font);
        void selectWrap(TextWrap wrap);
        void overrideTextStyle(int flags);
        void restoreTextStyle();
        void renderChar(Char character);
        void renderString(std::string const& str);
        void moveCursor(cocos2d::CCPoint const& pos);

        Font* getFont(FontID font) const;
    };
}

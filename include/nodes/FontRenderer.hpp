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
    enum TextDecoration {
        TextDecorationNone         = 0b1,
        TextDecorationUnderline    = 0b10,
        TextDecorationStrikethrough= 0b100,
    };

    class FontRenderer : public cocos2d::CCObject {
    public:
        struct Font {
            const char* m_fontName;
            bool m_isTTFFont;
            float m_bmScaleOrTTFize;
            cocos2d::ccColor4B m_color = { 255, 255, 255, 255 };
            int m_decoration = TextDecorationNone;
            TextCapitalization m_caps = TextCapitalization::Normal;
        };

    protected:
        cocos2d::CCPoint m_origin = cocos2d::CCPointZero;
        cocos2d::CCSize m_size = cocos2d::CCSizeZero;
        cocos2d::CCPoint m_cursor = cocos2d::CCPointZero;
        cocos2d::CCNode* m_target = nullptr;
        TextWrap m_selectedWrap = TextWrap::WordWrap;

        bool init();

        bool render(std::string const& word, cocos2d::CCNode* to, cocos2d::CCLabelProtocol* label);

    public:
        static FontRenderer* create();
        virtual ~FontRenderer();

        void begin(
            cocos2d::CCNode* target,
            cocos2d::CCPoint const& pos = cocos2d::CCPointZero,
            cocos2d::CCSize const& size = cocos2d::CCSizeZero
        );
        cocos2d::CCNode* end(bool fitToContent = true);

        cocos2d::CCArray* renderStringEx(
            std::string const& str,
            const char* fontName,
            bool isTTFFont,
            float bmScaleOrTTFize,
            cocos2d::ccColor4B color = { 255, 255, 255, 255 },
            int deco = TextDecorationNone,
            TextCapitalization caps = TextCapitalization::Normal,
            bool addToTarget = true
        );
        cocos2d::CCArray* renderString(
            std::string const& str,
            Font const& font,
            bool addToTarget = true
        );
        cocos2d::CCArray* renderStringBM(
            std::string const& str,
            const char* fontName,
            float scale = 1.f,
            cocos2d::ccColor4B color = { 255, 255, 255, 255 }
        );
        cocos2d::CCArray* renderStringTTF(
            std::string const& str,
            const char* fontName,
            float size,
            cocos2d::ccColor4B color = { 255, 255, 255, 255 }
        );
        void moveCursor(cocos2d::CCPoint const& pos);
    };
}

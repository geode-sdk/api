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
        TextStyleRegular = 0b0,
        TextStyleBold    = 0b1,
        TextStyleItalic  = 0b10,
    };

    // enum only as these are flags
    enum TextDecoration {
        TextDecorationNone         = 0b0,
        TextDecorationUnderline    = 0b1,
        TextDecorationStrikethrough= 0b10,
    };

    class FontRenderer : public cocos2d::CCObject {
    public:
        struct Label {
            cocos2d::CCNode* m_node;
            cocos2d::CCLabelProtocol* m_labelProtocol;
            cocos2d::CCRGBAProtocol* m_rgbaProtocol;
            float m_lineHeight;

            explicit inline Label() {
                m_node = nullptr;
                m_labelProtocol = nullptr;
                m_rgbaProtocol = nullptr;
                m_lineHeight = .0f;
            }

            template<class T>
            Label(T* label, float lineHeight = .0f) {
                static_assert(std::is_base_of_v<cocos2d::CCNode, T>, "Label must inherit from CCNode!");
                static_assert(std::is_base_of_v<cocos2d::CCLabelProtocol, T>, "Label must inherit from CCLabelProtocol!");
                static_assert(std::is_base_of_v<cocos2d::CCRGBAProtocol, T>, "Label must inherit from CCRGBAProtocol!");
                m_node = label;
                m_labelProtocol = label;
                m_rgbaProtocol = label;
                if (lineHeight) {
                    m_lineHeight = lineHeight;
                } else {
                    if constexpr (std::is_same_v<cocos2d::CCLabelBMFont, T>) {
                        m_lineHeight = label->getConfiguration()->m_nCommonHeight / cocos2d::CC_CONTENT_SCALE_FACTOR();
                    }
                }
            }
        };
        using Font = std::function<Label(int)>;

    protected:
        cocos2d::CCPoint m_origin = cocos2d::CCPointZero;
        cocos2d::CCSize m_size = cocos2d::CCSizeZero;
        cocos2d::CCPoint m_cursor = cocos2d::CCPointZero;
        cocos2d::CCNode* m_target = nullptr;
        std::vector<Font> m_fontStack;
        std::vector<float> m_scaleStack;
        std::vector<int> m_styleStack;
        std::vector<cocos2d::ccColor3B> m_colorStack;
        std::vector<GLubyte> m_opacityStack;
        std::vector<int> m_decorationStack;
        std::vector<TextCapitalization> m_capsStack;
        std::vector<Label> m_lastRendered;
        std::vector<float> m_indentationStack;
        cocos2d::CCNode* m_lastRenderedNode = nullptr;

        bool init();

        void decorate(std::vector<Label>& labels);
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

        std::vector<Label> renderStringEx(
            std::string const& str,
            Font font,
            float scale,
            cocos2d::ccColor3B color = { 255, 255, 255 },
            GLubyte opacity = 255,
            int style = TextStyleRegular,
            int deco = TextDecorationNone,
            TextCapitalization caps = TextCapitalization::Normal,
            bool addToTarget = true
        );
        std::vector<Label> renderString(std::string const& str);
        cocos2d::CCNode* renderNode(cocos2d::CCNode* node);
        void breakLine(float y = .0f);

        void pushFont(Font const& font);
        void popFont();
        Font getCurrentFont() const;

        void pushScale(float scale);
        void popScale();
        float getCurrentScale() const;

        void pushStyleFlags(int style);
        void popStyleFlags();
        int getCurrentStyle() const;

        void pushColor(cocos2d::ccColor3B const& color);
        void popColor();
        cocos2d::ccColor3B getCurrentColor() const;

        void pushOpacity(GLubyte opacity);
        void popOpacity();
        GLubyte getCurrentOpacity() const;

        void pushDecoFlags(int deco);
        void popDecoFlags();
        int getCurrentDeco() const;

        void pushCaps(TextCapitalization caps);
        void popCaps();
        TextCapitalization getCurrentCaps() const;

        void pushIndent(float indent);
        void popIndent();
        float getCurrentIndent() const;

        void moveCursor(cocos2d::CCPoint const& pos);
    };

    class TextDecorationWrapper : public cocos2d::CCNodeRGBA, public cocos2d::CCLabelProtocol {
    protected:
        int m_deco;
        FontRenderer::Label m_label;

        bool init(
            FontRenderer::Label const& label,
            int decoration,
            cocos2d::ccColor3B const& color,
            GLubyte opacity
        );

        void draw() override;

    public:
        static TextDecorationWrapper* create(
            FontRenderer::Label const& label,
            int decoration,
            cocos2d::ccColor3B const& color,
            GLubyte opacity
        );
        static TextDecorationWrapper* wrap(
            FontRenderer::Label const& label,
            int decoration,
            cocos2d::ccColor3B const& color,
            GLubyte opacity
        );

        void setString(const char* text) override;
        const char* getString() override;

        void setColor(cocos2d::ccColor3B const& color) override;
    };
}

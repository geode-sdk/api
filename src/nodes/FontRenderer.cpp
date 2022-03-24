#include <nodes/FontRenderer.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();
using namespace std::string_literals;

bool FontRenderer::init() {
    return true;
}

void FontRenderer::begin(CCNode* target, CCPoint const& pos, CCSize const& size) {
    m_target = target ? target : CCNode::create();
    m_origin = pos;
    m_cursor = m_origin;
}

CCNode* FontRenderer::end(bool fitToContent) {
    if (fitToContent && m_target) {
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
            auto size = m_target->getContentSize();
            auto csize = child->getPosition() + child->getScaledContentSize();
            if (csize.x > size.width) size.width = csize.x;
            if (csize.y > size.height) size.height = csize.y;
            m_target->setContentSize(size);
        }
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
        }
    }
    m_cursor = m_origin;
    auto ret = m_target;
    m_target = nullptr;
    return ret;
}

void FontRenderer::moveCursor(CCPoint const& pos) {
    m_cursor = pos;
}

bool FontRenderer::renderWord(std::string const& word, CCNode* to, CCLabelProtocol* label) {
    auto origLabelStr = label->getString();
    auto str = ((origLabelStr && strlen(origLabelStr)) ?
        (origLabelStr + " "s) : ""
    ) + word;
    if (m_size.width) {
        std::string orig = origLabelStr;
        label->setString(str.c_str());
        if (m_cursor.x + to->getScaledContentSize().width > m_size.width) {
            label->setString(orig.c_str());
            return false;
        }
        return true;
    } else {
        label->setString(str.c_str());
        return true;
    }
}

bool FontRenderer::renderWord(std::string const& word, CCLabelTTF* to) {
    return this->renderWord(word, to, to);
}

bool FontRenderer::renderWord(std::string const& word, CCLabelBMFont* to) {
    return this->renderWord(word, to, to);
}

CCArray* FontRenderer::renderStringEx(
    std::string const& str,
    const char* fontName,
    bool isTTFFont,
    float bmScaleOrTTFSize,
    ccColor4B color,
    int deco,
    TextCapitalization caps,
    bool addToTarget
) {
    auto res = CCArray::create();

    CCLabelProtocol* labelProtocol;
    CCRGBAProtocol* rgbaProtocol;
    CCNode* node = nullptr;
    float lineHeight = .0f;

    auto createLabel = [&]() -> bool {
        if (isTTFFont) {
            auto label = CCLabelTTF::create("", fontName, bmScaleOrTTFSize);
            if (!label) return false;
            labelProtocol = label;
            rgbaProtocol = label;
            node = label;
            lineHeight = .0f;
        } else {
            auto label = CCLabelBMFont::create("", fontName);
            if (!label) return false;
            label->setScale(bmScaleOrTTFSize);
            labelProtocol = label;
            rgbaProtocol = label;
            node = label;
            lineHeight = label->getConfiguration()->m_nCommonHeight / CC_CONTENT_SCALE_FACTOR();
        }
        res->addObject(node);
        if (addToTarget) {
            m_target->addChild(node);
        }

        node->setPosition(m_cursor);
        node->setAnchorPoint({ .0f, .5f });
        rgbaProtocol->setColor(to3B(color));
        rgbaProtocol->setOpacity(color.a);

        return true;
    };

    if (!createLabel()) {
        res->release();
        return nullptr;
    }

    for (auto line : string_utils::split(str, "\n")) {
        for (auto word : string_utils::split(line, " ")) {
            switch (caps) {
                case TextCapitalization::AllUpper: string_utils::toUpperIP(word); break;
                case TextCapitalization::AllLower: string_utils::toLowerIP(word); break;
            }
            while (!this->renderWord(word, node, labelProtocol)) {
                auto cursorIncrementY = lineHeight;
                if (!cursorIncrementY) {
                    cursorIncrementY = node->getScaledContentSize().height;
                }
                m_cursor.x = m_origin.x;
                m_cursor.y -= cursorIncrementY;
                if (!createLabel()) {
                    res->release();
                    return nullptr;
                }
            }
        }
        auto cursorIncrementY = lineHeight;
        if (!cursorIncrementY) {
            cursorIncrementY = node->getScaledContentSize().height;
        }
        m_cursor.x = m_origin.x;
        m_cursor.y -= cursorIncrementY;
    }

    return res;
}

CCArray* FontRenderer::renderString(
    std::string const& str,
    FontRenderer::Font const& font,
    bool addToTarget
) {
    return this->renderStringEx(
        str,
        font.m_fontName,
        font.m_isTTFFont,
        font.m_bmScaleOrTTFize,
        font.m_color,
        font.m_decoration,
        font.m_caps,
        addToTarget
    );
}

CCArray* FontRenderer::renderStringBM(
    std::string const& str,
    const char* fontName,
    float scale,
    ccColor4B color
) {
    return this->renderStringEx(str, fontName, false, scale, color);
}

CCArray* FontRenderer::renderStringTTF(
    std::string const& str,
    const char* fontName,
    float size,
    ccColor4B color
) {
    return this->renderStringEx(str, fontName, true, size, color);
}

FontRenderer::~FontRenderer() {
    this->end();
}

FontRenderer* FontRenderer::create() {
    auto ret = new FontRenderer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


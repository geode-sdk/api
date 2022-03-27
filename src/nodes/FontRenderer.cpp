#include <nodes/FontRenderer.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();
using namespace std::string_literals;

bool FontRenderer::init() {
    return true;
}

void FontRenderer::begin(CCNode* target, CCPoint const& pos, CCSize const& size) {
    m_target = target ? target : CCNode::create();
    m_target->setContentSize(size);
    m_target->setPosition(pos);
    m_target->removeAllChildren();
    m_cursor = CCPointZero;
    m_origin = pos;
    m_size = size;
}

CCNode* FontRenderer::end(bool fitToContent) {
    if (fitToContent && m_target) {
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
            auto size = m_target->getContentSize();
            auto csize = child->getPosition() + child->getScaledContentSize();
            csize.x = fabsf(csize.x);
            csize.y = fabsf(csize.y);
            if (csize.x > size.width) {
                size.width = csize.x;
            }
            if (csize.y > size.height) {
                size.height = csize.y;
            }
            m_target->setContentSize(size);
        }
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
            child->setPositionY(
                child->getPositionY() +
                m_target->getContentSize().height -
                child->getScaledContentSize().height / 2
            );
        }
    }
    m_cursor = CCPointZero;
    m_size = CCSizeZero;
    auto ret = m_target;
    m_target = nullptr;
    return ret;
}

void FontRenderer::moveCursor(CCPoint const& pos) {
    m_cursor = pos;
}

bool FontRenderer::render(std::string const& word, CCNode* to, CCLabelProtocol* label) {
    auto origLabelStr = label->getString();
    auto str = ((origLabelStr && strlen(origLabelStr)) ?
        origLabelStr : ""
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
    bool newLine = true;

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
            lineHeight = label->getConfiguration()->m_nCommonHeight * bmScaleOrTTFSize /
                CC_CONTENT_SCALE_FACTOR();
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

    auto nextLine = [&]() -> bool {
        auto cursorIncrementY = lineHeight;
        if (!cursorIncrementY) {
            cursorIncrementY = node->getScaledContentSize().height;
        }
        m_cursor.x = 0;
        m_cursor.y -= cursorIncrementY;
        if (!createLabel()) {
            res->release();
            return false;
        }
        newLine = true;
        return true;
    };

    if (!createLabel()) {
        res->release();
        return nullptr;
    }

    bool firstLine = true;
    for (auto line : string_utils::split(str, "\n")) {
        if (!firstLine && !nextLine()) {
            return nullptr;
        }
        firstLine = false;
        for (auto word : string_utils::split(line, " ")) {
            // add extra space in front of word if not on 
            // new line
            if (!newLine) {
                word = " " + word;
            }
            newLine = false;
            switch (caps) {
                case TextCapitalization::AllUpper: string_utils::toUpperIP(word); break;
                case TextCapitalization::AllLower: string_utils::toLowerIP(word); break;
            }
            // try to render at the end of current line
            if (!this->render(word, node, labelProtocol)) {
                if (!nextLine()) {
                    return nullptr;
                }
                // try to render on new line
                if (!this->render(word, node, labelProtocol)) {
                    // no need to create a new line as we know 
                    // the current one has no content and is 
                    // supposed to receive this one

                    // render character by character
                    for (auto& c : word) {
                        if (!this->render(std::string(1, c), node, labelProtocol)) {
                            if (!nextLine()) {
                                return nullptr;
                            }
                        }
                    }
                }
            }
        }
        // increment cursor position
        m_cursor.x += node->getScaledContentSize().width;
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

void FontRenderer::renderNode(cocos2d::CCNode* node) {
    auto cursor = m_cursor;
    cursor.x += node->getScaledContentSize().width / 2;
    node->setPosition(cursor);
    m_target->addChild(node);
    m_cursor.x = m_origin.x;
    m_cursor.y -= node->getScaledContentSize().height;
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


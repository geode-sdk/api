#include <nodes/FontRenderer.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();
using namespace std::string_literals;

bool TextDecorationWrapper::init(
    FontRenderer::Label const& label,
    int deco,
    ccColor3B const& color,
    GLubyte opacity
) {
    if (!CCNodeRGBA::init())
        return false;
    
    label.m_node->removeFromParent();
    this->addChild(label.m_node);
    this->setContentSize(label.m_node->getContentSize());
    this->setAnchorPoint(label.m_node->getAnchorPoint());
    this->setScale(label.m_node->getScale());
    label.m_node->setScale(1.f);
    label.m_node->setPosition(0, 0);

    m_label = label;
    m_deco = deco;
    this->setColor(color);
    this->setOpacity(opacity);

    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);

    return true;
}

void TextDecorationWrapper::setColor(ccColor3B const& color) {
    m_label.m_rgbaProtocol->setColor(color);
    return CCNodeRGBA::setColor(color);
}

void TextDecorationWrapper::draw() {
    // some nodes sometimes set the blend func to
    // something else without resetting it back
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->setContentSize(m_label.m_node->getScaledContentSize());
    m_label.m_node->setPosition(m_obContentSize * m_label.m_node->getAnchorPoint());
    if (m_deco & TextDecorationUnderline) {
        ccDrawSolidRect(
            { 0, 0 },
            { m_obContentSize.width, 1.f },
            {
                _realColor.r / 255.f,
                _realColor.g / 255.f,
                _realColor.b / 255.f,
                _realOpacity / 255.f
            }
        );
    }
    if (m_deco & TextDecorationStrikethrough) {
        ccDrawSolidRect(
            { 0, m_obContentSize.height * .4f - .75f },
            { m_obContentSize.width, m_obContentSize.height * .4f + .75f },
            {
                _realColor.r / 255.f,
                _realColor.g / 255.f,
                _realColor.b / 255.f,
                _realOpacity / 255.f
            }
        );
    }
    CCNode::draw();
}

void TextDecorationWrapper::setString(const char* text) {
    m_label.m_labelProtocol->setString(text);
}

const char* TextDecorationWrapper::getString() {
    return m_label.m_labelProtocol->getString();
}

TextDecorationWrapper* TextDecorationWrapper::create(
    FontRenderer::Label const& label,
    int deco,
    ccColor3B const& color,
    GLubyte opacity
) {
    auto ret = new TextDecorationWrapper;
    if (ret && ret->init(label, deco, color, opacity)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TextDecorationWrapper* TextDecorationWrapper::wrap(
    FontRenderer::Label const& label,
    int deco,
    ccColor3B const& color,
    GLubyte opacity
) {
    auto pos = label.m_node->getPosition();
    auto wrapper = TextDecorationWrapper::create(label, deco, color, opacity);
    wrapper->setPosition(pos);
    return wrapper;
}


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
        CCRect coverage;
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
            auto pos = child->getPosition() - child->getScaledContentSize() * child->getAnchorPoint();
            auto csize = child->getPosition() + child->getScaledContentSize() * (CCPoint { 1.f, 1.f } - child->getAnchorPoint());
            if (pos.x < coverage.origin.x) {
                coverage.origin.x = pos.x;
            }
            if (pos.y < coverage.origin.y) {
                coverage.origin.y = pos.y;
            }
            if (csize.x > coverage.size.width) {
                coverage.size.width = csize.x;
            }
            if (csize.y > coverage.size.height) {
                coverage.size.height = csize.y;
            }
        }
        // todo: fix this to be exact
        coverage.size.height += 10.f;
        m_target->setContentSize({
            -coverage.origin.x + coverage.size.width,
            -coverage.origin.y + coverage.size.height
        });
        CCARRAY_FOREACH_B_TYPE(m_target->getChildren(), child, CCNode) {
            child->setPositionY(
                child->getPositionY() +
                m_target->getContentSize().height -
                child->getScaledContentSize().height / 2
            );
        }
    }
    m_fontStack.clear();
    m_scaleStack.clear();
    m_styleStack.clear();
    m_colorStack.clear();
    m_opacityStack.clear();
    m_decorationStack.clear();
    m_capsStack.clear();
    m_cursor = CCPointZero;
    m_size = CCSizeZero;
    auto ret = m_target;
    m_target = nullptr;
    m_lastRendered = {};
    CC_SAFE_RELEASE_NULL(m_lastRenderedNode);
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

void FontRenderer::decorate(std::vector<Label>& labels) {
    if (this->getCurrentDeco() != TextDecorationNone) {
        std::vector<FontRenderer::Label> res;
        for (auto const& label : labels) {
            auto wrapper = TextDecorationWrapper::wrap(
                label, this->getCurrentDeco(), this->getCurrentColor(), 
                this->getCurrentOpacity()
            );
            m_target->addChild(wrapper);
            res.push_back(Label(wrapper));
        }
        labels = res;
    }
}

std::vector<FontRenderer::Label> FontRenderer::renderStringEx(
    std::string const& str,
    Font font,
    float scale,
    ccColor3B color,
    GLubyte opacity,
    int style,
    int deco,
    TextCapitalization caps,
    bool addToTarget
) {
    std::vector<Label> res;

    Label label;
    bool newLine = true;

    auto createLabel = [&]() -> bool {
        label = font(style);
        label.m_node->setScale(scale);
        res.push_back(label);
        if (addToTarget) {
            m_target->addChild(label.m_node);
        }
        label.m_node->setPosition(m_cursor);
        label.m_node->setAnchorPoint({ .0f, .5f });
        label.m_rgbaProtocol->setColor(color);
        label.m_rgbaProtocol->setOpacity(opacity);

        return true;
    };

    auto nextLine = [&]() -> bool {
        auto cursorIncrementY = label.m_lineHeight * scale;
        if (!cursorIncrementY) {
            cursorIncrementY = label.m_node->getScaledContentSize().height;
        }
        m_cursor.x = m_origin.x + getCurrentIndent();
        m_cursor.y -= cursorIncrementY;
        if (!createLabel()) {
            return false;
        }
        newLine = true;
        return true;
    };

    // create initial label
    if (!createLabel()) return {};

    bool firstLine = true;
    for (auto line : string_utils::split(str, "\n")) {
        if (!firstLine && !nextLine()) {
            return {};
        }
        firstLine = false;
        for (auto word : string_utils::split(line, " ")) {
            // add extra space in front of word if not on 
            // new line
            if (!newLine) word = " " + word;
            newLine = false;

            // update capitalization
            switch (caps) {
                case TextCapitalization::AllUpper: string_utils::toUpperIP(word); break;
                case TextCapitalization::AllLower: string_utils::toLowerIP(word); break;
                default: break;
            }

            // try to render at the end of current line
            if (this->render(word, label.m_node, label.m_labelProtocol)) continue;

            // try to create a new line
            if (!nextLine()) return {};

            if (string_utils::startsWith(word, " ")) word = word.substr(1);
            newLine = false;

            // try to render on new line
            if (this->render(word, label.m_node, label.m_labelProtocol)) continue;

            // no need to create a new line as we know 
            // the current one has no content and is 
            // supposed to receive this one

            // render character by character
            for (auto& c : word) {
                if (
                    !this->render(std::string(1, c), label.m_node, label.m_labelProtocol)
                ) {
                    if (!nextLine()) return {};
                    
                    if (string_utils::startsWith(word, " ")) word = word.substr(1);
                    newLine = false;
                }
            }
        }
        // increment cursor position
        m_cursor.x += label.m_node->getScaledContentSize().width;
    }

    // add decorations to rendered nodes 
    // (underline, strikethrough)
    this->decorate(res);

    CC_SAFE_RELEASE_NULL(m_lastRenderedNode);
    m_lastRendered = res;

    return res;
}

std::vector<FontRenderer::Label> FontRenderer::renderString(std::string const& str) {
    return this->renderStringEx(
        str,
        this->getCurrentFont(),
        this->getCurrentScale(),
        this->getCurrentColor(),
        this->getCurrentOpacity(),
        this->getCurrentStyle(),
        this->getCurrentDeco(),
        this->getCurrentCaps(),
        true
    );
}

CCNode* FontRenderer::renderNode(CCNode* node) {
    m_cursor.x += node->getScaledContentSize().width * node->getAnchorPoint().x;
    node->setPosition(m_cursor);
    m_target->addChild(node);
    m_cursor.x += node->getScaledContentSize().width * (1.f - node->getAnchorPoint().x);
    m_lastRendered.clear();
    CC_SAFE_RELEASE(m_lastRenderedNode);
    m_lastRenderedNode = node;
    CC_SAFE_RETAIN(m_lastRenderedNode);
    return node;
}

void FontRenderer::breakLine(float y) {
    if (!y && m_fontStack.size()) {
        y = m_fontStack.back()(this->getCurrentStyle()).m_lineHeight * this->getCurrentScale();
        if (!y && m_lastRendered.size()) {
            y = as<CCNode*>(m_lastRendered.back().m_node)->getScaledContentSize().height;
        }
        if (!y && m_lastRenderedNode) {
            y = m_lastRenderedNode->getScaledContentSize().height;
        }
    }
    m_cursor.y -= y;
    m_cursor.x = m_origin.x + getCurrentIndent();
}

void FontRenderer::pushFont(Font const& font) {
    m_fontStack.push_back(font);
}

void FontRenderer::popFont() {
    if (m_fontStack.size()) m_fontStack.pop_back();
}

FontRenderer::Font FontRenderer::getCurrentFont() const {
    if (!m_fontStack.size()) {
        return [](int) -> Label {
            return CCLabelBMFont::create("", "bigFont.fnt");
        };
    }
    return m_fontStack.back();
}

void FontRenderer::pushScale(float scale) {
    m_scaleStack.push_back(scale);
}

void FontRenderer::popScale() {
    if (m_scaleStack.size()) m_scaleStack.pop_back();
}

float FontRenderer::getCurrentScale() const {
    return m_scaleStack.size() ? m_scaleStack.back() : 1.f;
}

void FontRenderer::pushStyleFlags(int style) {
    int oldStyle = TextStyleRegular;
    if (m_styleStack.size()) oldStyle = m_styleStack.back();
    m_styleStack.push_back(oldStyle | style);
}

void FontRenderer::popStyleFlags() {
    if (m_styleStack.size()) m_styleStack.pop_back();
}

int FontRenderer::getCurrentStyle() const {
    return m_styleStack.size() ? m_styleStack.back() : TextStyleRegular;
}

void FontRenderer::pushColor(ccColor3B const& color) {
    m_colorStack.push_back(color);
}

void FontRenderer::popColor() {
    if (m_colorStack.size()) m_colorStack.pop_back();
}

ccColor3B FontRenderer::getCurrentColor() const {
    return m_colorStack.size() ? m_colorStack.back() : ccColor3B { 255, 255, 255 };
}

void FontRenderer::pushOpacity(GLubyte opacity) {
    m_opacityStack.push_back(opacity);
}

void FontRenderer::popOpacity() {
    if (m_opacityStack.size()) m_opacityStack.pop_back();
}

GLubyte FontRenderer::getCurrentOpacity() const {
    return m_opacityStack.size() ? m_opacityStack.back() : 255;
}

void FontRenderer::pushDecoFlags(int deco) {
    int oldDeco = TextDecorationNone;
    if (m_decorationStack.size()) oldDeco = m_decorationStack.back();
    m_decorationStack.push_back(oldDeco | deco);
}

void FontRenderer::popDecoFlags() {
    if (m_decorationStack.size()) m_decorationStack.pop_back();
}

int FontRenderer::getCurrentDeco() const {
    return m_decorationStack.size() ? m_decorationStack.back() : TextDecorationNone;
}

void FontRenderer::pushCaps(TextCapitalization caps) {
    m_capsStack.push_back(caps);
}

void FontRenderer::popCaps() {
    if (m_capsStack.size()) m_capsStack.pop_back();
}

TextCapitalization FontRenderer::getCurrentCaps() const {
    return m_capsStack.size() ? m_capsStack.back() : TextCapitalization::Normal;
}

void FontRenderer::pushIndent(float indent) {
    m_indentationStack.push_back(indent);
}

void FontRenderer::popIndent() {
    if (m_indentationStack.size()) m_indentationStack.pop_back();
}

float FontRenderer::getCurrentIndent() const {
    float res = .0f;
    for (auto& indent : m_indentationStack) {
        res += indent;
    }
    return res;
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


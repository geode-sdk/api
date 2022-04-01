#include <nodes/MDTextArea.hpp>
#include <WackyGeodeMacros.hpp>
#include "../../md4c/src/md4c.h"

USE_GEODE_NAMESPACE();

static constexpr float g_fontScale = .5f;
static constexpr float g_paragraphPadding = 7.f;
static constexpr ccColor3B g_linkColor = cc3x(0x7ff4f4);

bool TextLinkButton::init(
    FontRenderer::Label const& label,
    cocos2d::CCObject* target,
    cocos2d::SEL_MenuHandler handler
) {
    if (!CCMenuItemSprite::initWithNormalSprite(label.m_node, nullptr, nullptr, target, handler))
        return false;
    
    m_label = label.m_labelProtocol;
    m_rgba = label.m_rgbaProtocol;

    label.m_node->removeFromParent();
    this->addChild(label.m_node);
    this->setContentSize(label.m_node->getContentSize());
    this->setAnchorPoint(label.m_node->getAnchorPoint());
    this->setScale(label.m_node->getScale());
    label.m_node->setScale(1.f);
    label.m_node->setPosition(0, 0);

    return true;
}

void TextLinkButton::link(TextLinkButton* other) {
    if (this != other) {
        m_linked.push_back(other);
    }
}

void TextLinkButton::setString(const char* text) {
    return m_label->setString(text);
}

const char* TextLinkButton::getString() {
    return m_label->getString();
}

void TextLinkButton::selectedWithoutPropagation(bool selected) {
    if (selected) {
        m_opacity = m_rgba->getOpacity();
        m_color = m_rgba->getColor();
        m_rgba->setOpacity(150);
        m_rgba->setColor({ 255, 255, 255 });
    } else {
        m_rgba->setOpacity(m_opacity);
        m_rgba->setColor(m_color);
    }
}

void TextLinkButton::selected() {
    this->selectedWithoutPropagation(true);
    for (auto& node : m_linked) {
        node->selectedWithoutPropagation(true);
    }
}

void TextLinkButton::unselected() {
    this->selectedWithoutPropagation(false);
    for (auto& node : m_linked) {
        node->selectedWithoutPropagation(false);
    }
}

TextLinkButton* TextLinkButton::create(
    FontRenderer::Label const& label,
    cocos2d::CCObject* target,
    cocos2d::SEL_MenuHandler handler
) {
    auto ret = new TextLinkButton;
    if (ret && ret->init(label, target, handler)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


class MDContentLayer : public CCContentLayer {
protected:
    CCMenu* m_content;

public:
    static MDContentLayer* create(CCMenu* content, float width, float height) {
        auto ret = new MDContentLayer();
        if (ret && ret->initWithColor({ 0, 255, 0, 0 }, width, height)) {
            ret->m_content = content;
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void setPosition(CCPoint const& pos) override {
        // cringe CCContentLayer expect its children to 
        // all be TableViewCells
        CCLayerColor::setPosition(pos);

        // so that's why based MDContentLayer expects itself 
        // to have a CCMenu :-)
        if (m_content) {
            CCARRAY_FOREACH_B_TYPE(m_content->getChildren(), child, CCNode) {
               auto y = this->getPositionY() + child->getPositionY();
               child->setVisible(!(
                    (m_content->getContentSize().height < y) ||
                    (y < -child->getContentSize().height)
                ));
            }
        }
    }
};


Result<ccColor3B> colorForIdentifier(std::string const& tag) {
    if (string_utils::contains(tag, ' ')) {
        auto hexStr = string_utils::split(
            string_utils::normalize(tag), " "
        ).at(1);
        try {
            auto hex = std::stoi(hexStr, nullptr, 16);
            return Ok(cc3x(hex));
        } catch(...) {
            return Err("Invalid hex");
        }
    } else {
        auto colorText = tag.substr(1);
        if (!colorText.size()) {
            return Err("No color specified");
        } else if (colorText.size() > 1) {
            return Err("Color tag " + tag + " unexpectedly long, either do <cx> or <c hex>");
        } else {
            switch (colorText.front()) {
                case 'b': return Ok(cc3x(0x4a52e1)); break;
                case 'g': return Ok(cc3x(0x40e348)); break;
                case 'l': return Ok(cc3x(0x60abef)); break;
                case 'j': return Ok(cc3x(0x32c8ff)); break;
                case 'y': return Ok(cc3x(0xffff00)); break;
                case 'o': return Ok(cc3x(0xffa54b)); break;
                case 'r': return Ok(cc3x(0xff5a5a)); break;
                case 'p': return Ok(cc3x(0xff00ff)); break;
                default:
                    return Err("Unknown color " + colorText);
            }
        }
    }
    return Err("Unknown error");
}


bool MDTextArea::init(
    std::string const& str,
    CCSize const& size
) {
    if (!CCLayer::init())
        return false;

    m_text = str;
    m_size = size;
    this->setContentSize(size);
    m_renderer = FontRenderer::create();
    CC_SAFE_RETAIN(m_renderer);

    m_bgSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    m_bgSprite->setScale(.5f);
    m_bgSprite->setColor({ 0, 0, 0 });
    m_bgSprite->setOpacity(75);
    m_bgSprite->setContentSize(size * 2 + CCSize { 25.f, 25.f });
    m_bgSprite->setPosition(size / 2);
    this->addChild(m_bgSprite);

    m_scrollLayer = CCScrollLayerExt::create({ 0, 0, m_size.width, m_size.height }, true);

    m_content = CCMenu::create();
    m_content->setZOrder(2);

    m_scrollLayer->m_contentLayer->removeFromParent();
    m_scrollLayer->m_contentLayer = MDContentLayer::create(m_content, m_size.width, m_size.height);
    m_scrollLayer->m_contentLayer->setAnchorPoint({ 0, 0 });
    m_scrollLayer->m_contentLayer->addChild(m_content);
    m_scrollLayer->addChild(m_scrollLayer->m_contentLayer);

    this->addChild(m_scrollLayer);
    
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    m_scrollLayer->registerWithTouchDispatcher();

    m_scrollLayer->setMouseEnabled(true);
    m_scrollLayer->setTouchEnabled(true);

    this->setMouseEnabled(true);

    this->updateLabel();

    return true;
}

MDTextArea::~MDTextArea() {
    CC_SAFE_RELEASE(m_renderer);
}

void MDTextArea::scrollWheel(float y, float) {
    m_scrollLayer->scrollLayer(y);
}

class BreakLine : public CCNode {
protected:
    void draw() override {
        // some nodes sometimes set the blend func to
        // something else without resetting it back
        ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1.f, 1.f, 1.f, .2f });
        CCNode::draw();
    }

public:
    static BreakLine* create(float width) {
        auto ret = new BreakLine;
        if (ret && ret->init()) {
            ret->autorelease();
            ret->setContentSize({ width, 1.f });
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

void MDTextArea::onLink(CCObject* pSender) {
    auto href = as<CCString*>(as<CCNode*>(pSender)->getUserObject());
    auto layer = FLAlertLayer::create(
        this,
        "Hold Up!",
        "Links are spooky! Are you sure you want to go to <cy>" +
        std::string(href->getCString()) + "</c>?",
        "Cancel", "Yes"
    );
    layer->setUserObject(href);
    layer->show();
}

void MDTextArea::onGDProfile(CCObject* pSender) {
    auto href = as<CCString*>(as<CCNode*>(pSender)->getUserObject());
    auto profile = std::string(href->getCString());
    profile = profile.substr(profile.find(":") + 1);
    try {
        ProfilePage::create(std::stoi(profile), false)->show();
    } catch(...) {
        FLAlertLayer::create(
            "Error",
            "Invalid profile ID: <cr>" + profile + "</c>. This is "
            "probably the modder's fault, report the bug to them.",
            "OK"
        )->show();
    }
}

void MDTextArea::FLAlert_Clicked(FLAlertLayer* layer, bool btn) {
    if (btn) {
        web::openLinkInBrowser(as<CCString*>(layer->getUserObject())->getCString());
    }
}

struct MDParser {
    static std::string s_lastLink;
    static std::string s_lastImage;

    static int parseText(MD_TEXTTYPE type, MD_CHAR const* rawText, MD_SIZE size, void* mdtextarea) {
        auto textarea = reinterpret_cast<MDTextArea*>(mdtextarea);
        auto renderer = textarea->m_renderer;
        auto text = std::string(rawText, size);
        switch (type) {
            case MD_TEXTTYPE::MD_TEXT_BR: {
                renderer->breakLine();
            } break;

            case MD_TEXTTYPE::MD_TEXT_SOFTBR: {
                renderer->breakLine();
            } break;
            
            case MD_TEXTTYPE::MD_TEXT_NORMAL: {
                if (s_lastLink.size()) {
                    renderer->pushColor(g_linkColor);
                    renderer->pushDecoFlags(TextDecorationUnderline);
                    auto rendered = renderer->renderString(text);
                    std::vector<TextLinkButton*> btns;
                    for (auto const& label : rendered) {
                        label.m_node->removeFromParent();
                        auto pos = label.m_node->getPosition();
                        auto anchor = label.m_node->getAnchorPoint();
                        label.m_node->setPosition(0, 0);
                        auto btn = TextLinkButton::create(
                            label, textarea,
                            string_utils::startsWith(s_lastLink, "user:") ?
                                menu_selector(MDTextArea::onGDProfile) : 
                                menu_selector(MDTextArea::onLink)
                        );
                        btns.push_back(btn);
                        btn->setUserObject(CCString::create(s_lastLink));
                        btn->setPosition(pos - btn->getScaledContentSize() * anchor);
                        textarea->m_content->addChild(btn);
                    }
                    // O(N^2)
                    for (auto& btn : btns) {
                        for (auto& b : btns) {
                            if (b != btn) {
                                btn->link(b);
                            }
                        }
                    }
                    renderer->popDecoFlags();
                    renderer->popColor();
                } else if (s_lastImage.size()) {
                    bool isFrame = false;
                    if (string_utils::startsWith(s_lastImage, "frame:")) {
                        s_lastImage = s_lastImage.substr(s_lastImage.find(":") + 1);
                        isFrame = true;
                    }
                    CCSprite* spr = nullptr;
                    if (isFrame) {
                        spr = CCSprite::createWithSpriteFrameName(s_lastImage.c_str());
                    } else {
                        spr = CCSprite::create(s_lastImage.c_str());
                    }
                    if (spr) {
                        renderer->renderNode(spr);
                    } else {
                        renderer->renderString(text);
                    }
                    s_lastImage = "";
                } else {
                    renderer->renderString(text);
                }
            } break;
            
            case MD_TEXTTYPE::MD_TEXT_HTML: {
                if (text.size() > 2) {
                    auto tag = string_utils::trim(text.substr(1, text.size() - 2));
                    auto isClosing = tag.front() == '/';
                    if (isClosing) tag = tag.substr(1);
                    if (tag.front() != 'c') {
                        Log::get() << Severity::Warning << "Unknown tag " << text;
                        renderer->renderString(text);
                    } else {
                        if (isClosing) {
                            renderer->popColor();
                        } else {
                            auto color = colorForIdentifier(tag);
                            if (color) {
                                renderer->pushColor(color.value());
                            } else {
                                Log::get() << Severity::Warning
                                    << "Error parsing color: " << color.error();
                            }
                        }
                    }
                } else {
                    Log::get() << Severity::Warning
                        << "Too short tag " << text;
                    renderer->renderString(text);
                }
            } break;
        }
        return 0;
    }

    static int enterBlock(MD_BLOCKTYPE type, void* detail, void* mdtextarea) {
        auto renderer = reinterpret_cast<MDTextArea*>(mdtextarea)->m_renderer;
        switch (type) {
            case MD_BLOCKTYPE::MD_BLOCK_H: {
                auto hdetail = reinterpret_cast<MD_BLOCK_H_DETAIL*>(detail);
                renderer->pushStyleFlags(TextStyleBold);
                switch (hdetail->level) {
                    case 1: renderer->pushScale(g_fontScale * 2.f); break;
                    case 2: renderer->pushScale(g_fontScale * 1.5f); break;
                    case 3: renderer->pushScale(g_fontScale * 1.17f); break;
                    case 4: renderer->pushScale(g_fontScale); break;
                    case 5: renderer->pushScale(g_fontScale * .83f); break;
                    default:
                    case 6: renderer->pushScale(g_fontScale * .67f); break;
                }
                // switch (hdetail->level) {
                //     case 3: renderer->pushCaps(TextCapitalization::AllUpper); break;
                // }
            } break;
        }
        return 0;
    }
    
    static int leaveBlock(MD_BLOCKTYPE type, void* detail, void* mdtextarea) {
        auto textarea = reinterpret_cast<MDTextArea*>(mdtextarea);
        auto renderer = textarea->m_renderer;
        switch (type) {
            case MD_BLOCKTYPE::MD_BLOCK_H: {
                auto hdetail = reinterpret_cast<MD_BLOCK_H_DETAIL*>(detail);
                renderer->breakLine();
                if (hdetail->level == 1) {
                    renderer->breakLine(g_paragraphPadding / 2);
                    renderer->renderNode(BreakLine::create(textarea->m_size.width));
                }
                renderer->breakLine(g_paragraphPadding);
                renderer->popScale();
                renderer->popStyleFlags();
                // switch (hdetail->level) {
                //     case 3: renderer->popCaps(); break;
                // }
            } break;
            
            case MD_BLOCKTYPE::MD_BLOCK_P:
                renderer->breakLine();
                renderer->breakLine(g_paragraphPadding);
                break;
        }
        return 0;
    }
    
    static int enterSpan(MD_SPANTYPE type, void* detail, void* mdtextarea) {
        auto renderer = reinterpret_cast<MDTextArea*>(mdtextarea)->m_renderer;
        switch (type) {
            case MD_SPANTYPE::MD_SPAN_STRONG: {
                renderer->pushStyleFlags(TextStyleBold);
            } break;

            case MD_SPANTYPE::MD_SPAN_EM: {
                renderer->pushStyleFlags(TextStyleItalic);
            } break;
            
            case MD_SPANTYPE::MD_SPAN_DEL: {
                renderer->pushDecoFlags(TextDecorationStrikethrough);
            } break;
            
            case MD_SPANTYPE::MD_SPAN_U: {
                renderer->pushDecoFlags(TextDecorationUnderline);
            } break;

            case MD_SPANTYPE::MD_SPAN_IMG: {
                auto adetail = reinterpret_cast<MD_SPAN_IMG_DETAIL*>(detail);
                s_lastImage = std::string(adetail->src.text, adetail->src.size);
            } break;
            
            case MD_SPANTYPE::MD_SPAN_A: {
                auto adetail = reinterpret_cast<MD_SPAN_A_DETAIL*>(detail);
                s_lastLink = std::string(adetail->href.text, adetail->href.size);
            } break;
        }
        return 0;
    }
    
    static int leaveSpan(MD_SPANTYPE type, void* detail, void* mdtextarea) {
        auto renderer = reinterpret_cast<MDTextArea*>(mdtextarea)->m_renderer;
        switch (type) {
            case MD_SPANTYPE::MD_SPAN_STRONG:
                renderer->popStyleFlags();
                break;

            case MD_SPANTYPE::MD_SPAN_EM:
                renderer->popStyleFlags();
                break;
            
            case MD_SPANTYPE::MD_SPAN_DEL:
                renderer->popDecoFlags();
                break;
            
            case MD_SPANTYPE::MD_SPAN_U:
                renderer->popDecoFlags();
                break;
            
            case MD_SPANTYPE::MD_SPAN_A: {
                s_lastLink = "";
            } break;
            
            case MD_SPANTYPE::MD_SPAN_IMG: {
                s_lastImage = "";
            } break;
        }
        return 0;
    }
};
std::string MDParser::s_lastLink = "";
std::string MDParser::s_lastImage = "";

void MDTextArea::updateLabel() {
    m_renderer->begin(m_content, CCPointZero, m_size);

    FontRenderer::Font font = [](int style) -> FontRenderer::Label {
        if ((style & TextStyleBold) && (style & TextStyleItalic)) {
            return CCLabelBMFont::create("", "mdFontBI.fnt"_spr);
        }
        if ((style & TextStyleBold)) {
            return CCLabelBMFont::create("", "mdFontB.fnt"_spr);
        }
        if ((style & TextStyleItalic)) {
            return CCLabelBMFont::create("", "mdFontI.fnt"_spr);
        }
        return CCLabelBMFont::create("", "mdFont.fnt"_spr);
    };
    m_renderer->pushFont(font);
    m_renderer->pushScale(.5f);

    MD_PARSER parser;

    parser.abi_version = 0;
    parser.flags = MD_FLAG_UNDERLINE |
                   MD_FLAG_STRIKETHROUGH |
                   MD_FLAG_PERMISSIVEURLAUTOLINKS |
                   MD_FLAG_PERMISSIVEWWWAUTOLINKS;

    parser.text = &MDParser::parseText;
    parser.enter_block = &MDParser::enterBlock;
    parser.leave_block = &MDParser::leaveBlock;
    parser.enter_span = &MDParser::enterSpan;
    parser.leave_span = &MDParser::leaveSpan;
    parser.debug_log = nullptr;
    parser.syntax = nullptr;

    if (md_parse(m_text.c_str(), m_text.size(), &parser, this)) {
        m_renderer->renderString("Error parsing Markdown");
    }

    m_renderer->end();

    m_scrollLayer->m_contentLayer->setContentSize(m_content->getContentSize());
    m_scrollLayer->moveToTop();
}

void MDTextArea::setString(const char* text) {
    this->m_text = text;
    this->updateLabel();
}

const char* MDTextArea::getString() {
    return this->m_text.c_str();
}

MDTextArea* MDTextArea::create(
    std::string const& str,
    CCSize const& size
) {
    auto ret = new MDTextArea;
    if (ret && ret->init(str, size)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

#include <nodes/MDTextArea.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

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

    m_content = CCNode::create();
    m_content->setZOrder(2);
    this->addChild(m_content);

    this->updateLabel();

    return true;
}

MDTextArea::~MDTextArea() {
    CC_SAFE_RELEASE(m_renderer);
}

void MDTextArea::updateLabel() {
    m_renderer->begin(m_content, CCPointZero, m_size);

    FontRenderer::Font normalFont = { "mdFont.fnt"_spr, false, .5f };
    FontRenderer::Font boldFont = { "mdFontB.fnt"_spr, false, .5f };
    FontRenderer::Font italicFont = { "mdFontI.fnt"_spr, false, .5f };
    FontRenderer::Font boldItalicFont = { "mdFontBI.fnt"_spr, false, .5f };

    enum TextStyle {
        TextStyleRegular = 0b0,
        TextStyleItalic  = 0b10,
        TextStyleBold    = 0b100,
    };

    FontRenderer::Font* currentFont = &normalFont;
    char last = 0;
    int headingSize = 0;
    bool collectingHeading = false;
    bool escapeSpecial = false;
    bool collectingTag = false;
    bool closingTag = false;
    bool collectingLink = false;
    bool collectingLinkText = false;
    bool linkIsImage = false;
    int style = TextStyleRegular;
    int collectingStars = 0;
    float currentSize = .5f;
    TextCapitalization currentCaps = TextCapitalization::Normal;
    std::string collectingText = "";
    std::string tagData = "";
    std::string linkData = "";
    std::vector<std::string> tagStack;
    std::vector<ccColor4B> colorStack = {{ 255, 255, 255, 255 }};
    ccColor4B currentColor = colorStack.back();
    CCArray* lastRendered = nullptr;
    int index = -1;
    for (auto& c : m_text) {
        index++;
        bool renderLast = false;
        bool collectCurrent = true;

        char next = index + 1 < static_cast<int>(m_text.size()) ? m_text.at(index + 1) : 0;

        /*
            esc char -> escres
            \   \       0
            \   0       \
            0   \       \
            0   0       0
        */
        escapeSpecial ^= c == '\\';

        if ((last == '\n' || last == 0) && c == '#' && !escapeSpecial) {
            collectingHeading = true;
            headingSize = 0;
            renderLast = true;
        }
        if (collectingHeading) {
            collectCurrent = false;
            if (c == '#') {
                headingSize++;
            } else if (c != ' ') {
                collectCurrent = true;
                collectingHeading = false;
                currentCaps = TextCapitalization::Normal;
                switch (headingSize) {
                    case 1:
                        currentSize = 1.f;
                        currentFont = &boldFont;
                        break;
                    case 2:
                        currentSize = .8f;
                        currentFont = &boldFont;
                        break;
                    case 3: 
                        currentSize = .65f;
                        currentCaps = TextCapitalization::AllUpper;
                        currentFont = &boldFont;
                        break;
                    case 4: 
                        currentSize = .65f;
                        currentCaps = TextCapitalization::AllUpper;
                        break;
                    default: currentSize = .5f;
                }
            }
        }

        #define FLIP_FLAG(flag) \
            if (!(style & flag)) style |= flag; else style &= ~flag;
        
        if (!escapeSpecial) {
            if (c == '*') {
                renderLast = true;
                collectingStars++;
                collectCurrent = false;
            } else {
                if (collectingStars) {
                    switch (collectingStars) {
                        case 1: FLIP_FLAG(TextStyleItalic); break;
                        case 2: FLIP_FLAG(TextStyleBold); break;
                        case 3: FLIP_FLAG((TextStyleBold & TextStyleItalic)); break;
                    }
                    if ((style & TextStyleItalic) && (style & TextStyleBold)) {
                        currentFont = &boldItalicFont;
                    } else if (style & TextStyleItalic) {
                        currentFont = &italicFont;
                    } else if (style & TextStyleBold) {
                        currentFont = &boldFont;
                    } else {
                        currentFont = &normalFont;
                    }
                    collectingStars = 0;
                }
            }
        }

        if (c == '\n') {
            headingSize = 0;
            renderLast = true;
            collectCurrent = true;
        }

        if (c == '>' && !escapeSpecial) {
            if (!collectingTag) {
                Mod::get()->logInfo(
                    "Found '>' outside tag while parsing markdown, not doing "
                    "anything. Note that in GD markdown, if you want to include "
                    "the '>' character you need to escape it with a '\\'. "
                    "(At " + std::to_string(index) + ")",
                    Severity::Warning 
                );
            }

            string_utils::trimIP(tagData);

            auto first = tagData.size() > 1 ? (tagData.front() == '/' ? tagData.at(1) : tagData.front()) : 'c';
            if (first != 'c') {
                Mod::get()->logInfo(
                    "Unknown tag \"<" + tagData + ">\" "
                    "(At " + std::to_string(index) + ")",
                    Severity::Warning 
                );
            } else {
                if (closingTag) {
                    if (!tagStack.size()) {
                        Mod::get()->logInfo(
                            "Found a closing tag \"<" + tagData + ">\", but no opening tag. "
                            "Note that in GD markdown, if you want to include "
                            "the '>' or '<' characters you need to escape them with a '\\'. "
                            "(At " + std::to_string(index) + ")",
                            Severity::Warning 
                        );
                    } else {
                        if (tagData.size() == 2 || tagStack.back() == tagData.substr(1)) {
                            tagStack.pop_back();
                            colorStack.pop_back();
                            currentColor = colorStack.back();
                        } else {
                            Mod::get()->logInfo(
                                "Found a closing tag \"<" + tagData + ">\", but it does not match "
                                "the last opened tag \"<" + tagStack.back() + ">\". "
                                "Note that in GD markdown, if you want to include "
                                "the '>' or '<' characters you need to escape them with a '\\'. "
                                "(At " + std::to_string(index) + ")",
                                Severity::Warning 
                            );
                        }
                    }
                } else {
                    std::string tagName = "";
                    std::string hexStr = "";
                    bool collectingName = true;
                    for (auto& tc : tagData) {
                        if (collectingName) {
                            if (tc == ' ') {
                                collectingName = false;
                            } else {
                                tagName += tc;
                            }
                        } else {
                            if (tc != ' ' && hexStr.size() < 6 && (
                                ('0' <= tc && tc <= '9') ||
                                ('a' <= tc && tc <= 'f') ||
                                ('A' <= tc && tc <= 'F')
                            )) {
                                hexStr += tc;
                            }
                        }
                    }
                    ccColor3B color = { 255, 255, 255 };
                    bool gotColorFromTag = false;
                    if (tagName.size() > 2) {
                        Mod::get()->logInfo(
                            "Invalid tag name \"" + hexStr + "\""
                            "(At " + std::to_string(index) + ")",
                            Severity::Warning 
                        );
                    } else if (tagName.size() > 1) {
                        gotColorFromTag = true;
                        switch (tagName.back()) {
                            case 'b': color = cc3x(0x4a52e1); break;
                            case 'g': color = cc3x(0x40e348); break;
                            case 'l': color = cc3x(0x60abef); break;
                            case 'j': color = cc3x(0x32c8ff); break;
                            case 'y': color = cc3x(0xffff00); break;
                            case 'o': color = cc3x(0xffa54b); break;
                            case 'r': color = cc3x(0xff5a5a); break;
                            case 'p': color = cc3x(0xff00ff); break;
                            default:
                                Mod::get()->logInfo(
                                    "Unknown color \"" + std::string(1, tagName.back()) + "\", "
                                    "options are b,g,l,j,y,o,r,p. "
                                    "(At " + std::to_string(index) + ")",
                                    Severity::Warning 
                                );
                        }
                    }
                    if (hexStr.size()) {
                        if (gotColorFromTag) {
                            Mod::get()->logInfo(
                                "Both default color and hex specified, picking hex"
                                "(At " + std::to_string(index) + ")",
                                Severity::Warning 
                            );
                        }
                        try {
                            auto hex = std::stoi(hexStr, nullptr, 16);
                            color = cc3x(hex);
                        } catch(...) {
                            Mod::get()->logInfo(
                                "Invalid hex \"" + hexStr + "\""
                                "(At " + std::to_string(index) + ")",
                                Severity::Warning 
                            );
                        }
                    }
                    tagStack.push_back(tagName);
                    colorStack.push_back(to4B(color));
                    currentColor = to4B(color);
                }
            }
            tagData = "";
            collectingTag = false;
            collectCurrent = false;
            closingTag = false;
            renderLast = true;
        }

        if (collectingTag) {
            if (c == '/') {
                closingTag = true;
            }
            collectCurrent = false;
            tagData += c;
        }

        if (c == '<' && !escapeSpecial) {
            if (collectingTag) {
                Mod::get()->logInfo(
                    "Found another '<' inside tag while parsing markdown, "
                    "did you forget to close the other one? Note that in "
                    "GD markdown, if you want to include the '<' character "
                    "you need to escape it with a '\\'. (At " + std::to_string(index) + ")",
                    Severity::Warning 
                );
            } else {
                collectingTag = true;
                renderLast = true;
                collectCurrent = false;
            }
        }

        if (c == '!' && next == '[' && !escapeSpecial) {
            linkIsImage = true;
            collectCurrent = false;
        }

        if (c == '[' && !escapeSpecial) {
            collectingLinkText = true;
            collectCurrent = false;
            currentColor = { 148, 218, 255, 255 };
            renderLast = true;
        }

        if (collectingLinkText && c == ']') {
            collectCurrent = false;
        }

        if (collectingLinkText && last == ']') {
            collectingLinkText = false;
            if (c == '(') {
                collectingLink = true;
                collectCurrent = false;
            } else {
                renderLast = true;
            }
        }

        if (collectingLink) {
            if (c == ')') {
                linkData = linkData.substr(1);
                if (linkIsImage) {
                    if (string_utils::contains(linkData, ":")) {
                        if (linkData._Starts_with("frame:")) {
                            auto frameName = linkData.substr(linkData.find(":") + 1);
                            auto spr = CCSprite::createWithSpriteFrameName(frameName.c_str());
                            if (spr) {
                                m_renderer->renderNode(spr);
                                collectingText = "";
                            } else {
                                renderLast = true;
                            }
                        } else {
                            renderLast = true;
                        }
                    } else {
                        auto spr = CCSprite::create(linkData.c_str());
                        if (spr) {
                            m_renderer->renderNode(spr);
                            collectingText = "";
                        } else {
                            renderLast = true;
                        }
                    }
                } else {
                    renderLast = true;
                }
                linkData = "";
                linkIsImage = false;
            } else {
                linkData += c;
            }
            collectCurrent = false;
        }

        if (collectingLink && last == ')') {
            collectingLink = false;
            currentColor = colorStack.back();
        }

        if (collectCurrent) {
            collectingText += c;
        }

        if (renderLast) {
            auto font = *currentFont;
            font.m_bmScaleOrTTFize = currentSize;
            font.m_caps = currentCaps;
            font.m_color = currentColor;
            CC_SAFE_RELEASE(lastRendered);
            lastRendered = m_renderer->renderString(collectingText, font);
            CC_SAFE_RETAIN(lastRendered);
            renderLast = false;
            collectingText = "";
        }
        
        if (c == '\n') {
            currentFont = &normalFont;
            currentSize = .5f;
            currentCaps = TextCapitalization::Normal;
        }

        last = c;
    }
    CC_SAFE_RELEASE(lastRendered);

    m_renderer->end();
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

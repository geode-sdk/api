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
    int style = TextStyleRegular;
    int collectingStars = 0;
    float currentSize = .5f;
    TextCapitalization currentCaps = TextCapitalization::Normal;
    std::string collectingText = "";
    for (auto& c : m_text) {
        bool renderLast = false;
        bool collectCurrent = true;

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
                    case 1: currentSize = 1.f; break;
                    case 2: currentSize = .8f; break;
                    case 3: 
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

        if (collectCurrent) {
            collectingText += c;
        }

        if (renderLast) {
            auto font = *currentFont;
            font.m_bmScaleOrTTFize = currentSize;
            font.m_caps = currentCaps;
            m_renderer->renderString(collectingText, font);
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

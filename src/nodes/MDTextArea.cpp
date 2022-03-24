#include <nodes/MDTextArea.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

bool MDTextArea::init(
    std::string const& str,
    CCSize const& size
) {
    if (!CCNode::init())
        return false;

    m_text = str;
    m_obContentSize = size;
    m_renderer = FontRenderer::create();

    this->updateLabel();

    return true;
}

MDTextArea::~MDTextArea() {
    CC_SAFE_RELEASE(m_renderer);
}

void MDTextArea::updateLabel() {
    m_renderer->begin(this);

    FontRenderer::Font normalFont = { "chatFont.fnt", false, .5f };
    FontRenderer::Font h1Font = { "goldFont.fnt", false, 1.f };
    FontRenderer::Font h2Font = { "bigFont.fnt", false, .6f };
    FontRenderer::Font h3Font = { "chatFont.fnt", false, .6f };
    h3Font.m_caps = TextCapitalization::AllUpper;

    enum TextStyle {
        TextStyleRegular,
        TextStyleItalic,
        TextStyleBold,
    };

    FontRenderer::Font* currentFont = &normalFont;
    char last = 0;
    int headingSize = 0;
    bool collectingHeading = false;
    bool escapeSpecial = false;
    int style = TextStyleRegular;
    int collectingStars = 0;
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
                switch (headingSize) {
                    case 1: currentFont = &h1Font; break;
                    case 2: currentFont = &h2Font; break;
                    case 3: currentFont = &h3Font; break;
                    default: currentFont = &normalFont;
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
                    if (style) {
                        // m_renderer->overrideTextStyle(style);
                    } else {
                        // m_renderer->restoreTextStyle();
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
            m_renderer->renderString(collectingText, *currentFont);
            renderLast = false;
            collectingText = "";
        }
        
        if (c == '\n') {
            currentFont = &normalFont;
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

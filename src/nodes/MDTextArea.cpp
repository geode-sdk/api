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
    m_renderer = BMFontRenderer::create();

    this->updateLabel();

    return true;
}

MDTextArea::~MDTextArea() {
    CC_SAFE_RELEASE(m_renderer);
}

void MDTextArea::updateLabel() {
    m_renderer->begin(this);

    auto normalFont = m_renderer->addFont("chatFont.fnt", .5f);
    auto h1Font = m_renderer->addFont("chatFont.fnt", 1.f, TextStyleBold);
    auto h2Font = m_renderer->addFont("chatFont.fnt", .8f, TextStyleBold);
    auto h3Font = m_renderer->addFont("chatFont.fnt", .6f, TextStyleBold);

    m_renderer->getFont(h3Font)->m_caps = TextCapitalization::AllUpper;

    char last = 0;
    int headingSize = 0;
    bool collectingHeading = false;
    bool escapeSpecial = false;
    int style = TextStyleRegular;
    int collectingStars = 0;
    for (auto& c : m_text) {
        if ((last == '\n' || last == 0) && c == '#') {
            collectingHeading = true;
            headingSize = 0;
        }
        if (collectingHeading) {
            if (c == '#') {
                headingSize++;
            } else if (c != ' ') {
                collectingHeading = false;
                switch (headingSize) {
                    case 1: m_renderer->selectFont(h1Font); break;
                    case 2: m_renderer->selectFont(h2Font); break;
                    case 3: m_renderer->selectFont(h3Font); break;
                    default: m_renderer->selectFont(normalFont);
                }
            }
        }

        #define FLIP_FLAG(flag) \
            if (!(style & flag)) style |= flag; else style &= ~flag;
        
        /*
            esc char -> escres
            \   \       0
            \   0       \
            0   \       \
            0   0       0
        */
        escapeSpecial ^= c == '\\';

        if (!escapeSpecial) {
            if (c == '*') {
                collectingStars++;
            } else {
                if (collectingStars) {
                    std::cout << "collected " << collectingStars << " stars\n";
                    switch (collectingStars) {
                        case 1: FLIP_FLAG(TextStyleItalic); break;
                        case 2: FLIP_FLAG(TextStyleBold); break;
                        case 3: FLIP_FLAG((TextStyleBold & TextStyleItalic)); break;
                    }
                    if (style) {
                        std::cout << "overrode style to " << style << "\n";
                        m_renderer->overrideTextStyle(style);
                    } else {
                        std::cout << "restored style\n";
                        m_renderer->restoreTextStyle();
                    }
                    collectingStars = 0;
                }
            }
        }

        if (c == '\n') {
            m_renderer->selectFont(normalFont);
            headingSize = 0;
        }

        if (!collectingHeading && (c != '*' || escapeSpecial)) {
            m_renderer->renderChar(c);
        }

        last = c;
    }

    m_renderer->end();

    CCARRAY_FOREACH_B_TYPE(m_pChildren, child, CCNode) {
        child->setPosition(child->getPosition() - this->getContentSize() / 2);
    }
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

#include <nodes/BMFontRenderer.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

bool BMFontRenderer::Font::load(const char* bmFont) {
    m_configuration = FNTConfigLoadFile(bmFont);
    if (!m_configuration) return false;

    m_configuration->retain();
    m_texture = CCTextureCache::sharedTextureCache()->addImage(
        m_configuration->getAtlasName(), false
    );
    m_texture->retain();

    m_batchNode = CCSpriteBatchNode::createWithTexture(m_texture);
    m_batchNode->retain();

    return m_texture;
}

int BMFontRenderer::Font::kerningAmountForFirst(unsigned short first, unsigned short second) {
    if (m_configuration->m_pKerningDictionary) {
        tCCKerningHashElement* element = nullptr;
        unsigned int key = (first << 16) | (second & 0xffff);
        HASH_FIND_INT(m_configuration->m_pKerningDictionary, &key, element);        
        if (element) return element->amount;
    }
    return 0;
}

BMFontRenderer::Font::~Font() {
    CC_SAFE_RELEASE(m_batchNode);
    CC_SAFE_RELEASE(m_texture);
    CC_SAFE_RELEASE(m_configuration);
}

bool BMFontRenderer::init() {
    return true;
}

BMFontRenderer::Font* BMFontRenderer::getFont(FontID font) const {
    if (font == s_invalidFontID || font >= m_fonts.size()) {
        return nullptr;
    }
    return m_fonts[font];
}

void BMFontRenderer::begin(CCNode* target, CCPoint const& pos, CCSize const& size) {
    m_target = target ? target : CCNode::create();
    m_target->setContentSize(CCSizeZero);
    m_origin = CC_POINT_PIXELS_TO_POINTS(pos);
    m_cursor = m_origin;
}

CCNode* BMFontRenderer::end() {
    for (auto& font : m_fonts) {
        delete font;
    }
    m_fonts.clear();
    m_selectedFont = nullptr;
    m_cursor = m_origin;
    auto ret = m_target;
    m_target = nullptr;
    return ret;
}

void BMFontRenderer::moveCursor(CCPoint const& pos) {
    m_cursor = pos;
}

BMFontRenderer::FontID BMFontRenderer::addFont(
    const char* bmFile,
    float scale,
    int style,
    ccColor4B color,
    float spacing,
    TextCapitalization caps,
    float italicSkew
) {
    if (!m_target) return s_invalidFontID;
    auto font = new Font;
    if (!font->load(bmFile)) {
        delete font;
        return s_invalidFontID;
    }
    font->m_scale = scale;
    font->m_spacing = spacing;
    font->m_color = color;
    font->m_caps = caps;
    font->m_style = style;
    font->m_italicSkew = italicSkew;
    m_target->addChild(font->m_batchNode);
    auto id = m_fonts.size();
    m_fonts.push_back(font);
    return id;
}

void BMFontRenderer::selectWrap(TextWrap wrap) {
    m_selectedWrap = wrap;
}

void BMFontRenderer::selectFont(FontID font) {
    if (font != s_invalidFontID && font < m_fonts.size()) {
        m_selectedFont = m_fonts.at(font);
    }
    m_overrideStyle.m_do = false;
}

void BMFontRenderer::overrideTextStyle(int flags) {
    m_overrideStyle.m_value = flags;
    m_overrideStyle.m_do = true;
}

void BMFontRenderer::restoreTextStyle() {
    m_overrideStyle.m_do = false;
}

void BMFontRenderer::renderChar(BMFontRenderer::Char character) {
    if (!m_target || !m_selectedFont) return;

    if (character == '\n') {
        m_cursor.x = m_origin.x;
        m_cursor.y -= m_selectedFont->m_configuration->m_nCommonHeight * m_selectedFont->m_scale;
        return;
    }

    switch (m_selectedFont->m_caps) {
        case TextCapitalization::AllUpper: character = std::toupper(character); break;
        case TextCapitalization::AllLower: character = std::tolower(character); break;
    }

    auto charSet = m_selectedFont->m_configuration->getCharacterSet();
    if (!charSet->count(character)) {
        return;
    }

    tCCFontDefHashElement* element = nullptr;
    HASH_FIND_INT(m_selectedFont->m_configuration->m_pFontDefDictionary, &character, element);
    if (!element && character != ' ') return;

    auto fontDef = element->fontDef;
    auto rect = CC_RECT_PIXELS_TO_POINTS(fontDef.rect);

    auto textStyle = m_overrideStyle.m_do ? m_overrideStyle.m_value : m_selectedFont->m_style;

    auto spr = CCSprite::createWithTexture(m_selectedFont->m_texture, rect);
    spr->setTextureRect(rect, false, rect.size);
    spr->setPosition(m_cursor);

    auto kerningAmount = m_selectedFont->kerningAmountForFirst(character, m_lastRendered);

    int yOffset = m_selectedFont->m_configuration->m_nCommonHeight - fontDef.yOffset;
    CCPoint fontPos = ccp(
        m_cursor.x + (fontDef.xOffset + fontDef.rect.size.width * .5f + kerningAmount) * m_selectedFont->m_scale,
        m_cursor.y + (yOffset - rect.size.height * .5f * CC_CONTENT_SCALE_FACTOR()) * m_selectedFont->m_scale
    );
    spr->setPosition(CC_POINT_PIXELS_TO_POINTS(fontPos));
    spr->setVisible(true);
    spr->setColor(to3B(m_selectedFont->m_color));
    spr->setOpacity(m_selectedFont->m_color.a);
    spr->setScale(m_selectedFont->m_scale);
    if (textStyle & TextStyleItalic) {
        spr->setSkewX(m_selectedFont->m_italicSkew);
    }
    if (textStyle & TextStyleBold) {
        // spr->setColor({ 155, 80, 255 });
    }

    if (m_selectedFont->m_spacing) {
        m_cursor.x += m_selectedFont->m_spacing * m_selectedFont->m_scale * CC_CONTENT_SCALE_FACTOR();
    } else {
        m_cursor.x += (fontDef.xAdvance + kerningAmount) * m_selectedFont->m_scale;
    }
    m_lastRendered = character;

    auto csize = m_target->getContentSize();
    auto cursor = CC_POINT_PIXELS_TO_POINTS(m_cursor);
    auto origin = CC_POINT_PIXELS_TO_POINTS(m_origin);
    if (cursor.x - origin.x > csize.width) csize.width = cursor.x - origin.x;
    if (cursor.y - origin.y > csize.height) csize.height = cursor.y - origin.y;
    m_target->setContentSize(csize);

    m_selectedFont->m_batchNode->addChild(spr);
}

void BMFontRenderer::renderString(std::string const& str) {
    for (auto& c : str) {
        this->renderChar(c);
    }
}

BMFontRenderer::~BMFontRenderer() {
    this->end();
}

BMFontRenderer* BMFontRenderer::create() {
    auto ret = new BMFontRenderer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


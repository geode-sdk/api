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

    auto bigFont = m_renderer->addFont("bigFont.fnt", .5f);
    auto blueFont = m_renderer->addFont("bigFont.fnt", .7f, 25.f, { 0, 0, 255, 255 });
    auto goldFont = m_renderer->addFont("goldFont.fnt", 1.f);

    m_renderer->selectFont(bigFont);
    m_renderer->renderString("wow are ");

    m_renderer->selectFont(blueFont);
    m_renderer->renderString("those");

    m_renderer->selectFont(goldFont);
    m_renderer->renderString(" mixed ");

    m_renderer->selectFont(bigFont);
    m_renderer->renderString("fonts");

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

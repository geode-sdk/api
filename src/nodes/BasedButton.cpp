#include <nodes/BasedButton.hpp>

USE_GEODE_NAMESPACE();

TabButton* TabButton::create(const char* text, CCObject* target, SEL_MenuHandler callback) {
    auto ret = new TabButton();
    auto sprOff = TabButtonSprite::create(text, TabBaseColor::Unselected);
    auto sprOn  = TabButtonSprite::create(text, TabBaseColor::Selected);
    if (ret && ret->init(sprOff, sprOn, target, callback)) {
        ret->m_offButton->m_colorDip = .3f;
        ret->m_offButton->m_colorEnabled = true;
        ret->m_offButton->m_scaleMultiplier = 1.f;
        ret->m_onButton->setEnabled(false);
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

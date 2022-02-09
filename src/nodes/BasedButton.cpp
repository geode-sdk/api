#include <nodes/BasedButton.hpp>

USE_GEODE_NAMESPACE();

TabButton* TabButton::create(const char* text, CCObject* target, SEL_MenuHandler callback) {
    auto ret = new TabButton();
    auto sprOff = TabButtonSprite::create(text, TabBaseColor::Unselected);
    auto sprOn  = TabButtonSprite::create(text, TabBaseColor::Selected);
    if (ret && ret->init(sprOff, sprOn, target, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

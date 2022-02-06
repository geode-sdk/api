#include <nodes/BasedButtonSprite.hpp>

USE_GEODE_NAMESPACE();

bool BasedButtonSprite::init(CCNode* ontop, int type, int size, int color) {
    if (!CCSprite::initWithSpriteFrameName(
        CCString::createWithFormat("GEODE_blank%02d_%02d_%02d.png", type, size, color)->getCString()
    )) return false;

    this->m_type = type;
    this->m_size = size;
    this->m_color = color;
    
    if (ontop) {
        this->m_onTop = ontop;
        this->m_onTop->retain();
        this->addChild(this->m_onTop);
        this->m_onTop->setPosition(this->getContentSize() / 2 + this->getTopOffset());
    }

    return true;
}

CCPoint BasedButtonSprite::getTopOffset() const {
    switch (static_cast<BaseType>(this->m_type)) {
        case BaseType::Circle:
            switch (static_cast<CircleBaseSize>(this->m_size)) {
                case CircleBaseSize::Medium2: return { -1, 2.5f };
                default: break;
            }
            return { -3, -3 };
        default: break;
    }
    return { 0, 0 };
}

bool BasedButtonSprite::initWithSprite(const char* sprName, float sprScale, int type, int size, int color) {
    auto spr = CCSprite::create(sprName);
    if (!spr) return false;
    spr->setScale(sprScale);
    return this->init(spr, type, size, color);
}

bool BasedButtonSprite::initWithSpriteFrameName(const char* sprName, float sprScale, int type, int size, int color) {
    auto spr = CCSprite::createWithSpriteFrameName(sprName);
    if (!spr) return false;
    spr->setScale(sprScale);
    return this->init(spr, type, size, color);
}

BasedButtonSprite::~BasedButtonSprite() {
    CC_SAFE_RELEASE(this->m_onTop);
}

BasedButtonSprite* BasedButtonSprite::create(CCNode* ontop, int type, int size, int color) {
    auto ret = new BasedButtonSprite();
    if (ret && ret->init(ontop, type, size, color)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditorButtonSprite* EditorButtonSprite::create(cocos2d::CCNode* top, EditorBaseColor color) {
    auto ret = new EditorButtonSprite();
    if (ret && ret->init(top, static_cast<int>(BaseType::Editor), 0, static_cast<int>(color))) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditorButtonSprite* EditorButtonSprite::createWithSprite(
    const char* sprName,
    float sprScale,
    EditorBaseColor color
) {
    auto ret = new EditorButtonSprite();
    if (ret && ret->initWithSprite(
        sprName, sprScale,
        static_cast<int>(BaseType::Editor), 0, static_cast<int>(color))
    ) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditorButtonSprite* EditorButtonSprite::createWithSpriteFrameName(
    const char* sprName,
    float sprScale,
    EditorBaseColor color
) {
    auto ret = new EditorButtonSprite();
    if (ret && ret->initWithSpriteFrameName(
        sprName, sprScale,
        static_cast<int>(BaseType::Editor), 0, static_cast<int>(color))
    ) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CircleButtonSprite* CircleButtonSprite::create(
    cocos2d::CCNode* top,
    CircleBaseColor color,
    CircleBaseSize size
) {
    auto ret = new CircleButtonSprite();
    if (ret && ret->init(
        top,
        static_cast<int>(BaseType::Circle), static_cast<int>(size), static_cast<int>(color)
    )) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CircleButtonSprite* CircleButtonSprite::createWithSprite(
    const char* sprName,
    float sprScale,
    CircleBaseColor color,
    CircleBaseSize size
) {
    auto ret = new CircleButtonSprite();
    if (ret && ret->initWithSprite(
        sprName, sprScale,
        static_cast<int>(BaseType::Circle), static_cast<int>(size), static_cast<int>(color)
    )) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CircleButtonSprite* CircleButtonSprite::createWithSpriteFrameName(
    const char* sprName,
    float sprScale,
    CircleBaseColor color,
    CircleBaseSize size
) {
    auto ret = new CircleButtonSprite();
    if (ret && ret->initWithSpriteFrameName(
        sprName, sprScale,
        static_cast<int>(BaseType::Circle), static_cast<int>(size), static_cast<int>(color)
    )) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

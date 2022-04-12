#include "ModInfoLayer.hpp"
#include "../dev/HookListLayer.hpp"
#include "../dev/DevSettingsLayer.hpp"
#include "ModSettingsLayer.hpp"
#include <nodes/BasedButton.hpp>
#include <nodes/MDTextArea.hpp>

bool ModInfoLayer::init(Mod* mod) {
    m_noElasticity = true;
    m_mod = mod;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
	CCSize size { 440.f, 290.f };

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    m_mainLayer = CCLayer::create();
    this->addChild(m_mainLayer);

    auto bg = CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(size);
    bg->setPosition(winSize.width / 2, winSize.height / 2);
    m_mainLayer->addChild(bg);

    m_buttonMenu = CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);

    constexpr float logoSize = 40.f;
    constexpr float logoOffset = 10.f;

    auto nameLabel = CCLabelBMFont::create(
        m_mod->getName().c_str(), "bigFont.fnt"
    );
    nameLabel->setScale(.7f);
    nameLabel->setAnchorPoint({ .0f, .5f });
    m_mainLayer->addChild(nameLabel, 2); 

    auto logoSpr = this->createLogoSpr(mod);
    logoSpr->setScale(logoSize / logoSpr->getContentSize().width);
    m_mainLayer->addChild(logoSpr);

    auto developerStr = "by " + m_mod->getDeveloper();
    auto developerLabel = CCLabelBMFont::create(
        developerStr.c_str(), "goldFont.fnt"
    );
    developerLabel->setScale(.5f);
    developerLabel->setAnchorPoint({ .0f, .5f });
    m_mainLayer->addChild(developerLabel);

    auto logoTitleWidth = std::max(
        nameLabel->getScaledContentSize().width,
        developerLabel->getScaledContentSize().width
    ) + logoSize + logoOffset;

    nameLabel->setPosition(
        winSize.width / 2 - logoTitleWidth / 2 + logoSize + logoOffset,
        winSize.height / 2 + 125.f
    );
    logoSpr->setPosition({
        winSize.width / 2 - logoTitleWidth / 2 + logoSize / 2,
        winSize.height / 2 + 115.f
    });
    developerLabel->setPosition(
        winSize.width / 2 - logoTitleWidth / 2 + logoSize + logoOffset,
        winSize.height / 2 + 105.f
    );


    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();


    auto details = MDTextArea::create(
        mod->getDetails().size() ?
            mod->getDetails() :
            "### No description provided.",
        { 350.f, 147.f}
    );
    details->setPosition(
        winSize.width / 2 - details->getScaledContentSize().width / 2,
        winSize.height / 2 - details->getScaledContentSize().height / 2 - 20.f
    );
    this->m_mainLayer->addChild(details);


    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSpr->setScale(.65f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSpr, this, menu_selector(ModInfoLayer::onSettings)
    );
    settingsBtn->setPosition(-size.width / 2 + 25.f, -size.height / 2 + 25.f);
    m_buttonMenu->addChild(settingsBtn);

    
    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(.8f);

    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeSpr, this, menu_selector(ModInfoLayer::onClose)
    );
    closeBtn->setPosition(-size.width / 2 + 3.f, size.height / 2 - 3.f);
    m_buttonMenu->addChild(closeBtn);

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}

void ModInfoLayer::onDev(CCObject*) {
    auto layer = DevSettingsLayer::create(this->m_mod);
    this->addChild(layer);
    layer->showLayer(false);
}

void ModInfoLayer::onHooks(CCObject*) {
    auto layer = HookListLayer::create(this->m_mod);
    this->addChild(layer);
    layer->showLayer(false);
}

void ModInfoLayer::onSettings(CCObject*) {
    ModSettingsLayer::create(this->m_mod)->show();
}

void ModInfoLayer::keyDown(enumKeyCodes key) {
    if (key == KEY_Escape)
        return this->onClose(nullptr);
    if (key == KEY_Space)
        return;
    
    return FLAlertLayer::keyDown(key);
}

void ModInfoLayer::onClose(CCObject* pSender) {
    this->setKeyboardEnabled(false);
    this->removeFromParentAndCleanup(true);
};

ModInfoLayer* ModInfoLayer::create(Mod* mod) {
    auto ret = new ModInfoLayer;
    if (ret && ret->init(mod)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCNode* ModInfoLayer::createLogoSpr(Mod* mod) {
    CCNode* spr = nullptr;
    if (mod == Loader::getInternalMod()) {
        spr = CCSprite::create("com.geode.api.png");
    } else {
        spr = CCSprite::create(CCString::createWithFormat("%s.png", mod->getID().c_str())->getCString());
    }
    if (!spr) { spr = CCSprite::createWithSpriteFrameName("no-logo.png"_spr); }
    if (!spr) { spr = CCLabelBMFont::create("OwO", "goldFont.fnt"); }
    return spr;
}

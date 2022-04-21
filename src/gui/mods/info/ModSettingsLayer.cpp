#include "ModSettingsLayer.hpp"
#include "../settings/ModSettingsList.hpp"

bool ModSettingsLayer::init(Mod* mod) {
    m_noElasticity = true;
    m_mod = mod;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
	const CCSize size { 440.f, 290.f };

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    m_mainLayer = CCLayer::create();
    this->addChild(m_mainLayer);

    auto bg = CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(size);
    bg->setPosition(winSize.width / 2, winSize.height / 2);
    m_mainLayer->addChild(bg);

    m_buttonMenu = CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);

    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();

	auto nameStr = m_mod->getName() + " Settings";
    auto nameLabel = CCLabelBMFont::create(
        nameStr.c_str(), "bigFont.fnt"
    );
    nameLabel->setPosition(winSize.width / 2, winSize.height / 2 + 120.f);
    nameLabel->setScale(.7f);
    m_mainLayer->addChild(nameLabel, 2); 

	const CCSize listSize { 350.f, 200.f };

    auto bgSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bgSprite->setScale(.5f);
    bgSprite->setColor({ 0, 0, 0 });
    bgSprite->setOpacity(75);
    bgSprite->setContentSize(listSize * 2);
    bgSprite->setPosition(winSize.width / 2, winSize.height / 2);
    m_mainLayer->addChild(bgSprite);

	auto settings = ModSettingsList::create(mod, listSize.width, listSize.height);
	settings->setPosition(winSize.width / 2 - listSize.width / 2, winSize.height / 2 - listSize.height / 2);
	m_mainLayer->addChild(settings);
    
    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(.8f);

    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeSpr,
        this,
        menu_selector(ModSettingsLayer::onClose)
    );
    closeBtn->setPosition(-size.width / 2 + 3.f, size.height / 2 - 3.f);
    m_buttonMenu->addChild(closeBtn);

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}

void ModSettingsLayer::keyDown(enumKeyCodes key) {
    if (key == KEY_Escape)
        return this->onClose(nullptr);
    if (key == KEY_Space)
        return;
    return FLAlertLayer::keyDown(key);
}

void ModSettingsLayer::onClose(CCObject*) {
    this->setKeyboardEnabled(false);
    this->removeFromParentAndCleanup(true);
};

ModSettingsLayer* ModSettingsLayer::create(Mod* mod) {
    auto ret = new ModSettingsLayer();
    if (ret && ret->init(mod)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


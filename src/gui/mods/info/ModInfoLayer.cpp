#include "ModInfoLayer.hpp"
#include "../dev/HookListLayer.hpp"
#include "../dev/DevSettingsLayer.hpp"
#include "ModSettingsLayer.hpp"
#include <nodes/BasedButton.hpp>
#include <nodes/MDTextArea.hpp>

CCMenuItemToggler* ModInfoLayer::createTab(Tab id, const char* text, const char* icon) {
    std::string str = text;
    if (icon) str = "   " + str;

    auto offSpr = ButtonSprite::create(str.c_str(), "goldFont.fnt", "GJ_button_05.png", .7f);
    offSpr->setScale(.7f);
    if (icon) {
        auto offIcon = CCSprite::createWithSpriteFrameName(icon);
        offIcon->setScale(offIcon->getContentSize().height / offSpr->getContentSize().height);
        offIcon->setPosition({
            offSpr->getContentSize().height / 2 + 2.f,
            offSpr->getContentSize().height / 2
        });
        offSpr->addChild(offIcon);
    }

    auto onSpr = ButtonSprite::create(str.c_str(), "goldFont.fnt", "GJ_button_02.png", .7f);
    onSpr->setScale(.7f);
    if (icon) {
        auto onIcon = CCSprite::createWithSpriteFrameName(icon);
        onIcon->setScale(onIcon->getContentSize().height / offSpr->getContentSize().height);
        onIcon->setPosition({
            onSpr->getContentSize().height / 2 + 2.f,
            onSpr->getContentSize().height / 2
        });
        onSpr->addChild(onIcon);
    }

    auto ret = CCMenuItemToggler::create(offSpr, onSpr, this, menu_selector(ModInfoLayer::onTab));
	ret->setTag(static_cast<int>(id));
	this->m_buttonMenu->addChild(ret);
    this->m_tabBtns.push_back(ret);
    return ret;
}

bool ModInfoLayer::init(Mod* mod) {
    this->m_noElasticity = true;

    this->m_mod = mod;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
	CCSize size { 440.f, 290.f };

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    this->m_mainLayer = CCLayer::create();
    this->addChild(this->m_mainLayer);

    auto bg = CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(size);
    bg->setPosition(winSize.width / 2, winSize.height / 2);
    this->m_mainLayer->addChild(bg);

    this->m_buttonMenu = CCMenu::create();
    this->m_mainLayer->addChild(this->m_buttonMenu);

    constexpr float logoSize = 40.f;
    constexpr float logoOffset = 10.f;

    auto nameLabel = CCLabelBMFont::create(
        this->m_mod->getName().c_str(), "bigFont.fnt"
    );
    nameLabel->setScale(.7f);
    nameLabel->setAnchorPoint({ .0f, .5f });
    this->m_mainLayer->addChild(nameLabel, 2); 

    auto logoSpr = this->createLogoSpr(mod);
    logoSpr->setScale(logoSize / logoSpr->getContentSize().width);
    this->m_mainLayer->addChild(logoSpr);

    auto developerStr = "by " + this->m_mod->getDeveloper();
    auto developerLabel = CCLabelBMFont::create(
        developerStr.c_str(), "goldFont.fnt"
    );
    developerLabel->setScale(.5f);
    developerLabel->setAnchorPoint({ .0f, .5f });
    this->m_mainLayer->addChild(developerLabel);

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

	auto infoTab = this->createTab(Tab::Info, "Info", "GJ_infoIcon_001.png");
	this->createTab(Tab::Credits, "Credits", "GJ_starsIcon_001.png");
	this->createTab(Tab::Settings, "Settings", "GJ_hammerIcon_001.png");
	this->createTab(Tab::About, "About", "GJ_infoIcon_001.png");
    this->m_buttonMenu->alignItemsHorizontallyWithPadding(4.f);

    for (auto& btn : this->m_tabBtns) {
        btn->setPositionY(75.f);
    }

    this->onTab(infoTab);
    infoTab->toggle(true);

    auto details = MDTextArea::create(mod->getDetails(), {});
    details->setPosition(winSize / 2);
    this->m_mainLayer->addChild(details);

    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();
    
    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(.8f);

    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeSpr,
        this,
        (SEL_MenuHandler)&ModInfoLayer::onClose
    );
    closeBtn->setUserData(reinterpret_cast<void*>(this));

    this->m_buttonMenu->addChild(closeBtn);

    closeBtn->setPosition(-size.width / 2 + 3.f, size.height / 2 - 3.f);

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}

void ModInfoLayer::onTab(CCObject* pSender) {
    for (auto& tab : this->m_tabBtns) {
        tab->toggle(false);
    }
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
        return onClose(nullptr);
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
    if (!spr) { spr = CCSprite::createWithSpriteFrameName("no-logo.png"_sprite); }
    if (!spr) { spr = CCLabelBMFont::create("OwO", "goldFont.fnt"); }
    return spr;
}

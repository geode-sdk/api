#include "ModInfoLayer.hpp"
#include "../dev/HookListLayer.hpp"
#include "../dev/DevSettingsLayer.hpp"
#include "../settings/ModSettingsLayer.hpp"
#include <nodes/BasedButton.hpp>
#include <nodes/MDTextArea.hpp>
#include <fmt/include/fmt/format.h>
#include <APIInternal.hpp>
#include "../list/ModListView.hpp"
#include <nodes/Scrollbar.hpp>

bool ModInfoLayer::init(Mod* mod, ModListView* list) {
    m_noElasticity = true;
    m_mod = mod;
    m_list = list;

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
    m_mainLayer->addChild(details);

    auto detailsBar = Scrollbar::create(details->getScrollLayer());
    detailsBar->setPosition(
        winSize.width / 2 + details->getScaledContentSize().width / 2 + 20.f,
        winSize.height / 2 - 20.f
    );
    m_mainLayer->addChild(detailsBar);


    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(.85f);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(ModInfoLayer::onInfo)
    );
    infoBtn->setPosition(size.width / 2 - 25.f, size.height / 2 - 25.f);
    m_buttonMenu->addChild(infoBtn);


    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSpr->setScale(.65f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSpr, this, menu_selector(ModInfoLayer::onSettings)
    );
    settingsBtn->setPosition(-size.width / 2 + 25.f, -size.height / 2 + 25.f);
    m_buttonMenu->addChild(settingsBtn);

    if (!mod->getSettings().size()) {
        settingsSpr->setColor({ 150, 150, 150 });
        settingsBtn->setTarget(this, menu_selector(ModInfoLayer::onNoSettings));
    }


    auto devSpr = ButtonSprite::create("Dev Options", "bigFont.fnt", "GJ_button_05.png", .6f);
    devSpr->setScale(.5f);

    auto devBtn = CCMenuItemSpriteExtra::create(
        devSpr, this, menu_selector(ModInfoLayer::onDev)
    );
    devBtn->setPosition(size.width / 2 - 50.f, -size.height / 2 + 25.f);
    m_buttonMenu->addChild(devBtn);


    auto enableBtnSpr = ButtonSprite::create(
        "Enable", "bigFont.fnt", "GJ_button_01.png", .6f
    );
    enableBtnSpr->setScale(.5f);

    auto disableBtnSpr = ButtonSprite::create(
        "Disable", "bigFont.fnt", "GJ_button_06.png", .6f
    );
    disableBtnSpr->setScale(.5f);

    auto enableBtn = CCMenuItemToggler::create(
        disableBtnSpr, enableBtnSpr, this, menu_selector(ModInfoLayer::onEnableMod)
    );
    enableBtn->setPosition(-155.f, 78.f);
    enableBtn->toggle(!m_mod->isEnabled());
    m_buttonMenu->addChild(enableBtn);

    if (!mod->supportsDisabling()) {
        enableBtn->setTarget(this, menu_selector(ModInfoLayer::onDisablingNotSupported));
        enableBtnSpr->setColor({ 150, 150, 150 });
        disableBtnSpr->setColor({ 150, 150, 150 });
    }

    
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

void ModInfoLayer::onEnableMod(CCObject* pSender) {
    if (!APIInternal::get()->m_shownEnableWarning) {
        APIInternal::get()->m_shownEnableWarning = true;
        FLAlertLayer::create(
            "Notice",
            "<cb>Disabling</c> a <cy>mod</c> removes its hooks & patches and "
            "calls its user-defined disable function if one exists. You may "
            "still see some effects of the mod left however, and you may "
            "need to <cg>restart</c> the game to have it fully unloaded.",
            "OK"
        )->show();
        if (m_list) m_list->updateAllStates(nullptr);
        return;
    }
    if (as<CCMenuItemToggler*>(pSender)->isToggled()) {
        auto res = m_mod->enable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Enabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    } else {
        auto res = m_mod->disable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Disabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    }
    if (m_list) m_list->updateAllStates(nullptr);
    as<CCMenuItemToggler*>(pSender)->toggle(m_mod->isEnabled());
}

void ModInfoLayer::onDisablingNotSupported(CCObject* pSender) {
    FLAlertLayer::create(
        "Unsupported",
        "<cr>Disabling</c> is not supported for this mod.",
        "OK"
    )->show();
    as<CCMenuItemToggler*>(pSender)->toggle(m_mod->isEnabled());
}

void ModInfoLayer::onDev(CCObject*) {
    auto layer = DevSettingsLayer::create(m_mod);
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

void ModInfoLayer::onNoSettings(CCObject*) {
    FLAlertLayer::create(
        "No Settings Found",
        "This mod has no customizable settings.",
        "OK"
    )->show();
}

void ModInfoLayer::onInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        ("About " + m_mod->getName()).c_str(),
        fmt::format(
            "<cr>ID: {}</c>\n"
            "<cg>Version: {}</c>\n"
            "<cp>Developer: {}</c>\n"
            "<cb>Path: {}</c>",
            m_mod->getID(),
            m_mod->getVersion().toString(),
            m_mod->getDeveloper(),
            m_mod->getPath()
        ),
        "OK", nullptr, 400.f
    )->show();
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

ModInfoLayer* ModInfoLayer::create(Mod* mod, ModListView* list) {
    auto ret = new ModInfoLayer;
    if (ret && ret->init(mod, list)) {
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

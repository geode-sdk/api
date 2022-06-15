#include "ModInfoLayer.hpp"
#include "../dev/HookListLayer.hpp"
#include "../settings/ModSettingsLayer.hpp"
#include <nodes/BasedButton.hpp>
#include <nodes/MDTextArea.hpp>
#include <APIInternal.hpp>
#include "../list/ModListView.hpp"
#include <nodes/Scrollbar.hpp>
#include <utils/WackyGeodeMacros.hpp>
#include <settings/Setting.hpp>

// TODO: die
#undef min
#undef max

struct UninstalledDependency {
    std::string m_id;
    bool m_isInIndex;
};

void getUninstalledDependenciesRecursive(
    ModInfo const& info,
    std::vector<UninstalledDependency>& deps
) {
    for (auto& dep : info.m_dependencies) {
        UninstalledDependency d;
        d.m_isInIndex = Index::get()->isKnownItem(dep.m_id);
        if (!Loader::get()->isModInstalled(dep.m_id)) {
            d.m_id = dep.m_id;
            deps.push_back(d);
        }
        if (d.m_isInIndex) {
            getUninstalledDependenciesRecursive(
                Index::get()->getKnownItem(dep.m_id).m_info,
                deps
            );
        }
    }
}

std::vector<UninstalledDependency> getUninstalledDependencies(ModInfo const& info) {
    std::vector<UninstalledDependency> deps;
    getUninstalledDependenciesRecursive(info, deps);
    return deps;
}

bool ModInfoLayer::init(ModObject* obj, ModListView* list) {
    m_noElasticity = true;
    m_list = list;
    m_mod = obj->m_mod;

    bool isInstalledMod;
    switch (obj->m_type) {
        case ModObjectType::Mod: {
            m_info = obj->m_mod->getModInfo();
            isInstalledMod = true;
        } break;

        case ModObjectType::Index: {
            m_info = obj->m_index.m_info;
            isInstalledMod = false;
        } break;

        default: return false;
    }


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
        m_info.m_name.c_str(), "bigFont.fnt"
    );
    nameLabel->setScale(.7f);
    nameLabel->setAnchorPoint({ .0f, .5f });
    m_mainLayer->addChild(nameLabel, 2); 

    auto logoSpr = this->createLogoSpr(obj);
    logoSpr->setScale(logoSize / logoSpr->getContentSize().width);
    m_mainLayer->addChild(logoSpr);

    auto developerStr = "by " + m_info.m_developer;
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
        m_info.m_details.size() ?
            m_info.m_details :
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


    if (isInstalledMod) {
        auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        settingsSpr->setScale(.65f);

        auto settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSpr, this, menu_selector(ModInfoLayer::onSettings)
        );
        settingsBtn->setPosition(-size.width / 2 + 25.f, -size.height / 2 + 25.f);
        m_buttonMenu->addChild(settingsBtn);

	    if (!SettingManager::with(m_mod)->hasSettings()) {
	        settingsSpr->setColor({ 150, 150, 150 });
	        settingsBtn->setTarget(this, menu_selector(ModInfoLayer::onNoSettings));
	    }

        auto devSpr = ButtonSprite::create("Dev Options", "bigFont.fnt", "GJ_button_05.png", .6f);
        devSpr->setScale(.5f);

        auto devBtn = CCMenuItemSpriteExtra::create(
            devSpr, this, nullptr
        );
        devBtn->setPosition(size.width / 2 - 50.f, -size.height / 2 + 25.f);
        m_buttonMenu->addChild(devBtn);


        auto enableBtnSpr = ButtonSprite::create(
            "Enable", "bigFont.fnt", "GJ_button_01.png", .6f
        );
        enableBtnSpr->setScale(.6f);

        auto disableBtnSpr = ButtonSprite::create(
            "Disable", "bigFont.fnt", "GJ_button_06.png", .6f
        );
        disableBtnSpr->setScale(.6f);

        auto enableBtn = CCMenuItemToggler::create(
            disableBtnSpr, enableBtnSpr, this, menu_selector(ModInfoLayer::onEnableMod)
        );
        enableBtn->setPosition(-155.f, 75.f);
        enableBtn->toggle(!obj->m_mod->isEnabled());
        m_buttonMenu->addChild(enableBtn);

        if (!m_info.m_supportsDisabling) {
            enableBtn->setTarget(this, menu_selector(ModInfoLayer::onDisablingNotSupported));
            enableBtnSpr->setColor({ 150, 150, 150 });
            disableBtnSpr->setColor({ 150, 150, 150 });
        }

        if (
            m_mod != Loader::get()->getInternalMod() &&
            m_mod != Mod::get()
        ) {
            auto uninstallBtnSpr = ButtonSprite::create(
                "Uninstall", "bigFont.fnt", "GJ_button_05.png", .6f
            );
            uninstallBtnSpr->setScale(.6f);

            auto uninstallBtn = CCMenuItemSpriteExtra::create(
                uninstallBtnSpr, this,
                menu_selector(ModInfoLayer::onUninstall)
            );
            uninstallBtn->setPosition(-85.f, 75.f);
            m_buttonMenu->addChild(uninstallBtn);
        }
    } else {
        auto installBtnSpr = ButtonSprite::create(
            "Install", "bigFont.fnt", "GJ_button_02.png", .6f
        );
        installBtnSpr->setScale(.6f);

        m_installBtn = CCMenuItemSpriteExtra::create(
            installBtnSpr, this,
            menu_selector(ModInfoLayer::onInstallMod)
        );
        m_installBtn->setPosition(-155.f, 75.f);
        m_buttonMenu->addChild(m_installBtn);

        m_loadingLabel = CCLabelBMFont::create(
            "", "bigFont.fnt"
        );
        m_loadingLabel->setPosition(winSize.width / 2 - 75.f, winSize.height / 2 + 75.f);
        m_loadingLabel->setAnchorPoint({ .0f, .5f });
        m_loadingLabel->setScale(.35f);
        m_mainLayer->addChild(m_loadingLabel);
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

void ModInfoLayer::onInstallMod(CCObject*) {
    this->updateInstallStatus("Checking dependencies");
    auto deps = getUninstalledDependencies(m_info);
    if (deps.size()) {
        std::vector<std::string> unknownDeps;
        for (auto& dep : deps) {
            if (!dep.m_isInIndex) {
                unknownDeps.push_back(dep.m_id);
            }
        }
        if (unknownDeps.size()) {
            std::string list = "";
            for (auto& ud : unknownDeps) {
                list += "<cp>" + ud + "</c>, ";
            }
            list.pop_back();
            list.pop_back();
            return FLAlertLayer::create(
                nullptr,
                "Unknown Dependencies",
                "This mod or its dependencies <cb>depends</c> on the following "
                "unknown mods: " + list + ". You will have to manually "
                "install these mods before you can install this one.",
                "OK", nullptr, 360.f
            )->show();
        }
        std::string list = "";
        m_installing = {};
        for (auto& d : deps) {
            list += "<cy>" + d.m_id + "</c>, ";
            m_installing.push_back(d.m_id);
        }
        m_installing.push_back(m_info.m_id);
        list.pop_back();
        list.pop_back();
        auto layer = FLAlertLayer::create(
            this,
            "Installing Dependencies",
            "The following <cb>dependencies</c> will be installed: " + list + ".",
            "Cancel", "OK", 360.f
        );
        layer->setTag(4);
        layer->show();
    } else {
        this->installMod(m_info.m_id);
    }
}

void ModInfoLayer::onUninstall(CCObject*) {
    auto layer = FLAlertLayer::create(
        this,
        "Confirm Uninstall",
        "Are you sure you want to uninstall <cr>" + m_info.m_name + "</c>?",
        "Cancel", "OK"
    );
    layer->setTag(5);
    layer->show();
}

void ModInfoLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    switch (layer->getTag()) {
        case 4: {
            if (btn2) {
                if (m_installing.size()) {
                    this->installMod(m_installing.back());
                    m_installing.pop_back();
                }
            } else {
                this->updateInstallStatus();
            }
        } break;

        case 5: {
            if (btn2) {
                this->uninstall();
            }
        } break;

        case 6: {
            if (btn2) {
                if (ghc::filesystem::remove_all(m_mod->getSaveDir())) {
                    FLAlertLayer::create(
                        "Deleted",
                        "The mod's save data was deleted.",
                        "OK"
                    )->show();
                } else {
                    FLAlertLayer::create(
                        "Error",
                        "Unable to delete mod's save directory!",
                        "OK"
                    )->show();
                }
            }
            m_list->refreshList();
            this->onClose(nullptr);
        } break;
    }
}

void ModInfoLayer::updateInstallStatus(std::string const& status) {
    if (status.size()) {
        if (!m_loadingCircle) {
            m_loadingCircle = LoadingCircle::create();
            m_loadingCircle->setPosition(-90.f, 75.f);
            m_loadingCircle->setScale(.25f);
            m_loadingCircle->show();
        }
        m_loadingLabel->setString(status.c_str());
    } else {
        if (m_loadingCircle) {
            m_loadingCircle->fadeAndRemove();
            m_loadingCircle = nullptr;
        }
        m_loadingLabel->setString("");
    }
}

void ModInfoLayer::modInstallProgress(std::string const& info, uint8_t percentage) {
    this->updateInstallStatus(info + " (" + std::to_string(percentage) + "%)");
}

void ModInfoLayer::modInstallFailed(std::string const& info) {
    m_installing = {};
    this->updateInstallStatus("");
    FLAlertLayer::create(
        "Installation failed :(", info, "OK"
    )->show();
}

void ModInfoLayer::modInstallFinished() {
    if (m_installing.size()) {
        this->installMod(m_installing.back());
        m_installing.pop_back();
    } else {
        this->updateInstallStatus("");
        FLAlertLayer::create(
            "Install complete",
            "Mod succesfully installed! :) (You may need to <cy>restart the game</c> "
            "for the mod to take full effect)",
            "OK"
        )->show();
    }
    m_list->refreshList();
    this->onClose(nullptr);
}

void ModInfoLayer::installMod(std::string const& id) {
    this->updateInstallStatus("Installing " + id);
    Index::get()->installItem(id);
}

void ModInfoLayer::uninstall() {
    auto res = m_mod->uninstall();
    if (!res) {
        return FLAlertLayer::create(
            "Uninstall failed :(",
            res.error(),
            "OK"
        )->show();
    }
    auto layer = FLAlertLayer::create(
        this,
        "Uninstall complete",
        "Mod was succesfully uninstalled! :) (You may need to <cy>restart the game</c> "
        "for the mod to take full effect). <co>Would you also like to delete the mod's "
        "save data?</c>",
        "Cancel", "Delete", 350.f
    );
    layer->setTag(6);
    layer->show();
}

void ModInfoLayer::onDisablingNotSupported(CCObject* pSender) {
    FLAlertLayer::create(
        "Unsupported",
        "<cr>Disabling</c> is not supported for this mod.",
        "OK"
    )->show();
    as<CCMenuItemToggler*>(pSender)->toggle(m_mod->isEnabled());
}

void ModInfoLayer::onHooks(CCObject*) {
    auto layer = HookListLayer::create(this->m_mod);
    this->addChild(layer);
    layer->showLayer(false);
}

void ModInfoLayer::onSettings(CCObject*) {
    //ModSettingsLayer::create(this->m_mod)->show();
    // FIXME: No settings yet
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
        ("About " + m_info.m_name).c_str(),
        // fmt::format(
        //     "<cr>ID: {}</c>\n"
        //     "<cg>Version: {}</c>\n"
        //     "<cp>Developer: {}</c>\n"
        //     "<cb>Path: {}</c>",
        //     m_info.m_id,
        //     m_info.m_version.toString(),
        //     m_info.m_developer,
        //     m_info.m_path.string()
        // ),
        "",
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
    if (ret && ret->init(new ModObject(mod), list)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ModInfoLayer* ModInfoLayer::create(ModObject* obj, ModListView* list) {
    auto ret = new ModInfoLayer;
    if (ret && ret->init(obj, list)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCNode* ModInfoLayer::createLogoSpr(ModObject* modObj) {
    switch (modObj->m_type) {
        case ModObjectType::Mod:
            return ModInfoLayer::createLogoSpr(modObj->m_mod);
        
        case ModObjectType::Index:
            return ModInfoLayer::createLogoSpr(modObj->m_index);
        
        default: {
            CCNode* spr = CCSprite::createWithSpriteFrameName("no-logo.png"_spr);
            if (!spr) { spr = CCLabelBMFont::create("OwO", "goldFont.fnt"); }
            return spr;
        }
    }
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

CCNode* ModInfoLayer::createLogoSpr(IndexItem const& item) {
    CCNode* spr = nullptr;
    auto logoPath = ghc::filesystem::absolute(item.m_path / "logo.png");
    spr = CCSprite::create(logoPath.string().c_str());
    if (!spr) { spr = CCSprite::createWithSpriteFrameName("no-logo.png"_spr); }
    if (!spr) { spr = CCLabelBMFont::create("OwO", "goldFont.fnt"); }
    return spr;
}

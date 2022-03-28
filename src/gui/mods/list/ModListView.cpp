#include "ModListView.hpp"
#include "../info/ModInfoLayer.hpp"
#include <utils/WackyGeodeMacros.hpp>
#include <APIInternal.hpp>

ModCell::ModCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void ModCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

void ModCell::onFailedInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "Error Info",
        this->m_obj->m_info.m_reason.size() ?
            this->m_obj->m_info.m_reason :
            this->m_obj->m_mod->getLoadErrorInfo(),
        "OK",
        nullptr,
        360.f
    )->show();
}

void ModCell::loadFromFailureInfo(Loader::UnloadedModInfo info) {
    this->m_mainLayer->setVisible(true);
    
    auto menu = CCMenu::create();
    menu->setPosition(this->m_width - this->m_height, this->m_height / 2);
    this->m_mainLayer->addChild(menu);

    auto titleLabel = CCLabelBMFont::create("Failed to Load", "bigFont.fnt");
    titleLabel->setAnchorPoint({ .0f, .5f });
    titleLabel->setScale(.5f);
    titleLabel->setPosition(this->m_height / 2, this->m_height / 2 + 7.f);
    this->m_mainLayer->addChild(titleLabel);
    
    auto pathLabel = CCLabelBMFont::create(info.m_file.c_str(), "chatFont.fnt");
    pathLabel->setAnchorPoint({ .0f, .5f });
    pathLabel->setScale(.43f);
    pathLabel->setPosition(this->m_height / 2, this->m_height / 2 - 7.f);
    pathLabel->setColor({ 255, 255, 0 });
    this->m_mainLayer->addChild(pathLabel);

    auto whySpr = ButtonSprite::create(
        "Info", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f
    );
    whySpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        whySpr, this, menu_selector(ModCell::onFailedInfo)
    );
    menu->addChild(viewBtn);
}

void ModCell::loadFromMod(ModObject* modobj) {
    this->m_mod = modobj->m_mod;
    this->m_obj = modobj;

    if (!modobj->m_mod) {
        return this->loadFromFailureInfo(modobj->m_info);
    }

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
    
    auto menu = CCMenu::create();
    menu->setPosition(this->m_width - this->m_height, this->m_height / 2);
    this->m_mainLayer->addChild(menu);

    auto logoSize = this->m_height - 12.f;

    auto logoSpr = ModInfoLayer::createLogoSpr(modobj->m_mod);
    logoSpr->setPosition({ logoSize / 2 + 12.f, this->m_height / 2 });
    logoSpr->setScale(logoSize / logoSpr->getContentSize().width);
    this->m_mainLayer->addChild(logoSpr);

    auto titleLabel = CCLabelBMFont::create(
        this->m_mod->getName().c_str(), "bigFont.fnt"
    );
    titleLabel->setAnchorPoint({ .0f, .5f });
    titleLabel->setScale(.5f);
    titleLabel->setPosition(this->m_height / 2 + logoSize, this->m_height / 2 + 7.f);
    this->m_mainLayer->addChild(titleLabel);

    auto versionLabel = CCLabelBMFont::create(
        this->m_mod->getVersion().toString().c_str(), "bigFont.fnt"
    );
    versionLabel->setAnchorPoint({ .0f, .5f });
    versionLabel->setScale(.3f);
    versionLabel->setPosition(
        titleLabel->getPositionX() + titleLabel->getScaledContentSize().width + 5.f,
        this->m_height / 2 + 7.f
    );
    versionLabel->setColor({ 0, 255, 0 });
    this->m_mainLayer->addChild(versionLabel);
    
    auto creatorStr = "by " + this->m_mod->getDeveloper();
    auto creatorLabel = CCLabelBMFont::create(
        creatorStr.c_str(), "goldFont.fnt"
    );
    creatorLabel->setAnchorPoint({ .0f, .5f });
    creatorLabel->setScale(.43f);
    creatorLabel->setPosition(this->m_height / 2 + logoSize, this->m_height / 2 - 7.f);
    this->m_mainLayer->addChild(creatorLabel);

    auto viewSpr = this->m_mod->wasSuccesfullyLoaded() ?
        ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", .8f) :
        ButtonSprite::create("Why", "bigFont.fnt", "GJ_button_06.png", .8f);
    viewSpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, 
            this->m_mod->wasSuccesfullyLoaded() ?
                menu_selector(ModCell::onInfo) :
                menu_selector(ModCell::onFailedInfo)
    );
    menu->addChild(viewBtn);

    if (this->m_mod->wasSuccesfullyLoaded() && this->m_mod->supportsDisabling()) {
        this->m_enableToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModCell::onEnable), .7f
        );
        this->m_enableToggle->setPosition(-50.f, 0.f);
        menu->addChild(this->m_enableToggle);
    }

    auto exMark = CCSprite::createWithSpriteFrameName("exMark_001.png");
    exMark->setScale(.5f);

    this->m_unresolvedExMark = CCMenuItemSpriteExtra::create(
        exMark, this, menu_selector(ModCell::onUnresolvedInfo)
    );
    this->m_unresolvedExMark->setPosition(-80.f, 0.f);
    menu->addChild(this->m_unresolvedExMark);

    if (!this->m_mod->wasSuccesfullyLoaded()) {
        this->m_unresolvedExMark->setVisible(false);
    }

    this->updateState();
}

void ModCell::onInfo(CCObject*) {
    ModInfoLayer::create(this->m_mod)->show();
}

void ModCell::updateBGColor(int index) {
	if (index & 1) m_backgroundLayer->setColor(ccc3(0xc2, 0x72, 0x3e));
    else m_backgroundLayer->setColor(ccc3(0xa1, 0x58, 0x2c));
    m_backgroundLayer->setOpacity(0xff);
}

void ModCell::onEnable(CCObject* pSender) {
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
        this->m_list->updateAllStates(this);
        return;
    }
    if (!as<CCMenuItemToggler*>(pSender)->isToggled()) {
        auto res = this->m_mod->enable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Enabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    } else {
        auto res = this->m_mod->disable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Disabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    }
    this->m_list->updateAllStates(this);
}

void ModCell::onUnresolvedInfo(CCObject* pSender) {
    std::string info = "This mod has the following <cr>unresolved dependencies</c>: ";
    for (auto const& dep : this->m_mod->getUnresolvedDependencies()) {
        info += "<cg>" + dep.m_id + "</c> (<cy>" + dep.m_version.toString() + "</c>), ";
    }
    info.pop_back();
    info.pop_back();
    FLAlertLayer::create(
        nullptr,
        "Unresolved Dependencies",
        info,
        "OK", nullptr,
        400.f 
    )->show();
}

bool ModCell::init(ModListView* list) {
    this->m_list = list;
    return true;
}

void ModCell::updateState(bool invert) {
    bool unresolved = this->m_mod->hasUnresolvedDependencies();
    if (this->m_enableToggle) {
        this->m_enableToggle->toggle(this->m_mod->isEnabled() ^ invert);
        this->m_enableToggle->setEnabled(!unresolved);
        this->m_enableToggle->m_offButton->setOpacity(unresolved ? 100 : 255);
        this->m_enableToggle->m_offButton->setColor(unresolved ? cc3x(155) : cc3x(255));
        this->m_enableToggle->m_onButton->setOpacity(unresolved ? 100 : 255);
        this->m_enableToggle->m_onButton->setColor(unresolved ? cc3x(155) : cc3x(255));
    }

    this->m_unresolvedExMark->setVisible(unresolved);
}

ModCell* ModCell::create(ModListView* list, const char* key, CCSize size) {
    auto pRet = new ModCell(key, size);
    if (pRet && pRet->init(list)) {
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void ModListView::updateAllStates(ModCell* toggled) {
    CCARRAY_FOREACH_B_TYPE(this->m_tableView->m_cellArray, cell, ModCell) {
        cell->updateState(toggled == cell);
    }
}

void ModListView::setupList() {
    this->m_itemSeparation = 40.0f;

    if (!this->m_entries->count()) return;

    this->m_tableView->reloadData();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
    else
        this->m_tableView->moveToTop();
}

TableViewCell* ModListView::getListCell(const char* key) {
    return ModCell::create(this, key, { this->m_width, this->m_itemSeparation });
}

void ModListView::loadCell(TableViewCell* cell, unsigned int index) {
    auto obj = as<ModObject*>(this->m_entries->objectAtIndex(index));
    as<ModCell*>(cell)->loadFromMod(obj);
    if (obj->m_mod && obj->m_mod->wasSuccesfullyLoaded()) {
        as<ModCell*>(cell)->updateBGColor(index);
    } else {
        cell->m_backgroundLayer->setOpacity(255);
        cell->m_backgroundLayer->setColor({ 153, 0, 0 });
    }
}

bool ModListView::filter(Mod* mod, const char* searchFilter, int searchFlags) {
    if (!searchFilter || !strlen(searchFilter)) return true;
    auto check = [searchFlags, searchFilter](SearchFlags flag, std::string const& name) -> bool {
        if (!(searchFlags & flag)) return false;
        return string_utils::contains(
            string_utils::toLower(name),
            string_utils::toLower(searchFilter)
        );
    };
    if (check(SearchFlags::Name,        mod->getName())) return true;
    if (check(SearchFlags::ID,          mod->getID())) return true;
    if (check(SearchFlags::Developer,   mod->getDeveloper())) return true;
    if (check(SearchFlags::Credits,     mod->getCredits())) return true;
    if (check(SearchFlags::Description, mod->getDescription())) return true;
    if (check(SearchFlags::Details,     mod->getDetails())) return true;
    return false;
}

bool ModListView::init(
    CCArray* mods,
    ModListType type,
    float width,
    float height,
    const char* searchFilter,
    int searchFlags
) {
    if (!mods) {
        switch (type) {
            case ModListType::Installed: {
                mods = CCArray::create();
                for (auto const& mod : Loader::get()->getFailedMods()) {
                    mods->addObject(new ModObject(mod));
                }
                auto imod = Loader::getInternalMod();
                if (this->filter(imod, searchFilter, searchFlags)) {
                    mods->addObject(new ModObject(imod));
                }
                for (auto const& mod : Loader::get()->getLoadedMods()) {
                    if (this->filter(mod, searchFilter, searchFlags)) {
                        mods->addObject(new ModObject(mod));
                    }
                }
                if (!mods->count()) {
                    this->m_status = Status::SearchEmpty;
                }
            } break;

            case ModListType::Download: {
                mods = CCArray::create();
                this->m_status = Status::NoModsFound;
            } break;

            case ModListType::Featured: {
                mods = CCArray::create();
                this->m_status = Status::NoModsFound;
            } break;

            default: return false;
        }
    }
    return CustomListView::init(mods, kBoomListType_Mod, width, height);
}

ModListView* ModListView::create(
    CCArray* mods,
    ModListType type,
    float width,
    float height,
    const char* searchFilter,
    int searchFlags
) {
    auto pRet = new ModListView;
    if (pRet) {
        if (pRet->init(mods, type, width, height, searchFilter, searchFlags)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

ModListView* ModListView::create(
    ModListType type,
    float width,
    float height,
    const char* searchFilter,
    int searchFlags
) {
    return ModListView::create(nullptr, type, width, height, searchFilter, searchFlags);
}

ModListView::Status ModListView::getStatus() const {
    return m_status;
}

std::string ModListView::getStatusAsString() const {
    switch (m_status) {
        case Status::OK:          return "";
        case Status::Unknown:     return "Unknown Issue";
        case Status::NoModsFound: return "No Mods Found";
        case Status::SearchEmpty: return "No Mods Match Search Query";
    }
    return "Unrecorded Status";
}

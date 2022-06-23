#include "ModListView.hpp"
#include "../info/ModInfoLayer.hpp"
#include <utils/WackyGeodeMacros.hpp>
#include <APIInternal.hpp>
#include <index/Index.hpp>
#include "ModListLayer.hpp"

ModCell::ModCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void ModCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

void ModCell::onFailedInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "Error Info",
        m_obj->m_info.m_reason.size() ?
            m_obj->m_info.m_reason :
            m_obj->m_mod->getLoadErrorInfo(),
        "OK",
        nullptr,
        360.f
    )->show();
}

void ModCell::setupUnloaded() {
    m_mainLayer->setVisible(true);

    auto menu = CCMenu::create();
    menu->setPosition(m_width - m_height, m_height / 2);
    m_mainLayer->addChild(menu);

    auto titleLabel = CCLabelBMFont::create("Failed to Load", "bigFont.fnt");
    titleLabel->setAnchorPoint({ .0f, .5f });
    titleLabel->setScale(.5f);
    titleLabel->setPosition(m_height / 2, m_height / 2 + 7.f);
    m_mainLayer->addChild(titleLabel);
    
    auto pathLabel = CCLabelBMFont::create(m_obj->m_info.m_file.c_str(), "chatFont.fnt");
    pathLabel->setAnchorPoint({ .0f, .5f });
    pathLabel->setScale(.43f);
    pathLabel->setPosition(m_height / 2, m_height / 2 - 7.f);
    pathLabel->setColor({ 255, 255, 0 });
    m_mainLayer->addChild(pathLabel);

    auto whySpr = ButtonSprite::create(
        "Info", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f
    );
    whySpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        whySpr, this, menu_selector(ModCell::onFailedInfo)
    );
    menu->addChild(viewBtn);
}

void ModCell::setupLoadedButtons() {
    auto viewSpr = m_obj->m_mod->wasSuccesfullyLoaded() ?
        ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", .8f) :
        ButtonSprite::create("Why", "bigFont.fnt", "GJ_button_06.png", .8f);
    viewSpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, 
            m_obj->m_mod->wasSuccesfullyLoaded() ?
                menu_selector(ModCell::onInfo) :
                menu_selector(ModCell::onFailedInfo)
    );
    m_menu->addChild(viewBtn);

    if (m_obj->m_mod->wasSuccesfullyLoaded() && m_obj->m_mod->supportsDisabling()) {
        m_enableToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModCell::onEnable), .7f
        );
        m_enableToggle->setPosition(-50.f, 0.f);
        m_menu->addChild(m_enableToggle);
    }

    auto exMark = CCSprite::createWithSpriteFrameName("exMark_001.png");
    exMark->setScale(.5f);

    m_unresolvedExMark = CCMenuItemSpriteExtra::create(
        exMark, this, menu_selector(ModCell::onUnresolvedInfo)
    );
    m_unresolvedExMark->setPosition(-80.f, 0.f);
    m_menu->addChild(m_unresolvedExMark);

    if (!m_obj->m_mod->wasSuccesfullyLoaded()) {
        m_unresolvedExMark->setVisible(false);
    }
}

void ModCell::setupIndexButtons() {
    auto viewSpr = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", .8f);
    viewSpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(ModCell::onInfo)
    );
    m_menu->addChild(viewBtn);
}

void ModCell::loadFromObject(ModObject* modobj) {
    m_obj = modobj;

    if (modobj->m_type == ModObjectType::Unloaded) {
        return this->setupUnloaded();
    }

    m_mainLayer->setVisible(true);
    m_backgroundLayer->setOpacity(255);
    
    m_menu = CCMenu::create();
    m_menu->setPosition(m_width - m_height, m_height / 2);
    m_mainLayer->addChild(m_menu);

    auto logoSize = m_height - 12.f;

    auto logoSpr = ModInfoLayer::createLogoSpr(modobj);
    logoSpr->setPosition({ logoSize / 2 + 12.f, m_height / 2 });
    logoSpr->setScale(logoSize / logoSpr->getContentSize().width);
    m_mainLayer->addChild(logoSpr);

    ModInfo info;
    switch (modobj->m_type) {
        case ModObjectType::Mod:
            info = modobj->m_mod->getModInfo();
            break;

        case ModObjectType::Index:
            info = modobj->m_index.m_info;
            break;
        
        default: return;
    }

    auto titleLabel = CCLabelBMFont::create(info.m_name.c_str(), "bigFont.fnt");
    titleLabel->setAnchorPoint({ .0f, .5f });
    titleLabel->setPosition(m_height / 2 + logoSize, m_height / 2 + 7.f);
    titleLabel->limitLabelWidth(m_width / 2 - 30.f, .5f, .1f);
    m_mainLayer->addChild(titleLabel);

    auto versionLabel = CCLabelBMFont::create(info.m_version.toString().c_str(), "bigFont.fnt");
    versionLabel->setAnchorPoint({ .0f, .5f });
    versionLabel->setScale(.3f);
    versionLabel->setPosition(
        titleLabel->getPositionX() + titleLabel->getScaledContentSize().width + 5.f,
        m_height / 2 + 7.f
    );
    versionLabel->setColor({ 0, 255, 0 });
    m_mainLayer->addChild(versionLabel);
    
    auto creatorStr = "by " + info.m_developer;
    auto creatorLabel = CCLabelBMFont::create(creatorStr.c_str(), "goldFont.fnt");
    creatorLabel->setAnchorPoint({ .0f, .5f });
    creatorLabel->setScale(.43f);
    creatorLabel->setPosition(m_height / 2 + logoSize, m_height / 2 - 7.f);
    m_mainLayer->addChild(creatorLabel);

    switch (modobj->m_type) {
        case ModObjectType::Mod:
            this->setupLoadedButtons();
            break;

        case ModObjectType::Index:
            this->setupIndexButtons();
            break;

        default: 
        	break;
    }
    this->updateState();
}

void ModCell::onInfo(CCObject*) {
    ModInfoLayer::create(m_obj, m_list)->show();
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
        m_list->updateAllStates(this);
        return;
    }
    if (!as<CCMenuItemToggler*>(pSender)->isToggled()) {
        auto res = m_obj->m_mod->load();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Loading Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
        else {
        	auto res = m_obj->m_mod->enable();
	        if (!res) {
	            FLAlertLayer::create(
	                nullptr,
	                "Error Enabling Mod",
	                res.error(),
	                "OK", nullptr
	            )->show();
	        }
        }
    } else {
        auto res = m_obj->m_mod->disable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Disabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    }
    m_list->updateAllStates(this);
}

void ModCell::onUnresolvedInfo(CCObject* pSender) {
    std::string info = "This mod has the following <cr>unresolved dependencies</c>: ";
    for (auto const& dep : m_obj->m_mod->getUnresolvedDependencies()) {
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
    m_list = list;
    return true;
}

void ModCell::updateState(bool invert) {
    if (m_obj->m_type == ModObjectType::Mod) {
        bool unresolved = m_obj->m_mod->hasUnresolvedDependencies();
        if (m_enableToggle) {
            m_enableToggle->toggle(m_obj->m_mod->isEnabled() ^ invert);
            m_enableToggle->setEnabled(!unresolved);
            m_enableToggle->m_offButton->setOpacity(unresolved ? 100 : 255);
            m_enableToggle->m_offButton->setColor(unresolved ? cc3x(155) : cc3x(255));
            m_enableToggle->m_onButton->setOpacity(unresolved ? 100 : 255);
            m_enableToggle->m_onButton->setColor(unresolved ? cc3x(155) : cc3x(255));
        }
        m_unresolvedExMark->setVisible(unresolved);
    }
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
    CCARRAY_FOREACH_B_TYPE(m_tableView->m_cellArray, cell, ModCell) {
        cell->updateState(toggled == cell);
    }
}

void ModListView::setupList() {
    m_itemSeparation = 40.0f;

    if (!m_entries->count()) return;

    m_tableView->reloadData();

    // fix content layer content size so the 
    // list is properly aligned to the top
    auto coverage = calculateChildCoverage(m_tableView->m_contentLayer);
    m_tableView->m_contentLayer->setContentSize({
        -coverage.origin.x + coverage.size.width,
        -coverage.origin.y + coverage.size.height
    });

    if (m_entries->count() == 1) {
        m_tableView->moveToTopWithOffset(m_itemSeparation * 2);
    } else if (m_entries->count() == 2) {
        m_tableView->moveToTopWithOffset(-m_itemSeparation);
    } else {
        m_tableView->moveToTop();
    }
}

TableViewCell* ModListView::getListCell(const char* key) {
    return ModCell::create(this, key, { m_width, m_itemSeparation });
}

void ModListView::loadCell(TableViewCell* cell, unsigned int index) {
    auto obj = as<ModObject*>(m_entries->objectAtIndex(index));
    as<ModCell*>(cell)->loadFromObject(obj);
    if (obj->m_type == ModObjectType::Mod) {
        if (obj->m_mod->wasSuccesfullyLoaded()) {
            as<ModCell*>(cell)->updateBGColor(index);
        } else {
            cell->m_backgroundLayer->setOpacity(255);
            cell->m_backgroundLayer->setColor({ 153, 0, 0 });
        }
        if (obj->m_mod->isUninstalled()) {
            cell->m_backgroundLayer->setColor({ 50, 50, 50 });
        }
    } else {
        as<ModCell*>(cell)->updateBGColor(index);
    }
}

bool ModListView::filter(ModInfo const& info, const char* searchFilter, int searchFlags) {
    if (!searchFilter || !strlen(searchFilter)) return true;
    auto check = [searchFlags, searchFilter](SearchFlags flag, std::string const& name) -> bool {
        if (!(searchFlags & flag)) return false;
        return string_utils::contains(
            string_utils::toLower(name),
            string_utils::toLower(searchFilter)
        );
    };
    if (check(SearchFlags::Name,        info.m_name)) return true;
    if (check(SearchFlags::ID,          info.m_id)) return true;
    if (check(SearchFlags::Developer,   info.m_developer)) return true;
    if (check(SearchFlags::Description, info.m_description)) return true;
    if (check(SearchFlags::Details,     info.m_details)) return true;
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
                if (this->filter(imod->getModInfo(), searchFilter, searchFlags)) {
                    mods->addObject(new ModObject(imod));
                }
                for (auto const& mod : Loader::get()->getAllMods()) {
                    if (mod->isUninstalled() && !mod->isLoaded()) continue;
                    if (this->filter(mod->getModInfo(), searchFilter, searchFlags)) {
                        mods->addObject(new ModObject(mod));
                    }
                }
                if (!mods->count()) {
                    m_status = Status::SearchEmpty;
                }
            } break;

            case ModListType::Download: {
                mods = CCArray::create();
                for (auto const& item : Index::get()->getUninstalledItems()) {
                    mods->addObject(new ModObject(item));
                }
                if (!mods->count()) {
                    m_status = Status::NoModsFound;
                }
            } break;

            case ModListType::Featured: {
                mods = CCArray::create();
                m_status = Status::NoModsFound;
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

void ModListView::setLayer(ModListLayer* layer) {
    m_layer = layer;
}

void ModListView::refreshList() {
    if (m_layer) {
        m_layer->reloadList();
    }
}

#include "ModListLayer.hpp"
#include <nodes/BasedButton.hpp>
#include "SearchFilterPopup.hpp"

ModListType g_tab = ModListType::Installed;

bool ModListLayer::init() {
	if (!CCLayer::init())
		return false;
	
    auto winSize = CCDirector::sharedDirector()->getWinSize();
	
	auto bg = CCSprite::create("GJ_gradientBG.png");
	auto bgSize = bg->getTextureRect().size;

	bg->setAnchorPoint({ 0.0f, 0.0f });
	bg->setScaleX((winSize.width + 10.0f) / bgSize.width);
	bg->setScaleY((winSize.height + 10.0f) / bgSize.height);
	bg->setPosition({ -5.0f, -5.0f });
	bg->setColor({ 0, 102, 255 });

	this->addChild(bg);

	this->m_menu = CCMenu::create();
	this->m_topMenu = CCMenu::create();


    auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ModListLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	this->m_menu->addChild(backBtn);

	auto reloadSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
	reloadSpr->setScale(.8f);
    auto reloadBtn = CCMenuItemSpriteExtra::create(
		reloadSpr, this, menu_selector(ModListLayer::onReload)
	);
	reloadBtn->setPosition(-winSize.width / 2 + 30.0f, - winSize.height / 2 + 30.0f);
	this->m_menu->addChild(reloadBtn);

	CCSprite* openSpr = CircleButtonSprite::createWithSpriteFrameName(
		"gj_folderBtn_001.png", 0.7f,
		CircleBaseColor::Green, CircleBaseSize::Small
	);

    auto openBtn = CCMenuItemSpriteExtra::create(
		openSpr, this, menu_selector(ModListLayer::onOpenFolder)
	);
	openBtn->setPosition(-winSize.width / 2 + 30.0f, - winSize.height / 2 + 80.0f);
	this->m_menu->addChild(openBtn);

	
    this->m_listLabel = CCLabelBMFont::create("", "bigFont.fnt");

    this->m_listLabel->setPosition(winSize / 2);
    this->m_listLabel->setScale(.6f);
    this->m_listLabel->setVisible(false);
    this->m_listLabel->setZOrder(1001);

    this->addChild(this->m_listLabel);

	this->m_installedTabBtn = TabButton::create("Installed", this, menu_selector(ModListLayer::onTab));
	this->m_installedTabBtn->setPosition(-95.f, 138.5f);
	this->m_installedTabBtn->setTag(static_cast<int>(ModListType::Installed));
	this->m_menu->addChild(this->m_installedTabBtn);

	this->m_downloadTabBtn = TabButton::create("Download", this, menu_selector(ModListLayer::onTab));
	this->m_downloadTabBtn->setPosition(0.f, 138.5f);
	this->m_downloadTabBtn->setTag(static_cast<int>(ModListType::Download));
	this->m_menu->addChild(this->m_downloadTabBtn);

	this->m_featuredTabBtn = TabButton::create("Featured", this, menu_selector(ModListLayer::onTab));
	this->m_featuredTabBtn->setPosition(95.f, 138.5f);
	this->m_featuredTabBtn->setTag(static_cast<int>(ModListType::Featured));
	this->m_menu->addChild(this->m_featuredTabBtn);

	this->m_menu->setZOrder(0);
	this->m_topMenu->setZOrder(10);

	this->addChild(this->m_menu);
	this->addChild(this->m_topMenu);

	this->onTab(nullptr);

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

	return true;
}

std::tuple<CCNode*, CCTextInputNode*> ModListLayer::createSearchControl() {
	auto layer = CCLayerColor::create({ 194, 114, 62, 255 }, 358.f, 30.f);

	auto menu = CCMenu::create();
	menu->setPosition(340.f, 15.f);

	auto filterSpr = EditorButtonSprite::createWithSpriteFrameName("filters.png"_spr, 1.0f, EditorBaseColor::Gray);
	filterSpr->setScale(.7f);

	auto filterBtn = CCMenuItemSpriteExtra::create(filterSpr, this, menu_selector(ModListLayer::onSearchFilters));
	filterBtn->setPosition(-8.f, 0.f);
	menu->addChild(filterBtn);

	auto searchSpr = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
	searchSpr->setScale(.7f);

	this->m_searchBtn = CCMenuItemSpriteExtra::create(searchSpr, this, nullptr);
	this->m_searchBtn->setPosition(-35.f, 0.f);
	menu->addChild(this->m_searchBtn);

	auto searchClearSpr = CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png");
	searchClearSpr->setScale(.7f);

	this->m_searchClearBtn = CCMenuItemSpriteExtra::create(searchClearSpr, this, menu_selector(ModListLayer::onResetSearch));
	this->m_searchClearBtn->setPosition(-35.f, 0.f);
	this->m_searchClearBtn->setVisible(false);
	menu->addChild(this->m_searchClearBtn);

	auto inputBG = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    inputBG->setColor({ 126, 59, 7 });
	inputBG->setContentSize({ 530.f, 40.f });
	inputBG->setPosition(153.f, 15.f);
	inputBG->setScale(.5f);
	layer->addChild(inputBG);

	auto input = CCTextInputNode::create(250.f, 20.f, "Search Mods...", "bigFont.fnt");
	input->setLabelPlaceholderColor({ 150, 150, 150 });
	input->setLabelPlaceholderScale(.4f);
	input->setMaxLabelScale(.4f);
	input->setDelegate(this);
	input->m_textField->setAnchorPoint({ .0f, .5f });
	input->m_placeholderLabel->setAnchorPoint({ .0f, .5f });

	layer->addChild(menu);
	return { layer, input };
}

void ModListLayer::reloadList() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (this->m_list) {
		if (this->m_searchBG) this->m_searchBG->retain();
        this->m_list->removeFromParent();
	}

	const char* filter = this->m_searchInput ? this->m_searchInput->getString() : nullptr;
	auto list = ModListView::create(g_tab, 358.f, 190.f, filter, this->m_searchFlags);

	auto status = list->getStatusAsString();
	if (status.size()) {
		this->m_listLabel->setVisible(true);
		this->m_listLabel->setString(status.c_str());
	} else {
		this->m_listLabel->setVisible(false);
	}

    this->m_list = GJListLayer::create(
        list, nullptr, { 0, 0, 0, 180 },
		358.f, 220.f
    );
	this->m_list->setZOrder(2);
    this->m_list->setPosition(
        winSize / 2 - this->m_list->getScaledContentSize() / 2
    );
    this->addChild(this->m_list);

	if (!this->m_searchInput) {
		auto search = this->createSearchControl();

		this->m_searchBG = std::get<0>(search);
		this->m_searchBG->setPosition(0.f, 190.f);
		this->m_list->addChild(this->m_searchBG);

		this->m_searchInput = std::get<1>(search);
		this->m_searchInput->setPosition(
			winSize.width / 2 - 155.f,
			winSize.height / 2 + 95.f
		);
		this->m_searchInput->setZOrder(60);
		this->addChild(this->m_searchInput);
	} else {
		this->m_list->addChild(this->m_searchBG);
		this->m_searchBG->release();
	}

	auto hasQuery = filter && strlen(filter);
	this->m_searchBtn->setVisible(!hasQuery);
	this->m_searchClearBtn->setVisible(hasQuery);
}

void ModListLayer::textChanged(CCTextInputNode* input) {
	this->reloadList();
}

void ModListLayer::onExit(CCObject*) {
	CCDirector::sharedDirector()->replaceScene(
		CCTransitionFade::create(.5f, MenuLayer::scene(false))
	);
}

void ModListLayer::onReload(CCObject*) {
	Loader::get()->refreshMods();
	this->reloadList();
}

void ModListLayer::onOpenFolder(CCObject*) {
	dirs::openFolder(
		ghc::filesystem::canonical(Loader::get()->getGeodeDirectory() / "mods")
	);
}

void ModListLayer::onResetSearch(CCObject*) {
	this->m_searchInput->setString("");
}

void ModListLayer::keyDown(enumKeyCodes key) {
	if (key == KEY_Escape) {
        this->onExit(nullptr);
	}
}

void ModListLayer::onTab(CCObject* pSender) {
	if (pSender) {
		g_tab = static_cast<ModListType>(pSender->getTag());
	}
	this->reloadList();

	auto toggleTab = [this](CCMenuItemToggler* member) -> void {
		auto isSelected = member->getTag() == static_cast<int>(g_tab);
		auto targetMenu = isSelected ? this->m_topMenu : this->m_menu;
		member->toggle(isSelected);
		if (member->getParent() != targetMenu) {
			member->retain();
			member->removeFromParent();
			targetMenu->addChild(member);
			member->release();
		}
	};

	toggleTab(this->m_downloadTabBtn);
	toggleTab(this->m_installedTabBtn);
	toggleTab(this->m_featuredTabBtn);
}

void ModListLayer::onSearchFilters(CCObject*) {
	SearchFilterPopup::create(this)->show();
}

ModListLayer* ModListLayer::create() {
	auto ret = new ModListLayer();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

ModListLayer* ModListLayer::scene() {
	auto scene = CCScene::create();
	auto layer = ModListLayer::create();
	scene->addChild(layer);
	CCDirector::sharedDirector()->replaceScene(
		CCTransitionFade::create(.5f, scene)
	);
	return layer;
}

ModListLayer::~ModListLayer() {
	removeAllChildrenWithCleanup(true);
}

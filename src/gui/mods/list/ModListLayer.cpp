#include "ModListLayer.hpp"
#include <nodes/BasedButton.hpp>
#include "SearchFilterPopup.hpp"
#include <general/Notification.hpp>

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

	m_menu = CCMenu::create();
	m_topMenu = CCMenu::create();


    auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ModListLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	m_menu->addChild(backBtn);

	auto reloadSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
	reloadSpr->setScale(.8f);
    auto reloadBtn = CCMenuItemSpriteExtra::create(
		reloadSpr, this, menu_selector(ModListLayer::onReload)
	);
	reloadBtn->setPosition(-winSize.width / 2 + 30.0f, - winSize.height / 2 + 30.0f);
	m_menu->addChild(reloadBtn);


	for (int i = 0; i <= static_cast<int>(NotificationLocation::BottomRight); i++) {
		auto testSpr = CCLabelBMFont::create(std::to_string(i).c_str(), "bigFont.fnt");
		testSpr->setScale(.8f);
		auto testBtn = CCMenuItemSpriteExtra::create(
			testSpr, this, menu_selector(ModListLayer::onTest)
		);
		testBtn->setPosition(
			-winSize.width / 2 + 65.0f,
			-winSize.height / 2 + 75.0f + 25 * i
		);
		testBtn->setTag(i);
		m_menu->addChild(testBtn);
	}

	CCSprite* openSpr = CircleButtonSprite::createWithSpriteFrameName(
		"gj_folderBtn_001.png", 0.7f,
		CircleBaseColor::Green, CircleBaseSize::Small
	);

    auto openBtn = CCMenuItemSpriteExtra::create(
		openSpr, this, menu_selector(ModListLayer::onOpenFolder)
	);
	openBtn->setPosition(-winSize.width / 2 + 30.0f, - winSize.height / 2 + 80.0f);
	this->m_menu->addChild(openBtn);

	
    m_listLabel = CCLabelBMFont::create("", "bigFont.fnt");

    m_listLabel->setPosition(winSize / 2);
    m_listLabel->setScale(.6f);
    m_listLabel->setVisible(false);
    m_listLabel->setZOrder(1001);

    this->addChild(m_listLabel);

	
    m_indexUpdateLabel = CCLabelBMFont::create("", "goldFont.fnt");

    m_indexUpdateLabel->setPosition(winSize.width / 2, winSize.height / 2 - 80.f);
    m_indexUpdateLabel->setScale(.5f);
    m_indexUpdateLabel->setZOrder(1001);

    this->addChild(m_indexUpdateLabel);
	

	m_installedTabBtn = TabButton::create("Installed", this, menu_selector(ModListLayer::onTab));
	m_installedTabBtn->setPosition(-95.f, 138.5f);
	m_installedTabBtn->setTag(static_cast<int>(ModListType::Installed));
	m_menu->addChild(m_installedTabBtn);

	m_downloadTabBtn = TabButton::create("Download", this, menu_selector(ModListLayer::onTab));
	m_downloadTabBtn->setPosition(0.f, 138.5f);
	m_downloadTabBtn->setTag(static_cast<int>(ModListType::Download));
	m_menu->addChild(m_downloadTabBtn);

	m_featuredTabBtn = TabButton::create("Featured", this, menu_selector(ModListLayer::onTab));
	m_featuredTabBtn->setPosition(95.f, 138.5f);
	m_featuredTabBtn->setTag(static_cast<int>(ModListType::Featured));
	m_menu->addChild(m_featuredTabBtn);

	m_menu->setZOrder(0);
	m_topMenu->setZOrder(10);

	this->addChild(m_menu);
	this->addChild(m_topMenu);

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

	m_searchBtn = CCMenuItemSpriteExtra::create(searchSpr, this, nullptr);
	m_searchBtn->setPosition(-35.f, 0.f);
	menu->addChild(m_searchBtn);

	auto searchClearSpr = CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png");
	searchClearSpr->setScale(.7f);

	m_searchClearBtn = CCMenuItemSpriteExtra::create(searchClearSpr, this, menu_selector(ModListLayer::onResetSearch));
	m_searchClearBtn->setPosition(-35.f, 0.f);
	m_searchClearBtn->setVisible(false);
	menu->addChild(m_searchClearBtn);

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

void ModListLayer::indexUpdateProgress(
	UpdateStatus status,
	std::string const& info,
	uint8_t percentage
) {
	if (g_tab == ModListType::Download) {
		if (status == UpdateStatus::Finished) {
			m_indexUpdateLabel->setVisible(false);
			this->reloadList();
		} else {
			m_indexUpdateLabel->setVisible(true);
			m_indexUpdateLabel->setString(info.c_str());
		}

		if (status == UpdateStatus::Failed) {
			FLAlertLayer::create(
				"Error Updating Index",
				info, "OK"
			)->show();
		}
	}
}

void ModListLayer::onTest(CCObject* sender) {
	Notification::build()
		.title("Gay sex")
		.text("Have you ever like uhh and the uhhh yeah uhhhh like that yeah")
		.time(0)
		.location(static_cast<NotificationLocation>(sender->getTag()))
		.show();
}

void ModListLayer::reloadList() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (m_list) {
		if (m_searchBG) m_searchBG->retain();
        m_list->removeFromParent();
	}

	const char* filter = m_searchInput ? m_searchInput->getString() : nullptr;
	auto list = ModListView::create(g_tab, 358.f, 190.f, filter, m_searchFlags);
	list->setLayer(this);

	auto status = list->getStatusAsString();
	if (status.size()) {
		m_listLabel->setVisible(true);
		m_listLabel->setString(status.c_str());
	} else {
		m_listLabel->setVisible(false);
	}

	if (g_tab == ModListType::Download && !Index::get()->isIndexUpdated()) {
		auto failMsg = Index::get()->indexUpdateFailed();
		if (failMsg.size()) {
			// todo: display whole fail message here
			m_listLabel->setString("Updating index failed :(");
			if (m_loadingCircle) {
				m_loadingCircle->fadeAndRemove();
				m_loadingCircle = nullptr;
			}
		} else {
			m_listLabel->setString("Updating index...");
			if (!m_loadingCircle) {
				m_loadingCircle = LoadingCircle::create();

				m_loadingCircle->setPosition(.0f, -40.f);
				m_loadingCircle->setScale(.7f);
				m_loadingCircle->setZOrder(1001);

				m_loadingCircle->show();
			}
			Index::get()->updateIndex();
		}
	} else {
		if (m_loadingCircle) {
			m_loadingCircle->fadeAndRemove();
			m_loadingCircle = nullptr;
		}
	}

    m_list = GJListLayer::create(
        list, nullptr, { 0, 0, 0, 180 },
		358.f, 220.f
    );
	m_list->setZOrder(2);
    m_list->setPosition(
        winSize / 2 - m_list->getScaledContentSize() / 2
    );
    this->addChild(m_list);

	if (!m_searchInput) {
		auto search = this->createSearchControl();

		m_searchBG = std::get<0>(search);
		m_searchBG->setPosition(0.f, 190.f);
		m_list->addChild(m_searchBG);

		m_searchInput = std::get<1>(search);
		m_searchInput->setPosition(
			winSize.width / 2 - 155.f,
			winSize.height / 2 + 95.f
		);
		m_searchInput->setZOrder(60);
		this->addChild(m_searchInput);
	} else {
		m_list->addChild(m_searchBG);
		m_searchBG->release();
	}

	auto hasQuery = filter && strlen(filter);
	m_searchBtn->setVisible(!hasQuery);
	m_searchClearBtn->setVisible(hasQuery);
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
	m_searchInput->setString("");
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
		auto targetMenu = isSelected ? m_topMenu : m_menu;
		member->toggle(isSelected);
		if (member->getParent() != targetMenu) {
			member->retain();
			member->removeFromParent();
			targetMenu->addChild(member);
			member->release();
		}
	};

	toggleTab(m_downloadTabBtn);
	toggleTab(m_installedTabBtn);
	toggleTab(m_featuredTabBtn);
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

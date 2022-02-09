#include "ModListLayer.hpp"
#include <nodes/BasedButton.hpp>

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


    auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ModListLayer::onExit)
	);
	backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
	this->m_menu->addChild(backBtn);

	this->addChild(this->m_menu);

	auto reloadSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
	reloadSpr->setScale(.8f);
    auto reloadBtn = CCMenuItemSpriteExtra::create(
		reloadSpr, this, menu_selector(ModListLayer::onReload)
	);
	reloadBtn->setPosition(-winSize.width / 2 + 30.0f, - winSize.height / 2 + 30.0f);
	this->m_menu->addChild(reloadBtn);

	
    this->m_listLabel = CCLabelBMFont::create("No mods loaded!", "bigFont.fnt");

    this->m_listLabel->setPosition(winSize / 2);
    this->m_listLabel->setScale(.6f);
    this->m_listLabel->setVisible(false);
    this->m_listLabel->setZOrder(1001);

    this->addChild(this->m_listLabel);

	this->reloadList();

	auto installedTab = TabButton::create("Installed", this, menu_selector(ModListLayer::onTab));
	installedTab->setPosition(-30.f, 80.f);
	installedTab->setTag(static_cast<int>(ModListType::Installed));
	this->m_menu->addChild(installedTab);

	auto tab = TabButton::create("Download", this, menu_selector(ModListLayer::onTab));
	tab->setPosition(30.f, 80.f);
	tab->setTag(static_cast<int>(ModListType::Download));
	this->m_menu->addChild(tab);

	this->m_menu->setZOrder(10000);
	this->addChild(this->m_menu);

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

	return true;
}

void ModListLayer::reloadList() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (this->m_list)
        this->m_list->removeFromParent();

    CustomListView* list = nullptr;

	auto mods = Loader::get()->getLoadedMods();
    if (!mods.size()) {
        m_listLabel->setVisible(true);
    } else {
		m_listLabel->setVisible(false);
		list = ModListView::create(g_tab);
    }

    this->m_list = GJListLayer::create(
        list, nullptr, { 0, 0, 0, 180 }, 356.0f, 220.0f
    );
    this->m_list->setPosition(
        winSize / 2 - this->m_list->getScaledContentSize() / 2
    );
    this->addChild(this->m_list);
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

void ModListLayer::keyDown(enumKeyCodes key) {
	if (key == KEY_Escape) {
        this->onExit(nullptr);
	}
}

void ModListLayer::onTab(CCObject* pSender) {
	g_tab = static_cast<ModListType>(pSender->getTag());
	this->reloadList();
}

ModListLayer* ModListLayer::create() {
	auto ret = new ModListLayer;
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

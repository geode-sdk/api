#include "hook.hpp"
#include "BasedButtonSprite.hpp"
#include "ModListLayer.hpp"

class $modify(CustomPauseLayer, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

        CCSprite* spr = CircleButtonSprite::createWithSpriteFrameName(
			"geode-logo-outline-gold.png"_spr, 1.0f,
			CircleBaseColor::Green, CircleBaseSize::Medium2
		);
		if (!spr) {
			spr = ButtonSprite::create("!!");
		}
		auto btn = CCMenuItemSpriteExtra::create(
			spr, this, menu_selector(CustomPauseLayer::onGeode)
		);
        btn->setScale(1.0f);

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition(45, size.height - 45);

		this->addChild(menu);
	}

	void onGeode(CCObject*) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = ModListLayer::create(this);
        this->setVisible(false);
        scene->addChild(layer);
	}
};


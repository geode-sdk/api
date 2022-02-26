#include "hook.hpp"
#include <mods/list/ModListLayer.hpp>
#include <WackyGeodeMacros.hpp>
#include <nodes/BasedButtonSprite.hpp>
#include <views/BasicViews.hpp>

class $modify(CustomMenuLayer, MenuLayer) {

	bool init() {
		if (!MenuLayer::init())
			return false;
		
		auto bottomMenu = getChild<CCMenu*>(this, 3);

		auto chest = getChild<>(bottomMenu, -1);
		chest->retain();
		chest->removeFromParent();

		auto y = getChild<>(bottomMenu, 0)->getPositionY();

		CCSprite* spr = CircleButtonSprite::createWithSpriteFrameName(
			"geode-logo-outline-gold.png"_sprite, 1.0f,
			CircleBaseColor::Green, CircleBaseSize::Medium2
		);
		if (!spr) {
			spr = ButtonSprite::create("!!");
		}
		auto btn = CCMenuItemSpriteExtra::create(
			spr, this, menu_selector(CustomMenuLayer::onGeode)
		);
		bottomMenu->addChild(btn);

		// bottomMenu->alignItemsHorizontallyWithPadding(3.f);

		auto row = Row::create()->padding(3.0f);

		CCARRAY_FOREACH_B_TYPE(bottomMenu->getChildren(), node, CCNode) {
			node->setPositionY(y);
			row->addItem("", node);
		}

		bottomMenu->addChild(chest);
		chest->release();

		auto failed = Loader::get()->getFailedMods();
		if (failed.size()) {
            auto layer = FLAlertLayer::create(
				"Notice",
				"Some mods failed to load; see <cy>Geode</c> for details",
				"OK"
			);
			layer->m_scene = this;
			layer->m_noElasticity = true;
			layer->show();
        }

		return true;
	}

	void onGeode(CCObject*) {
		ModListLayer::scene();
	}
};


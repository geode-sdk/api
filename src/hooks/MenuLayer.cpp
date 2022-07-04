#include "hook.hpp"
#include <gui/mods/list/ModListLayer.hpp>
#include <utils/WackyGeodeMacros.hpp>
#include <nodes/BasedButtonSprite.hpp>
#include <views/BasicViews.hpp>
#include <index/Index.hpp>
#include <general/Notification.hpp>

// class $modify(PlatformToolbox) {
// 	bool isControllerConnected() {
// 		return true;
// 	}
// };

static Notification* INDEX_UPDATE_NOTIF = nullptr;

class $modify(CustomMenuLayer, MenuLayer) {
	CCSprite* m_geodeButtonSpr = nullptr;

	bool init() {
		if (!MenuLayer::init())
			return false;
		
		CCMenu* bottomMenu = nullptr;

		size_t indexCounter = 0;
		for (size_t i = 0; i < this->getChildren()->count(); i++) {
			auto obj = typeinfo_cast<CCMenu*>(this->getChildren()->objectAtIndex(i));
			if (obj != nullptr) {
				++indexCounter;
				if (indexCounter == 2) {
					bottomMenu = obj;
					break;
				}
			}
		}

		// if (!GameManager::sharedState()->m_clickedGarage) {
		// 	bottomMenu = getChild<CCMenu*>(this, 4);
		// }
		// else {
		// 	bottomMenu = getChild<CCMenu*>(this, 3);
		// }

		auto chest = getChild<>(bottomMenu, -1);
		if (chest) {
			chest->retain();
			chest->removeFromParent();
		}
		
		auto y = getChild<>(bottomMenu, 0)->getPositionY();

		m_fields->m_geodeButtonSpr = CircleButtonSprite::createWithSpriteFrameName(
			"geode-logo-outline-gold.png"_spr,
			1.0f,
			CircleBaseColor::Green,
			CircleBaseSize::Medium2
		);
		if (!m_fields->m_geodeButtonSpr) {
			m_fields->m_geodeButtonSpr = ButtonSprite::create("!!");
		}
		this->addUpdateIcon();
		auto btn = CCMenuItemSpriteExtra::create(
			m_fields->m_geodeButtonSpr, this, menu_selector(CustomMenuLayer::onGeode)
		);
		bottomMenu->addChild(btn);

		bottomMenu->alignItemsHorizontallyWithPadding(3.f);

		auto row = Row::create()->padding(3.0f);

		CCARRAY_FOREACH_B_TYPE(bottomMenu->getChildren(), node, CCNode) {
			node->setPositionY(y);
			row->addItem("", node);
		}
		if (chest) {
			bottomMenu->addChild(chest);
			chest->release();
		}

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

		if (!INDEX_UPDATE_NOTIF && !Index::get()->isIndexUpdated()) {
			INDEX_UPDATE_NOTIF = NotificationBuilder()
				.title("Index Update")
				.text("Updating index...")
				.loading()
				.stay()
				.show();

			Index::get()->updateIndex(
				this, indexupdateprogress_selector(CustomMenuLayer::onIndexUpdate)
			);
		}

		return true;
	}

	void addUpdateIcon() {
		if (Index::get()->areUpdatesAvailable()) {
            auto updateIcon = CCSprite::createWithSpriteFrameName("update.png"_spr);
            updateIcon->setPosition(m_fields->m_geodeButtonSpr->getContentSize() - CCSize { 10.f, 10.f });
            updateIcon->setZOrder(99);
            updateIcon->setScale(.5f);
            m_fields->m_geodeButtonSpr->addChild(updateIcon);
		}
	}

	void onIndexUpdate(UpdateStatus status, std::string const& info, uint8_t progress) {
		if (status == UpdateStatus::Failed) {
			INDEX_UPDATE_NOTIF->hide();
			INDEX_UPDATE_NOTIF = nullptr;
			NotificationBuilder()
				.title("Index Update")
				.text("Index update failed :(")
				.icon("info-alert.png"_spr)
				.show();
		}
		if (status == UpdateStatus::Finished) {
			INDEX_UPDATE_NOTIF->hide();
			INDEX_UPDATE_NOTIF = nullptr;
			if (Index::get()->areUpdatesAvailable()) {
				NotificationBuilder()
					.title("Updates available")
					.text("Some mods have updates available!")
					.icon("update.png"_spr)
					.show();
				this->addUpdateIcon();
			}
		}
	}

	void onGeode(CCObject*) {
		ModListLayer::scene();
	}
};


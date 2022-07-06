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

class CustomMenuLayer;

static Ref<Notification> g_indexUpdateNotif = nullptr;
static Ref<CCSprite> g_geodeButton = nullptr;

static void addUpdateIcon() {
	if (g_geodeButton && Index::get()->areUpdatesAvailable()) {
		auto updateIcon = CCSprite::createWithSpriteFrameName("updates-available.png"_spr);
		updateIcon->setPosition(g_geodeButton->getContentSize() - CCSize { 10.f, 10.f });
		updateIcon->setZOrder(99);
		updateIcon->setScale(.5f);
		g_geodeButton->addChild(updateIcon);
	}
}

class $modify(CustomMenuLayer, MenuLayer) {
	void destructor() {
		g_geodeButton = nullptr;
		MenuLayer::~MenuLayer();
	}

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

		g_geodeButton = CircleButtonSprite::createWithSpriteFrameName(
			"geode-logo-outline-gold.png"_spr,
			1.0f,
			CircleBaseColor::Green,
			CircleBaseSize::Medium2
		);
		if (!g_geodeButton) {
			g_geodeButton = ButtonSprite::create("!!");
		}
		addUpdateIcon();
		auto btn = CCMenuItemSpriteExtra::create(
			g_geodeButton.data(), this, menu_selector(CustomMenuLayer::onGeode)
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

		if (!g_indexUpdateNotif && !Index::get()->isIndexUpdated()) {
			g_indexUpdateNotif = NotificationBuilder()
				.title("Index Update")
				.text("Updating index...")
				.loading()
				.stay()
				.show();

			Index::get()->updateIndex([](
				UpdateStatus status,
				std::string const& info,
				uint8_t progress
			) -> void {
				if (status == UpdateStatus::Failed) {
					g_indexUpdateNotif->hide();
					g_indexUpdateNotif = nullptr;
					NotificationBuilder()
						.title("Index Update")
						.text("Index update failed :(")
						.icon("info-alert.png"_spr)
						.show();
				}

				if (status == UpdateStatus::Finished) {
					g_indexUpdateNotif->hide();
					g_indexUpdateNotif = nullptr;
					if (Index::get()->areUpdatesAvailable()) {
						NotificationBuilder()
							.title("Updates available")
							.text("Some mods have updates available!")
							.icon("updates-available.png"_spr)
							.clicked([](auto) -> void {
								FLAlertLayer::create(
									"Hi",
									"Hi",
									"Hi"
								)->show();
							})
							.show();
						addUpdateIcon();
					}
				}
			});
		}

		return true;
	}

	void onGeode(CCObject*) {
		ModListLayer::scene();
	}
};


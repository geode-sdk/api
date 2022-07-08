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

static void addUpdateIcon(const char* icon = "updates-available.png"_spr) {
	if (g_geodeButton && Index::get()->areUpdatesAvailable()) {
		auto updateIcon = CCSprite::createWithSpriteFrameName(icon);
		updateIcon->setPosition(
			g_geodeButton->getContentSize() - CCSize { 10.f, 10.f }
		);
		updateIcon->setZOrder(99);
		updateIcon->setScale(.5f);
		g_geodeButton->addChild(updateIcon);
	}
}

static void updateModsProgress(
	UpdateStatus status,
	std::string const& info,
	uint8_t progress
) {
	if (status == UpdateStatus::Failed) {
		g_indexUpdateNotif->hide();
		g_indexUpdateNotif = nullptr;
		NotificationBuilder()
			.title("Some Updates Failed")
			.text("Some mods failed to update, click for details")
			.icon("info-alert.png"_spr)
			.clicked([info](auto) -> void {
				FLAlertLayer::create("Info", info, "OK")->show();
			})
			.show();
		addUpdateIcon("updates-failed.png"_spr);
	}

	if (status == UpdateStatus::Finished) {
		g_indexUpdateNotif->hide();
		g_indexUpdateNotif = nullptr;
		NotificationBuilder()
			.title("Updates Installed")
			.text(
				"Mods have been updated, please "
				"restart to apply changes"
			)
			.icon("updates-available.png"_spr)
			.clicked([info](auto) -> void {
				FLAlertLayer::create("Info", info, "OK")->show();
			})
			.show();
	}
}

static void updateIndexProgress(
	UpdateStatus status,
	std::string const& info,
	uint8_t progress
) {
	if (status == UpdateStatus::Failed) {
		g_indexUpdateNotif->hide();
		g_indexUpdateNotif = nullptr;
		NotificationBuilder()
			.title("Index Update")
			.text("Index update failed :(")
			.icon("info-alert.png"_spr)
			.show();
		addUpdateIcon("updates-failed.png"_spr);
	}

	if (status == UpdateStatus::Finished) {
		g_indexUpdateNotif->hide();
		g_indexUpdateNotif = nullptr;
		if (Index::get()->areUpdatesAvailable()) {
			if (Mod::get()->getDataStore()["enable-auto-updates"]) {
				auto ticket = Index::get()->installUpdates(updateModsProgress);
				if (!ticket) {
					NotificationBuilder()
						.title("Unable to auto-update")
						.text("Unable to update mods :(")
						.icon("updates-failed.png"_spr)
						.show();
				} else {
					g_indexUpdateNotif = NotificationBuilder()
						.title("Installing updates")
						.text("Installing updates...")
						.clicked([ticket](auto) -> void {
							createQuickPopup(
								"Cancel Updates",
								"Do you want to <cr>cancel</c> updates?",
								"Cancel", "Cancel",
								[ticket](auto, bool btn2) -> void {
									if (g_indexUpdateNotif && btn2) {
										ticket.value()->cancel();
									}
								}
							);
						}, false)
						.loading()
						.stay()
						.show();
				}
			} else {
				NotificationBuilder()
					.title("Updates available")
					.text("Some mods have updates available!")
					.icon("updates-available.png"_spr)
					.clicked([](auto) -> void {
						ModListLayer::scene();
					})
					.show();
			}
			addUpdateIcon();
		}
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

			Index::get()->updateIndex(updateIndexProgress);
		}

		return true;
	}

	void onGeode(CCObject*) {
		ModListLayer::scene();
	}
};


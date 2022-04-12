#include "ModSettingsListView.hpp"
#include "SettingNodeManager.hpp"

bool ModSettingsListView::init(Mod* mod, float width, float height) {
	m_mod = mod;
	m_delegate = nullptr;
	m_cutContent = true;
	m_disableVertical = false;

	float offset = 0.f;
	for (auto const& sett : mod->getSettings()) {
		// auto node = SettingNodeManager::get()->generateNode(mod, sett, width);
		// if (node) {
		// 	// otherwise causes crashes idk
		// 	node->m_tableView = nullptr;
			
		// 	node->setPosition(0.f, offset);
		// 	m_contentLayer->addChild(node);
		// 	offset += node->m_height;
		// }
	}
	m_contentLayer->setContentSize({ width, offset });
	this->moveToTop();

	CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
	this->registerWithTouchDispatcher();
	this->setMouseEnabled(true);

	return true;
}

void ModSettingsListView::scrollWheel(float y, float) {
	this->scrollLayer(y);
}

ModSettingsListView::ModSettingsListView(CCRect const& rect) :
	CCScrollLayerExt(rect) {}

ModSettingsListView* ModSettingsListView::create(
	Mod* mod, float width, float height
) {
	auto ret = new ModSettingsListView({ 0.f, 0.f, width, height });
	if (ret && ret->init(mod, width, height)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

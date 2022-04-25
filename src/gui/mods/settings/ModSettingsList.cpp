#include "ModSettingsList.hpp"
#include "SettingNodeManager.hpp"

bool ModSettingsList::init(Mod* mod, float width, float height) {
	m_mod = mod;

	m_scrollLayer = ScrollLayer::create({ width, height });
	this->addChild(m_scrollLayer);

	float offset = 0.f;
	bool coloredBG = false;
	std::vector<CCNode*> gen;
	for (auto const& sett : mod->getSettings()) {
		auto node = SettingNodeManager::get()->generateNode(mod, sett, width);
		if (node) {
			if (coloredBG) {
				node->m_backgroundLayer->setColor({ 0, 0, 0 });
				node->m_backgroundLayer->setOpacity(50);
			}
			node->setPosition(
				0.f, offset - node->getScaledContentSize().height
			);
			m_scrollLayer->m_contentLayer->addChild(node);
			
			auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, width, 1.f);
			separator->setPosition(0.f, offset - .5f);
			m_scrollLayer->m_contentLayer->addChild(separator);
			gen.push_back(separator);

			offset -= node->m_height;
			coloredBG = !coloredBG;
			gen.push_back(node);
		}
	}
	auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, width, 1.f);
	separator->setPosition(0.f, offset);
	m_scrollLayer->m_contentLayer->addChild(separator);
	gen.push_back(separator);

	offset = -offset;
	for (auto& node : gen) {
		node->setPositionY(node->getPositionY() + offset);
	}
	// to avoid needing to do moveToTopWithOffset, 
	// just set the content size to the viewport 
	// size if its less
	if (offset < height) offset = height;
	m_scrollLayer->m_contentLayer->setContentSize({ width, offset });
	m_scrollLayer->moveToTop();

	CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
	m_scrollLayer->registerWithTouchDispatcher();

	return true;
}

ModSettingsList* ModSettingsList::create(
	Mod* mod, float width, float height
) {
	auto ret = new ModSettingsList();
	if (ret && ret->init(mod, width, height)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

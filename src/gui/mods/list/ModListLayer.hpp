#pragma once

#include <Geode.hpp>
#include "ModListView.hpp"

USE_GEODE_NAMESPACE();

class ModListLayer : public CCLayer {
protected:
	GJListLayer* m_list = nullptr;
	CCLabelBMFont* m_listLabel;
	CCMenu* m_menu;

	bool init() override;

	void onExit(CCObject*);
	void onReload(CCObject*);
	void keyDown(enumKeyCodes) override;
	void onTab(CCObject*);

	void reloadList();

public:
	static ModListLayer* create();
	static ModListLayer* scene();
};

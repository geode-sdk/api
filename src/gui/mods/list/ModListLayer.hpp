#pragma once

#include <Geode.hpp>
#include "ModListView.hpp"

USE_GEODE_NAMESPACE();

class SearchFilterPopup;

class ModListLayer : public CCLayer, public TextInputDelegate {
protected:
	GJListLayer* m_list = nullptr;
	CCLabelBMFont* m_listLabel;
	CCMenu* m_menu;
	CCMenu* m_topMenu;
	CCMenuItemToggler* m_installedTabBtn;
	CCMenuItemToggler* m_downloadTabBtn;
	CCMenuItemToggler* m_featuredTabBtn;
	CCMenuItemSpriteExtra* m_searchBtn;
	CCMenuItemSpriteExtra* m_searchClearBtn;
	CCNode* m_searchBG = nullptr;
	CCTextInputNode* m_searchInput = nullptr;
	int m_searchFlags = ModListView::s_allFlags;

	~ModListLayer() override;

	bool init() override;

	void onExit(CCObject*);
	void onReload(CCObject*);
	void onOpenFolder(CCObject*);
	void onResetSearch(CCObject*);
	void keyDown(enumKeyCodes) override;
	void onTab(CCObject*);
	void onSearchFilters(CCObject*);
	void textChanged(CCTextInputNode*) override;
	std::tuple<CCNode*, CCTextInputNode*> createSearchControl();

	void reloadList();

	friend class SearchFilterPopup;

public:
	static ModListLayer* create();
	static ModListLayer* scene();
};

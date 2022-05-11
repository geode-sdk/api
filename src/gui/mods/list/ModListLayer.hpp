#pragma once

#include <Geode.hpp>
#include "ModListView.hpp"
#include <index/Index.hpp>

USE_GEODE_NAMESPACE();

class SearchFilterPopup;

class ModListLayer : public CCLayer, public TextInputDelegate, public IndexDelegate {
protected:
	GJListLayer* m_list = nullptr;
	CCLabelBMFont* m_listLabel;
	CCLabelBMFont* m_indexUpdateLabel;
	CCMenu* m_menu;
	CCMenu* m_topMenu;
	CCMenuItemToggler* m_installedTabBtn;
	CCMenuItemToggler* m_downloadTabBtn;
	CCMenuItemToggler* m_featuredTabBtn;
	CCMenuItemSpriteExtra* m_searchBtn;
	CCMenuItemSpriteExtra* m_searchClearBtn;
	CCNode* m_searchBG = nullptr;
	CCTextInputNode* m_searchInput = nullptr;
	LoadingCircle* m_loadingCircle = nullptr;
	int m_searchFlags = ModListView::s_allFlags;

	~ModListLayer() override;

	bool init() override;

	void onTest(CCObject*);
	void onExit(CCObject*);
	void onReload(CCObject*);
	void onOpenFolder(CCObject*);
	void onResetSearch(CCObject*);
	void keyDown(enumKeyCodes) override;
	void onTab(CCObject*);
	void onSearchFilters(CCObject*);
	void textChanged(CCTextInputNode*) override;
	void indexUpdateProgress(std::string const& info) override;
	void indexUpdateFailed(std::string const& info) override;
	void indexUpdateFinished() override;
	std::tuple<CCNode*, CCTextInputNode*> createSearchControl();

	friend class SearchFilterPopup;

public:
	static ModListLayer* create();
	static ModListLayer* scene();

	void reloadList();
};

#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

class DevSettingsLayer : public GJDropDownLayer {
protected:
	Mod* m_mod;
	CCTextInputNode* m_input;

	bool init(Mod* mod);

	void onEnableHotReload(CCObject*);
	void onPastePathFromClipboard(CCObject*);

public:
	static DevSettingsLayer* create(Mod* mod);
};


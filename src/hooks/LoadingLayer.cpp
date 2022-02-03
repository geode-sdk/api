#include "hook.hpp"

static std::string loadStepToString(int step) {
    if (step < 1) {
        return "GJ_GameSheet";
    }
    if (step >= 14) {
        return "Finished";
    }
    const char* steps[14] = {
        "GJ_GameSheet02",
        "GJ_GameSheet03",
        "GJ_GameSheet04",
        "GJ_GameSheetGlow",
        "FireSheet_01, GJ_ShopSheet, smallDot, square02_001",
        "CCControlColourPickerSpriteSheet, GJ_gradientBG, edit_barBG_001, GJ_button_01, slidergroove2, sliderBar2",
        "GJ_square01, GJ_square02, GJ_square03, GJ_square04, GJ_square05, gravityLine_001",
        "chatFont",
        "Robot_AnimDesc, glassDestroy01, coinPickupEffect, explodeEffect",
        "nothing lol",
        "GameManager",
        "CCTextInputNode",
        "CCCircleWave",
        "Finished",
    };
    return steps[step - 1];
}

class $modify(LoadingLayer) {
    bool init(bool fromReload) {
        if (!$LoadingLayer::init(fromReload))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        auto [count, unresolvedCount] = Loader::get()->getLoadedModCount();

        const char* text = unresolvedCount ?
            CCString::createWithFormat("Geode: Loaded %d mods (%d unresolved)", count, unresolvedCount)->getCString() : 
            CCString::createWithFormat("Geode: Loaded %d mods", count)->getCString();

        auto label = CCLabelBMFont::create(text, "goldFont.fnt");
        label->setPosition(winSize.width / 2, 30.f);
        label->setScale(.45f);
        label->setTag(5);
        this->addChild(label);

        return true;
    }

	void loadAssets() {
        auto str = "Loading " + loadStepToString(this->m_loadStep);
        as<CCLabelBMFont*>(this->getChildByTag(5))->setString(str.c_str());

		$LoadingLayer::loadAssets();
	}
};

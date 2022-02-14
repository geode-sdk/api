#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

class ModInfoLayer : public FLAlertLayer {
public:
    enum Tab {
        Info,
        Credits,
        Settings,
        About,
    };

protected:
    Mod* m_mod;
    std::vector<CCMenuItemToggler*> m_tabBtns;

    void onHooks(CCObject*);
    void onDev(CCObject*);
    void onSettings(CCObject*);
    void onTab(CCObject*);

    CCMenuItemToggler* createTab(Tab id, const char* text, const char* icon = nullptr);

    bool init(Mod* mod);

    void keyDown(cocos2d::enumKeyCodes) override;
    void onClose(cocos2d::CCObject*);
    
public:
    static ModInfoLayer* create(Mod* Mod);

    static CCNode* createLogoSpr(Mod* mod);
};

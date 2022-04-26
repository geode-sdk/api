#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

class ModListView;

class ModInfoLayer : public FLAlertLayer {
protected:
    Mod* m_mod;
    ModListView* m_list = nullptr;

    void onHooks(CCObject*);
    void onDev(CCObject*);
    void onSettings(CCObject*);
    void onNoSettings(CCObject*);
    void onInfo(CCObject*);
    void onEnableMod(CCObject*);
    void onDisablingNotSupported(CCObject*);

    bool init(Mod* mod, ModListView* list);

    void keyDown(cocos2d::enumKeyCodes) override;
    void onClose(cocos2d::CCObject*);
    
public:
    static ModInfoLayer* create(Mod* mod, ModListView* list);

    static CCNode* createLogoSpr(Mod* mod);
};

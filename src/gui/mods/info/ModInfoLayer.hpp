#pragma once

#include <Geode.hpp>
#include <index/Index.hpp>

USE_GEODE_NAMESPACE();

class ModListView;
class ModObject;

class ModInfoLayer : public FLAlertLayer, public FLAlertLayerProtocol, public IndexDelegate {
protected:
    Mod* m_mod = nullptr;
    ModInfo m_info;
    bool m_isIndexMod = false;
    ModListView* m_list = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCLabelBMFont* m_loadingLabel = nullptr;
    CCMenuItemSpriteExtra* m_installBtn;
    std::vector<std::string> m_installing;

    void onHooks(CCObject*);
    void onDev(CCObject*);
    void onSettings(CCObject*);
    void onNoSettings(CCObject*);
    void onInfo(CCObject*);
    void onEnableMod(CCObject*);
    void onInstallMod(CCObject*);
    void onUninstall(CCObject*);
    void onDisablingNotSupported(CCObject*);
    void installMod(std::string const& id);
    void uninstall();
    void updateInstallStatus(std::string const& status = "");

    void modInstallProgress(std::string const& info, uint8_t percentage) override;
    void modInstallFailed(std::string const& info) override;
    void modInstallFinished() override;
    void FLAlert_Clicked(FLAlertLayer*, bool);

    bool init(ModObject* obj, ModListView* list);

    void keyDown(cocos2d::enumKeyCodes) override;
    void onClose(cocos2d::CCObject*);
    
public:
    static ModInfoLayer* create(Mod* mod, ModListView* list);
    static ModInfoLayer* create(ModObject* obj, ModListView* list);

    static CCNode* createLogoSpr(ModObject* modObj);
    static CCNode* createLogoSpr(Mod* mod);
    static CCNode* createLogoSpr(IndexItem const& item);
};

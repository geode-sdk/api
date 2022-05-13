#include "DevSettingsLayer.hpp"

bool DevSettingsLayer::init(Mod* mod) {
    if (!GJDropDownLayer::init("Dev Settings", 220.f))
        return false;

	this->m_mod = mod;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    return true;
}

void DevSettingsLayer::onEnableHotReload(CCObject* pSender) {
}

void DevSettingsLayer::onPastePathFromClipboard(CCObject*) {
	auto data = clipboard::read();
	if (data.size()) {
		this->m_input->setString(data.c_str());
	}
}

DevSettingsLayer* DevSettingsLayer::create(Mod* mod) {
    auto ret = new DevSettingsLayer;
    if (ret && ret->init(mod)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}



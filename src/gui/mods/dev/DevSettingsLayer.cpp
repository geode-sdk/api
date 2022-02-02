#include "DevSettingsLayer.hpp"

bool DevSettingsLayer::init(Mod* mod) {
    if (!GJDropDownLayer::init("Dev Settings", 220.f))
        return false;

	this->m_mod = mod;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto menu = CCMenu::create();
	GameToolbox::createToggleButton(
		"Enable Hot Reload",
		menu_selector(DevSettingsLayer::onEnableHotReload),
		mod->isHotReloadEnabled(), menu, 
		{ winSize.width / 2 - 40.f, winSize.height / 2 + 40.f },
		this, menu, .75f, .5f, 100.f, { 10, 0 }, nullptr,
		false, 0, nullptr
	);

	this->m_input = CCTextInputNode::create(
		200.f, 50.f, "Path to .geode file", "chatFont.fnt"
	);
	auto path = mod->getHotReloadPath();
	if (!path.empty()) {
		this->m_input->setString(path.string());
	}
	this->m_input->setAllowedChars("0123456789.:-_/\\()[]abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|!}");
	this->m_input->setPosition(winSize.width / 2, winSize.height / 2);
	this->m_mainLayer->addChild(this->m_input);

	auto spr = ButtonSprite::create(
		"Paste From Clipboard"
	);
	spr->setScale(.45f);
	auto btn = CCMenuItemSpriteExtra::create(
		spr, nullptr, this, menu_selector(DevSettingsLayer::onPastePathFromClipboard)
	);
	btn->setPosition(0, -40.f);
	menu->addChild(btn);
	
	this->m_mainLayer->addChild(menu);

    return true;
}

void DevSettingsLayer::onEnableHotReload(CCObject* pSender) {
	std::string path = this->m_input->getString();
	if (!as<CCMenuItemToggler*>(pSender)->isToggled()) {
		if (!path.size()) {
			FLAlertLayer::create(
				"Error",
				"Set a .geode file path first",
				"OK"
			)->show();
			as<CCMenuItemToggler*>(pSender)->toggle(true);
		} else {
			auto res = this->m_mod->enableHotReload();
			if (!res) {
				FLAlertLayer::create(
					"Error",
					res.error(),
					"OK" 
				)->show();
			}
		}
	} else {
		this->m_mod->disableHotReload();
	}
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



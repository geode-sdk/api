#pragma warning(disable: 4067)

#include "SettingNode.hpp"

USE_GEODE_NAMESPACE();

#define GEODE_GENERATE_SETTING_CREATE(_sett_, _height_)                \
	_sett_##Node* _sett_##Node::create(_sett_* setting, float width) { \
		auto ret = new _sett_##Node(width, _height_);                  \
		if (ret && ret->init(setting)) {                               \
			ret->autorelease();                                        \
			return ret;                                                \
		} CC_SAFE_DELETE(ret); return nullptr; }
	

// bool

bool BoolSettingNode::init(BoolSetting* setting) {
	if (!GeodeSettingNode<BoolSetting>::init(setting))
		return false;

	auto toggle = CCMenuItemToggler::createWithStandardSprites(
		this, menu_selector(BoolSettingNode::onToggle), .8f
	);
	toggle->setPosition(0, 0);
	toggle->toggle(setting->getValue());
	m_buttonMenu->addChild(toggle);

	return true;
}

void BoolSettingNode::onToggle(CCObject* pSender) {
	this->m_setting->setValue(!as<CCMenuItemToggler*>(pSender)->isToggled());
}

// int

bool IntSettingNode::init(IntSetting* setting) {
	if (!GeodeSettingNode<IntSetting>::init(setting))
		return false;
	
	m_valueLabel = CCLabelBMFont::create("", "bigFont.fnt");
	m_valueLabel->setPosition(-20, 0);
	m_valueLabel->setScale(.5f);
	m_buttonMenu->addChild(m_valueLabel);

	if (setting->hasArrows()) {
		auto decSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		decSpr->setScale(.6f);
		decSpr->setFlipX(true);

		auto decBtn = CCMenuItemSpriteExtra::create(
			decSpr, this, menu_selector(IntSettingNode::onArrow)
		);
		decBtn->setTag(-1);
		decBtn->setPosition(-40.f, 0);
		m_buttonMenu->addChild(decBtn);

		auto incSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		incSpr->setScale(.6f);

		auto incBtn = CCMenuItemSpriteExtra::create(
			incSpr, this, menu_selector(IntSettingNode::onArrow)
		);
		incBtn->setTag(1);
		incBtn->setPosition(0.f, 0);
		m_buttonMenu->addChild(incBtn);
	}

	this->updateValue();

	return true;
}

void IntSettingNode::onArrow(CCObject* pSender) {
	m_setting->setValue(m_setting->getValue() + pSender->getTag());
	if (m_setting->getValue() < m_setting->getMin()) {
		m_setting->setValue(m_setting->getMin());
	}
	if (m_setting->getValue() > m_setting->getMax()) {
		m_setting->setValue(m_setting->getMax());
	}
	this->updateValue();
}

void IntSettingNode::updateValue() {
	m_valueLabel->setString(std::to_string(m_setting->getValue()).c_str());
}

// float

bool FloatSettingNode::init(FloatSetting* setting) {
	if (!GeodeSettingNode<FloatSetting>::init(setting))
		return false;
	
	m_valueLabel = CCLabelBMFont::create("", "bigFont.fnt");
	m_valueLabel->setPosition(-20, 0);
	m_valueLabel->setScale(.5f);
	m_buttonMenu->addChild(m_valueLabel);

	if (setting->hasArrows()) {
		auto decSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		decSpr->setScale(.6f);
		decSpr->setFlipX(true);

		auto decBtn = CCMenuItemSpriteExtra::create(
			decSpr, this, menu_selector(FloatSettingNode::onArrow)
		);
		decBtn->setTag(-1);
		decBtn->setPosition(-40.f, 0);
		m_buttonMenu->addChild(decBtn);

		auto incSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		incSpr->setScale(.6f);

		auto incBtn = CCMenuItemSpriteExtra::create(
			incSpr, this, menu_selector(FloatSettingNode::onArrow)
		);
		incBtn->setTag(1);
		incBtn->setPosition(0.f, 0);
		m_buttonMenu->addChild(incBtn);
	}

	this->updateValue();

	return true;
}

void FloatSettingNode::onArrow(CCObject* pSender) {
	m_setting->setValue(m_setting->getValue() + pSender->getTag());
	if (m_setting->getValue() < m_setting->getMin()) {
		m_setting->setValue(m_setting->getMin());
	}
	if (m_setting->getValue() > m_setting->getMax()) {
		m_setting->setValue(m_setting->getMax());
	}
	this->updateValue();
}

void FloatSettingNode::updateValue() {
	m_valueLabel->setString(std::to_string(m_setting->getValue()).c_str());
}

// string

bool StringSettingNode::init(StringSetting* setting) {
	if (!GeodeSettingNode<StringSetting>::init(setting))
		return false;

	return true;
}

// color

bool ColorSettingNode::init(ColorSetting* setting) {
	if (!GeodeSettingNode<ColorSetting>::init(setting))
		return false;

	return true;
}

// rgba

bool ColorAlphaSettingNode::init(ColorAlphaSetting* setting) {
	if (!GeodeSettingNode<ColorAlphaSetting>::init(setting))
		return false;

	return true;
}

// path

bool PathSettingNode::init(PathSetting* setting) {
	if (!GeodeSettingNode<PathSetting>::init(setting))
		return false;

	return true;
}

// string[]

bool StringSelectSettingNode::init(StringSelectSetting* setting) {
	if (!GeodeSettingNode<StringSelectSetting>::init(setting))
		return false;

	return true;
}

// custom

bool CustomSettingPlaceHolderNode::init(CustomSettingPlaceHolder* setting) {
	if (!CCNode::init())
		return false;

	this->setContentSize({ m_width, m_height });

	auto text = "This setting (" + setting->getKey() + ") is a custom setting\n which has "
		"no registered setting node.";
	auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
	label->setAnchorPoint({ .0f, .5f });
	label->setPosition(m_height / 2, m_height / 2);
	label->setScale(.4f);
	this->addChild(label);

	return true;
}

CustomSettingPlaceHolderNode* CustomSettingPlaceHolderNode::create(CustomSettingPlaceHolder* setting, float width) {
	auto ret = new CustomSettingPlaceHolderNode(width, 30.f);
	if (ret && ret->init(setting)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

GEODE_GENERATE_SETTING_CREATE(BoolSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(IntSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(FloatSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(StringSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(ColorSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(ColorAlphaSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(PathSetting, 30.f);
GEODE_GENERATE_SETTING_CREATE(StringSelectSetting, 30.f);

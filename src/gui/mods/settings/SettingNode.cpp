#pragma warning(disable: 4067)

#include "SettingNode.hpp"
#include <nodes/TextRenderer.hpp>
#include <random>

USE_GEODE_NAMESPACE();

#define GEODE_GENERATE_SETTING_CREATE(_sett_, _height_)                \
	_sett_##Node* _sett_##Node::create(_sett_* setting, float width) { \
		auto ret = new _sett_##Node(width, _height_);                  \
		if (ret && ret->init(setting)) {                               \
			ret->autorelease();                                        \
			return ret;                                                \
		} CC_SAFE_DELETE(ret); return nullptr; }
	
template <typename T>
std::string to_string_with_precision(const T a_value, const size_t n = 6) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    auto str = out.str();
	while (string_utils::endsWith(str, "0")) {
		str = str.substr(0, str.size() - 1);
	}
	return str;
}


// bool

bool BoolSettingNode::init(BoolSetting* setting) {
	if (!GeodeSettingNode<BoolSetting>::init(setting))
		return false;

	auto toggle = CCMenuItemToggler::createWithStandardSprites(
		this, menu_selector(BoolSettingNode::onToggle), .65f
	);
	toggle->setPosition(-toggle->m_onButton->getScaledContentSize().width / 2, 0);
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
	
    auto bgSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bgSprite->setScale(.25f);
    bgSprite->setColor({ 0, 0, 0 });
    bgSprite->setOpacity(75);
    bgSprite->setContentSize({ 45.f * 4, m_height * 3 });
    bgSprite->setPosition(-30, 0);
    m_buttonMenu->addChild(bgSprite);

	m_valueInput = CCTextInputNode::create(45.f, m_height, "Num", "bigFont.fnt");
	m_valueInput->setAllowedChars("0123456789+- ");
	m_valueInput->setPosition(-30.f, .0f);
	m_valueInput->setMaxLabelScale(.5f);
    m_valueInput->setLabelPlaceholderColor({ 150, 150, 150 });
    m_valueInput->setLabelPlaceholderScale(.75f);
	m_valueInput->setDelegate(this);
	m_buttonMenu->addChild(m_valueInput);

	if (setting->hasArrows()) {
		auto decSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		decSpr->setScale(.3f);
		decSpr->setFlipX(true);

		auto decBtn = CCMenuItemSpriteExtra::create(
			decSpr, this, menu_selector(IntSettingNode::onArrow)
		);
		decBtn->setTag(-1);
		decBtn->setPosition(-60.f, 0);
		m_buttonMenu->addChild(decBtn);

		auto incSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		incSpr->setScale(.3f);

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

void IntSettingNode::textInputClosed(CCTextInputNode* input) {
	std::cout << __FUNCTION__ << "\n";
	try {
		m_setting->setValue(m_setting->getValue() + std::stoi(input->getString()));
	} catch(...) {}
	this->updateValue();
}

void IntSettingNode::onArrow(CCObject* pSender) {
	m_setting->setValue(m_setting->getValue() + pSender->getTag());
	m_valueInput->detachWithIME();
	this->updateValue();
}

void IntSettingNode::updateValue() {
	if (m_setting->getValue() < m_setting->getMin()) {
		m_setting->setValue(m_setting->getMin());
	}
	if (m_setting->getValue() > m_setting->getMax()) {
		m_setting->setValue(m_setting->getMax());
	}
	m_valueInput->setString(std::to_string(m_setting->getValue()).c_str());
}

// float

bool FloatSettingNode::init(FloatSetting* setting) {
	if (!GeodeSettingNode<FloatSetting>::init(setting))
		return false;
	
    auto bgSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bgSprite->setScale(.25f);
    bgSprite->setColor({ 0, 0, 0 });
    bgSprite->setOpacity(75);
    bgSprite->setContentSize({ 45.f * 4, m_height * 3 });
    bgSprite->setPosition(-30, 0);
    m_buttonMenu->addChild(bgSprite);

	m_valueInput = CCTextInputNode::create(45.f, m_height, "Num", "bigFont.fnt");
	m_valueInput->setAllowedChars("0123456789+-. ");
	m_valueInput->setPosition(-30.f, .0f);
	m_valueInput->setMaxLabelScale(.5f);
    m_valueInput->setLabelPlaceholderColor({ 150, 150, 150 });
    m_valueInput->setLabelPlaceholderScale(.75f);
	m_valueInput->setDelegate(this);
	m_buttonMenu->addChild(m_valueInput);

	if (setting->hasArrows()) {
		auto decSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		decSpr->setScale(.3f);
		decSpr->setFlipX(true);

		auto decBtn = CCMenuItemSpriteExtra::create(
			decSpr, this, menu_selector(FloatSettingNode::onArrow)
		);
		decBtn->setTag(-1);
		decBtn->setPosition(-60.f, 0);
		m_buttonMenu->addChild(decBtn);

		auto incSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
		incSpr->setScale(.3f);

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

void FloatSettingNode::textInputClosed(CCTextInputNode* input) {
	std::cout << __FUNCTION__ << "\n";
	try {
		m_setting->setValue(m_setting->getValue() + std::stoi(input->getString()));
	} catch(...) {}
	this->updateValue();
}

void FloatSettingNode::onArrow(CCObject* pSender) {
	m_setting->setValue(m_setting->getValue() + pSender->getTag());
	this->updateValue();
}

void FloatSettingNode::updateValue() {
	if (m_setting->getValue() < m_setting->getMin()) {
		m_setting->setValue(m_setting->getMin());
	}
	if (m_setting->getValue() > m_setting->getMax()) {
		m_setting->setValue(m_setting->getMax());
	}
	m_valueInput->setString(to_string_with_precision(
		m_setting->getValue(), m_setting->getPrecision()
	).c_str());
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

bool CustomSettingPlaceHolderNode::init(CustomSettingPlaceHolder* setting, bool isLoaded) {
	if (!CCNode::init())
		return false;

	auto pad = m_height;
	this->setContentSize({ m_width, m_height });

	// i'm using TextRenderer instead of TextArea because 
	// i couldn't get TextArea to work for some reason 
	// and TextRenderer should be fast enough for short 
	// static text

	auto render = TextRenderer::create();

	render->begin(this, CCPointZero, { m_width - pad * 2, m_height });

	render->pushFont(
		[](int) -> TextRenderer::Label {
			return CCLabelBMFont::create("", "chatFont.fnt");
		}
	);
	render->pushHorizontalAlign(TextAlignment::Begin);
	render->pushVerticalAlign(TextAlignment::Begin);
	render->pushScale(.7f);
	auto rendered = render->renderString(
		isLoaded ?
			"This setting (id: " + setting->getKey() + ") is a "
			"custom setting which has no registered setting node. "
			"This is likely a bug in the mod; report it to the "
			"developer." :
			"This setting (id: " + setting->getKey() + ") is a " 
			"custom setting, which means that you need to enable "
			"& load the mod to change its value."
	);

	render->end(true, TextAlignment::Center, TextAlignment::Center);
	render->release();

	m_height = this->getContentSize().height + pad / 4;
	m_width = this->getContentSize().width;
	m_width += pad * 2;
	this->setContentSize({ m_width, m_height });

	for (auto& label : rendered) {
		label.m_node->setPositionX(pad * 1.5f);
		label.m_node->setPositionY(label.m_node->getPositionY() + pad / 8);
	}

    auto bgSprite = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bgSprite->setScale(.5f);
    bgSprite->setColor({ 0, 0, 0 });
    bgSprite->setOpacity(75);
    bgSprite->setZOrder(-1);
    bgSprite->setContentSize(m_obContentSize * 2 - CCSize { pad, 0 });
    bgSprite->setPosition(m_obContentSize / 2);
    this->addChild(bgSprite);

	auto iconSprite = CCSprite::createWithSpriteFrameName(
		isLoaded ? "info-warning.png"_spr : "GJ_infoIcon_001.png"
	);
	iconSprite->setPosition({ pad * .9f, m_height / 2 });
	iconSprite->setScale(.8f);
	this->addChild(iconSprite);

	return true;
}

CustomSettingPlaceHolderNode* CustomSettingPlaceHolderNode::create(CustomSettingPlaceHolder* setting, bool isLoaded, float width) {
	auto ret = new CustomSettingPlaceHolderNode(width, 30.f);
	if (ret && ret->init(setting, isLoaded)) {
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

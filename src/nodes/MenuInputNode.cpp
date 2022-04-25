#include <nodes/MenuInputNode.hpp>

USE_GEODE_NAMESPACE();

bool MenuInputNode::init(
    float width, float height, char const* placeholder, char const* fontPath
) {
    if (!CCMenuItem::init())
        return false;
    
    this->setContentSize({ width, height });
    this->setAnchorPoint({ .5f, .5f });
    m_input = CCTextInputNode::create(width, height, placeholder, fontPath);
    m_input->setPosition(width / 2, height / 2);
    this->addChild(m_input);
    
    this->setEnabled(true);

    return true;
}

MenuInputNode* MenuInputNode::create(
    float width, float height, char const* placeholder, char const* fontPath
) {
    auto ret = new MenuInputNode;
    if (ret && ret->init(width, height, placeholder, fontPath)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void MenuInputNode::selected() {
    m_input->onClickTrackNode(true);
}

CCTextInputNode* MenuInputNode::getInput() const {
    return m_input;
}

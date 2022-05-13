#include <general/Notification.hpp>
#include <general/TextRenderer.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

Notification::Notification() {
    SceneManager::get()->keepAcrossScenes(this);
}

Notification::~Notification() {
    CC_SAFE_RELEASE(m_labels);
    CC_SAFE_RELEASE(m_buttons);
    SceneManager::get()->forget(this);
}

// void Notification::mouseEnterExt(CCPoint const&) {
//     if (m_target && m_handler) {
//         m_bg->setColor({ 150, 150, 150 });
//     }
// }

// void Notification::mouseLeaveExt(CCPoint const&) {
//     if (m_target && m_handler) {
//         m_bg->setColor({ 255, 255, 255 });
//     }
// }

// bool Notification::mouseUpExt(MouseEvent button, CCPoint const& pos) {
//     if (button == MouseEvent::Left) {
//         this->clicked();
//     }
//     return false;
// }

void Notification::clicked() {
    if (!m_hiding) {
        this->stopAllActions();
        this->hide();
    }
}

bool Notification::init(
    std::string const& title,
    std::string const& text,
    CCNode* icon,
    const char* bg
) {
    if (!CCNode::init())
        return false;

    m_labels = CCArray::create();
    m_labels->retain();
    m_buttons = CCArray::create();
    m_buttons->retain();

    m_bg = CCScale9Sprite::create(bg);
    m_bg->setScale(.6f);

    if (title.size()) {
        m_title = CCLabelBMFont::create(title.c_str(), "goldFont.fnt");
    }

    auto renderer = TextRenderer::create();
    renderer->begin(this, CCPointZero, { 120.f, 20.f });

    renderer->pushBMFont("chatFont.fnt");
    renderer->pushScale(.4f);
    for (auto& label : renderer->renderString(text)) {
        m_labels->addObject(label.m_node);
    }

    renderer->end();
    renderer->release();

    float iconSpace = .0f;
    if (icon) {
        m_icon = icon;
        iconSpace = 20.f;
        m_icon->setPosition({ iconSpace / 2, m_obContentSize.height / 2 });
        limitNodeSize(m_icon, { iconSpace - 8.f, m_obContentSize.height - 8.f }, 1.f, .1f);
        this->addChild(m_icon);
    }

    if (m_title) {
        m_title->limitLabelWidth(m_obContentSize.width - iconSpace, .4f, .01f);
        m_obContentSize.height += 12;
        m_title->setPosition(iconSpace, m_obContentSize.height - 6.f);
        m_title->setAnchorPoint({ .0f, .5f });
        if (m_icon) {
            m_icon->setPositionY(m_obContentSize.height / 2);
        }
        this->addChild(m_title);
    }

    m_obContentSize.width += iconSpace;
    CCARRAY_FOREACH_B_TYPE(m_labels, label, CCNode) {
        label->setPositionX(label->getPositionX() + iconSpace);
    }

    m_bg->setContentSize(m_obContentSize / m_bg->getScale() + CCSize { 6.f, 6.f });
    m_bg->setPosition(m_obContentSize / 2);
    m_bg->setZOrder(-1);
    this->addChild(m_bg);

    m_menu = CCMenu::create();
    m_menu->setPosition(iconSpace, 10.f);
    this->addChild(m_menu);

    auto closeMenu = CCMenu::create();
    closeMenu->setPosition(CCPointZero);
    this->addChild(closeMenu);

    auto closeBtnSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeBtnSpr->setScale(.4f);
    m_closeBtn = CCMenuItemSpriteExtra::create(
        closeBtnSpr, this, menu_selector(Notification::onHide)
    );
    m_closeBtn->setPosition(m_obContentSize);
    closeMenu->addChild(m_closeBtn);

    this->setAnchorPoint({ .0f, .0f });
    this->setVisible(false);

    return true;
}

Notification* Notification::create(
    std::string const& title,
    std::string const& text,
    CCNode* icon,
    const char* bg
) {
    auto ret = new Notification();
    if (ret && ret->init(title, text, icon, bg)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void Notification::addButton(
    std::string const& title,
    CCObject* target,
    SEL_MenuHandler handler
) {
    if (!m_buttons->count()) {
        static constexpr const float heightIncrease = 10.f;

        m_obContentSize.height += heightIncrease;

        CCARRAY_FOREACH_B_TYPE(m_labels, label, CCNode) {
            label->setPositionY(label->getPositionY() + heightIncrease);
        }
        m_icon->setPositionY(m_obContentSize.height / 2);

        m_title->setPositionY(m_title->getPositionY() + heightIncrease);

        m_bg->setContentSize(m_obContentSize / m_bg->getScale() + CCSize { 6.f, 6.f });
        m_bg->setPosition(m_obContentSize / 2);

        m_closeBtn->setPosition(m_obContentSize);
    }

    auto buttonSpr = ButtonSprite::create(title.c_str());
    buttonSpr->setScale(.4f);
    auto button = CCMenuItemSpriteExtra::create(buttonSpr, target, handler);
    m_buttons->addObject(button);
    m_menu->addChild(button);

    m_menu->alignItemsHorizontallyWithPadding(3.f);

    auto btns = calculateChildCoverage(m_menu);
    CCARRAY_FOREACH_B_TYPE(m_buttons, btn, CCNode) {
        btn->setPositionX(btn->getPositionX() + (btns.size.width - btns.origin.x) / 2);
    }
}

void Notification::showForReal() {
    // haha i am incredibly mature
    this->setZOrder(0xB00B1E5);
    this->setVisible(true);

    static constexpr const float pad = 15.f;

    float xMovement = .0f, yMovement = .0f;
    float xStart = .0f, yStart = .0f;
    switch (m_location) {
        case NotificationLocation::TopLeft:
            xMovement = pad * 2 + this->getScaledContentSize().width;
            xStart = -this->getScaledContentSize().width;
            yStart = m_pParent->getContentSize().height
                - pad - this->getScaledContentSize().height * (1.f - m_obAnchorPoint.y);
            break;

        case NotificationLocation::BottomLeft:
            xMovement = pad * 2 + this->getScaledContentSize().width;
            xStart = -this->getScaledContentSize().width;
            yStart = pad - this->getScaledContentSize().height * m_obAnchorPoint.y;
            break;

        case NotificationLocation::TopRight:
            xMovement = -pad * 2 - this->getScaledContentSize().width * 2;
            xStart = m_pParent->getContentSize().width + this->getScaledContentSize().width;
            yStart = m_pParent->getContentSize().height
                - pad - this->getScaledContentSize().height * (1.f - m_obAnchorPoint.y);
            break;

        case NotificationLocation::BottomRight:
            xMovement = -pad * 2 - this->getScaledContentSize().width * 2;
            xStart = m_pParent->getContentSize().width + this->getScaledContentSize().width;
            yStart = pad + this->getScaledContentSize().height * m_obAnchorPoint.y;
            break;

        case NotificationLocation::BottomCenter:
            yMovement = pad * 2 + this->getScaledContentSize().height;
            xStart = m_pParent->getContentSize().width / 2
                - this->getScaledContentSize().width / 2;
            yStart = -pad - this->getScaledContentSize().height * (1.f - m_obAnchorPoint.y);
            break;

        case NotificationLocation::TopCenter:
            yMovement = -pad * 2 - this->getScaledContentSize().height;
            xStart = m_pParent->getContentSize().width / 2
                - this->getScaledContentSize().width / 2;
            yStart = m_pParent->getContentSize().height + pad;
            break;
    }

    m_hideDest = CCPoint { xStart, yStart };

    this->setPosition(xStart, yStart);
    this->runAction(CCEaseOut::create(
        CCMoveTo::create(.3f, { xStart + xMovement, yStart + yMovement }),
        6.f
    ));
    if (m_time)  {
        this->runAction(CCSequence::create(
            CCDelayTime::create(m_time),
            CCCallFunc::create(this, callfunc_selector(Notification::hide)),
            nullptr
        ));
    }
}

void Notification::show(NotificationLocation location, float time) {
    if (!this->getParent()) {
        CCDirector::sharedDirector()->getRunningScene()->addChild(this);
    }

    if (location == NotificationLocation::TopCenter) {
        // the notification is larger at top center to 
        // be more easily readable on mobile
        this->setScale(1.5f);
    } else {
        this->setScale(1.2f);
    }

    m_time = time;
    m_location = location;
    NotificationManager::get()->push(this);
    this->showForReal();
}

void Notification::onHide(CCObject*) {
    this->hide();
}

void Notification::hide() {
    m_hiding = true;
    this->runAction(CCSequence::create(
        CCEaseIn::create(
            CCMoveTo::create(.3f, { m_hideDest }),
            6.f
        ),
        CCCallFunc::create(this, callfunc_selector(Notification::hidden)),
        nullptr
    ));
}

void Notification::hidden() {
    NotificationManager::get()->pop(this);
    this->removeFromParent();
}

NotificationBuilder Notification::build() {
    return NotificationBuilder();
}

Notification* NotificationBuilder::show() {
    auto icon = m_iconNode;
    if (!icon && m_icon.size()) {
        icon = CCSprite::create(m_icon.c_str());
        if (!icon) icon = CCSprite::createWithSpriteFrameName(m_icon.c_str());
    }
    auto notif = Notification::create(
        m_title, m_text, icon, m_bg.c_str()
    );
    for (auto& btn : m_buttons) {
        notif->addButton(btn.m_text, btn.m_sender, btn.m_handler);
    }
    notif->show(m_location, m_time);
    return notif;
}

void NotificationManager::push(Notification* notification) {
    auto location = notification->m_location;
    if (!m_notifications.count(location)) {
        m_notifications[location] = { notification };
    } else {
        m_notifications[location].push_back(notification);
    }
}

void NotificationManager::pop(Notification* notification) {
    auto location = notification->m_location;
    if (m_notifications.count(location)) {
        vector_utils::erase(m_notifications.at(location), notification);
        if (!m_notifications.at(location).size()) {
            m_notifications.erase(location);
        } else {
            m_notifications.at(location).front()->showForReal();
        }
    }
}

NotificationManager* NotificationManager::get() {
    static auto inst = new NotificationManager;
    return inst;
}

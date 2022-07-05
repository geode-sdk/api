#include <general/Notification.hpp>
#include <general/TextRenderer.hpp>
#include <utils/WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

// todo: make sure notifications dont disappear 
// off the screen if the user happens to switch 
// scenes or smth that causes actions from being 
// run / completed

Notification::Notification() {}

Notification::~Notification() {
    CCDirector::sharedDirector()->getTouchDispatcher()->decrementForcePrio(2);
    CC_SAFE_RELEASE(m_labels);
}

void Notification::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(
        this,
        0,
        true
    );
}

void Notification::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    auto dist = touch->getLocation() - touch->getStartLocation();
    switch (m_location) {
        case NotificationLocation::BottomLeft:
        case NotificationLocation::TopLeft:
            this->setPositionX(m_posAtTouchStart.x + dist.x);
            if (this->getPositionX() > m_showDest.x) {
                this->setPositionX(m_showDest.x);
            }
            break;
            
        case NotificationLocation::BottomRight:
        case NotificationLocation::TopRight:
            this->setPositionX(m_posAtTouchStart.x + dist.x);
            if (this->getPositionX() < m_showDest.x) {
                this->setPositionX(m_showDest.x);
            }
            break;

        case NotificationLocation::BottomCenter:
            this->setPositionY(m_posAtTouchStart.y + dist.y);
            if (this->getPositionY() > m_showDest.y) {
                this->setPositionY(m_showDest.y);
            }
            break;

        case NotificationLocation::TopCenter:
            this->setPositionY(m_posAtTouchStart.y + dist.y);
            if (this->getPositionY() < m_showDest.y) {
                this->setPositionY(m_showDest.y);
            }
            break;
    }
}

bool Notification::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto csize = this->getScaledContentSize();
    if (
        !CCRect {
            m_obPosition.x - csize.width * m_obAnchorPoint.x,
            m_obPosition.y - csize.height * m_obAnchorPoint.y,
            csize.width,
            csize.height
        }.containsPoint(touch->getLocation())
    ) {
        return false;
    }
    m_bg->setColor({ 150, 150, 150 });
    this->stopAllActions();
    m_posAtTouchStart = this->getPosition();
    m_touchTime = std::chrono::system_clock::now();
    return true;
}

void Notification::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    static constexpr const float HIDE_THRESHOLD = 20.f;

    m_bg->setColor({ 255, 255, 255 });
    auto dist = touch->getLocation() - touch->getStartLocation();
    switch (m_location) {
        case NotificationLocation::BottomLeft:
        case NotificationLocation::TopLeft:
            if (dist.x < -HIDE_THRESHOLD) {
                this->hide();
                return;
            } break;

        case NotificationLocation::BottomRight:
        case NotificationLocation::TopRight:
            if (dist.x > HIDE_THRESHOLD) {
                this->hide();
                return;
            } break;

        case NotificationLocation::BottomCenter:
            if (dist.y < -HIDE_THRESHOLD) {
                this->hide();
                return;
            } break;

        case NotificationLocation::TopCenter:
            if (dist.y > HIDE_THRESHOLD) {
                this->hide();
                return;
            } break;
    }
    auto touchTime = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
        m_touchTime - touchTime
    ).count() > 500) {
        if (m_hiding) {
            this->animateOut();
        } else {
            this->animateIn();
        }
    } else {
        this->animateIn();
        this->clicked();
    }
}

void Notification::clicked() {
    if (m_callback) {
        m_callback(this);
    }
}

bool Notification::init(
    Mod* owner,
    std::string const& title,
    std::string const& text,
    CCNode* icon,
    const char* bg,
    std::function<void(Notification*)> callback
) {
    if (!CCNode::init())
        return false;
    
    m_owner = owner;
    m_callback = callback;

    m_labels = CCArray::create();
    m_labels->retain();

    m_bg = CCScale9Sprite::create(bg);
    m_bg->setScale(.6f);

    if (title.size()) {
        m_title = CCLabelBMFont::create(title.c_str(), "goldFont.fnt");
    }

    auto renderer = TextRenderer::create();
    renderer->begin(this, CCPointZero, { 120.f, 20.f });

    renderer->pushBMFont("chatFont.fnt");
    renderer->pushScale(.4f);
    for (auto& label : renderer->renderString(
        text + " (from " + owner->getName() + ")"
    )) {
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

    this->setAnchorPoint({ .0f, .0f });
    this->setVisible(false);

    this->setTouchEnabled(true);
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();

    return true;
}

Notification* Notification::create(
    Mod* owner,
    std::string const& title,
    std::string const& text,
    CCNode* icon,
    const char* bg,
    std::function<void(Notification*)> callback
) {
    auto ret = new Notification();
    if (ret && ret->init(owner, title, text, icon, bg, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void Notification::showForReal() {
    if (!m_pParent) {
        CCDirector::sharedDirector()->getRunningScene()->addChild(this);
    }
    SceneManager::get()->keepAcrossScenes(this);
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
    m_showDest = CCPoint { xStart + xMovement, yStart + yMovement };

    GameSoundManager::sharedManager()->playEffect(
        "newNotif03.ogg"_spr, 1.f, 1.f, 1.f
    );

    this->setPosition(xStart, yStart);
    this->animateIn();
}

void Notification::hide() {
    GameSoundManager::sharedManager()->playEffect(
        "byeNotif00.ogg"_spr, 1.f, 1.f, 1.f
    );
    m_hiding = true;
    this->animateOut();
}

void Notification::animateIn() {
    this->runAction(CCEaseInOut::create(
        CCMoveTo::create(.3f, m_showDest),
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

void Notification::animateOut() {
    this->runAction(CCSequence::create(
        CCEaseInOut::create(
            CCMoveTo::create(.3f, { m_hideDest }),
            6.f
        ),
        CCCallFunc::create(this, callfunc_selector(Notification::hidden)),
        nullptr
    ));
}

void Notification::show(NotificationLocation location, float time) {
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
}

void Notification::hidden() {
    NotificationManager::get()->pop(this);
    this->removeFromParent();
    SceneManager::get()->forget(this);
}

NotificationBuilder Notification::build() {
    return std::move(NotificationBuilder());
}

Notification* NotificationBuilder::show() {
    auto icon = m_iconNode;
    if (!icon && m_icon.size()) {
        icon = CCSprite::create(m_icon.c_str());
        if (!icon) icon = CCSprite::createWithSpriteFrameName(m_icon.c_str());
    }
    auto notif = Notification::create(
        m_owner, m_title, m_text, icon, m_bg.c_str(), m_callback
    );
    notif->show(m_location, m_time);
    return notif;
}


void NotificationManager::push(Notification* notification) {
    auto location = notification->m_location;
    if (!m_notifications.count(location)) {
        m_notifications[location] = { notification };
        notification->showForReal();
    } else {
        m_notifications[location].push_back(notification);
    }
}

void NotificationManager::pop(Notification* notification) {
    auto location = notification->m_location;
    if (m_notifications.count(location)) {
        vector_utils::erase(
            m_notifications.at(location), Ref(notification)
        );
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

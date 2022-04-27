#include <nodes/Scrollbar.hpp>

USE_GEODE_NAMESPACE();

bool Scrollbar::mouseDownExt(MouseEvent, cocos2d::CCPoint const& mpos) {
    if (!m_list) return false;

    ExtMouseManager::get()->captureMouse(this);

    auto pos = this->convertToNodeSpace(mpos);

    auto h = m_list->m_tableView->m_contentLayer->getScaledContentSize().height
        - m_list->m_height + m_list->m_tableView->m_scrollLimitTop;
    auto p = m_list->m_height /
        m_list->m_tableView->m_contentLayer->getScaledContentSize().height;

    auto trackHeight = std::min(p, 1.0f) * m_list->m_height / .4f;

    auto posY = h * (
        (-pos.y - m_list->m_height / 2 + trackHeight / 4 - 5) /
        (m_list->m_height - trackHeight / 2 + 10)
    );

    if (posY > 0.0f) posY = 0.0f;
    if (posY < -h) posY = -h;
    
    auto offsetY = m_list->m_tableView->m_contentLayer->getPositionY() - posY;

    if (fabsf(offsetY) < trackHeight) {
        m_extMouseHitArea.origin = CCPoint {
            pos.x,
            m_list->m_tableView->m_contentLayer->getPositionY() - posY
        };
    } else {
        m_extMouseHitArea.origin = CCPointZero;
    }
    
    return true;
}

bool Scrollbar::mouseUpExt(MouseEvent, cocos2d::CCPoint const&) {
    ExtMouseManager::get()->releaseCapture(this);
    return true;
}

void Scrollbar::mouseMoveExt(cocos2d::CCPoint const& mpos) {
    if (!m_list) return;

    if (this->m_extMouseDown.size()) {
        auto pos = this->convertToNodeSpace(mpos);

        auto h = m_list->m_tableView->m_contentLayer->getScaledContentSize().height
            - m_list->m_height + m_list->m_tableView->m_scrollLimitTop;
        auto p = m_list->m_height /
            m_list->m_tableView->m_contentLayer->getScaledContentSize().height;

        auto trackHeight = std::min(p, 1.0f) * m_list->m_height / .4f;

        auto posY = h * (
            (-pos.y - m_list->m_height / 2 + trackHeight / 4 - 5) /
            (m_list->m_height - trackHeight / 2 + 10)
        );

        posY += m_extMouseHitArea.origin.y;

        if (posY > 0.0f) posY = 0.0f;
        if (posY < -h) posY = -h;

        m_list->m_tableView->m_contentLayer->setPositionY(posY);
    }
}

bool Scrollbar::mouseScrollExt(float y, float x) {
    if (!m_list)
        return false;
    m_list->scrollWheel(x, y);
    return true;
}

void Scrollbar::draw() {
    CCLayer::draw();

    if (!m_list) return;

    m_bg->setContentSize({
        m_width, m_list->m_height
    });
    m_bg->setScale(1.0f);
    m_bg->setColor({ 0, 0, 0 });
    m_bg->setOpacity(150);
    m_bg->setPosition(0.0f, 0.0f);

    m_extMouseHitArea.size = CCSize {
        m_width, m_list->m_height
    };

    auto h = m_list->m_tableView->m_contentLayer->getScaledContentSize().height
        - m_list->m_height + m_list->m_tableView->m_scrollLimitTop;
    auto p = m_list->m_height /
        m_list->m_tableView->m_contentLayer->getScaledContentSize().height;

    auto trackHeight = std::min(p, 1.0f) * m_list->m_height / .4f;
    auto trackPosY = m_track->getPositionY();

    GLubyte o = 100;

    if (m_extMouseHovered) {
        o = 160;
    }
    if (m_extMouseDown.size()) {
        o = 255;
    }

    m_track->setScale(.4f);
    m_track->setColor({ o, o, o });


    auto y = m_list->m_tableView->m_contentLayer->getPositionY();

    trackPosY = - m_list->m_height / 2 + trackHeight / 4 - 5.0f + 
        ((-y) / h) * (m_list->m_height - trackHeight / 2 + 10.0f);

    auto fHeightTop = [&]() -> float {
        return trackPosY - m_list->m_height / 2 + trackHeight * .4f / 2 + 3.0f;
    };
    auto fHeightBottom = [&]() -> float {
        return trackPosY + m_list->m_height / 2 - trackHeight * .4f / 2 - 3.0f;
    };
    
    if (fHeightTop() > 0.0f) {
        trackHeight -= fHeightTop();
        trackPosY -= fHeightTop();
    }
    
    if (fHeightBottom() < 0.0f) {
        trackHeight += fHeightBottom();
        trackPosY -= fHeightBottom();
    }

    m_track->setPosition(0.0f, trackPosY);

    m_track->setContentSize({
        m_width, trackHeight
    });
}

void Scrollbar::setList(BoomListView* list) {
    m_list = list;
}

bool Scrollbar::init(BoomListView* list) {
    if (!this->CCLayer::init())
        return false;
    
    this->setList(list);
    m_width = 8.0f;

    m_bg = CCScale9Sprite::create("scrollbar.png"_spr);
    m_track = CCScale9Sprite::create("scrollbar.png"_spr);
    // if (BGDInternal::isFileInSearchPaths("BGD_scrollbar.png")) {
    //     m_bg = CCScale9Sprite::create(
    //         "BGD_scrollbar.png"
    //     );
    //     m_track = CCScale9Sprite::create(
    //         "BGD_scrollbar.png"
    //     );
    // } else {
    //     m_bg = CCScale9Sprite::create(
    //         "square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f }
    //     );
    //     m_track = CCScale9Sprite::create(
    //         "square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f }
    //     );
    // }

    this->addChild(m_bg);
    this->addChild(m_track);

    return true;
}

Scrollbar* Scrollbar::create(BoomListView* list) {
    auto ret = new Scrollbar;

    if (ret && ret->init(list)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


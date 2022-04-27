#include <dispatch/ExtMouseManager.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

void ExtMouseDelegate::mouseEnterExt(CCPoint const&) {}
void ExtMouseDelegate::mouseLeaveExt(CCPoint const&) {}
bool ExtMouseDelegate::mouseDownExt(MouseEvent, CCPoint const&) { return false; }
bool ExtMouseDelegate::mouseUpExt(MouseEvent, CCPoint const&) { return false; }
void ExtMouseDelegate::mouseMoveExt(CCPoint const&) {}
void ExtMouseDelegate::mouseDownOutsideExt(MouseEvent, CCPoint const&) {}
void ExtMouseDelegate::mouseUpOutsideExt(MouseEvent, CCPoint const&) {}
bool ExtMouseDelegate::mouseScrollExt(float, float) { return false; }
void ExtMouseDelegate::mouseScrollOutsideExt(float, float) {}

ExtMouseDelegate::ExtMouseDelegate() {
    ExtMouseManager::get()->pushDelegate(this);
}

ExtMouseDelegate::~ExtMouseDelegate() {
    ExtMouseManager::get()->popDelegate(this);
}


void getTouchChildren(CCArray* arr, CCNode* node) {
    if (!node) return;
    CCARRAY_FOREACH_B_TYPE(node->getChildren(), obj, CCNode) {
        if (dynamic_cast<CCTouchDelegate*>(obj)) {
            arr->addObject(obj);
        } else if (obj->getChildrenCount()) {
            getTouchChildren(arr, obj);
        }
    }
}


bool ExtMouseManager::init() {
    return true;
}

ExtMouseManager::ExtMouseManager() {
    this->init();
}

ExtMouseManager::~ExtMouseManager() {
    for (auto const& delegate : m_delegates) {
        this->popDelegate(delegate);
    }
}

ExtMouseManager* ExtMouseManager::get() {
    static auto g_manager = new ExtMouseManager;
    return g_manager;
}

void ExtMouseManager::pushDelegate(ExtMouseDelegate* delegate) {
    m_delegates.insert(m_delegates.begin(), delegate);
}

void ExtMouseManager::popDelegate(ExtMouseDelegate* delegate) {
    m_delegates.erase(std::remove(
        m_delegates.begin(), m_delegates.end(), delegate
    ), m_delegates.end());
}

bool ExtMouseManager::delegateIsHovered(ExtMouseDelegate* delegate, CCPoint const& mpos) {
    auto size = delegate->m_extMouseHitArea.size;

    auto p = dynamic_cast<CCNode*>(delegate);

    if (p && size == CCSizeZero) size = p->getScaledContentSize();
    if (p && !cocos::nodeIsVisible(p)) return false;

    auto pos = p ? p->getPosition() : CCPointZero;

    if (p && p->getParent())
        pos = p->getParent()->convertToWorldSpace(p->getPosition());
    
    pos = pos + delegate->m_extMouseHitArea.origin;

    auto rect = CCRect {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };

    return rect.containsPoint(mpos);
}

bool ExtMouseManager::dispatchClickEvent(MouseEvent btn, bool down, CCPoint const& pos) {
    if (down) {
        this->m_pressedButtons.insert(btn);
    } else {
        this->m_pressedButtons.erase(btn);
    }
    if (m_capturing) {
        m_capturing->m_extMouseDown = m_pressedButtons;
        if (down) {
            return m_capturing->mouseDownExt(btn, pos);
        } else {
            m_capturing->m_extMouseHovered = delegateIsHovered(m_capturing, pos);
            return m_capturing->mouseUpExt(btn, pos);
        }
    }
    for (auto const& d : m_delegates) {
        if (!down) d->m_extMouseDown.clear();
        if (delegateIsHovered(d, pos)) {
            if (down) {
                d->m_extMouseDown = m_pressedButtons;
                if (d->mouseDownExt(btn, pos)) return true;
            } else {
                if (d->mouseUpExt(btn, pos)) return true;
            }
        } else {
            if (down) d->mouseDownOutsideExt(btn, pos);
        }
    }
    return false;
}

bool ExtMouseManager::dispatchScrollEvent(float y, float x, CCPoint const& pos) {
    if (m_capturing) {
        return m_capturing->mouseScrollExt(y, x);
    }
    for (auto const& d : m_delegates) {
        if (delegateIsHovered(d, pos)) {
            if (d->mouseScrollExt(y, x))
                return true;
        } else
            d->mouseScrollOutsideExt(y, x);
    }
    return false;
}

void ExtMouseManager::dispatchMoveEvent(CCPoint const& pos) {
    if (m_capturing) {
        this->m_lastPosition = pos;
        return m_capturing->mouseMoveExt(pos);
    }
    if (this->m_lastPosition != pos) {
        for (auto const& d : m_delegates) {
            auto hover = this->delegateIsHovered(d, pos);
            if (d->m_extMouseHovered != hover) {
                d->m_extMouseHovered = hover;

                if (hover) {
                    d->m_extMouseDown = m_pressedButtons;
                    d->mouseEnterExt(pos);
                } else {
                    d->m_extMouseDown.clear();
                    d->mouseLeaveExt(pos);
                }
            }
            if (hover) {
                d->mouseMoveExt(pos);
            }
        }
    }
    this->m_lastPosition = pos;
}

void ExtMouseManager::captureMouse(ExtMouseDelegate* delegate) {
    if (!m_capturing) {
        m_capturing = delegate;
    }
}

void ExtMouseManager::releaseCapture(ExtMouseDelegate* delegate) {
    if (!delegate || m_capturing == delegate) {
        m_capturing = nullptr;
    }
}

bool ExtMouseManager::isMouseDown(MouseEvent btn) const {
    return this->m_pressedButtons.count(btn);
}

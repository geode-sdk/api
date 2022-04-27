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
    delegate->m_targetPriority = CCDirector::sharedDirector()->getTouchDispatcher()->getTargetPrio();
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

class BypassCCTouchDispatcher : public CCTouchDispatcher {
public:
    CCArray* getStandardHandlers() const {
        return m_pStandardHandlers;
    }
    CCArray* getTargetedHandlers() const {
        return m_pTargetedHandlers;
    }
};

int ExtMouseManager::maxTargetPrio() const {
    // this is very odd code...
    // all i wanted to do was make sure that 
    // if some layer on top doesn't pass touches 
    // through normally then this wouldn't 
    // pass mouse stuff aswell
    // however CCTouchDispatcher is very weird
    // i should probably just hook it and update 
    // this through that
    auto d = as<BypassCCTouchDispatcher*>(
        CCDirector::sharedDirector()->getTouchDispatcher()
    );
    int prio = 0;
    CCARRAY_FOREACH_B_TYPE(d->getTargetedHandlers(), handler, CCTouchHandler) {
        if (handler->getPriority() < prio) {
            prio = handler->getPriority();
        }
    }
    CCARRAY_FOREACH_B_TYPE(d->getStandardHandlers(), handler, CCTouchHandler) {
        if (handler->getPriority() < prio) {
            prio = handler->getPriority();
        }
    }
    return prio;
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
    auto prio = maxTargetPrio();
    for (auto const& d : m_delegates) {
        if (d->m_targetPriority != prio) continue;
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
    auto prio = maxTargetPrio();
    for (auto const& d : m_delegates) {
        if (d->m_targetPriority != prio) continue;
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
    auto prio = maxTargetPrio();
    if (this->m_lastPosition != pos) {
        for (auto const& d : m_delegates) {
            if (d->m_targetPriority != prio) continue;
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

bool ExtMouseManager::isCapturing(ExtMouseDelegate* delegate) const {
    return m_capturing == delegate;
}

bool ExtMouseManager::isMouseDown(MouseEvent btn) const {
    return this->m_pressedButtons.count(btn);
}

CCPoint ExtMouseManager::getMousePosition() {
    static auto cachedMousePos = CCPointZero;
    auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
    if (mpos == cachedMousePos) return cachedMousePos;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    mpos.y = winSizePx.size.height - mpos.y;
    mpos.x *= ratio_w;
    mpos.y *= ratio_h;
    return mpos;
}

#include <dispatch/ExtMouseDispatcher.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

struct CopyTouchDispatcher : public CCTouchDispatcher {
    void copyTo(CCTouchDispatcher* rawOther) {
        auto other = as<CopyTouchDispatcher*>(rawOther);
        other->m_pStandardHandlers->addObjectsFromArray(m_pStandardHandlers);
        other->m_pTargetedHandlers->addObjectsFromArray(m_pTargetedHandlers);
        other->m_bToAdd = m_bToAdd;
        other->m_bToRemove = m_bToRemove;
        other->m_pHandlersToAdd->addObjectsFromArray(m_pHandlersToAdd);
        ccCArrayAppendArrayWithResize(other->m_pHandlersToRemove, m_pHandlersToRemove);
    }
};

struct CopyMouseDispatcher : public CCMouseDispatcher {
    void copyTo(CCMouseDispatcher* rawOther) {
        auto other = as<CopyMouseDispatcher*>(rawOther);
        other->m_pMouseHandlers->addObjectsFromArray(m_pMouseHandlers);
        other->m_bToAdd = m_bToAdd;
        other->m_bToRemove = m_bToRemove;
        ccCArrayAppendArrayWithResize(other->m_pHandlersToAdd, m_pHandlersToAdd);
        ccCArrayAppendArrayWithResize(other->m_pHandlersToRemove, m_pHandlersToRemove);
    }
};

// ExtMouseDelegate

void ExtMouseDelegate::mouseEnterExt(CCPoint const&) {}
void ExtMouseDelegate::mouseLeaveExt(CCPoint const&) {}
bool ExtMouseDelegate::mouseDownExt(MouseEvent, CCPoint const&) { return false; }
bool ExtMouseDelegate::mouseUpExt(MouseEvent, CCPoint const&) { return false; }
void ExtMouseDelegate::mouseMoveExt(CCPoint const&) {}
void ExtMouseDelegate::mouseDownOutsideExt(MouseEvent, CCPoint const&) {}
void ExtMouseDelegate::mouseUpOutsideExt(MouseEvent, CCPoint const&) {}

ExtMouseDelegate::ExtMouseDelegate() : CCTouchDelegate(), CCMouseDelegate() {
}

ExtMouseDelegate::~ExtMouseDelegate() {
    ExtMouseDispatcher::get()->popDelegate(this);
}

void ExtMouseDelegate::registerWithMouseDispatcher() {
    ExtMouseDispatcher::get()->pushDelegate(this);
}

void ExtMouseDelegate::attainCapture() {
    ExtMouseDispatcher::get()->attainCapture(this);
}

void ExtMouseDelegate::releaseCapture() {
    ExtMouseDispatcher::get()->releaseCapture(this);
}

// ExtMouseDispatcher

bool ExtMouseDispatcher::init() {
    if (!CCTouchDispatcher::init()) {
        return false;
    }
    return true;
}

ExtMouseDispatcher::ExtMouseDispatcher() : CCTouchDispatcher(), CCMouseDispatcher() {
    this->init();
}

ExtMouseDispatcher* ExtMouseDispatcher::get() {
    static auto g_manager = new ExtMouseDispatcher;
    return g_manager;
}

void ExtMouseDispatcher::registerDispatcher() {
    // sharedDirector will call init if it hasn't
    // been done yet so this'll work where-ever 
    // you call it
    
    m_oldTouchDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();\
    CC_SAFE_RETAIN(m_oldTouchDispatcher);\
    as<CopyTouchDispatcher*>(m_oldTouchDispatcher)->copyTo(this);\
    CCDirector::sharedDirector()->setTouchDispatcher(this);

    m_oldMouseDispatcher = CCDirector::sharedDirector()->getMouseDispatcher();\
    CC_SAFE_RETAIN(m_oldMouseDispatcher);\
    as<CopyMouseDispatcher*>(m_oldMouseDispatcher)->copyTo(this);\
    CCDirector::sharedDirector()->setMouseDispatcher(this);
}

void ExtMouseDispatcher::unregisterDispatcher() {
    #define RESTORE_DISPATCHER(disp) \
        if (m_old##disp##Dispatcher) {\
            as<Copy##disp##Dispatcher*>(this)->copyTo(m_old##disp##Dispatcher);\
            CC_SAFE_RELEASE(m_old##disp##Dispatcher);\
        }

    RESTORE_DISPATCHER(Touch);
    RESTORE_DISPATCHER(Mouse);
}

void ExtMouseDispatcher::pushDelegate(ExtMouseDelegate* delegate) {
    CCTouchDispatcher::addTargetedDelegate(delegate, m_nTargetPrio, true);
    CCMouseDispatcher::addDelegate(delegate);
}

void ExtMouseDispatcher::popDelegate(ExtMouseDelegate* delegate) {
    CCTouchDispatcher::removeDelegate(delegate);
    CCMouseDispatcher::removeDelegate(delegate);
}

bool ExtMouseDispatcher::delegateIsHovered(ExtMouseDelegate* delegate, CCPoint const& mpos) {
    auto p = dynamic_cast<CCNode*>(delegate);
    if (!p) return false;

    auto size = p->getScaledContentSize();
    if (!cocos::nodeIsVisible(p)) return false;

    auto pos = p ? p->getPosition() : CCPointZero;

    if (p->getParent())
        pos = p->getParent()->convertToWorldSpace(p->getPosition());
    
    auto rect = CCRect {
        pos.x - size.width * p->getAnchorPoint().x,
        pos.y - size.height * p->getAnchorPoint().y,
        size.width,
        size.height
    };

    return rect.containsPoint(mpos);
}

void ExtMouseDispatcher::attainCapture(ExtMouseDelegate* delegate) {
    if (!m_capturing) {
        m_capturing = delegate;
    }
}

void ExtMouseDispatcher::releaseCapture(ExtMouseDelegate* delegate) {
    if (!delegate || m_capturing == delegate) {
        m_capturing = nullptr;
    }
}

bool ExtMouseDispatcher::isCapturing(ExtMouseDelegate* delegate) const {
    return m_capturing == delegate;
}

void ExtMouseDispatcher::mouseDown(MouseEvent btn) {
    m_pressedButtons.insert(btn);
}

void ExtMouseDispatcher::mouseUp(MouseEvent btn) {
    m_pressedButtons.erase(btn);
}

bool ExtMouseDispatcher::isMouseDown(MouseEvent btn) const {
    return m_pressedButtons.count(btn);
}

void ExtMouseDispatcher::handleTouchDelegates() {
    if (CCTouchDispatcher::m_bToRemove) {
        CCTouchDispatcher::m_bToRemove = false;
        for (unsigned int i = 0; i < CCTouchDispatcher::m_pHandlersToRemove->num; i++) {
            CCTouchDispatcher::forceRemoveDelegate(as<CCTouchDelegate*>(
                CCTouchDispatcher::m_pHandlersToRemove->arr[i]
            ));
        }
        ccCArrayRemoveAllValues(CCTouchDispatcher::m_pHandlersToRemove);
    }

    if (CCTouchDispatcher::m_bToAdd) {
        CCTouchDispatcher::m_bToAdd = false;
        CCARRAY_FOREACH_B_TYPE(CCTouchDispatcher::m_pHandlersToAdd, handler, CCTouchHandler) {
            if (!handler) break;
            if (dynamic_cast<CCTargetedTouchHandler*>(handler)) {
                this->forceAddHandler(handler, m_pTargetedHandlers);
            } else {
                this->forceAddHandler(handler, m_pStandardHandlers);
            }
        }
        CCTouchDispatcher::m_pHandlersToAdd->removeAllObjects();
    }

    if (m_bToQuit) {
        m_bToQuit = false;
        this->forceRemoveAllDelegates();
    }
}

void ExtMouseDispatcher::update() {
    // check if mouse has moved
    auto mpos = getMousePosition();
    if (m_lastPosition == mpos) {
        return;
    }
    m_lastPosition = mpos;

    // pass to capturing delegate if one exists
    if (m_capturing) {
        // if you've captured the mouse, the 
        // mouse cant really enter / leave the 
        // node unless you tell it to
        return m_capturing->mouseMoveExt(mpos);
    }
    
    CCTouchDispatcher::m_bLocked = true;
    CCARRAY_FOREACH_B_TYPE(m_pTargetedHandlers, handler, CCTargetedTouchHandler) {
        if (!handler) break;
        auto extDelegate = dynamic_cast<ExtMouseDelegate*>(handler->getDelegate());
        if (extDelegate) {
            auto hovered = delegateIsHovered(extDelegate, mpos);
            if (hovered != extDelegate->m_extMouseHovered) {
                extDelegate->m_extMouseHovered = hovered;
                if (hovered) {
                    extDelegate->m_extMouseDown = m_pressedButtons;
                    extDelegate->mouseEnterExt(mpos);
                } else {
                    extDelegate->m_extMouseDown.clear();
                    extDelegate->mouseLeaveExt(mpos);
                }
            }
            if (hovered) {
                extDelegate->mouseMoveExt(mpos);
            }
        }
    }
    CCTouchDispatcher::m_bLocked = false;

    // extdelegates are never standard delegates 
    // so no need to even check

    this->handleTouchDelegates();
}

void ExtMouseDispatcher::touches(
    cocos2d::CCSet* touches,
    cocos2d::CCEvent* event,
    unsigned int touchType
) {
    CCTouchDispatcher::m_bLocked = true;
    auto mutableSet = 
        m_pTargetedHandlers->count() == 0 ||
        m_pStandardHandlers->count() == 0;
    
    auto mutableTouches = mutableSet ? touches->copy() : touches;
    auto helperData = m_sHandlerHelperData[touchType];
    if (m_pTargetedHandlers->count()) {
        for (auto& obj : *touches) {
            auto touch = as<CCTouch*>(obj);
            CCARRAY_FOREACH_B_TYPE(m_pTargetedHandlers, handler, CCTargetedTouchHandler) {
                if (!handler) break;

                bool claimed = false;
                if (touchType == CCTOUCHBEGAN) {
                    claimed = handler->getDelegate()->ccTouchBegan(touch, event);
                    if (claimed) {
                        handler->getClaimedTouches()->addObject(touch);
                    }
                } else {
                    if (handler->getClaimedTouches()->containsObject(touch)) {
                        claimed = true;
                        switch (helperData.m_type) {
                            case CCTOUCHMOVED:
                                handler->getDelegate()->ccTouchMoved(touch, event);
                                break;

                            case CCTOUCHENDED:
                                handler->getDelegate()->ccTouchEnded(touch, event);
                                handler->getClaimedTouches()->removeObject(touch);
                                break;
                            
                            case CCTOUCHCANCELLED:
                                handler->getDelegate()->ccTouchCancelled(touch, event);
                                handler->getClaimedTouches()->removeObject(touch);
                                break;
                        }
                    }
                }

                if (claimed && handler->isSwallowsTouches()) {
                    if (mutableSet) {
                        mutableTouches->removeObject(touch);
                    }
                    break;
                }
            }
        }
    }

    if (m_pStandardHandlers->count() && mutableTouches->count()) {
        CCARRAY_FOREACH_B_TYPE(m_pStandardHandlers, handler, CCStandardTouchHandler) {
            if (!handler) break;
            switch (helperData.m_type) {
                case CCTOUCHBEGAN:
                    handler->getDelegate()->ccTouchesBegan(touches, event);
                    break;
                
                case CCTOUCHMOVED:
                    handler->getDelegate()->ccTouchesMoved(touches, event);
                    break;
                
                case CCTOUCHENDED:
                    handler->getDelegate()->ccTouchesEnded(touches, event);
                    break;
                
                case CCTOUCHCANCELLED:
                    handler->getDelegate()->ccTouchesCancelled(touches, event);
                    break;
            }
        }
    }

    if (mutableSet) {
        mutableTouches->release();
    }
    CCTouchDispatcher::m_bLocked = false;

    this->handleTouchDelegates();
}

bool ExtMouseDispatcher::dispatchMouseEvent(
    MouseEvent event,
    bool down,
    cocos2d::CCPoint const& mpos
) {
    // handle capturing delegates
    if (m_capturing) {
        if (down) {
            m_capturing->m_extMouseDown = m_pressedButtons;
            m_capturing->mouseDownExt(event, getMousePosition());
        } else {
            m_capturing->m_extMouseDown = m_pressedButtons;
            m_capturing->m_extMouseHovered = delegateIsHovered(m_capturing, getMousePosition());
            m_capturing->mouseUpExt(event, getMousePosition());
        }
        return true;
    }

    bool noPropagate = false;

    CCTouchDispatcher::m_bLocked = true;
    CCARRAY_FOREACH_B_TYPE(m_pTargetedHandlers, handler, CCTargetedTouchHandler) {
        if (!handler) break;
        auto extDelegate = dynamic_cast<ExtMouseDelegate*>(handler->getDelegate());
        if (extDelegate && delegateIsHovered(extDelegate, mpos)) {
            if (down) {
                extDelegate->m_extMouseDown = m_pressedButtons;
                if (extDelegate->mouseDownExt(event, mpos)) {
                    noPropagate = true;
                    break;
                }
            } else {
                extDelegate->m_extMouseDown = m_pressedButtons;
                if (extDelegate->mouseUpExt(event, mpos)) {
                    noPropagate = true;
                    break;
                }
            }
        }
        if (handler->isSwallowsTouches()) {
            break;
        }
    }
    CCTouchDispatcher::m_bLocked = false;

    return noPropagate;
}

bool ExtMouseDispatcher::dispatchScrollMSG(float x, float y) {
    CCMouseDispatcher::m_bLocked = true;
    CCARRAY_FOREACH_B_TYPE(m_pMouseHandlers, handler, CCMouseHandler) {
        if (!handler) break;

        bool invoke;
        bool noPropagate;

        auto delegate = dynamic_cast<ExtMouseDelegate*>(handler->getDelegate());
        if (delegate) {
            if (delegate->m_targetedScroll) {
                invoke = this->delegateIsHovered(delegate, this->getMousePosition());
            } else {
                invoke = true;
            }
            noPropagate = delegate->m_swallowScroll;
        } else {
            invoke = true;
            noPropagate = false;
        }

        if (invoke) {
            handler->getDelegate()->scrollWheel(x, y);
            if (noPropagate) {
                break;
            }
        }
    }
    CCMouseDispatcher::m_bLocked = false;

    if (CCMouseDispatcher::m_bToRemove) {
        CCMouseDispatcher::m_bToRemove = false;
        for (unsigned int i = 0; i < CCMouseDispatcher::m_pHandlersToRemove->num; i++) {
            CCMouseDispatcher::forceRemoveDelegate(as<CCMouseDelegate*>(
                CCMouseDispatcher::m_pHandlersToRemove->arr[i]
            ));
        }
        ccCArrayRemoveAllValues(CCMouseDispatcher::m_pHandlersToRemove);
    }

    if (CCMouseDispatcher::m_bToAdd) {
        CCMouseDispatcher::m_bToAdd = false;
        for (unsigned int i = 0; i < CCMouseDispatcher::m_pHandlersToAdd->num; i++) {
            CCMouseDispatcher::forceAddDelegate(as<CCMouseDelegate*>(
                CCMouseDispatcher::m_pHandlersToAdd->arr[i]
            ));
        }
        ccCArrayRemoveAllValues(CCMouseDispatcher::m_pHandlersToAdd);
    }
    return true;
}

#ifdef GEODE_IS_WINDOWS
CCPoint ExtMouseDispatcher::getMousePosition() {
    static auto cachedMouseCompare = CCPointZero;
    static auto cachedMousePos = CCPointZero;
    auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
    if (mpos == cachedMouseCompare) return cachedMousePos;
    cachedMouseCompare = mpos;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    mpos.y = winSizePx.size.height - mpos.y;
    mpos.x *= ratio_w;
    mpos.y *= ratio_h;
    cachedMousePos = mpos;
    return mpos;
}
#endif


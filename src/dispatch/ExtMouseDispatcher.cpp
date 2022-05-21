#include <dispatch/ExtMouseDispatcher.hpp>
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
    ExtMouseDispatcher::get()->pushDelegate(this);
}

ExtMouseDelegate::~ExtMouseDelegate() {
    ExtMouseDispatcher::get()->popDelegate(this);
}

void ExtMouseDelegate::attainCapture() {
    ExtMouseDispatcher::get()->attainCapture(this);
}

void ExtMouseDelegate::releaseCapture() {
    ExtMouseDispatcher::get()->releaseCapture(this);
}


static void getTouchChildren(CCArray* arr, CCNode* node) {
    if (!node) return;
    CCARRAY_FOREACH_B_TYPE(node->getChildren(), obj, CCNode) {
        if (dynamic_cast<CCTouchDelegate*>(obj)) {
            arr->addObject(obj);
        } else if (obj->getChildrenCount()) {
            getTouchChildren(arr, obj);
        }
    }
}


bool ExtMouseDispatcher::init() {
    if (!CCTouchDispatcher::init()) {
        return false;
    }
    return true;
}

ExtMouseDispatcher::ExtMouseDispatcher() : CCTouchDispatcher(), CCMouseDispatcher() {
    this->init();
}

ExtMouseDispatcher::~ExtMouseDispatcher() {
}

ExtMouseDispatcher* ExtMouseDispatcher::get() {
    static auto g_manager = new ExtMouseDispatcher;
    return g_manager;
}

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
        other->m_pDelegates->addObjectsFromArray(m_pDelegates);
        other->m_bToAdd = m_bToAdd;
        other->m_bToRemove = m_bToRemove;
        ccCArrayAppendArrayWithResize(other->m_pHandlersToAdd, m_pHandlersToAdd);
        ccCArrayAppendArrayWithResize(other->m_pHandlersToRemove, m_pHandlersToRemove);
    }
};

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
    this->addTargetedDelegate(delegate, m_nTargetPrio, true);
}

void ExtMouseDispatcher::popDelegate(ExtMouseDelegate* delegate) {
    this->CCTouchDispatcher::removeDelegate(delegate);
}

bool ExtMouseDispatcher::delegateIsHovered(ExtMouseDelegate* delegate, CCPoint const& mpos) {
    auto size = delegate->m_extMouseHitArea.size;

    auto p = dynamic_cast<CCNode*>(delegate);

    if (p && size == CCSizeZero) size = p->getScaledContentSize();
    if (p && !cocos::nodeIsVisible(p)) return false;

    auto pos = p ? p->getPosition() : CCPointZero;

    if (p && p->getParent())
        pos = p->getParent()->convertToWorldSpace(p->getPosition());
    
    pos = pos + delegate->m_extMouseHitArea.origin;

    auto rect = CCRect {
        pos.x - size.width * p->getAnchorPoint().x,
        pos.y - size.height * p->getAnchorPoint().y,
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

int ExtMouseDispatcher::maxTargetPrio() const {
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
    m_lastPressed = btn;
}

void ExtMouseDispatcher::mouseUp(MouseEvent btn) {
    m_pressedButtons.erase(btn);
    m_lastPressed = btn;
}

bool ExtMouseDispatcher::isMouseDown(MouseEvent btn) const {
    return m_pressedButtons.count(btn);
}

void ExtMouseDispatcher::update() {}

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

                auto extDelegate = dynamic_cast<ExtMouseDelegate*>(handler->getDelegate());
                if (extDelegate) {
                    if (touchType == CCTOUCHBEGAN) {
                        if (extDelegate->mouseDownExt(m_lastPressed, getMousePosition())) {
                            break;
                        }
                    } else if (
                        touchType == CCTOUCHENDED ||
                        touchType == CCTOUCHCANCELLED
                    ) {
                        if (extDelegate->mouseUpExt(m_lastPressed, getMousePosition())) {
                            break;
                        }
                    }
                }

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
        CCARRAY_FOREACH_B_TYPE(m_pTargetedHandlers, handler, CCStandardTouchHandler) {
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


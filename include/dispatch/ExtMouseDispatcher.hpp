#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include <unordered_set>
#include "../shortcuts/MouseEvent.hpp"

namespace geode {
    class ExtMouseDispatcher;

    static constexpr const unsigned int EXTMOUSEMOVED = 0x10;

    class GEODE_API_DLL ExtMouseDelegate :
        public cocos2d::CCTouchDelegate,
        public cocos2d::CCMouseDelegate
    {
    protected:
        cocos2d::CCRect m_extMouseHitArea = cocos2d::CCRectZero;
        bool m_extMouseHovered = false;
        bool m_targetedScroll = true;
        bool m_swallowScroll = true;
        std::unordered_set<MouseEvent> m_extMouseDown = {};
        int m_targetPriority;

        ExtMouseDelegate();
        virtual ~ExtMouseDelegate();

        void attainCapture();
        void releaseCapture();
        bool isMouseDown(MouseEvent event) const;

        friend class ExtMouseDispatcher;
    
    public:
        virtual void mouseEnterExt(cocos2d::CCPoint const& pos);
        virtual void mouseLeaveExt(cocos2d::CCPoint const& pos);
        virtual bool mouseDownExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual bool mouseUpExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseDownOutsideExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseUpOutsideExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseMoveExt(cocos2d::CCPoint const& pos);

        void registerWithMouseDispatcher();
    };

    class GEODE_API_DLL ExtMouseDispatcher :
        public cocos2d::CCTouchDispatcher,
        public cocos2d::CCMouseDispatcher
    {
        API_INIT("com.geode.api");
    protected:
        cocos2d::CCTouchDispatcher* m_oldTouchDispatcher = nullptr;
        cocos2d::CCMouseDispatcher* m_oldMouseDispatcher = nullptr;
        cocos2d::CCPoint m_lastPosition;
        ExtMouseDelegate* m_capturing = nullptr;
        std::unordered_set<MouseEvent> m_pressedButtons;
        MouseEvent m_lastPressed;

        bool init();
        bool delegateIsHovered(ExtMouseDelegate* delegate, cocos2d::CCPoint const& pos);
        bool delegateIsHovered(cocos2d::CCMouseDelegate* delegate, cocos2d::CCPoint const& pos);
        int maxTargetPrio() const;

        void lockPopDelegate(ExtMouseDelegate* delegate);
        void unlockPopDelegate(ExtMouseDelegate* delegate);

        ExtMouseDispatcher();
        ~ExtMouseDispatcher();
    
    public:
        static ExtMouseDispatcher* get();
        void registerDispatcher();
        void unregisterDispatcher();

        void pushDelegate(ExtMouseDelegate* delegate);
        void popDelegate(ExtMouseDelegate* delegate);
        bool isPopLocked(ExtMouseDelegate* delegate);

        void attainCapture(ExtMouseDelegate* delegate);
        void releaseCapture(ExtMouseDelegate* delegate);
        bool isCapturing(ExtMouseDelegate* delegate) const;

        void update();

        void mouseDown(MouseEvent btn);
        void mouseUp(MouseEvent btn);
        bool isMouseDown(MouseEvent btn) const;

        void touches(
            cocos2d::CCSet* touches,
            cocos2d::CCEvent* event,
            unsigned int touchType
        );
        bool dispatchScrollMSG(float x, float y);

        static cocos2d::CCPoint getMousePosition();
    };
}

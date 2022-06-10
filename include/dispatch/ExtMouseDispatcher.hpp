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
        bool m_extMouseHovered = false;
        bool m_targetedScroll = true;
        bool m_swallowScroll = true;
        std::unordered_set<MouseEvent> m_extMouseDown = {};

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

        bool init();
        bool delegateIsHovered(ExtMouseDelegate* delegate, cocos2d::CCPoint const& pos);
        void handleTouchDelegates();

        ExtMouseDispatcher();
    
    public:
        /**
         * Get the shared ExtMouseDispatcher state
         */
        static ExtMouseDispatcher* get();
        /**
         * Replace CCTouchDispatcher & CCMouseDispatcher 
         * with ExtMouseDispatcher
         */
        void registerDispatcher();
        /**
         * Restore the original CCTouchDispatcher & 
         * CCMouseDispatcher 
         */
        void unregisterDispatcher();

        /**
         * Add a delegate
         * @param delegate Delegate to add
         */
        void pushDelegate(ExtMouseDelegate* delegate);
        /**
         * Remove a delegate
         * @param delegate Delegate to remove
         */
        void popDelegate(ExtMouseDelegate* delegate);

        /**
         * Get mouse capture for delegate (all mouse 
         * events will be sent to the delegate until
         * it releases capture)
         * @param delegate Delegate to attain capture for
         */
        void attainCapture(ExtMouseDelegate* delegate);
        /**
         * Release mouse capture from delegate
         * @param delegate Delegate to release capture from
         */
        void releaseCapture(ExtMouseDelegate* delegate);
        /**
         * Check if the given delegate is capturing 
         * the mouse
         * @param delegate Delegate to check
         * @returns True if the delegate has capture, 
         * false if not
         */
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
        // will not dispatch touch events - call touches manually 
        // if this returns false
        bool dispatchMouseEvent(MouseEvent event, bool down, cocos2d::CCPoint const& mousePos);
        bool dispatchScrollMSG(float x, float y);

        static cocos2d::CCPoint getMousePosition();
    };
}

#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include <unordered_set>


namespace geode {

    // remove
    enum class MouseEvent {
        Left = 0,
        Right = 1,
        Middle = 2,
        Prev = 3,
        Next = 4,
        DoubleClick = 8,
        ScrollUp,
        ScrollDown,
    };

    class ExtMouseManager;

    class GEODE_API_DLL ExtMouseDelegate {
    protected:
        cocos2d::CCRect m_extMouseHitArea = cocos2d::CCRectZero;
        bool m_extMouseHovered = false;
        std::unordered_set<MouseEvent> m_extMouseDown = {};
        int m_targetPriority;

        ExtMouseDelegate();
        virtual ~ExtMouseDelegate();

        void captureMouse();
        bool isMouseDown(MouseEvent event) const;

        friend class ExtMouseManager;
    
    public:
        virtual void mouseEnterExt(cocos2d::CCPoint const& pos);
        virtual void mouseLeaveExt(cocos2d::CCPoint const& pos);
        virtual bool mouseDownExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual bool mouseUpExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseDownOutsideExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseUpOutsideExt(MouseEvent button, cocos2d::CCPoint const& pos);
        virtual void mouseMoveExt(cocos2d::CCPoint const& pos);
        virtual bool mouseScrollExt(float y, float x);
        virtual void mouseScrollOutsideExt(float y, float x);
    };

    class GEODE_API_DLL ExtMouseManager {
    protected:
        std::vector<ExtMouseDelegate*> m_delegates;
        cocos2d::CCPoint m_lastPosition;
        ExtMouseDelegate* m_capturing = nullptr;
        std::unordered_set<MouseEvent> m_pressedButtons;

        bool init();
        bool delegateIsHovered(ExtMouseDelegate* delegate, cocos2d::CCPoint const& pos);
        int maxTargetPrio() const;

        ExtMouseManager();
        ~ExtMouseManager();
    
    public:
        static ExtMouseManager* get();

        void pushDelegate(ExtMouseDelegate* delegate);
        void popDelegate(ExtMouseDelegate* delegate);

        void captureMouse(ExtMouseDelegate* delegate);
        void releaseCapture(ExtMouseDelegate* delegate);
        bool isCapturing(ExtMouseDelegate* delegate) const;

        bool dispatchClickEvent(MouseEvent button, bool down, cocos2d::CCPoint const& pos);
        void dispatchMoveEvent(cocos2d::CCPoint const& pos);
        bool dispatchScrollEvent(float y, float x, cocos2d::CCPoint const& pos);

        bool isMouseDown(MouseEvent btn) const;

        static cocos2d::CCPoint getMousePosition();
    };
}

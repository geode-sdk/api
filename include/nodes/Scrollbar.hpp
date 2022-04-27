#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "../dispatch/ExtMouseManager.hpp"

namespace geode {
    class GEODE_API_DLL Scrollbar :
        public cocos2d::CCLayer,
        public ExtMouseDelegate
    {
    protected:
        CCScrollLayerExt* m_target = nullptr;
        cocos2d::extension::CCScale9Sprite* m_bg;
        cocos2d::extension::CCScale9Sprite* m_track;
        cocos2d::CCPoint m_clickOffset;
        float m_width;

        bool mouseDownExt(MouseEvent, cocos2d::CCPoint const&) override;
        bool mouseUpExt(MouseEvent, cocos2d::CCPoint const&) override;
        void mouseMoveExt(cocos2d::CCPoint const&) override;
        bool mouseScrollExt(float y, float x) override;
    
        void draw() override;

        bool init(CCScrollLayerExt*);

    public:
        void setTarget(CCScrollLayerExt* list);

        static Scrollbar* create(CCScrollLayerExt* list);
    };
}

#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    /**
     * CCContentLayer expects all of its children 
     * to be TableViewCells, which is not ideal for 
     * a generic content layer
     */
    class GEODE_API_DLL GenericContentLayer : public CCContentLayer {
    public:
        static GenericContentLayer* create(
            float width, float height
        );

        void setPosition(cocos2d::CCPoint const& pos) override;
    };

    class GEODE_API_DLL ScrollLayer : public CCScrollLayerExt {
    protected:
        bool m_scrollWheelEnabled;

        ScrollLayer(
            cocos2d::CCRect const& rect,
            bool scrollWheelEnabled,
            bool vertical
        );

    public:
        static ScrollLayer* create(
            cocos2d::CCRect const& rect,
            bool scrollWheelEnabled = true,
            bool vertical = true
        );
        static ScrollLayer* create(
            cocos2d::CCSize const& size,
            bool scrollWheelEnabled = true,
            bool vertical = true
        );

        void scrollWheel(float y, float) override;
        void enableScrollWheel(bool enable = true);
    };
}


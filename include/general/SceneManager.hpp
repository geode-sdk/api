#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    struct StayAcrossScenes {
        StayAcrossScenes();
        virtual ~StayAcrossScenes();
    };

    class SceneManager {
    protected:
        cocos2d::CCArray* m_persistedNodes;

        SceneManager* setup();

    public:
        static SceneManager* get();

        void keepAcrossScenes(cocos2d::CCNode* node);
        void forget(cocos2d::CCNode* node);

        void willSwitchToScene(cocos2d::CCScene* scene);
    };
}

#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    /**
     * Simple wrapper around CCTextInputNode that 
     * turns it into a CCMenuItem that can be used 
     * in a CCMenu. Can help with touch dispatcher 
     * issues
     */
    class GEODE_API_DLL MenuInputNode : public cocos2d::CCMenuItem {
    protected:
        CCTextInputNode* m_input;

        bool init(
            float width, float height, char const* placeholder, char const* fontPath
        );

    public:
        static MenuInputNode* create(
            float width, float height, char const* placeholder, char const* fontPath
        );

        void selected() override;

        CCTextInputNode* getInput() const;
    };
}


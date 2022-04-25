#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    class GEODE_API_DLL Input : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_bgSprite;
        CCTextInputNode* m_input;

        bool init(float, float, const char*, const char*, std::string const&, int);
        bool init(float, const char*, const char*, std::string const&, int);

    public:
        static Input* create(
            float width,
            const char* placeholder,
            const char* fontFile,
            std::string const& filter,
            int limit
        );
        static Input* create(
            float width,
            const char* placeholder,
            std::string const& filter,
            int limit
        );
        static Input* create(
            float width,
            const char* placeholder,
            std::string const& filter
        );
        static Input* create(
            float width,
            const char* placeholder,
            const char* fontFile
        );
        static Input* create(
            float width,
            const char* placeholder
        );

        CCTextInputNode* getInputNode() const;
        cocos2d::extension::CCScale9Sprite* getBGSprite() const;

        void setEnabled(bool);

        void setString(const char*);
        const char* getString();
    };
}



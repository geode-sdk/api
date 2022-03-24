#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include "FontRenderer.hpp"

namespace geode {
    class GEODE_API_DLL MDTextArea :
        public cocos2d::CCNode,
        public cocos2d::CCLabelProtocol
    {
    protected:
        std::string m_text;
        FontRenderer* m_renderer = nullptr;

        bool init(
            std::string const& str,
            cocos2d::CCSize const& size
        );

        virtual ~MDTextArea();
    
    public:
        static MDTextArea* create(
            std::string const& str,
            cocos2d::CCSize const& size
        );

        void updateLabel();

        void setString(const char* text) override;
        const char* getString() override;
    };
}

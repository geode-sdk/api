#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    class GEODE_API_DLL RichTextArea : public TextArea {
    protected:
        bool init(
            std::string const& str,
            const char* font,
            float width,
            float height,
            cocos2d::CCPoint const& anchor,
            float scale
        );
    
    public:
        static RichTextArea* create(
            std::string const& str,
            const char* font,
            float width,
            float height,
            cocos2d::CCPoint const& anchor,
            float scale
        );
    };
}

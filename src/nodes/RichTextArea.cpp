#include <nodes/RichTextArea.hpp>

USE_GEODE_NAMESPACE();

bool RichTextArea::init(
    std::string const& str,
    const char* font,
    float width,
    float height,
    CCPoint const& anchor,
    float scale
) {
    if (!TextArea::init(str, font, width, height, anchor, scale, false))
        return false;
    return true;
}

RichTextArea* RichTextArea::create(
    std::string const& str,
    const char* font,
    float width,
    float height,
    CCPoint const& anchor,
    float scale
) {
    auto ret = new RichTextArea;
    if (ret && ret->init(str, font, width, height, anchor, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

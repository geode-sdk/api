#include "hook.hpp"
#include <array>

class $modify(CustomLoadingLayer, LoadingLayer) {
    bool init(bool fromReload) {
        if (!LoadingLayer::init(fromReload))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto count = Loader::get()->getAllMods().size();
        auto unresolvedCount = Loader::get()->getFailedMods().size();

        // const char* text = "Loaded Geode";
        // commented out until someone figures out why this crashes
        const char* text = unresolvedCount ?
            CCString::createWithFormat(
                "Geode: Loaded %lu mods (%lu unresolved)",
                static_cast<unsigned long>(count),
                static_cast<unsigned long>(unresolvedCount)
            )->getCString() : 
            CCString::createWithFormat(
                "Geode: Loaded %lu mods",
                static_cast<unsigned long>(count)
            )->getCString();

        auto label = CCLabelBMFont::create(text, "goldFont.fnt");
        label->setPosition(winSize.width / 2, 30.f);
        label->setScale(.45f);
        label->setTag(5);
        this->addChild(label);

        return true;
    }
};

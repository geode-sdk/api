#pragma once

#include <Geode.hpp>
#include <nodes/ScrollLayer.hpp>

USE_GEODE_NAMESPACE();

class ModSettingsList : public CCLayer {
protected:
    Mod* m_mod;
    ScrollLayer* m_scrollLayer;

    bool init(Mod* mod, float width, float height);

public:
    static ModSettingsList* create(
        Mod* Mod, float width, float height
    );
};

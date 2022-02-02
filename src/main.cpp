#include <Geode>

USE_GEODE_NAMESPACE();

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
    Interface::get()->init(mod);
    return true;
}

GEODE_API void GEODE_CALL geode_unload() {}

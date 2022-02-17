#include "APIInternal.hpp"

APIInternal* APIInternal::get() {
    static auto inst = new APIInternal();
    return inst;
}

#define SAVE_KEY(name, var) \
    json[name] = this->var;

#define LOAD_KEY(name, var, type, typecheck)\
    if (json.contains(name)) {              \
        if (json[name].typecheck()) {       \
            this->var = json[name];         \
        } else {                            \
            return Err<>("\"" name "\" is not a " #type);\
        }                                   \
    }

Result<> APIInternal::save(nlohmann::json& json) {
    SAVE_KEY("shown-mod-enable-warning", m_shownEnableWarning);
    return Ok<>();
}

Result<> APIInternal::load(nlohmann::json const& json) {
    Interface::mod()->log() << __FUNCTION__;
    LOAD_KEY("shown-mod-enable-warning", m_shownEnableWarning, boolean, is_boolean);
    return Ok<>();
}

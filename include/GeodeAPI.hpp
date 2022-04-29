#pragma once

#include <Geode.hpp>
#include "settings/SettingNode.hpp"
#include "nodes/BasedButtonSprite.hpp"
#include "nodes/BasedButton.hpp"


#define EXPORT_NAME GeodeAPI
#include <loader/API.hpp>

namespace geode {
    class GEODE_API_DLL GeodeAPI : public ModAPI {
    public:
        API_INIT("com.geode.api");

        Result<> setCustomSettingNode(
            std::string const& key,
            CustomSettingNodeGenerator generator
        ) API_DECL(&GeodeAPI::setCustomSettingNode, key, generator);
    };
}

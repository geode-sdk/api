#pragma once

#pragma warning(disable: 4067)

#include <Geode.hpp>
#include <settings/CustomSettingNode.hpp>

USE_GEODE_NAMESPACE();

class SettingNodeManager {
protected:
    using CustomNodeMap = std::unordered_map<std::string, CustomSettingNodeGenerator>;

    std::unordered_map<Mod*, CustomNodeMap> m_customSettingNodes;

public:
    static SettingNodeManager* get();

    bool registerCustomNode(Mod* mod, std::string const& key, CustomSettingNodeGenerator generator);
	TableViewCell* generateNode(Mod* mod, Setting* setting, float width);
};

#include <settings/SettingNodeManager.hpp>
#include "GeodeSettingNode.hpp"

SettingNodeManager* SettingNodeManager::get() {
    static auto inst = new SettingNodeManager();
    return inst;
}

bool SettingNodeManager::registerCustomNode(
    Mod* mod, std::string const& key, CustomSettingNodeGenerator generator
) {
    this->m_customSettingNodes[mod][key] = generator;
    return true;
}

#define GEODE_GENERATE_SETTING_SWITCH_CREATE(_node_) \
	case SettingType::_node_: return _node_##SettingNode::create(dynamic_cast<_node_##Setting*>(setting), width);

SettingNode* SettingNodeManager::generateNode(Mod* mod, Setting* setting, float width) {
	switch (setting->getType()) {
		GEODE_GENERATE_SETTING_SWITCH_CREATE(Bool);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(Int);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(Float);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(String);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(Color);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(ColorAlpha);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(Path);
		GEODE_GENERATE_SETTING_SWITCH_CREATE(StringSelect);
		case SettingType::Custom: {
            if (
                this->m_customSettingNodes.count(mod) && 
                this->m_customSettingNodes[mod].count(setting->getKey())
            ) {
                return this->m_customSettingNodes[mod][setting->getKey()](setting, width);
            }
            return CustomSettingPlaceHolderNode::create(
                dynamic_cast<CustomSettingPlaceHolder*>(setting), mod->isLoaded(), width
            ); 
        }
	}
	return nullptr;
}

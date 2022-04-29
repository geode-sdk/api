#include <settings/SettingNode.hpp>
#include "ModSettingsList.hpp"

USE_GEODE_NAMESPACE();

void SettingNode::updateSettingsList() {
    if (m_list) m_list->updateList();
}

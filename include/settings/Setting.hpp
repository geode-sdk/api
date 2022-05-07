#pragma once

#include <Geode.hpp>
#include <unordered_map>

class ModSettingsList;

namespace geode {
    class SettingManager;

    class Setting {
     protected:
        std::string m_id;
        friend class SettingManager;
        inline Setting() {}
     public:
        virtual void loadJSON(nlohmann::json& jsn) = 0;
        virtual nlohmann::json saveJSON() = 0;

        virtual cocos2d::CCNode* createControl() = 0;
    };

    class SettingManager {
     protected:
        Mod* m_mod;
        std::unordered_map<std::string, Setting*> m_settings;

        std::vector<cocos2d::CCNode*> generateSettingNodes();
        void updateSetting(std::string id);
        SettingManager(Mod* m);
        friend class Setting;
        friend class ::ModSettingsList;
     public:
        static SettingManager* with(Mod* m);
        Setting* getSetting(std::string id);
        bool hasSettings();
        template <typename T>
        T* getSetting(std::string id) {
            return reinterpret_cast<T*>(getSetting(id));
        }
    };
}

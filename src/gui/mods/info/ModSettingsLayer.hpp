#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

class ModSettingsLayer : public FLAlertLayer {
    protected:
        Mod* m_mod;

		bool init(Mod* mod);

		void keyDown(enumKeyCodes) override;
		void onClose(CCObject*);
		
    public:
        static ModSettingsLayer* create(Mod* Mod);
};


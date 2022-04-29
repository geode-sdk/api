#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

class ModSettingsList;

namespace geode {
	#pragma warning(disable: 4275)
	class GEODE_API_DLL SettingNode : public TableViewCell {
	private:
		// private because this class is not 
		// part of api
		ModSettingsList* m_list = nullptr;

		friend class ModSettingsList;

	public:
		virtual void commitChanges() = 0;
        virtual bool hasUnsavedChanges() const = 0;
		void updateSettingsList();

		SettingNode(float width, float height) : TableViewCell("steve", width, height) {}
	};
	#pragma warning(default: 4275)

    // java
    using CustomSettingNodeGenerator = std::function<SettingNode*(Setting*, float)>;
}

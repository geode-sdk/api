#pragma once

#include <string>

namespace geode {
	struct EventProtocol {
		virtual bool accepts(EventProtocol* evt);
		virtual std::string eventID();
	};
}

#pragma once

#include <Geode.hpp>
#include "EventProtocol.hpp"
#include "EventAction.hpp"
#include <functional>

namespace geode {
	class EventCenter {
	 protected:
	 public:
	 	static EventCenter* get();

	 	void broadcast(EventProtocol* evt);
	 	EventAction registerAction(EventProtocol* evt, std::function<void(EventProtocol*)> f);
	 	void addAction(EventAction ea);
	 	void unregisterAction(EventAction ea);
	};
}
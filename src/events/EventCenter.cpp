#include <EventCenter.hpp>

USE_GEODE_NAMESPACE();

EventCenter* EventCenter::get() {
	static EventCenter* shared;
	if (!shared)
		shared = new EventCenter;
	return shared;
}

void EventCenter::broadcast(EventProtocol* evt) {
	for (auto& i : m_actions) {
		i.intercept(evt);
	}
}

EventAction& EventCenter::registerAction(EventProtocol* evt, std::function<void(EventProtocol*)> f) {
	auto hdl = Dispatcher::get()->addFunction(evt->eventID(), f);

	auto ea = EventAction(hdl, evt);
	if (vector_utils::contains(m_actions, ea)) {
		return m_actions[vector_utils::indexOf(m_actions, ea) - m_actions.begin()];
	} else {
		m_actions.push_back(ea);
		return m_actions.back();
	}
}

void EventCenter::addAction(EventAction ea) {
	if (!vector_utils::contains(m_actions, ea))
		m_actions.push_back(ea);
}

void EventCenter::unregisterAction(EventAction ea) {
	vector_utils::erase(m_actions, ea);
}

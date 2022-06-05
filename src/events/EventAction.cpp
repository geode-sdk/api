#include <EventAction.hpp>
#include <EventCenter.hpp>

USE_GEODE_NAMESPACE();

void EventAction::unregister() {
	EventCenter::get()->unregisterAction(*this);
}

void EventAction::intercept(EventProtocol* evt) {
	if (m_filter->accepts(evt)) {
		m_callback.into<void(EventProtocol*)>()(evt);
	}
}

bool EventAction::operator==(EventAction const& ea) const {
	return m_filter == ea.m_filter && m_callback == ea.m_callback;
}
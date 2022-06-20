#include <events/Event.hpp>

USE_GEODE_NAMESPACE();

std::vector<BasicEventHandler*> Event::handlers = {};

bool BasicEventHandler::addToHandlers() {
	if (!vector_utils::contains(Event::handlers, this)) {
		Event::handlers.push_back(this);
		return true;
	}
	return false;
}

void BasicEventHandler::removeFromHandlers() {
	vector_utils::erase(Event::handlers, this);
}

Event::Event(Mod* m) : m_owner(m) {}
Event::Event() : Event(Mod::get()) {}
Event::~Event() {}

void Event::post() {
	for (auto h : Event::handlers) {
		if (h->passThrough(this) == PassThrough::Stop) {
			break;
		}
	}
}


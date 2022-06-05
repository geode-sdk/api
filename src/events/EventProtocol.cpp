#include <Geode.hpp>
#include <EventProtocol.hpp>

USE_GEODE_NAMESPACE();

bool EventProtocol::accepts(EventProtocol* other) {
	return typeid(this) == typeid(other);
}
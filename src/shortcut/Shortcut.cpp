#include <Shortcut.hpp>

USE_GEODE_NAMESPACE();
using namespace api;


bool Shortcut::accepts(Shortcut const& sc) const {
	return sc.modifiers == this->modifiers && this->input == sc.input;
}
#include <Geode.hpp>

USE_GEODE_NAMESPACE();

namespace geode::api::notifications {
	constexpr ConstNotifInfo<ghc::filesystem::path> dragDrop(char const a[]) {
		char b[sizeof(a) + 9] = {"dragdrop.", a};
		return ConstNotifInfo<ghc::filesystem::path>(a);
	}
}
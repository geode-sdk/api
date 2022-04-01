#include <Geode.hpp>

USE_GEODE_NAMESPACE();

namespace geode::api::notifications {
	constexpr ConstNotifInfo<ghc::filesystem::path> dragDrop(char const a[]) {
		char b[sizeof(a) + 9] = {"dragdrop."};

		for (int i = 0; i < sizeof(a); ++i) {
			b[9 + i] = a[i];
		}

		b[sizeof(a) + 9] = '\0';

		return ConstNotifInfo<ghc::filesystem::path>(b);
	}
}
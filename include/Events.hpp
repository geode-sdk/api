#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

namespace geode {

	template <size_t T>
	inline constexpr ConstEventInfo<ghc::filesystem::path> dragDrop(char const a[T]) {
		char b[T + 9] = {"dragdrop."};

		for (int i = 0; i < T; ++i) {
			b[9 + i] = a[i];
		}

		b[T + 9] = '\0';

		return ConstEventInfo<ghc::filesystem::path>(b);
	}
}

#pragma once

#include <Geode.hpp>
#include <settings/Setting.hpp>

USE_GEODE_NAMESPACE();

namespace geode::events {

	template <size_t T>
	inline constexpr ConstEventInfo<ghc::filesystem::path> dragDrop(char const a[T]) {
		char b[T + 9] = {"dragdrop."};

		for (int i = 0; i < T; ++i) {
			b[9 + i] = a[i];
		}

		b[T + 9] = '\0';

		return ConstEventInfo<ghc::filesystem::path>(b);
	}

	template <size_t T>
	inline constexpr ConstEventInfo<Setting**> getSetting(char const a[T]) {
		char b[T + 8] = {"setting-"};

		for (int i = 0; i < T; ++i) {
			b[8 + i] = a[i];
		}

		b[T + 8] = '\0';

		return ConstEventInfo<Setting**>(b);
	}

	inline EventInfo<Setting**> getSetting(std::string a) {
		return EventInfo<Setting**>(std::string("settings-") + a);
	}
}

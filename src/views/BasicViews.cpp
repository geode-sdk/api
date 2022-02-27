#include <views/BasicViews.hpp>

USE_GEODE_NAMESPACE();


bool Row::format() {
	auto count = m_items.size();
	if (count == 0) return true;

	auto totalWidth = m_padding * (count - 1);
	for (auto& item : m_items) {
		totalWidth += item.getWidth();
	}

	auto x = -totalWidth / 2.0f;
	for (auto& item : m_items) {
		item.setX(x + item.getWidth() / 2.0f);
		x += item.getWidth() + m_padding;
	}
	return true;
}

bool Column::format() {
	auto count = m_items.size();
	if (count == 0) return true;

	auto totalHeight = m_padding * (count - 1);
	for (auto& item : m_items) {
		totalHeight += item.getHeight();
	}

	auto y = totalHeight / 2.0f;
	for (auto& item : m_items) {
		item.setY(y + item.getHeight() / 2.0f);
		y -= item.getHeight() + m_padding;
	}
	return true;
}

bool Grid::format() {
	return true;
}

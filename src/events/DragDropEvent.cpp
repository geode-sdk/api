#include <DragDropEvent.hpp>

USE_GEODE_NAMESPACE();

DragDropEvent::DragDropEvent(ghc::filesystem::path path) : m_path(path) {}

bool DragDropHandler::handle(DragDropEvent* ev) {
	auto ext = ev->path().extension().string();
	if (ext.size() > 0)
		ext = ext.substr(1);

	if (m_extensions.size() == 0 || vector_utils::contains(m_extensions, ext))
		return m_callback(ev);

	return true;
}

DragDropHandler::DragDropHandler(
    std::vector<std::string> extensions,
    std::function<bool(DragDropEvent*)> callback
) : EventHandler(callback) {
	for (auto s : extensions) {
		if (s[0] == '.') {
			m_extensions.push_back(s.substr(1));
		} else {
			m_extensions.push_back(s);
		}
	}
}

DragDropHandler::DragDropHandler(
    std::string extension,
    std::function<bool(DragDropEvent*)> callback
) : DragDropHandler(std::vector<std::string>{extension}, callback) {}

DragDropHandler::DragDropHandler(
    std::function<bool(DragDropEvent*)> callback
) : DragDropHandler(std::vector<std::string>(), callback) {}

/*DragDropEvent::DragDropEvent(ghc::filesystem::path const& url) : Event(), m_draggedURL(url) {}

bool DragDropEvent::passThrough(DragDropHandler const& handler, DragDropFilter const& filters) {
	auto mut_filters = filters;

	for (auto& s: mut_filters) {
		if (s[0] == '.')
			s.remove_prefix(1);
	}

	std::string_view ext = m_draggedURL.extension().string();
	if (ext.size() > 0)
		ext.remove_prefix(1);

	if (filters.size() == 0 || vector_utils::contains(mut_filters, ext))
		return handler(m_draggedURL);

	return true;
}

bool DragDropEvent::filtersMatchExtension(std::string_view extension) {
	geode::log << extension;
	for (auto& f : DragDropEvent::getHandlers()) {
		geode::log << f.second[0];
		if (vector_utils::contains(f.second, extension))
			return true;
	}
	return false;
}*/
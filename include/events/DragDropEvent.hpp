#pragma once

#include <vector>
#include <string>
#include <Geode.hpp>
#include <Event.hpp>

namespace geode {

    using DragDropHandler = std::function<bool(ghc::filesystem::path const&)>;
    using DragDropFilter = std::vector<std::string_view>;

    class DragDropEvent : public Event<DragDropEvent, DragDropHandler, DragDropFilter> {
        ghc::filesystem::path m_draggedURL;

     public:

        DragDropEvent() = delete;
        DragDropEvent(ghc::filesystem::path const& url);

        bool passThrough(DragDropHandler const& f, DragDropFilter const& h) override;

        static bool filtersMatchExtension(std::string_view extension);
    };

}
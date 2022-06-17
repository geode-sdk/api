#pragma once

#include <vector>
#include <string>
#include <Geode.hpp>
#include <Event.hpp>

namespace geode {

    class DragDropEvent : public Event {
     protected:
        ghc::filesystem::path m_path;
     public:
        DragDropEvent(ghc::filesystem::path path);

        inline ghc::filesystem::path const& path() { return m_path; }
    };

    class DragDropHandler : public EventHandler<DragDropEvent>{
     protected:
        std::vector<std::string> m_extensions;
        
     public:
        bool handle(DragDropEvent* ev);

        DragDropHandler(
            std::vector<std::string> extensions,
            std::function<bool(DragDropEvent*)> callback
        );
        DragDropHandler(
            std::string extension,
            std::function<bool(DragDropEvent*)> callback
        );
        DragDropHandler(
            std::function<bool(DragDropEvent*)> callback
        );

        ~DragDropHandler();
    };


    /*using DragDropHandler = std::function<bool(ghc::filesystem::path const&)>;
    using DragDropFilter = std::vector<std::string_view>;

    class DragDropEvent : public Event<DragDropEvent, DragDropHandler, DragDropFilter> {
        ghc::filesystem::path m_draggedURL;

     public:

        DragDropEvent() = delete;
        DragDropEvent(ghc::filesystem::path const& url);

        bool passThrough(DragDropHandler const& f, DragDropFilter const& h) override;

        static bool filtersMatchExtension(std::string_view extension);
    };*/

}
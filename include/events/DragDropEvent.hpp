#pragma once

#include <vector>
#include <string>
#include <Geode.hpp>
#include "Event.hpp"

#include "../APIMacros.hpp"

namespace geode {

    class GEODE_API_DLL DragDropEvent : public Event {
    protected:
        ghc::filesystem::path m_path;
    public:
        DragDropEvent(ghc::filesystem::path path);

        inline ghc::filesystem::path const& path() { return m_path; }
    };

    class GEODE_API_DLL DragDropHandler : public EventHandler<DragDropEvent> {
    protected:
        std::vector<std::string> m_extensions;
        DragDropHandler(std::function<PassThrough(DragDropEvent*)> callback);

    public:
        bool wouldAccept(std::string ext);
        PassThrough handle(DragDropEvent* ev);

        static DragDropHandler* create(
            std::vector<std::string> const& extensions,
            std::function<PassThrough(DragDropEvent*)> callback
        );
        static DragDropHandler* create(
            std::string const& extension,
            std::function<PassThrough(DragDropEvent*)> callback
        );
        static DragDropHandler* create(
            std::function<PassThrough(DragDropEvent*)> callback
        );

        virtual ~DragDropHandler();
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

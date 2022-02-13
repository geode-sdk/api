#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode {
    struct Path {
        std::string absolute_path;
        std::string file_name;
        std::string extension;
    };

    struct GEODE_API_DLL DragDropDelegate {
        Mod* owner;
        std::function<bool(Path)> handler;

        DragDropDelegate(Mod*, decltype(handler));
        bool handleDrop(Path file);
    };
}
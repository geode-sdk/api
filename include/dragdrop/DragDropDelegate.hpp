#include <Geode.hpp>
#include "../APIMacros.hpp"
#include <fs/filesystem.hpp>

namespace geode {
    struct GEODE_API_DLL DragDropDelegate {
        Mod* owner;
        std::function<bool(ghc::filesystem::path)> handler;

        DragDropDelegate(Mod*, decltype(handler));
        bool handleDrop(ghc::filesystem::path file);
    };
}
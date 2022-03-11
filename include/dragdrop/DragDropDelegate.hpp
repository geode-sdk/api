#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"
#include <fs/filesystem.hpp>

namespace geode {
    class GEODE_API_DLL DragDropDelegate {
    public:
        using Handler = std::function<bool(ghc::filesystem::path const&)>;

    protected:
        Mod* m_owner;
        Handler m_handler;

    public:
        DragDropDelegate(Mod* owner, Handler handler);
        bool handleDrop(ghc::filesystem::path const& file);
    };
}
#include <Geode.hpp>
#include "DragDropDelegate.hpp"
#include "../APIMacros.hpp"

namespace geode {
    class GEODE_API_DLL DragDropManager {
    protected:
        std::unordered_map<std::string, DragDropDelegate*> m_extensiondelegates;
        std::unordered_map<std::string, std::vector<std::string>> m_extensions;
        
        std::unordered_map<std::string, DragDropDelegate*> m_delegates;

        DragDropManager();
        virtual ~DragDropManager();
    public:
        static DragDropManager* get();

        void dispatchEvent(std::string path);
        
        bool addDropHandler(Mod* owner, std::string handler_id, std::function<bool(ghc::filesystem::path)> handler);
        bool addDropHandler(Mod* owner, std::string handler_id, std::function<bool(ghc::filesystem::path)> handler, std::string extension);
    
        bool removeDropHandler(std::string handler_id);
    };

    
}
#include <Geode.hpp>
#include <DragDropManager.hpp>

USE_GEODE_NAMESPACE();

DragDropManager::DragDropManager() {};
DragDropManager::~DragDropManager() {};

DragDropManager* DragDropManager::get() {
    static auto g_manager = new DragDropManager;
    return g_manager;
}

void DragDropManager::dispatchEvent(std::string path) {
    Interface::mod()->logInfo("Handling drag+drop for " + path, Severity::Info);

    ghc::filesystem::path p(path.c_str());

    std::string fileExtension = p.extension().u8string();
    if (fileExtension.at(0) == '.') {
        fileExtension = fileExtension.substr(1);
    }

    for (std::string handler_id : m_extensions[fileExtension]) {
        if (m_extensiondelegates[handler_id]->handleDrop(p)) return;
    }

    for (auto i : m_delegates) {
        if (i.second->handleDrop(p)) return;
    }

    FLAlertLayer::create("Error", "No manager registered for <cr>." + fileExtension + "</c> files!", "OK")->show();
}

bool DragDropManager::addDropHandler(Mod* owner, std::string handler_id, std::function<bool(ghc::filesystem::path)> handler) {
    if (m_delegates.find(handler_id) != m_delegates.end() || m_extensiondelegates.find(handler_id) != m_extensiondelegates.end()) return false;
    m_delegates[handler_id] = new DragDropDelegate(owner, handler);
    return true;
}

bool DragDropManager::addDropHandler(Mod* owner, std::string handler_id, std::function<bool(ghc::filesystem::path)> handler, std::string extension) {
    if (m_extensiondelegates.find(handler_id) != m_extensiondelegates.end() || m_delegates.find(handler_id) != m_delegates.end()) return false;
    m_extensiondelegates[handler_id] = new DragDropDelegate(owner, handler);
    
    int index = extension.find_last_of(".");
    if (index == extension.size() - 1) return false;

    std::string cleaned = index == std::string::npos ? extension : extension.substr(index + 1);

    m_extensions[cleaned].push_back(handler_id);
    return true;
}

bool DragDropManager::removeDropHandler(std::string handler_id) {
    for (auto i : m_extensions) {
        i.second.erase(std::remove(i.second.begin(), i.second.end(), handler_id), i.second.end());
    }
    return m_delegates.erase(handler_id) || m_extensiondelegates.erase(handler_id);
}

DragDropDelegate::DragDropDelegate(Mod* owner, decltype(handler) handler) {
    this->owner = owner;
    this->handler = handler;
}

bool DragDropDelegate::handleDrop(ghc::filesystem::path file) {
    return this->owner->isEnabled() && this->handler(file);
}

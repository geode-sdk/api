#pragma once

#include <Geode.hpp>
#include <mutex>
#include <events/Event.hpp>

USE_GEODE_NAMESPACE();

class Index;
struct ModInstallUpdate;
class InstallTicket;

// todo: make index use events

struct IndexItem {
    struct Download {
        std::string m_url;
        std::string m_filename;
        std::string m_hash;
    };

    ghc::filesystem::path m_path;
    ModInfo m_info;
    std::unordered_map<PlatformID, Download> m_download;
    std::string m_installFailed;
};

enum class UpdateStatus {
    Progress,
    Failed,
    Finished,
};

using IndexUpdateCallback = std::function<void(UpdateStatus, std::string const&, uint8_t)>;

using SEL_ModInstallProgress = void(CCObject::*)(
    InstallTicket*, UpdateStatus, std::string const&, uint8_t
);
#define modinstallprogress_selector(_SELECTOR)\
    (SEL_ModInstallProgress)(&_SELECTOR)

class InstallTicket : public CCObject {
protected:
    CCObject* m_target;
    SEL_ModInstallProgress m_progress;
    std::vector<std::string> m_installList;
    mutable std::mutex m_cancelMutex;
    bool m_cancelling = false;
    bool m_replaceFiles = true;
    size_t m_installIndex = 0;
    Index* m_index;

    void postProgress(
        UpdateStatus status,
        std::string const& info = "",
        uint8_t progress = 0
    );
    void installNext();

    bool init(
        Index* index,
        std::vector<std::string> const& list,
        CCObject* target,
        SEL_ModInstallProgress progress
    );
    
    friend class Index;

public:
    static InstallTicket* create(
        Index* index,
        std::vector<std::string> const& list,
        CCObject* target,
        SEL_ModInstallProgress progress
    );

    std::vector<std::string> getInstallList() const;
    void cancel();
    void start();
};

class Index : public CCObject {
protected:
    bool m_upToDate = false;
    bool m_updating = false;
    mutable std::mutex m_callbacksMutex;
    std::vector<IndexUpdateCallback> m_callbacks;
    std::vector<IndexItem> m_items;

    void indexUpdateProgress(
        UpdateStatus status,
        std::string const& info = "",
        uint8_t percentage = 0
    );

    void updateIndexFromLocalCache();

    Result<std::vector<std::string>> checkDependenciesForItem(IndexItem const& item);

public:
    static Index* get();

    std::vector<IndexItem> const& getItems() const;
    std::vector<IndexItem> getUninstalledItems() const;
    bool isKnownItem(std::string const& id) const;
    IndexItem getKnownItem(std::string const& id) const;
    Result<InstallTicket*> installItem(
        IndexItem const& item,
        CCObject* target = nullptr,
        SEL_ModInstallProgress progress = nullptr
    );
    bool isUpdateAvailableForItem(std::string const& id) const;
    bool areUpdatesAvailable() const;

    bool isIndexUpdated() const;
    void updateIndex(IndexUpdateCallback callback, bool force = false);
};

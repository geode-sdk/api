#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

struct IndexDelegate;
struct ModInstallUpdate;

struct IndexItem {
    ghc::filesystem::path m_path;
    ModInfo m_info;
    struct {
        std::unordered_set<PlatformID> m_platforms;
        std::string m_url;
        std::string m_filename;
        uint32_t m_checksum;
    } m_download;
    std::string m_installFailed;
};

class Index : public CCObject {
protected:
    bool m_upToDate = false;
    bool m_updating = false;
    std::string m_indexUpdateFailed = "";
    std::vector<IndexDelegate*> m_delegates;
    std::vector<IndexItem> m_items;

    void indexUpdateProgress(CCObject* info);
    void indexUpdateFailed(CCObject* info);
    void indexUpdateFinished();

    void modInstallProgress(CCObject* info);
    void modInstallFailed(CCObject* info);
    void modInstallFinished();

    void postMSG(SEL_CallFunc func);
    void postMSG(SEL_CallFuncO func, std::string const& info);
    void postMSG(SEL_CallFuncO func, ModInstallUpdate* info);

    void updateIndexFromLocalCache();

public:
    static Index* get();

    uint32_t calculateChecksum(ghc::filesystem::path const& path);

    std::vector<IndexItem> const& getItems() const;
    std::vector<IndexItem> getUninstalledItems() const;
    bool isKnownItem(std::string const& id) const;
    IndexItem getKnownItem(std::string const& id) const;
    void installItem(std::string const& id);

    bool isIndexUpdated() const;
    std::string indexUpdateFailed() const;
    void updateIndex(bool force = false);

    void pushDelegate(IndexDelegate*);
    void popDelegate(IndexDelegate*);
};

struct IndexDelegate {
    virtual void indexUpdateProgress(std::string const& info);
    virtual void indexUpdateFailed(std::string const& info);
    virtual void indexUpdateFinished();

    virtual void modInstallProgress(std::string const& info, uint8_t percentage);
    virtual void modInstallFailed(std::string const& info);
    virtual void modInstallFinished();

    IndexDelegate();
    virtual ~IndexDelegate();
};

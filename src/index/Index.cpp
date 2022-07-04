#include "Index.hpp"
#include <thread>
#include <utils/json.hpp>
#include "fetch.hpp"

#define GITHUB_DONT_RATE_LIMIT_ME_PLS 1

Result<nlohmann::json> readJSON(ghc::filesystem::path const& path) {
    auto indexJsonData = file_utils::readString(path);
    if (!indexJsonData) {
        return Err("Unable to read " + path.string());
    }
    try {
        return Ok(nlohmann::json::parse(indexJsonData.value()));
    } catch(std::exception& e) {
        return Err("Error parsing JSON: " + std::string(e.what()));
    }
}


Index* Index::get() {
    static auto ret = new Index();
    return ret;
}

bool Index::isIndexUpdated() const {
    return m_upToDate;
}

std::string Index::indexUpdateFailed() const {
    return m_indexUpdateFailed;
}


void Index::indexUpdateProgress(
    UpdateStatus status,
    std::string const& info,
    uint8_t percentage
) {
    Loader::get()->queueInGDThread([this, status, info, percentage]() -> void {
        for (auto& d : m_delegates) {
            d->indexUpdateProgress(status, info, percentage);
        }
    });
}

void Index::updateIndex(bool force) {
    if (m_upToDate || m_updating) return;
    m_updating = true;

    auto indexDir = Loader::get()->getGeodeDirectory() / "index";
    ghc::filesystem::create_directories(indexDir);
    std::thread updateThread([force, this, indexDir]() -> void {
        #if GITHUB_DONT_RATE_LIMIT_ME_PLS == 0

        indexUpdateProgress(UpdateStatus::Progress, "Fetching index metadata", 0);

        auto commit = fetchJSON("https://api.github.com/repos/geode-sdk/mods/commits");
        if (!commit) {
            return indexUpdateProgress(UpdateStatus::Failed, commit.error());
        }
        auto json = commit.value();
        if (json.is_object() && json.contains("documentation_url") && json.contains("message")) {
            // whoops! got rate limited
            return indexUpdateProgress(
                UpdateStatus::Failed,
                json["message"].get<std::string>()
            );
        }

        indexUpdateProgress(UpdateStatus::Progress, "Checking index status", 25);

        if (!json.is_array()) {
            return indexUpdateProgress(
                UpdateStatus::Failed,
                "Fetched commits, expected 'array', got '" + std::string(json.type_name()) + "'. "
                "Report this bug to the Geode developers!"
            );
        }

        if (!json.at(0).is_object()) {
            return indexUpdateProgress(
                UpdateStatus::Failed,
                "Fetched commits, expected 'array.object', got 'array." + std::string(json.type_name()) + "'. "
                "Report this bug to the Geode developers!"
            );
        }

        if (!json.at(0).contains("sha")) {
            return indexUpdateProgress(
                UpdateStatus::Failed,
                "Fetched commits, missing '0.sha'. "
                "Report this bug to the Geode developers!"
            );
        }

        auto upcomingCommitSHA = json.at(0)["sha"];

        std::string currentCommitSHA = "";
        if (ghc::filesystem::exists(indexDir / "current")) {
            auto data = file_utils::readString(indexDir / "current");
            if (data) {
                currentCommitSHA = data.value();
            }
        }

        if (force || currentCommitSHA != upcomingCommitSHA) {
            file_utils::writeString(indexDir / "current", upcomingCommitSHA);

            indexUpdateProgress(
                UpdateStatus::Progress,
                "Downloading index",
                50
            );
            auto gotZip = fetchFile(
                "https://github.com/geode-sdk/mods/zipball/main",
                indexDir / "index.zip"
            );
            if (!gotZip) {
                return indexUpdateProgress(
                    UpdateStatus::Failed,
                    gotZip.error()
                );
            }

            // delete old index
            if (ghc::filesystem::exists(indexDir / "index")) {
                ghc::filesystem::remove_all(indexDir / "index");
            }

            // unzip downloaded
            auto unzip = ZipFile((indexDir / "index.zip").string());
            if (!unzip.isLoaded()) {
                return indexUpdateProgress(
                    UpdateStatus::Failed,
                    "Unable to unzip index.zip"
                );
            }

            for (auto file : unzip.getAllFiles()) {
                // this is a very bad check for seeing 
                // if file is a directory. it seems to 
                // work on windows at least. idk why 
                // getAllFiles returns the directories 
                // aswell now
                if (
                    string_utils::endsWith(file, "\\") ||
                    string_utils::endsWith(file, "/")
                ) continue;

                auto zipPath = file;

                // dont include the github repo folder
                file = file.substr(file.find_first_of("/") + 1);

                auto path = ghc::filesystem::path(file);
                if (path.has_parent_path()) {
                    if (
                        !ghc::filesystem::exists(indexDir / path.parent_path()) &&
                        !ghc::filesystem::create_directories(indexDir / path.parent_path())
                    ) {
                        return indexUpdateProgress(
                            UpdateStatus::Failed,
                            "Unable to create directories \"" + path.parent_path().string() + "\""
                        );
                    }
                }
                unsigned long size;
                auto data = unzip.getFileData(zipPath, &size);
                if (!data || !size) {
                    return indexUpdateProgress(
                        UpdateStatus::Failed,
                        "Unable to read \"" + std::string(zipPath) + "\""
                    );
                }
                auto wrt = file_utils::writeBinary(
                    indexDir / file,
                    byte_array(data, data + size)
                );
                if (!wrt) {
                    return indexUpdateProgress(
                        UpdateStatus::Failed,
                        "Unable to write \"" + file + "\": " + wrt.error()
                    );
                }
            }
        }
        #endif

        indexUpdateProgress(
            UpdateStatus::Progress,
            "Updating index",
            75
        );
        this->updateIndexFromLocalCache();

        m_upToDate = true;
        indexUpdateProgress(
            UpdateStatus::Finished,
            "",
            100
        );
    });
    updateThread.detach();
}

PlatformID platformFromString(std::string const& str) {
    switch (hash(string_utils::trim(string_utils::toLower(str)).c_str())) {
        default:
        case hash("unknown"): return PlatformID::Unknown;
        case hash("windows"): return PlatformID::Windows;
        case hash("macos"): return PlatformID::MacOS;
        case hash("ios"): return PlatformID::iOS;
        case hash("android"): return PlatformID::Android;
        case hash("linux"): return PlatformID::Linux;
    }
}

void Index::updateIndexFromLocalCache() {
    m_items.clear();
    auto indexDir = Loader::get()->getGeodeDirectory() / "index" / "index";
    for (auto const& dir : ghc::filesystem::directory_iterator(indexDir)) {
        if (ghc::filesystem::is_directory(dir)) {
            if (ghc::filesystem::exists(dir / "index.json")) {

                auto readJson = readJSON(dir / "index.json");
                if (!readJson) {
                    Log::get() << Severity::Warning
                        << "Error reading index.json: "
                        << readJson.error() << ", skipping";
                    continue;
                }
                auto json = readJson.value();
                if (!json.is_object()) {
                    Log::get() << Severity::Warning
                        << "[index.json] is not an object, skipping";
                    continue;
                }

                auto readModJson = readJSON(dir / "mod.json");
                if (!readModJson) {
                    Log::get() << Severity::Warning
                        << "Error reading mod.json: "
                        << readModJson.error() << ", skipping";
                    continue;
                }
                auto info = ModInfo::create(readModJson.value());
                if (!info) {
                    Log::get() << Severity::Warning
                        << info.error() << ", skipping";
                    continue;
                }

                IndexItem item;

                item.m_path = dir.path();
                item.m_info = info.value();

                if (json.contains("download") && json["download"].is_object()) {
                    auto download = json["download"];
                    std::set<PlatformID> unsetPlatforms = {
                        PlatformID::Windows,
                        PlatformID::MacOS,
                        PlatformID::iOS,
                        PlatformID::Android,
                        PlatformID::Linux,
                    };
                    for (auto& key : std::initializer_list<std::string> {
                        "windows",
                        "macos",
                        "android",
                        "ios",
                        "*",
                    }) {
                        if (download.contains(key)) {
                            auto platformDownload = download[key];
                            if (!platformDownload.is_object()) {
                                Log::get() << Severity::Warning
                                    << "[index.json].download." + key + " is not an object, skipping";
                                continue;
                            }
                            if (!platformDownload.contains("url") || !platformDownload["url"].is_string()) {
                                Log::get() << Severity::Warning
                                    << "[index.json].download." + key + ".url is not a string, skipping";
                                continue;
                            }
                            if (!platformDownload.contains("name") || !platformDownload["name"].is_string()) {
                                Log::get() << Severity::Warning
                                    << "[index.json].download." + key + ".name is not a string, skipping";
                                continue;
                            }
                            if (!platformDownload.contains("hash") || !platformDownload["hash"].is_string()) {
                                Log::get() << Severity::Warning
                                    << "[index.json].download." + key + ".hash is not a string, skipping";
                                continue;
                            }
                            IndexItem::Download down;
                            down.m_url = platformDownload["url"].get<std::string>();
                            down.m_filename = platformDownload["name"].get<std::string>();
                            down.m_hash = platformDownload["hash"].get<std::string>();
                            if (key == "*") {
                                for (auto& platform : unsetPlatforms) {
                                    item.m_download[platform] = down;
                                }
                            } else {
                                auto id = platformFromString(key);
                                item.m_download[id] = down;
                                unsetPlatforms.erase(id);
                            }
                        }
                    }
                } else {
                    Log::get() << Severity::Warning
                        << "[index.json] is missing \"download\", adding anyway";
                }

                m_items.push_back(item);

            } else {
                Log::get() << Severity::Warning << "Index directory "
                    << dir << " is missing index.json, skipping";
            }
        }
    }
}

std::vector<IndexItem> const& Index::getItems() const {
    return m_items;
}

std::vector<IndexItem> Index::getUninstalledItems() const {
    std::vector<IndexItem> items;
    for (auto& item : m_items) {
        if (!Loader::get()->isModInstalled(item.m_info.m_id)) {
            if (item.m_download.count(GEODE_PLATFORM_TARGET)) {
                items.push_back(item);
            }
        }
    }
    return items;
}

bool Index::isKnownItem(std::string const& id) const {
    for (auto& item : m_items) {
        if (item.m_info.m_id == id) return true;
    }
    return false;
}

IndexItem Index::getKnownItem(std::string const& id) const {
    for (auto& item : m_items) {
        if (item.m_info.m_id == id) {
            return item;
        }
    }
    return IndexItem();
}

struct UninstalledDependency {
    std::string m_id;
    bool m_isInIndex;
};

static void getUninstalledDependenciesRecursive(
    ModInfo const& info,
    std::vector<UninstalledDependency>& deps
) {
    for (auto& dep : info.m_dependencies) {
        UninstalledDependency d;
        d.m_isInIndex = Index::get()->isKnownItem(dep.m_id);
        if (!Loader::get()->isModInstalled(dep.m_id)) {
            d.m_id = dep.m_id;
            deps.push_back(d);
        }
        if (d.m_isInIndex) {
            getUninstalledDependenciesRecursive(
                Index::get()->getKnownItem(dep.m_id).m_info,
                deps
            );
        }
    }
}

Result<std::vector<std::string>> Index::checkDependenciesForItem(IndexItem const& item) {
    // todo: check versions
    std::vector<UninstalledDependency> deps;
    getUninstalledDependenciesRecursive(item.m_info, deps);
    if (deps.size()) {
        std::vector<std::string> unknownDeps;
        for (auto& dep : deps) {
            if (!dep.m_isInIndex) {
                unknownDeps.push_back(dep.m_id);
            }
        }
        if (unknownDeps.size()) {
            std::string list = "";
            for (auto& ud : unknownDeps) {
                list += "<cp>" + ud + "</c>, ";
            }
            list.pop_back();
            list.pop_back();
            return Err(
                "This mod or its dependencies <cb>depends</c> on the following "
                "unknown mods: " + list + ". You will have to manually "
                "install these mods before you can install this one."
            );
        }
        std::vector<std::string> list = {};
        for (auto& d : deps) {
            list.push_back(d.m_id);
        }
        list.push_back(item.m_info.m_id);
        return Ok(list);
    } else {
        return Ok<std::vector<std::string>>({ item.m_info.m_id });
    }
}

Result<InstallTicket*> Index::installItem(
    IndexItem const& item,
    CCObject* target,
    SEL_ModInstallProgress progress
) {
    if (!item.m_download.count(GEODE_PLATFORM_TARGET)) {
        return Err(
            "This mod is not available on your "
            "current platform \"" GEODE_PLATFORM_NAME "\" - Sorry! :("
        );
    }
    auto download = item.m_download.at(GEODE_PLATFORM_TARGET);
    if (!download.m_url.size()) {
        return Err(
            "Download URL not set! Report this bug to "
            "the Geode developers - this should not happen, ever."
        );
    }
    if (!download.m_filename.size()) {
        return Err(
            "Download filename not set! Report this bug to "
            "the Geode developers - this should not happen, ever."
        );
    }
    if (!download.m_hash.size()) {
        return Err(
            "Checksum not set! Report this bug to "
            "the Geode developers - this should not happen, ever."
        );
    }
    auto list = checkDependenciesForItem(item);
    if (!list) {
        return Err(list.error());
    }
    auto ticket = InstallTicket::create(this, list.value(), target, progress);
    CC_SAFE_RETAIN(ticket);
    return Ok(ticket);
}

bool Index::isUpdateAvailableForItem(std::string const& id) const {
    return true;
    if (!Loader::get()->isModInstalled(id)) {
        return false;
    }
    if (!this->isKnownItem(id)) {
        return false;
    }
    return
        this->getKnownItem(id).m_info.m_version > 
        Loader::get()->getInstalledMod(id)->getVersion();
}

void Index::pushDelegate(IndexDelegate* delegate) {
    m_delegates.push_back(delegate);
}

void Index::popDelegate(IndexDelegate* delegate) {
    vector_utils::erase(m_delegates, delegate);
}

void IndexDelegate::indexUpdateProgress(
    UpdateStatus, std::string const&, uint8_t
) {}

IndexDelegate::IndexDelegate() {
    Index::get()->pushDelegate(this);
}

IndexDelegate::~IndexDelegate() {
    Index::get()->popDelegate(this);
}

#include "Index.hpp"
#include <curl/curl.h>
#include <thread>
#include <json.hpp>
#include <checksum.hpp>

#define GITHUB_DONT_RATE_LIMIT_ME_PLS 0

using FetchProgFunc = void(*)(double, double, void*);
struct FetchProgInfo {
    FetchProgFunc m_func = nullptr;
    void* m_userdata = nullptr;
};

namespace fetch_utils {
    static size_t writeData(char* data, size_t size, size_t nmemb, void* str) {
        as<std::string*>(str)->append(data, size * nmemb);
        return size * nmemb;
    }

    static size_t writeBinaryData(char* data, size_t size, size_t nmemb, void* file) {
        as<std::ofstream*>(file)->write(data, size * nmemb);
        return size * nmemb;
    }

    static int progress(void* ptr, double total, double now, double, double) {
        auto info = as<FetchProgInfo*>(ptr);
        info->m_func(now, total, info->m_userdata);
        return 0;
    }
}

Result<> fetchFile(
    std::string const& url,
    ghc::filesystem::path const& into,
    FetchProgInfo prog = FetchProgInfo()
) {
    auto curl = curl_easy_init();
    
    if (!curl) return Err("Curl not initialized!");

    std::ofstream file(into, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        return Err("Unable to open output file");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_utils::writeBinaryData);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "github_api/1.0");
    if (prog.m_func) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, fetch_utils::progress);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
    }
    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return Err("Fetch failed");
    }

    char* ct;
    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    if ((res == CURLE_OK) && ct) {
        curl_easy_cleanup(curl);
        return Ok();
    }
    curl_easy_cleanup(curl);
    return Err("Error getting info: " + std::string(curl_easy_strerror(res)));
}

Result<std::string> fetch(std::string const& url) {
    auto curl = curl_easy_init();
    
    if (!curl) return Err("Curl not initialized!");

    std::string ret;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_utils::writeData);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "github_api/1.0");
    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return Err("Fetch failed");
    }

    char* ct;
    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    if ((res == CURLE_OK) && ct) {
        curl_easy_cleanup(curl);
        return Ok(ret);
    }
    curl_easy_cleanup(curl);
    return Err("Error getting info: " + std::string(curl_easy_strerror(res)));
}

Result<nlohmann::json> fetchJSON(std::string const& url) {
    auto res = fetch(url);
    if (!res) return Err(res.error());
    try {
        return Ok(nlohmann::json::parse(res.value()));
    } catch(std::exception& e) {
        return Err(e.what());
    }
}

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


struct ModInstallUpdate : public CCObject {
    IndexItem* m_item = nullptr;
    std::string m_info = "";
    uint8_t m_percentage = 0;

    ModInstallUpdate(IndexItem* item, std::string const& info, uint8_t p = 0)
     : m_item(item), m_info(info), m_percentage(p) {
        this->autorelease();
    }
    ModInstallUpdate() {
        this->autorelease();
    }
};


uint32_t Index::calculateChecksum(ghc::filesystem::path const& path) {
    return ::calculateChecksum(path.string());
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

void Index::indexUpdateProgress(CCObject* info) {
    m_indexUpdateFailed.clear();
    for (auto& d : m_delegates) {
        d->indexUpdateProgress(as<CCString*>(info)->getCString());
    }
}

void Index::indexUpdateFailed(CCObject* info) {
    m_updating = false;
    m_indexUpdateFailed = as<CCString*>(info)->getCString();
    for (auto& d : m_delegates) {
        d->indexUpdateFailed(m_indexUpdateFailed);
    }
}

void Index::indexUpdateFinished() {
    m_updating = false;
    m_indexUpdateFailed.clear();
    for (auto& d : m_delegates) {
        d->indexUpdateFinished();
    }
}

void Index::modInstallProgress(CCObject* info) {
    auto obj = as<ModInstallUpdate*>(info);
    for (auto& d : m_delegates) {
        d->modInstallProgress(obj->m_info, obj->m_percentage);
    }
}

void Index::modInstallFailed(CCObject* info) {
    for (auto& d : m_delegates) {
        d->modInstallFailed(as<ModInstallUpdate*>(info)->m_info);
    }
}

void Index::modInstallFinished() {
    for (auto& d : m_delegates) {
        d->modInstallFinished();
    }
}

void Index::postMSG(SEL_CallFunc func) {
    CCDirector::sharedDirector()->getActionManager()->addAction(
        CCCallFunc::create(this, func),
        CCNode::create(), false
    );
}

void Index::postMSG(SEL_CallFuncO func, std::string const& info) {
    CCDirector::sharedDirector()->getActionManager()->addAction(
        CCCallFuncO::create(this, func, CCString::create(info)),
        CCNode::create(), false
    );
}

void Index::postMSG(SEL_CallFuncO func, ModInstallUpdate* info) {
    CCDirector::sharedDirector()->getActionManager()->addAction(
        CCCallFuncO::create(this, func, info),
        CCNode::create(), false
    );
}

#define POST_IU_PROG(msg) this->postMSG(callfuncO_selector(Index::indexUpdateProgress), msg)
#define POST_MI_PROG(msg, prog) \
    {\
        auto info = new ModInstallUpdate(&item, msg, prog);\
        this->postMSG(callfuncO_selector(Index::modInstallProgress), info);\
    }


void Index::updateIndex(bool force) {
    if (m_upToDate || m_updating) return;
    m_updating = true;

    auto indexDir = Loader::get()->getGeodeDirectory() / "index";
    ghc::filesystem::create_directories(indexDir);
    std::thread updateThread([force, this, indexDir]() -> void {
        #if GITHUB_DONT_RATE_LIMIT_ME_PLS == 0
        POST_IU_PROG("Fetching index metadata");
        auto commit = fetchJSON("https://api.github.com/repos/geode-sdk/mods/commits");
        if (!commit) {
            return this->postMSG(callfuncO_selector(Index::indexUpdateFailed), commit.error());
        }
        auto json = commit.value();
        if (json.is_object() && json.contains("documentation_url") && json.contains("message")) {
            // whoops! got rate limited
            return this->postMSG(callfuncO_selector(Index::indexUpdateFailed), json["message"].get<std::string>());
        }

        POST_IU_PROG("Checking index status");

        if (!json.is_array()) {
            return this->postMSG(
                callfuncO_selector(Index::indexUpdateFailed),
                "Fetched commits, expected 'array', got '" + std::string(json.type_name()) + "'. "
                "Report this bug to the Geode developers!"
            );
        }

        if (!json.at(0).is_object()) {
            return this->postMSG(
                callfuncO_selector(Index::indexUpdateFailed),
                "Fetched commits, expected 'array.object', got 'array." + std::string(json.type_name()) + "'. "
                "Report this bug to the Geode developers!"
            );
        }

        if (!json.at(0).contains("sha")) {
            return this->postMSG(
                callfuncO_selector(Index::indexUpdateFailed),
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

            POST_IU_PROG("Downloading index");
            auto gotZip = fetchFile(
                "https://github.com/geode-sdk/mods/zipball/main",
                indexDir / "index.zip"
            );
            if (!gotZip) {
                return this->postMSG(callfuncO_selector(Index::indexUpdateFailed), gotZip.error());
            }

            // delete old index
            if (ghc::filesystem::exists(indexDir / "index")) {
                ghc::filesystem::remove_all(indexDir / "index");
            }

            // unzip downloaded
            auto unzip = ZipFile((indexDir / "index.zip").string());
            if (!unzip.isLoaded()) {
                return this->postMSG(
                    callfuncO_selector(Index::indexUpdateFailed),
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
                        return this->postMSG(
                            callfuncO_selector(Index::indexUpdateFailed),
                            "Unable to create directories \"" + path.parent_path().string() + "\""
                        );
                    }
                }
                unsigned long size;
                auto data = unzip.getFileData(zipPath, &size);
                if (!data || !size) {
                    return this->postMSG(
                        callfuncO_selector(Index::indexUpdateFailed),
                        "Unable to read \"" + std::string(zipPath) + "\""
                    );
                }
                auto wrt = file_utils::writeBinary(
                    indexDir / file,
                    byte_array(data, data + size)
                );
                if (!wrt) {
                    return this->postMSG(
                        callfuncO_selector(Index::indexUpdateFailed),
                        "Unable to write \"" + file + "\": " + wrt.error()
                    );
                }
            }
        }
        #endif

        POST_IU_PROG("Updating index");
        this->updateIndexFromLocalCache();

        m_upToDate = true;
        this->postMSG(callfunc_selector(Index::indexUpdateFinished));
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
                auto info = Loader::get()->parseModJson(dir.path().string(), readModJson.value());
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
                    if (!download.contains("platforms") || !download["platforms"].is_array()) {
                        Log::get() << Severity::Warning
                            << "[index.json].download.platforms is not an array, skipping";
                        continue;
                    }
                    if (!download.contains("url") || !download["url"].is_string()) {
                        Log::get() << Severity::Warning
                            << "[index.json].download.url is not a string, skipping";
                        continue;
                    }
                    if (!download.contains("name") || !download["name"].is_string()) {
                        Log::get() << Severity::Warning
                            << "[index.json].download.name is not a string, skipping";
                        continue;
                    }
                    if (!download.contains("checksum") || !download["checksum"].is_number_unsigned()) {
                        Log::get() << Severity::Warning
                            << "[index.json].download.checksum is not an integer, skipping";
                        continue;
                    }
                    item.m_download.m_url = download["url"].get<std::string>();
                    item.m_download.m_filename = download["name"].get<std::string>();
                    item.m_download.m_checksum = download["checksum"].get<uint32_t>();
                    for (auto& platform : download["platforms"]) {
                        if (!platform.is_string()) {
                            Log::get() << Severity::Warning
                                << "[index.json].download.platforms contains a non-string, skipping";
                            goto skip_this;
                        }
                        auto str = platform.get<std::string>();
                        auto id = platformFromString(str);
                        if (id != PlatformID::Unknown) {
                            item.m_download.m_platforms.insert(id);
                        } else {
                            Log::get() << Severity::Warning
                                << "[index.json].download.platforms contains an unknown platform \""
                                << str << "\", skipping";
                            goto skip_this;
                        }
                    }
                } else {
                    Log::get() << Severity::Warning
                        << "[index.json] is missing \"download\", adding anyway";
                }

                m_items.push_back(item);

            skip_this: continue;

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
            if (item.m_download.m_platforms.count(GEODE_PLATFORM_TARGET)) {
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

void Index::installItem(std::string const& id) {
    if (!this->isKnownItem(id)) {
        auto info = new ModInstallUpdate();
        info->m_info = "Unknown item \"" + id + "\"";
        return this->indexUpdateFailed(info);
    }
    auto indexDir = Loader::get()->getGeodeDirectory() / "index" / "index";
    std::thread t([indexDir, this, id]() -> void {
        auto item = this->getKnownItem(id);

        POST_MI_PROG("Checking status", 0);
        if (!item.m_download.m_platforms.count(GEODE_PLATFORM_TARGET)) {
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "This mod is not available on your "
                "current platform \"" GEODE_PLATFORM_NAME "\" - Sorry! :("
            ));
        }
        if (!item.m_download.m_url.size()) {
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Download URL not set! Report this bug to "
                "the Geode developers - this should not happen, ever."
            ));
        }
        if (!item.m_download.m_filename.size()) {
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Download filename not set! Report this bug to "
                "the Geode developers - this should not happen, ever."
            ));
        }
        if (!item.m_download.m_checksum) {
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Checksum not set! Report this bug to "
                "the Geode developers - this should not happen, ever."
            ));
        }

        struct Data {
            IndexItem* m_item;
            Index* m_self;
        } data = { &item, this };

        auto tempFile = indexDir / item.m_download.m_filename;

        POST_MI_PROG("Fetching binary", 0);
        auto res = fetchFile(
            item.m_download.m_url,
            tempFile,
            { [](double now, double total, void* self) -> void {
                auto data = as<Data*>(self);
                data->m_self->postMSG(callfuncO_selector(Index::modInstallProgress), new ModInstallUpdate(
                    data->m_item, "Downloading binary",
                    static_cast<uint8_t>(now / total * 100.0)
                ));
            }, &data }
        );
        if (!res) {
            ghc::filesystem::remove(tempFile);
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Downloading failed: " + res.error()
            ));
        }

        auto notFound = file_utils::readString(tempFile);
        if (notFound && notFound.value() == "Not Found") {
            ghc::filesystem::remove(tempFile);
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Binary file download returned \"Not found\". Report "
                "this to the Geode development team."
            ));
        }

        POST_MI_PROG("Verifying", 100);
        auto checksum = this->calculateChecksum(tempFile);

        if (checksum != item.m_download.m_checksum) {
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Checksum mismatch! (Downloaded file did not match what "
                "was expected. Try again, and if the download fails another time, "
                "report this to the Geode development team."
            ));
        }

        if (!ghc::filesystem::copy_file(
            tempFile,
            Loader::get()->getGeodeDirectory() / "mods" / item.m_download.m_filename,
            ghc::filesystem::copy_options::overwrite_existing
        )) {
            ghc::filesystem::remove(tempFile);
            return this->postMSG(callfuncO_selector(Index::modInstallFailed), new ModInstallUpdate(
                &item, "Unable to copy downloaded file to mods directory! ("
                "might be due to insufficient permissions to write files under "
                "SteamLibrary, try running GD as administrator)"
            ));
        }

        ghc::filesystem::remove(tempFile);
        Loader::get()->refreshMods();

        this->postMSG(callfunc_selector(Index::modInstallFinished));
    });
    t.detach();
}

void Index::pushDelegate(IndexDelegate* delegate) {
    m_delegates.push_back(delegate);
}

void Index::popDelegate(IndexDelegate* delegate) {
    vector_utils::erase(m_delegates, delegate);
}


void IndexDelegate::indexUpdateFailed(std::string const&) {}
void IndexDelegate::indexUpdateProgress(std::string const&) {}
void IndexDelegate::indexUpdateFinished() {}

void IndexDelegate::modInstallProgress(std::string const& info, uint8_t percentage) {}
void IndexDelegate::modInstallFailed(std::string const& info) {}
void IndexDelegate::modInstallFinished() {}

IndexDelegate::IndexDelegate() {
    Index::get()->pushDelegate(this);
}

IndexDelegate::~IndexDelegate() {
    Index::get()->popDelegate(this);
}

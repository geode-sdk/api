#include "Index.hpp"
#include <thread>
#include <utils/json.hpp>
#include <hash.hpp>
#include "fetch.hpp"

void InstallTicket::postProgress(
    UpdateStatus status,
    std::string const& info,
    uint8_t percentage
) {
    if (m_target && m_progress) {
        Loader::get()->queueInGDThread([this, status, info, percentage]() -> void {
            (m_target->*m_progress)(this, status, info, percentage);
        });
    }
    if (status == UpdateStatus::Failed || status == UpdateStatus::Finished) {
        Loader::get()->queueInGDThread([this]() -> void {
            this->release();
        });
    }
}

bool InstallTicket::init(
    Index* index,
    std::vector<std::string> const& list,
    CCObject* target,
    SEL_ModInstallProgress progress
) {
    m_index = index;
    m_installList = list;
    m_target = target;
    m_progress = progress;
    return true;
}

InstallTicket* InstallTicket::create(
    Index* index,
    std::vector<std::string> const& list,
    CCObject* target,
    SEL_ModInstallProgress progress
) {
    auto ret = new InstallTicket();
    if (ret && ret->init(index, list, target, progress)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::vector<std::string> InstallTicket::getInstallList() const {
    return m_installList;
}

void InstallTicket::installNext() {
    // make sure we have stuff to install
    if (!m_installList.size()) {
        return this->postProgress(UpdateStatus::Failed, "Nothing to install", 0);
    }
    // run installing in another thread in order 
    // to render progress on screen while installing
    std::thread t([this]() -> void {
        auto indexDir = Loader::get()->getGeodeDirectory() / "index";

        auto id = m_installList.at(m_installIndex);
        auto item = m_index->getKnownItem(id);
        auto currentProgress = static_cast<uint8_t>(
            100.0 * (m_installIndex + 1) / m_installList.size()
        );

        auto download = item.m_download.at(GEODE_PLATFORM_TARGET);

        this->postProgress(UpdateStatus::Progress, "Checking status", currentProgress);
        
        // download to temp file in index dir
        auto tempFile = indexDir / download.m_filename;

        this->postProgress(UpdateStatus::Progress, "Fetching binary", currentProgress);
        auto res = fetchFile(
            download.m_url,
            tempFile,
            [this, currentProgress](double now, double total) -> int {
                // check if cancelled
                m_cancelMutex.lock();
                if (m_cancelling) {
                    m_cancelMutex.unlock();
                    return 1;
                }
                m_cancelMutex.unlock();

                this->postProgress(
                    UpdateStatus::Progress,
                    "Downloading binary",
                    static_cast<uint8_t>(now / total * currentProgress)
                );
                return 0;
            }
        );
        if (!res) {
            ghc::filesystem::remove(tempFile);
            return this->postProgress(
                UpdateStatus::Failed,
                "Downloading failed: " + res.error()
            );
        }

        // check if cancelled
        m_cancelMutex.lock();
        if (m_cancelling) {
            ghc::filesystem::remove(tempFile);
            m_cancelMutex.unlock();
            return;
        }
        m_cancelMutex.unlock();

        // check for 404
        auto notFound = file_utils::readString(tempFile);
        if (notFound && notFound.value() == "Not Found") {
            ghc::filesystem::remove(tempFile);
            return this->postProgress(
                UpdateStatus::Failed,
                "Binary file download returned \"Not found\". Report "
                "this to the Geode development team."
            );
        }

        // verify checksum
        this->postProgress(UpdateStatus::Progress, "Verifying", currentProgress);
        auto checksum = ::calculateHash(tempFile.string());

        if (checksum != download.m_hash) {
            return this->postProgress(
                UpdateStatus::Failed,
                "Checksum mismatch! (Downloaded file did not match what "
                "was expected. Try again, and if the download fails another time, "
                "report this to the Geode development team."
            );
        }

        // move temp file to geode directory
        try {
            auto modDir = Loader::get()->getGeodeDirectory() / "mods";
            auto targetFile = modDir / download.m_filename;

            // find valid filename that doesn't exist yet
            if (!m_replaceFiles) {
                auto filename = ghc::filesystem::path(
                    download.m_filename
                ).replace_extension("").string();

                size_t number = 0;
                while (ghc::filesystem::exists(targetFile)) {
                    targetFile = modDir / (filename + std::to_string(number) + ".geode");
                    number++;
                }
            }

            // move file
            ghc::filesystem::rename(tempFile, targetFile);
        } catch(std::exception& e) {
            ghc::filesystem::remove(tempFile);
            return this->postProgress(
                UpdateStatus::Failed,
                "Unable to move downloaded file to mods directory: \"" + 
                std::string(e.what()) + " \" "
                "(This might be due to insufficient permissions to "
                "write files under SteamLibrary, try running GD as "
                "administrator)"
            );
        }

        // call next in queue or post finish message
        Loader::get()->queueInGDThread([this]() -> void {
            m_installIndex++;
            if (m_installIndex < m_installList.size()) {
                this->installNext();
            } else {
                Loader::get()->refreshMods();
                if (m_target && m_progress) {
                    (m_target->*m_progress)(
                        this, UpdateStatus::Finished, "", 100
                    );
                }
            }
        });
    });
    t.detach();
}

void InstallTicket::cancel() {
    m_cancelMutex.lock();
    m_cancelling = true;
    m_cancelMutex.unlock();
}

void InstallTicket::start() {
    this->installNext();
}

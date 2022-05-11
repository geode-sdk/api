#pragma once

USE_GEODE_NAMESPACE();

struct APIInternal {
    bool m_shownEnableWarning = false;

    static APIInternal* get();

    Result<> save(nlohmann::json& json);
    Result<> load(nlohmann::json const& json);
};

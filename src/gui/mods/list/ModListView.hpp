#pragma once

#include <Geode.hpp>

USE_GEODE_NAMESPACE();

static constexpr const BoomListType kBoomListType_Mod
    = static_cast<BoomListType>(0x350);

enum class ModListType {
	Installed,
	Download,
	Featured,
};

// Wrapper so you can pass Mods in a CCArray
struct ModObject : public CCObject {
    Mod* m_mod = nullptr;
    Loader::FailedModInfo m_info;
    inline ModObject(Mod* mod) : m_mod(mod) {
        this->autorelease();
    };
    inline ModObject(Loader::FailedModInfo info) : m_info(info) {
        this->autorelease();
    };
};

class ModListView;
class ModCell : public TableViewCell {
protected:
    ModListView* m_list;
    Mod* m_mod;
    ModObject* m_obj;
    CCMenuItemToggler* m_enableToggle = nullptr;
    CCMenuItemSpriteExtra* m_unresolvedExMark;

    ModCell(const char* name, CCSize size);

    void draw() override;
    void onInfo(CCObject*);
    void onFailedInfo(CCObject*);
    void onEnable(CCObject*);
    void onUnresolvedInfo(CCObject*);

    bool init(ModListView* list);

public:
    void updateBGColor(int index);
    void loadFromMod(ModObject*);
    void loadFromFailureInfo(Loader::FailedModInfo info);
    void updateState(bool invert = false);

    static ModCell* create(ModListView* list, const char* key, CCSize size);
};

class ModListView : public CustomListView {
public:
    // this is not enum class so | works
    enum SearchFlags {
        Name        = 0b1,
        ID          = 0b10,
        Developer   = 0b100,
        Credits     = 0b1000,
        Description = 0b10000,
        Details     = 0b100000,
    };
    static constexpr int s_allFlags =
        SearchFlags::Name |
        SearchFlags::ID |
        SearchFlags::Developer |
        SearchFlags::Credits |
        SearchFlags::Description |
        SearchFlags::Details;

protected:
    enum class Status {
        OK,
        Unknown,
        NoModsFound,
        SearchEmpty,
    };

    Status m_status = Status::OK;

    void setupList() override;
    TableViewCell* getListCell(const char* key) override;
    void loadCell(TableViewCell* cell, unsigned int index) override;

    bool init(
        CCArray* mods,
        ModListType type,
        float width,
        float height,
        const char* searchFilter,
        int searchFlags
    );
    bool filter(Mod* mod, const char* searchFilter, int searchFlags);

public:
    static ModListView* create(
        CCArray* mods,
        ModListType type = ModListType::Installed,
        float width = 358.f,
        float height = 220.f,
        const char* searchFilter = nullptr,
        int searchFlags = 0
    );
    static ModListView* create(
        ModListType type,
        float width = 358.f,
        float height = 220.f,
        const char* searchFilter = nullptr,
        int searchFlags = 0
    );

    void updateAllStates(ModCell* toggled = nullptr);

    Status getStatus() const;
    std::string getStatusAsString() const;
};

#pragma once

#include <Geode.hpp>
#include "../APIMacros.hpp"

namespace geode::view {
	class GEODE_API_DLL ViewItem {
	public:
		std::string_view m_id;
		cocos2d::CCNode* m_node;

		ViewItem(std::string_view id, cocos2d::CCNode* node);

		float getWidth() const;
		float getHeight() const;
		float getWorldX() const;
		float getWorldY() const;
		float getX() const;
		float getY() const;
		void setX(float x);
		void setY(float y);

		bool operator==(ViewItem const& other) const;
	};

	class GEODE_API_DLL ViewImpl : public cocos2d::CCObject {
    protected:
    	std::vector<ViewItem> m_items;

    public:
    	virtual bool init();
    	// virtual bool initWithItems(cocos2d::CCDictionary*);
    	// virtual bool initWithItems(std::unordered_map<std::string, cocos2d::CCNode*> const&);

    	virtual bool format() = 0;

    	virtual void addItem(std::string_view id, cocos2d::CCNode* node);
    	virtual void removeItem(std::string_view id);
    	virtual void removeItem(cocos2d::CCNode* node);
    };


	template <typename Subclass>
    class View : public ViewImpl {
    public:
    	static Subclass* create() {
    		auto ret = new Subclass();
    		if (ret && ret->init()) {
		        ret->autorelease();
		        return ret;
		    }
		    CC_SAFE_DELETE(ret);
		    return nullptr;
    	}

    	// static Subclass* createWithItems(cocos2d::CCDictionary* dictionary) {
    	// 	auto ret = new Subclass();
    	// 	if (ret && ret->initWithItems(dictionary)) {
		   //      ret->autorelease();
		   //      return ret;
		   //  }
		   //  CC_SAFE_DELETE(ret);
		   //  return nullptr;
    	// }

    	// static Subclass* createWithItems(std::unordered_map<std::string_view, cocos2d::CCNode*> const& map) {
    	// 	auto ret = new Subclass();
    	// 	if (ret && ret->initWithItems(map)) {
		   //      ret->autorelease();
		   //      return ret;
		   //  }
		   //  CC_SAFE_DELETE(ret);
		   //  return nullptr;
    	// }
    };
}

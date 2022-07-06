#pragma once

#include <Geode.hpp>
#include <type_traits>

#include "../APIMacros.hpp"

namespace geode {

	class Event;
	struct GEODE_API_DLL BasicEventHandler {
		virtual bool passThrough(Event*) = 0;

		void addToHandlers();
		void removeFromHandlers();

	};

	class GEODE_API_DLL Event {
		static std::vector<BasicEventHandler*> handlers;
	 	friend BasicEventHandler;
	 public:
	 	void post();
	 	static std::vector<BasicEventHandler*> const& getHandlers();

	 	virtual ~Event();
	};

	template <typename T>
	class EventHandler : public BasicEventHandler {
	 protected:
	 	std::function<bool(T*)> m_callback;
	 public:
		virtual bool handle(T*) = 0;
		bool passThrough(Event* ev) override {
			if (auto myev = dynamic_cast<T*>(ev)) {
				return handle(myev);
			}
			return true;
		}

		EventHandler(std::function<bool(T*)> cb) : m_callback(cb) {}
		EventHandler() {}
	};
}

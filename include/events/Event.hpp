#pragma once

#include <Geode.hpp>
#include <type_traits>

#include "../APIMacros.hpp"

namespace geode {
	class Event;

	enum class PassThrough : bool {
		Propagate,
		Stop,
	};

	struct GEODE_API_DLL BasicEventHandler {
		virtual PassThrough passThrough(Event*) = 0;

		bool addToHandlers();
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
	 	std::function<PassThrough(T*)> m_callback;

	public:
		virtual PassThrough handle(T*) = 0;
		PassThrough passThrough(Event* ev) override {
			if (auto myev = dynamic_cast<T*>(ev)) {
				return handle(myev);
			}
			return PassThrough::Propagate;
		}

		EventHandler(std::function<PassThrough(T*)> cb) : m_callback(cb) {}
	};
}

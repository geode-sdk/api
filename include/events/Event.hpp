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
		
	protected:
	 	Mod* m_owner;
	 	friend BasicEventHandler;

	public:
	 	void post();

	 	Event();
	 	Event(Mod*);
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

	/*template <class D, typename H, typename F = std::monostate>
	class Event {
	 private:
	 	Mod* m_owner;

	 	Event(Mod* m) : m_owner(m) {}
	 	Event() : Event(Mod::get()) {}

	 	virtual bool passThrough(H const& f, F const& h) = 0;

	 	static std::vector<std::pair<H, F>>& getHandlers() {
	 		static std::vector<std::pair<H, F>> handlers;
	 		return handlers;
	 	}

	 	friend D;
	 public:
	 	Mod* getOwner() { return m_owner; }

	 	static H* addHandler(F const& f, H const& h) {
	 		D::getHandlers().push_back({h, f});
	 		return &D::getHandlers().back().first;
	 	}

	 	static void removeHandler(H* h) {
	 		vector_utils::erase(D::handlers, [h](auto& t) {
	 			return &t.first == h; 
	 		});
	 	}

	 	void post() {
	 		for (auto& h : D::getHandlers()) {
	 			if (!passThrough(h.first, h.second))
	 				break;
	 		}
	 	}
	};*/
}

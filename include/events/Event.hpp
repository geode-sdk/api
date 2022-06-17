#pragma once

#include <Geode.hpp>
#include <type_traits>
namespace geode {

	class Event;
	struct BasicEventHandler {
		virtual bool passThrough(Event*) = 0;

		void addToHandlers();
		void removeFromHandlers();

	};

	class Event {
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
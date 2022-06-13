#pragma once

#include <Geode.hpp>
#include <type_traits>
namespace geode {

	template <class D, typename H, typename F = std::monostate>
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
	};
}
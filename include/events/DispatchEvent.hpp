#pragma once

#include <vector>
#include <string>
#include <Geode.hpp>
#include <Event.hpp>
#include <optional>

template <typename T>
struct no_void {
	using type = T;
};

template <>
struct no_void<void> {
	using type = std::monostate;
};

namespace geode {
	template <typename T>
	class DispatchHandler;

	template <typename T>
	class DispatchEvent : public Event {
		DispatchEvent() = delete;
	};

	template <typename R, typename ...Args>
	class DispatchEvent<R(Args...)> : public Event {
		using ret_type = std::optional<typename no_void<R>::type>;

		std::string m_name;
		std::tuple<Args...> m_args;
		ret_type m_return;
		bool m_passThroughAll;
	 public:
	 	DispatchEvent(std::string name, Args... args) : m_name(name), m_args(std::make_tuple(args...)), m_passThroughAll(false) {}
	 	DispatchEvent(bool throughAll, std::string name, Args... args) : m_name(name), m_args(std::make_tuple(args...)), m_passThroughAll(throughAll) {}


	 	ret_type const& returnVal() {
	 		return m_return;
	 	}

	 	std::string const& name() {
	 		return m_name;
	 	}

	 	ret_type operator()() {
	 		post();
	 		return m_return;
	 	}

	 	friend class DispatchHandler<R(Args...)>;
	};

	template <typename R, typename ...Args>
	std::optional<typename no_void<R>::type> invokeDispatchEvent(std::string name, Args... args) {
		return DispatchEvent<R(Args...)>(name, args...)();
	}

	template <typename T>
	class DispatchHandler : public EventHandler<DispatchEvent<T>> {
		std::string m_name;
		std::function<T> m_callback;
		DispatchHandler(std::string name, std::function<T> callback) : m_name(name), m_callback(callback) {}
	 public:
	 	bool handle(DispatchEvent<T>* ev) {
	 		if (ev->name() == m_name) {
	 			ev->m_return = std::apply(m_callback, ev->m_args);
	 			return ev->m_passThroughAll;
	 		}
	 		return true;
	 	}

	 	static DispatchHandler* create(std::string name, std::function<T> callback) {
	 		return new DispatchHandler(name, callback);
	 	}
	};
}

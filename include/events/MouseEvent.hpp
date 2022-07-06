#pragma once

#include <vector>
#include <string>
#include <Geode.hpp>
#include <Event.hpp>
#include <optional>
#include <chrono>

#include "../APIMacros.hpp"

namespace geode {
	struct GEODE_API_DLL MouseButtons {
		enum {
			NoButton      = 0b00000000,

			ButtonLeft    = 0b00000001,
			ButtonRight   = 0b00000010,
			ButtonMiddle  = 0b00000100,
			Button4       = 0b00001000,
			Button5       = 0b00010000,

			AnyButton     = 0b11111111
		};

		unsigned char inner;

		using Type = decltype(NoButton);

		inline operator Type() { return static_cast<Type>(inner); }
		inline MouseButtons(Type t) : inner(static_cast<unsigned char>(t)) {}
		inline MouseButtons() : inner(0) {}
		inline bool operator==(MouseButtons const& btn) { return inner == btn.inner; }
		inline bool operator<(MouseButtons const& btn) { return inner < btn.inner; } // for keys

	 	inline MouseButtons operator|(MouseButtons const& btn) const { return static_cast<Type>(inner | btn.inner); }
	 	inline MouseButtons operator&(MouseButtons const& btn) const { return static_cast<Type>(inner & btn.inner); }
	 	inline MouseButtons operator^(MouseButtons const& btn) const { return static_cast<Type>(inner ^ btn.inner); }
	 	inline MouseButtons operator-(MouseButtons const& btn) const { return static_cast<Type>(inner - btn.inner); }

	 	inline MouseButtons operator|(Type const& btn) const { return static_cast<Type>(inner | static_cast<unsigned char>(btn)); }
	 	inline MouseButtons operator&(Type const& btn) const { return static_cast<Type>(inner & static_cast<unsigned char>(btn)); }
	 	inline MouseButtons operator^(Type const& btn) const { return static_cast<Type>(inner ^ static_cast<unsigned char>(btn)); }
	 	inline MouseButtons operator-(Type const& btn) const { return static_cast<Type>(inner - static_cast<unsigned char>(btn)); }

	 	template <Type Btn>
	 	static MouseButtons combine() {
	 		return Btn;
	 	}

	 	template <Type Btn, Type ...Args>
	 	static MouseButtons combine() {
	 		return Btn | MouseButtons::combine<Args...>();
	 	}

	 	std::vector<MouseButtons> split();
	};

	struct MouseState {
		cocos2d::CCPoint position;
		MouseButtons buttons;
		float scrollX;
		float scrollY;

	 	static MouseState const& GEODE_API_DLL globalState();

	 	// for API hooks only, cannot use in other mods!
	 	static MouseState& globalStateMut();
	 private:
	 	static MouseState* shared;
	};

	class GEODE_API_DLL MouseEvent : public Event {
	 protected:
		MouseState m_newState;
		MouseEvent();
	 public:
	 	static MouseEvent* create(MouseState const& newState);
	 	inline MouseState const& mouseState() { return m_newState; }

	 	cocos2d::CCPoint distanceMoved();
	 	bool justPressedButton(MouseButtons btn);
	 	bool justReleasedButton(MouseButtons btn);
	 	cocos2d::CCPoint distanceScrolled();

	 	void post();
	};

	using MouseCallback = std::optional<std::function<void(MouseEvent*)>>;

	struct MouseHandlerBtnInfo {
	 	MouseCallback drag;
	 	MouseCallback hold;
	 	MouseCallback release;
	 	std::map<unsigned int, MouseCallback> clicks; // i promise its map for a reason

	 	std::chrono::time_point<std::chrono::high_resolution_clock> timeOfFire;
	 	unsigned int clickCount;
	 	bool holdOn;
	};

	class GEODE_API_DLL MouseHandler : public EventHandler<MouseEvent> {
	 protected:
	 	MouseCallback m_allFn;
	 	MouseCallback m_moveFn;
	 	MouseCallback m_scrollFn;

	 	float m_holdDelay = 0.2f;
	 	float m_multiClickDelay = 0.2f;

	 	std::unordered_map<MouseButtons::Type, MouseHandlerBtnInfo> m_btnFns;

	 	MouseHandler();

	 public:
		static MouseHandler* create();

		void setHoldDelay(float delay);
		void setMultiClickDelay(float delay);

		void setGeneralCallback(MouseCallback const& cb);
		void setMoveCallback(MouseCallback const& cb);
		void setScrollCallback(MouseCallback const& cb);

		template <MouseButtons::Type Button>
		void setMultiClickCallback(uint64_t clickCount, MouseCallback const& cb) {
			if (clickCount > 0) {
				for (auto i : MouseButtons(Button).split()) {
					m_btnFns[i].clicks[clickCount] = cb;
				}
			}
		}

		template <MouseButtons::Type Button = MouseButtons::ButtonLeft>
		void setClickCallback(MouseCallback const& cb) {
			setMultiClickCallback<Button>(1, cb);
		}
		template <MouseButtons::Type Button = MouseButtons::ButtonLeft>
		void setDoubleClickCallback(MouseCallback const& cb) {
			setMultiClickCallback<Button>(2, cb);
		}

		template <MouseButtons::Type Button = MouseButtons::ButtonLeft>
		void setDragCallback(MouseCallback const& cb) {
			for (auto i : MouseButtons(Button).split()) {
				m_btnFns[i].drag = cb;
			}
		}

		template <MouseButtons::Type Button = MouseButtons::ButtonLeft>
		void setHoldCallback(MouseCallback const& cb) {
			for (auto i : MouseButtons(Button).split()) {
				m_btnFns[i].hold = cb;
			}
		}

		template <MouseButtons::Type Button = MouseButtons::ButtonLeft>
		void setReleaseCallback(MouseCallback const& cb) {
			for (auto i : MouseButtons(Button).split()) {
				m_btnFns[i].release = cb;
			}
		}

		bool handle(MouseEvent*) override;
	};
}

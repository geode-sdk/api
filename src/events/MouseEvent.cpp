#include <MouseEvent.hpp>

USE_GEODE_NAMESPACE();
using namespace cocos2d;

std::vector<MouseButtons> MouseButtons::split() {
	std::vector<MouseButtons> out;

	auto accum = static_cast<MouseButtons::Type>(inner);
	for (MouseButtons::Type i : {ButtonLeft, ButtonRight, ButtonMiddle, Button4, Button5}) {
		if (accum & i) {
			accum = static_cast<MouseButtons::Type>(accum ^ i);
			out.push_back(i);
		}
	}

	return out;
}

MouseState& MouseState::globalStateMut() {
	static MouseState* shared = new MouseState;
	return *shared;
}

MouseState const& MouseState::globalState() {
	return MouseState::globalStateMut();
}

MouseEvent::MouseEvent() {}

MouseEvent* MouseEvent::create(MouseState const& newState) {
	auto ret = new MouseEvent;
	ret->m_newState = newState;

	return ret;
}

CCPoint MouseEvent::distanceMoved() {
	return m_newState.position - MouseState::globalState().position;
}

bool MouseEvent::justPressedButton(MouseButtons btn) {
	return (m_newState.buttons & btn) && !(MouseState::globalState().buttons & btn);
}

bool MouseEvent::justReleasedButton(MouseButtons btn) {
	return !(m_newState.buttons & btn) && (MouseState::globalState().buttons & btn);
}

CCPoint MouseEvent::distanceScrolled() {
	return ccp(m_newState.scrollX, m_newState.scrollY);
}

void MouseEvent::post() {
	Event::post();
	MouseState& state = MouseState::globalStateMut();
	state = m_newState;
}

MouseHandler::MouseHandler() {

}

MouseHandler* MouseHandler::create() {
	return new MouseHandler();
}

void MouseHandler::setHoldDelay(float delay) {
	m_holdDelay = delay;
}

void MouseHandler::setMultiClickDelay(float delay) {
	m_multiClickDelay = delay;
}

void MouseHandler::setGeneralCallback(MouseCallback const& cb) {
	m_allFn = cb;
}

void MouseHandler::setMoveCallback(MouseCallback const& cb) {
	m_moveFn = cb;
}

void MouseHandler::setScrollCallback(MouseCallback const& cb) {
	m_scrollFn = cb;
}

void callOpt(MouseCallback const& f, MouseEvent* ev) {
	if (f) f.value()(ev);
}

bool MouseHandler::handle(MouseEvent* ev) {
	callOpt(m_allFn, ev);

	if (ev->distanceMoved() != CCPointZero) // mouse moved
		callOpt(m_moveFn, ev);

	for (auto& [btn, btnData] : m_btnFns) { // evaluate per-button

		auto timeOfFire = std::chrono::high_resolution_clock::now();
		float duration = std::chrono::duration<float, std::nano>(timeOfFire - btnData.timeOfFire).count() / 1000000000.f;

		if (ev->mouseState().buttons & btn) { // button is being pressed
			if (ev->distanceMoved() != CCPointZero) {
				callOpt(btnData.drag, ev); // move + button down = drag
			}

			if (ev->justPressedButton(btn)) { // button was just pressed
				btnData.clickCount += 1;
				btnData.timeOfFire = timeOfFire;
			} else if (duration >= m_holdDelay && btnData.clickCount == 1) { // multi-clicks take priority over hold
				callOpt(btnData.hold, ev);
				btnData.holdOn = true;
				btnData.clickCount = 0; // can't multi-click with a hold
			}
		} else if (btnData.holdOn) {
			callOpt(btnData.release, ev);
			btnData.holdOn = false;
			btnData.timeOfFire = timeOfFire;
		} else if (btnData.clickCount > 0) { // button was just released
			if (duration >= m_multiClickDelay || btnData.clickCount == btnData.clicks.rbegin()->first) { // multi support
				callOpt(btnData.clicks[btnData.clickCount], ev);
				btnData.clickCount = 0;
			}
			btnData.timeOfFire = timeOfFire;
		}
	}
	return true;
}

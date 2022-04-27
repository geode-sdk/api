#include <Shortcut.hpp>

USE_GEODE_NAMESPACE();

std::string mouseBtnToString(MouseEvent btn) {
    switch (btn) {
        case MouseEvent::Left: return "Left Click";
        case MouseEvent::DoubleClick: return "Double Click";
        case MouseEvent::Right: return "Right Click";
        case MouseEvent::Middle: return "Middle Click";
        case MouseEvent::Next: return "Mouse Forward";
        case MouseEvent::Prev: return "Mouse Back";
        case MouseEvent::ScrollUp: return "Scroll Up";
        case MouseEvent::ScrollDown: return "Scroll Down";
    }
    return "Mouse Unk";
}

std::string keyToString(cocos2d::enumKeyCodes code) {
    switch (code) {
        case KEY_None:      return "";
        case KEY_C:         return "C";
        case KEY_Multiply:  return "Mul";
        case KEY_Divide:    return "Div";
        case KEY_OEMPlus:   return "OEMPlus";
        case KEY_OEMMinus:  return "OEMMinus";
        
        case static_cast<cocos2d::enumKeyCodes>(-1):
            return "Unk";
        
        default:
            return CCDirector::sharedDirector()
                ->getKeyboardDispatcher()
                ->keyToString(code);
    }
}

std::string modifierToString(KeyModifiers modifs) {
	switch (static_cast<int>(modifs)) {
		case 0b1:
			return "Ctrl";
		case 0b10:
			return "Shift";
		case 0b11:
			return "Ctrl + Shift";
		case 0b100:
			return "Alt";
		case 0b101:
			return "Ctrl + Alt";
		case 0b110:
			return "Alt + Shift";
		case 0b111:
			return "Ctrl + Alt + Shift";
		case 0b1000:
			return "Command";
		case 0b1001:
			return "Command + Ctrl";
		case 0b1010:
			return "Command + Shift";
		case 0b1011:
			return "Command + Ctrl + Shift";
		case 0b1100:
			return "Command + Alt";
		case 0b1101:
			return "Command + Ctrl + Alt";
		case 0b1110:
			return "Command + Alt + Shift";
		case 0b1111:
			return "Command + Ctrl + Alt + Shift";
		default:
			return "";
	}
}

bool Shortcut::accepts(Shortcut const& sc) const {
	return sc.modifiers == this->modifiers && this->input == sc.input;
}

std::string Shortcut::toString() const {
	std::string out = modifierToString(this->modifiers);
	if (!out.empty() && input.index() != 2)
		out += " + ";

	if (input.index() == 0) {
		out += keyToString(std::get<KeyboardEvent>(input));
	} else if (input.index() == 1) {
		out += mouseBtnToString(std::get<MouseEvent>(input));
	}

	return out;
}


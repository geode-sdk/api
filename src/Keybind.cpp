#include <Geode.hpp>
#include <Keybind.hpp>

USE_GEODE_NAMESPACE();

std::string geode::keyToStringFixed(enumKeyCodes code) {
    switch (code) {
        case KEY_None:      return "";
        case KEY_C:         return "C";
        case KEY_Multiply:  return "Mul";
        case KEY_Divide:    return "Div";
        case KEY_OEMPlus:   return "OEMPlus";
        case KEY_OEMMinus:  return "OEMMinus";
        
        case static_cast<enumKeyCodes>(-1):
            return "Unk";
        
        default:
            return CCDirector::sharedDirector()
                ->getKeyboardDispatcher()
                ->keyToString(code);
    }
}

bool Keybind::operator==(Keybind const& other) const {
    return
        other.input.key == this->input.key &&
        other.modifiers == this->modifiers &&
        other.input.mouse == this->input.mouse;
}

bool Keybind::operator<(Keybind const& other) const {
    return
        this->input.key < other.input.key ||
        this->modifiers < other.modifiers ||
        this->input.mouse < other.input.mouse;
}

std::string Keybind::toString() const {
    std::string res = "";

    res = std::to_string(this->modifiers.m_value) + "; ";

    if (this->modifiers.has(Modifiers::Control))    res += "Ctrl + ";
    if (this->modifiers.has(Modifiers::Command))    res += "Command + ";
    if (this->modifiers.has(Modifiers::Alt))        res += "Alt + ";
    if (this->modifiers.has(Modifiers::Shift))      res += "Shift + ";

    std::string r = "";

    if (r.size())
        res += r;
    else
        res = res.substr(0, res.size() - 3);
        
    return res;
}

void Keybind::save(DS_Dictionary* dict) const {
    // dict->setIntegerForKey("key", this->key);
    // dict->setIntegerForKey("modifiers", this->modifiers);
    // dict->setIntegerForKey("click", this->mouse);
}

std::string Input::toString() const {
    if (this->mouse != MouseButton::None) {
        return mouseToString(this->mouse);
    } else {
        return keyToStringFixed(this->key);
    }
}

Input::Input(cocos2d::enumKeyCodes key) {
    this->key = key;
    this->type = Type::Key;
}

Input::Input(MouseButton mouse) {
    this->mouse = mouse;
    this->type = Type::Mouse;
}

Input& Input::operator=(cocos2d::enumKeyCodes key) {
    this->key = key;
    this->type = Type::Key;
    return *this;
}

Input& Input::operator=(MouseButton mouse) {
    this->mouse = mouse;
    this->type = Type::Mouse;
    return *this;
}

bool Input::operator==(Input const& other) const {
    if (this->type != other.type) return false;
    switch (this->type) {
        case Type::Key:   return this->key == other.key;
        case Type::Mouse: return this->mouse == other.mouse;
        case Type::None:  return true;
    }
    return false;
}

bool Input::isntNone() const {
    return this->key != KEY_None || this->mouse != MouseButton::None;
}

Keybind::Keybind() {
    this->input = KEY_None;
    this->modifiers = 0;
}

Keybind::Keybind(Input const& pressed) {
    switch (pressed.key) {
        // same for command for mac users
        case KEY_Control:       this->input = KEY_None; break;
        case KEY_Shift:         this->input = KEY_None; break;
        case KEY_Alt:           this->input = KEY_None; break;
        default:                this->input = pressed;  break;
    }

    auto kb = CCDirector::sharedDirector()->getKeyboardDispatcher();

    this->modifiers = 0;
    if (kb->getControlKeyPressed())
        this->modifiers |= Keybind::Modifiers::Control;
    if (kb->getCommandKeyPressed())
        this->modifiers |= Keybind::Modifiers::Command;
    if (kb->getShiftKeyPressed())
        this->modifiers |= Keybind::Modifiers::Shift;
    if (kb->getAltKeyPressed())
        this->modifiers |= Keybind::Modifiers::Alt;
}

Keybind::Keybind(Input const& pressed, Modifiers mods) {
    this->input = pressed;
    this->modifiers = mods;
}

Keybind::Keybind(Input const& pressed, int mods) {
    this->input = pressed;
    this->modifiers = mods;
}

Keybind::Keybind(DS_Dictionary* dict, int version) {
    // this->key = static_cast<enumKeyCodes>(dict->getIntegerForKey("key"));
    // this->modifiers = dict->getIntegerForKey("modifiers");
    // if (version > 1)
    //     this->mouse = static_cast<decltype(this->mouse)>(
    //         dict->getIntegerForKey("click")
    //     );
}

std::ostream& geode::operator<<(std::ostream& stream, Keybind const& bind) {
    return stream << bind.toString();
}

std::ostream& geode::operator<<(std::ostream& stream, keybind_action_id const& id) {
    return stream << id.m_value;
}


keybind_action_id::keybind_action_id() {
    m_value = "";
}

keybind_action_id::keybind_action_id(const char* val) {
    if (val) {
        m_value = val;
    } else {
        m_value = "";
    }
}

keybind_action_id::keybind_action_id(std::string const& val) {
    *this = val;
}

keybind_action_id::~keybind_action_id() {}

const char* keybind_action_id::c_str() const {
    return m_value.c_str();
}

keybind_action_id keybind_action_id::operator=(std::string const& val) {
    m_value = val;
    return *this;
}

keybind_action_id keybind_action_id::operator=(std::nullptr_t const&) {
    m_value = "";
    return *this;
}

keybind_action_id::operator int() const {
    if (!m_value.size()) return 0;
    return hash(m_value.c_str());
}

size_t keybind_action_id::size() const {
    return m_value.size();
}

keybind_action_id::operator std::string() const {
    return m_value;
}

bool keybind_action_id::operator==(keybind_action_id const& other) const {
    return
        string_utils::toLower(this->m_value) ==
        string_utils::toLower(other.m_value);
}

std::size_t std::hash<Keybind>::operator()(Keybind const& key) const {
    return (key.modifiers.value()) + (std::hash<Input>{}(key.input) << 4);
}

std::size_t std::hash<keybind_action_id>::operator()(keybind_action_id const& key) const {
    return std::hash<decltype(key.m_value)>()(key.m_value);
}

std::size_t std::hash<Input>::operator()(Input const& inp) const {
    switch (inp.type) {
        case Input::Type::Mouse: return static_cast<size_t>(inp.type) + (static_cast<size_t>(inp.mouse) << 2);
        case Input::Type::Key:   return static_cast<size_t>(inp.type) + (inp.key << 2);
        default: break;
    }
    return 0;
}

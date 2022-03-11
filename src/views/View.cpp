#include <views/View.hpp>

USE_GEODE_NAMESPACE();

ViewItem::ViewItem(std::string_view id, cocos2d::CCNode* node) : m_id(id), m_node(node) {}

float ViewItem::getWidth() const {
	return m_node->getContentSize().width * m_node->getScaleX();
}

float ViewItem::getHeight() const {
	return m_node->getContentSize().height * m_node->getScaleY();
}

float ViewItem::getWorldX() const {
	return m_node->convertToWorldSpace(m_node->getPosition()).x;
}

float ViewItem::getWorldY() const {
	return m_node->convertToWorldSpace(m_node->getPosition()).y;
}

float ViewItem::getX() const {
	return m_node->getPositionX();
}

float ViewItem::getY() const {
	return m_node->getPositionY();
}

void ViewItem::setX(float x) {
	m_node->setPositionX(x);
}

void ViewItem::setY(float y) {
	m_node->setPositionY(y);
}

bool ViewItem::operator==(ViewItem const& other) const {
	if (m_id == "" || other.m_id == "") return m_node == other.m_node;
	return m_id == other.m_id || m_node == other.m_node;
}

bool ViewImpl::init() {
	return true;
}

// virtual bool ViewImpl::initWithItems(cocos2d::CCDictionary*) {
// 	//im too lazy
// }

// virtual bool ViewImpl::initWithItems(std::unordered_map<std::string, cocos2d::CCNode*> const& items) {
// 	this->m_items = items;
// 	for (auto& [key, value] : items) {
// 		m_order.push_back(key);
// 	}
// 	this->format();
// }

void ViewImpl::addItem(std::string_view id, cocos2d::CCNode* node) {
	auto item = ViewItem(id, node);
	auto it = std::find(m_items.begin(), m_items.end(), item);
	if (it == m_items.end()) {
		m_items.push_back(item);
		this->format();
	}
	else {
		Mod::get()->logInfo("The node " + std::string(id) + " already exists in the view. ", Severity::Warning);
	}
}

void ViewImpl::removeItem(std::string_view id) {
	auto item = ViewItem(id, nullptr);
	auto it = std::find(m_items.begin(), m_items.end(), item);
	if (it != m_items.end()) {
		m_items.erase(it);
		this->format();
	}
	else {
		Mod::get()->logInfo("The node " + std::string(id) + " is not in the view. ", Severity::Warning);
	}
}

void ViewImpl::removeItem(cocos2d::CCNode* node) {
	auto item = ViewItem("", node);
	auto it = std::find(m_items.begin(), m_items.end(), item);
	if (it != m_items.end()) {
		m_items.erase(it);
		this->format();
	}
	else {
		Mod::get()->logInfo("The node is not in the view. ", Severity::Warning);
	}
}

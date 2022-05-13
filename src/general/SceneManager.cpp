#include <general/SceneManager.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

bool SceneManager::setup() {
    m_persistedNodes = CCArray::create();
    m_persistedNodes->retain();
    return true;
}

SceneManager* SceneManager::get() {
    static SceneManager* inst = nullptr;
	if (!inst) {
		inst = new SceneManager();
		inst->setup();
	}
    return inst;
}

SceneManager::~SceneManager() {
    m_persistedNodes->release();
}

void SceneManager::keepAcrossScenes(CCNode* node) {
    m_persistedNodes->addObject(node);
}

void SceneManager::forget(CCNode* node) {
    m_persistedNodes->removeObject(node);
}

void SceneManager::willSwitchToScene(CCScene* scene) {
    CCARRAY_FOREACH_B_TYPE(m_persistedNodes, node, CCNode) {
        node->removeFromParent();
        scene->addChild(node);
    }
}

#include <general/SceneManager.hpp>
#include <WackyGeodeMacros.hpp>

USE_GEODE_NAMESPACE();

StayAcrossScenes::StayAcrossScenes() {
    SceneManager::get()->keepAcrossScenes(dynamic_cast<CCNode*>(this));
}

StayAcrossScenes::~StayAcrossScenes() {
    SceneManager::get()->forget(dynamic_cast<CCNode*>(this));
}

SceneManager* SceneManager::setup() {
    m_persistedNodes = CCArray::create();
    m_persistedNodes->retain();
    return this;
}

static SceneManager* s_sceneManager = nullptr;
SceneManager* SceneManager::get() {
	if (s_sceneManager) {
		s_sceneManager = new SceneManager();
		s_sceneManager->setup();
	}
    return s_sceneManager;
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

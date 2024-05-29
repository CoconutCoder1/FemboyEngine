#include "scenesystem.h"

namespace fe {

SceneSystem::SceneSystem() {
	m_pDefaultScene = ScopedPtr<Scene>(new Scene("Default Scene"));
	m_pActiveScene = m_pDefaultScene.get();
}

Scene* SceneSystem::GetActiveScene() const {
	return m_pActiveScene;
}

}
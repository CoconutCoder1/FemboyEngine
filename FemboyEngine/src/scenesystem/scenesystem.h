#pragma once

#include "scene.h"

#include "typeinfo/object.h"

#include "fstdlib/pointers.h"

namespace fe {

class SceneSystem : public Inherit<Object, SceneSystem> {
public:
	SceneSystem();

	Scene* GetActiveScene() const;

private:
	ScopedPtr<Scene> m_pDefaultScene;
	Scene* m_pActiveScene;
};

}
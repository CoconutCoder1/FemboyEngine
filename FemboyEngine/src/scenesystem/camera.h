#pragma once

#include "typeinfo/object.h"

#include "mathlib/mathlib.h"
#include "mathlib/matrix.h"

#include "rendersystem/screenlayer.h"

#include "component.h"
#include "sceneinfo.h"

namespace fe {

class Camera : public Inherit<Component, Camera> {
public:
	Camera();
	virtual ~Camera() = default;

	void Update();
	void Render(render::RenderContext* pRenderContext);

	float GetAspectRatio() const;

private:
	float m_Fov;
	float m_AspectRatio;
	float m_Near;
	float m_Far;

	SceneInfo m_SceneInfo;
	ScopedPtr<render::ScreenLayer> m_pScreenLayer;
};

}
#include "camera.h"

#include "core/gameconfig.h"

namespace fe {

Camera::Camera() {
	m_Fov = 70.f;
	m_AspectRatio = 0.f;
	m_Near = 0.02f;
	m_Far = 1000.f;

	m_pScreenLayer = ScopedPtr<render::ScreenLayer>(new render::ScreenLayer());
}

void Camera::Update() {
	m_SceneInfo.projMat = math::MakePerspectiveFovRH(m_Fov * math::Deg2Rad, GetAspectRatio(), m_Near, m_Far);
	m_SceneInfo.viewMat = math::MakeLookAtRH(math::Vector3(0.f, 0.f, 0.f), math::Vector3(0.f, 0.f, 0.f), math::Vector3(0.f, 1.f, 0.f));
}

void Camera::Render(render::RenderContext* pRenderContext) {
	m_pScreenLayer->Draw(pRenderContext);
}

float Camera::GetAspectRatio() const {
	if (m_AspectRatio > 0.f)
		return m_AspectRatio;

	return static_cast<float>(GetGameConfig().width) / GetGameConfig().height;
}

}
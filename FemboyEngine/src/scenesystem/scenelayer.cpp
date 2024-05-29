#include "scenelayer.h"
#include "scenesystem.h"

#include <SDL3/SDL_assert.h>

namespace fe {

void SceneLayer::OnAttach(render::RenderDevice* pDevice)
{

}

void SceneLayer::OnDetach(render::RenderDevice* pDevice)
{

}

void SceneLayer::Draw(render::RenderContext* pRenderContext)
{
	Scene* pScene = SceneSystem::Instance().GetActiveScene();
	
	SDL_assert(pScene != nullptr);

	
}

}
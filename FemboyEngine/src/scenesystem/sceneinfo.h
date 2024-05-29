#pragma once

#include "mathlib/matrix.h"

namespace fe {

struct SceneInfo {
	math::Matrix4x4 projMat;
	math::Matrix4x4 viewMat;
};

}
#pragma once

#include <cstdint>

namespace fe {

struct GameConfig_t {
	int32_t width;
	int32_t height;
	bool isFullscreen;
	bool isBorderless;
};

const GameConfig_t& GetGameConfig();
void SetGameConfig(const GameConfig_t& config);
void SetGameConfigResolution(int32_t width, int32_t height);

}
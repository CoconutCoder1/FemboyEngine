#include "gameconfig.h"

namespace fe {

static GameConfig_t gameConfig;

const GameConfig_t& GetGameConfig() {
	return gameConfig;
}

void SetGameConfig(const GameConfig_t& config) {
	gameConfig = config;
}

void SetGameConfigResolution(int32_t width, int32_t height) {
	gameConfig.width = width;
	gameConfig.height = height;
}

}
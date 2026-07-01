/*****************************************************************//**
 * @file   Texture.h
 * @brief  Function declarations for texture creation and destruction.
 * 
 * @author Luis Camilo Alvarez Carrau
 * @date   6-30-2026
 *********************************************************************/
#pragma once

#include "../Structures/Texture.h"

namespace PersistanceBackend {

	Texture createTexture(const char* imagePath);

	void cleanUpTextures(Texture* textures, const uint32_t textureCount);

}

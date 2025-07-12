
#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>

#include "third_party/stb/stb_image.h"
#define VK_NO_PROTOTYPES
#include "third_party/volk/volk.h"
#include "third_party/VulkanMemoryAllocator/include/vk_mem_alloc.h"


#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <array>
#include <cstring>
#include <optional>
#include <set>
#include <algorithm>
#include <limits>
#include <memory>
#include <fstream>
#include <functional>

#include "vk_macros.hpp"

#ifdef NDEBUG
    const bool enableValidationLayers = false; 
#else
    const bool enableValidationLayers = true;
#endif


#define MAX_FRAMES_IN_FLIGHT 2

namespace HelloTriangle {

}

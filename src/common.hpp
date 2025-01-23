#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>

#include "third_party/volk.h"
#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <algorithm>
#include <limits>
#include <fstream>


#ifdef NDEBUG
    const bool enableValidationLayers = false; 
#else
    const bool enableValidationLayers = true;
#endif


#define MAX_FRAMES_IN_FLIGHT 2

namespace HelloTriangle {

}
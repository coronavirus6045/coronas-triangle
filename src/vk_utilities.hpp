#pragma once

#include "common.hpp"
#include "vk_image.hpp"

namespace HelloTriangle {



HelloTriangle::Image HelloTriangle::load_image_from_file(Device& device, std::string& path);
}

# coronas-triangle

vulcan moment

## Building
You'll need to install some dependencies in order to build.


Dependencies:
- [Meson](https://mesonbuild.com/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)

### Windows (haven't tested yet) :
Get the [SDK]()

### Linux :
Ubuntu
'''
sudo apt install libvulkan-dev vulkan-validationlayers-dev spirv-tools libglfw3-dev libglm-dev
'''
Unfortunately, Ubuntu's repositories lack glslc
**Fedora**
'''
sudo dnf install mesa-vulkan-devel vulkan-validation-layers-devel glfw-devel glm-devel glslc
'''

**Arch**
'''
sudo pacman -S vulkan-devel glfw-x11 glm shaderc
'''
**Note:** If using Wayland, replace 'glfw-x11' with 'glfw-wayland'.

## Compiling shaders
Before starting, 


## Third-party libraries used

- [Volk](https://github.com/zeux/volk) - meta loader t


if anyone is reading this code, this lacks design, and in the future i will restructure the code.
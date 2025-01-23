set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(dir ${PROJECT_SOURCE_DIR}/src/)
set(thirdparty ${dir}/src/)
set(EXECUTABLE_OUTPUT_PATH ${dir}/build/)
set(LIBRARY_OUTPUT_PATH ${dir}/build/)

project(triangle VERSION 0.1)

set

add_executable(triangle 
    main.cpp
    init_window.cpp
    load_file.cpp
    log.cpp
    vk_buffers.cpp
    vk_command.cpp
    vk_debugmsg.cpp
    vk_device.cpp
    vk_instance.cpp
    vk_pipeline.cpp
    vk_presentation.cpp
    vk_syncobjects.cpp
)

add_library(volk 
    ${dir}/volk.c
    ${dir}/volk.h
)
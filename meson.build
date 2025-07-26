project('triangle', 'cpp',
 version : '0.1',
 default_options : ['cpp_std=c++17', 'c_std=c17'])
add_languages('c')
subdir('src')
subdir('src/third_party/')

includes = include_directories(['src/third_party/imgui/', 'src/third_party/volk/'])

deps = [dependency('dl'),
        dependency('sdl2'),
       
]

executable('vktest', sources, dependencies: deps, include_directories : includes)

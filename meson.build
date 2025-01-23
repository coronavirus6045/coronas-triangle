project('triangle', 'cpp',
 version : '0.1',
 default_options : ['cpp_std=c++17', 'c_std=c17'])
add_languages('c')
subdir('src')
subdir('src/third_party/')

deps = [dependency('dl'),
        dependency('glfw3', modules: ['glfw']),
       
]

executable('vktest', sources, dependencies: deps)
#version 450


layout(binding = 0) uniform MVPMatrix {
    mat4 model;
    mat4 view;
    mat4 proj;
} _mvp_matrix;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 tex_coord;
// down right = 1
//vec2 positions[3] = vec2[](
//  vec2(0.0, -0.5),
//  vec2(0.5, 0.5),
//  vec2(-0.5, 0.5)
//);


//vec3 colors[3] = vec3[](
//    vec3(1.0, 0.0, 0.0),
//    vec3(0.0, 1.0, 0.0),
//    vec3(0.0, 0.0, 1.0)
//);

void main() {
    gl_Position = _mvp_matrix.proj * _mvp_matrix.view * _mvp_matrix.model * vec4(in_position, 1.0);
    frag_color = in_color;
    tex_coord = in_tex_coord;
}

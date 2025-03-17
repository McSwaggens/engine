#version 450

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

layout(binding = 0) uniform Ubo {
	float time;
	mat4 mvp;
};

void main() {
	gl_Position = vec4(in_pos * (sin(time) + 1) * 0.5, 0.0, 1.0) * mvp;
	frag_color = in_color;
}


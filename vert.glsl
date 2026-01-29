#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 position;

layout(location = 0) out vec3 frag_color;

layout(binding = 0) uniform Ubo {
	float time;
	mat4  vp;
};

void main() {
	gl_Position = vp * vec4(in_pos + position, 1);
	frag_color = in_color;
}


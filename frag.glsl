#version 450

layout(location = 0) in  vec3 vertex_color;
layout(location = 0) out vec4 color;

void main() {
	vec2 c = (gl_FragCoord.xy / vec2(1920, 1080) - 0.5) * 2 * 1.5;
	vec2 z = vec2(0);
	for (int i = 0; i < 1000; i++) {
		z = vec2(z.x*z.x - z.y*z.y, z.x*z.y + z.y*z.x) + c;
		if (length(z) >= 2) {
			color = vec4(1, 1, 1, 1);
			return;
		}
	}

	color = vec4(0, 0, 0, 1);
}

#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in int textureSlot;
layout (location = 3) in float transparency;

flat out int v_TextureSlot;
out vec2 v_UV;
flat out float v_Transparency;

uniform mat4 u_Projection;

void main() {
	v_TextureSlot = textureSlot;
	v_UV = uv;
	v_Transparency = transparency;

	gl_Position = u_Projection * vec4(position, 0, 1);
}

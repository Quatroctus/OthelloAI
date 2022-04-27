#version 330 core

flat in int v_TextureSlot;
in vec2 v_UV;
flat in float v_Transparency;

uniform sampler2D light_piece;
uniform sampler2D dark_piece;
uniform sampler2D light_board;
uniform sampler2D dark_board;
uniform sampler2D font;

out vec4 color;

void main() {
	if (v_TextureSlot == 0) {
		color = texture(light_piece, v_UV);
	} else if (v_TextureSlot == 1) {
		color = texture(dark_piece, v_UV);
	} else if (v_TextureSlot == 2) {
		color = texture(light_board, v_UV);
	} else if (v_TextureSlot == 3) {
		color = texture(dark_board, v_UV);
	} else if (v_TextureSlot == 4) {
		float r = texture(font, v_UV).r;
		color = vec4(1,0,0,r);
	} else {
		color = vec4(1.0f, .3f, .6f, v_Transparency); // Error color
	}
	color.a = min(color.a, v_Transparency);
}

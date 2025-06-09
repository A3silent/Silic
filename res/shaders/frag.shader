#version 450 core
in vec2 texCoord;
out vec4 fragColor;
uniform sampler1D palette;
uniform int color;
void main()
{
	fragColor = texelFetch(palette, color, 0);
};
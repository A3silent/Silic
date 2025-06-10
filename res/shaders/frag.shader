#version 410 core
in vec2 texCoordOut;
out vec4 fragColor;
uniform bool useTexture;
uniform usampler2D tex;
uniform sampler1D palette;
uniform int color;
void main()
{
	if (useTexture) {
    	fragColor = texelFetch(palette, int(texture(tex, texCoordOut).r), 0);
    } else {
    	fragColor = texelFetch(palette, color, 0);
	}
};
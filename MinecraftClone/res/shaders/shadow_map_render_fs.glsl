#version 460 core

in vec2 ioTexCoords;

uniform float uAlphaThreshold = 0.0;
uniform sampler2D uTexture;

void main()
{
    // Since we have no color buffer and disabled the draw and read buffers,
    // the resulting fragments do not require any processing so we can simply use an empty fragment shader.
    //
    // gl_FragDepth = gl_FragCoord.z;

    if (texture(uTexture, ioTexCoords).a < uAlphaThreshold)
    {
        discard;
    }
}

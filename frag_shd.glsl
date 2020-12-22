#version 330 core
out vec4 FragColor;
layout(pixel_center_integer) in vec4 gl_FragCoord;
uniform vec2 WindowSize;

void main()
{
    float xnorm = float(gl_FragCoord.x)/WindowSize[0];
    float ynorm = float(gl_FragCoord.y)/WindowSize[1];
    FragColor = vec4(xnorm, ynorm, 0.5, 1.0);
}
#version 450

layout (location = 0) in vec4 oiColor;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = oiColor;
}

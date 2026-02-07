#version 330

in vec3 fragNormal;

out vec4 finalColor;

void main()
{
    vec3 lightDir = vec3(0.,1.,0.);
    float dir = dot(normalize(fragNormal), lightDir);
    dir = (dir * 0.5 + 0.5);
    vec4 white = vec4(1,1,1,1);
    vec4 black = vec4(0,0,0,1);
    finalColor = vec4(dir,dir,dir,1);
}
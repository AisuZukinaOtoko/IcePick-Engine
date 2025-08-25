#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 vertexColour;

out vec2 v_TexCoord;
out vec3 v_Colour;

uniform mat4 u_MVP;

void main(){
   gl_Position = u_MVP * vec4(position, 1.0f);
   //gl_Position = vec4(position, 1.0);
   v_Colour = vertexColour;
   v_TexCoord = texCoord;
};

//--------------------------------------------------------
#shader fragment
#version 330 core

in vec2 v_TexCoord;
in vec3 v_Colour;

uniform vec4 u_color;
uniform sampler2D  u_Texture;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(){
    //color = vec4(0.0);
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    gl_FragColor = vec4(vec3(depth), 1.0);
    //gl_FragColor = vec4(v_Colour, 1.0);
    //vec4 texColor = texture(u_Texture, v_TexCoord);
    //gl_FragColor = vec4(1.0);
};
#shader vertex
#version 330 core

layout(location = 0) in vec2 b_VertexBuffer;
layout(location = 1) in vec4 b_UVBuffer;
layout(location = 2) in float b_RotationsBuffer;
layout(location = 3) in vec2 b_RelativePosition;


uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;


out vec2 v_TextureVertex;


void main(){

	float sine = sin(b_RotationsBuffer);
	float cosine = cos(b_RotationsBuffer);


	//vec2 RotatedPosition = vec2(
	//	b_VertexBuffer.x * cosine - b_VertexBuffer.y * sine,
	//	b_VertexBuffer.x * sine + b_VertexBuffer.y * cosine
	//);


	vec2 WorldPosition = b_VertexBuffer + b_RelativePosition;

	gl_Position = u_Projection * u_View * u_Model * vec4(WorldPosition, 0.f, 1.f); 

	vec2 MinUV = b_UVBuffer.xy;
	vec2 MaxUV = b_UVBuffer.zw;

	v_TextureVertex = mix(MinUV, MaxUV, vec2(
		clamp(b_VertexBuffer.x, 0.f, 1.f),
		clamp(b_VertexBuffer.y, 0.f, 1.f)
	));
}

#shader fragment
#version 330 core

uniform sampler2D u_Texture;

in vec2 v_TextureVertex;


out vec4 o_FragColour;

void main(){
	o_FragColour = texture(u_Texture, v_TextureVertex);
}


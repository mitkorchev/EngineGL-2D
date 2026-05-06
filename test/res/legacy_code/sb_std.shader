#shader vertex
#version 330 core

layout(location = 0) in vec2 b_VertexBuffer;
layout(location = 1) in vec2 b_TexBuffer;
layout(location = 2) in vec4 b_UVRegionBuffer;
layout(location = 3) in float b_Rotations;
layout(location = 4) in vec2 b_PositionsRelativeToModel;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;


out vec2 v_TextureVertex;
	
const vec2 PivotOffset = vec2(50, 50);

void main(){

	float sine = sin(b_Rotations);
	float cosine = cos(b_Rotations);
	
	vec2 PointOfRotation = b_VertexBuffer - PivotOffset;
	vec2 RotatedPosition = vec2(
		PointOfRotation.x * cosine - PointOfRotation.y * sine,
		PointOfRotation.x * sine + PointOfRotation.y * cosine
	);

	vec2 WorldPosition = b_PositionsRelativeToModel + RotatedPosition;
	
	gl_Position = u_Projection * u_View * u_Model * vec4(WorldPosition, 0.f, 1.f);
	

	//	texture sprit indexing
	vec2 MinUV = b_UVRegionBuffer.xy;
	vec2 MaxUV = b_UVRegionBuffer.zw;

	v_TextureVertex = mix(MinUV, MaxUV, b_TexBuffer);
}


#shader fragment
#version 330 core
in vec2 v_TextureVertex;


uniform sampler2D u_Texture;


out vec4 o_FragColour;

void main(){
	o_FragColour = texture(u_Texture, v_TextureVertex);
}


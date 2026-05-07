#shader vertex
#version 330 core

layout(location = 0) in vec2 b_VertexBuffer;
layout(location = 1) in vec2 b_TexBuffer;
layout(location = 2) in vec4 b_UVRegionBuffer;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform int u_SheetRowSpriteCount;
uniform vec2 u_SpriteTexUVDimensions;

uniform int u_RowSpriteCount;	// how many instances per row
uniform int u_SpriteSideLengthPx;

out vec2 v_TexCoord;

void main(){
	//	figure out proper tex UVs
	vec2 MinUV = b_UVRegionBuffer.xy;
	vec2 MaxUV = b_UVRegionBuffer.zw;

	v_TexCoord = mix(MinUV, MaxUV, b_TexBuffer);


	//	figure out locations
	int xSpriteCoord = gl_InstanceID % u_RowSpriteCount;
	int ySpriteCoord = gl_InstanceID / u_RowSpriteCount;

	vec2 PreparedInstancePosition = b_VertexBuffer;
	PreparedInstancePosition += vec2(xSpriteCoord, ySpriteCoord) * vec2(u_SpriteSideLengthPx, u_SpriteSideLengthPx);

	gl_Position = u_Projection * u_View * u_Model * vec4(PreparedInstancePosition, 0.f, 1.f);
}


#shader fragment
#version 330 core
in vec2 v_TexCoord;


uniform sampler2D u_Texture;


out vec4 o_FragColour;

void main(){
	o_FragColour = texture(u_Texture, v_TexCoord);	
}


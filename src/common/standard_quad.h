#pragma once

#include "../opengl/sprite_sheet.h"

/*
This is a global structure, meant to hold common data
such as standard quad vertices, indices and similar.

*/
struct StandardQuad {

	int m_StandardSpritePixelLength = 100;

	unsigned int m_VAO = 0;

	unsigned int m_VextexBuffer = 0;
	unsigned int m_TexCoordBuffer = 0;
	unsigned int m_IndexBuffer = 0;
	unsigned int m_InterpolationBuffer = 0;

	UVRegion m_UVregionCurrentlyUsed;
	UVRegion& GetCurrentUVregion() { return m_UVregionCurrentlyUsed; }

	StandardQuad();

	void Init();
	void Bind();
	void Unbind();

	//	Returns true if this and last used SpriteSheet are the same
	//	and no buffering happens.
	//	Returns false if they're different and buffering does occur.
	bool BufferTexCoords(
		const SpriteSheet* _spriteSheet
	);

	void BindVertexBufferAt(
		int _verAttribPointerIndex
	);

	void BindTexUVbufferAt(
		int _verAttribPointerIndex
	);


	void BindElementIndexArray();

public:

	unsigned int m_OriginalTextureUVBuffer = 0;
	unsigned int GetUnmodifiedTextureUVBuffer() const { return m_OriginalTextureUVBuffer; }
	void BindUnmodifiedTextureUVBuffer() const { glBindBuffer(GL_ARRAY_BUFFER, m_OriginalTextureUVBuffer); }


	unsigned int m_OriginalStandardQuadBuffer = 0;
	unsigned int GetUnmodifiedStandardQuadBuffer() const { return m_OriginalStandardQuadBuffer; }
	void BindUnmodifiedStandardQuadBufferAt(int _index) const;
	
public:

	float  g_stdVertexCoordArray[8] = {
		0,										0,
		0,									(float)m_StandardSpritePixelLength,
		(float)m_StandardSpritePixelLength,	(float)m_StandardSpritePixelLength,
		(float)m_StandardSpritePixelLength,	0
	};

	float g_stdTexCoordArray[8] = {
		0.f,	0.f,
		0.f,	1.f,
		1.f,	1.f,
		1.f,	0.f
	};

	unsigned short g_stdIndexArray[6] = {
		0,	1,	2,
		0,	2,	3
	};

};

extern StandardQuad g_StandardQuad;
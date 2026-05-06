#include "standard_quad.h"



StandardQuad g_StandardQuad = StandardQuad();

StandardQuad::StandardQuad() {}

void StandardQuad::Unbind() {
	glBindVertexArray(0);
}

void StandardQuad::Init() {

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VextexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VextexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_stdVertexCoordArray), g_stdVertexCoordArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &m_TexCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_stdTexCoordArray), g_stdTexCoordArray, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_stdIndexArray), g_stdIndexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);


	glGenBuffers(1, &m_OriginalTextureUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_OriginalTextureUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_stdTexCoordArray), g_stdTexCoordArray, GL_STATIC_DRAW);



	glGenBuffers(1, &m_InterpolationBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_InterpolationBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_stdTexCoordArray), g_stdTexCoordArray, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StandardQuad::Bind() {
	glBindVertexArray(m_VAO);
}


/*	

	This function is a bit of a problem, as in it's not thread-safe.
	Sometime in the future, this whole mechanism needs to be replaced.

	Best idea for now is:
	Add another data member to Renderer2D which holds UVRegions(or buffers with them)
	and SpriteSheet instances hold pointers to them.

*/
bool StandardQuad::BufferTexCoords(
	const SpriteSheet* _spriteSheet
) {
	const UVRegion Region = (_spriteSheet)->GetSheetSpriteUVregion();

	if (Region == GetCurrentUVregion()) return true;

	float texCoords[8] = {
		Region.u0, Region.v0,
		Region.u0, Region.v1,
		Region.u1, Region.v1,
		Region.u1, Region.v0
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoords), texCoords);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_UVregionCurrentlyUsed = Region;
	return false;
}


void StandardQuad::BindVertexBufferAt(
	int _verAttribPointerIndex
) {
	glBindBuffer(GL_ARRAY_BUFFER, m_VextexBuffer);
	glEnableVertexAttribArray(_verAttribPointerIndex);
	glVertexAttribPointer(
		_verAttribPointerIndex,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(float),
		0
	);
}

void StandardQuad::BindTexUVbufferAt(
	int _verAttribPointerIndex
) {
	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
	glEnableVertexAttribArray(_verAttribPointerIndex);
	glVertexAttribPointer(
		_verAttribPointerIndex,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(float),
		0
	);
}


void StandardQuad::BindElementIndexArray() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
}


void StandardQuad::BindUnmodifiedStandardQuadBufferAt(
	int _index
) const {
	glBindBuffer(GL_ARRAY_BUFFER, m_OriginalStandardQuadBuffer);
	glEnableVertexAttribArray(_index);
	glVertexAttribPointer(
		_index,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(float),
		0
	);
}



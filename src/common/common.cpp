
#include "common.h"

bool fEqual(
	float _val1,
	float _val2) 
{
	return 
		( abs(_val1 - _val2) < FLOAT_COMPARE_TOLERANCE ) ? true : false;
}


bool PointInRect(const glm::vec2& p, const glm::vec2& pos, const glm::vec2& size)
{
	return p.x >= pos.x && p.x <= pos.x + size.x &&
		p.y >= pos.y && p.y <= pos.y + size.y;
}


bool FastStringCompare(const char* _str1, const char* _str2) {

	if (!_str1 || !_str2) return false;

	if (*_str1 == *_str2) {
		return !strcmp(++_str1, ++_str2);
	}

	return false;
}


void CheckGLErrors(const char* context) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		const char* errorStr = "Unknown error";
		switch (err) {
		case GL_INVALID_ENUM:					errorStr = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE:					errorStr = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:				errorStr = "GL_INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:					errorStr = "GL_STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:				errorStr = "GL_STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:					errorStr = "GL_OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:	errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cerr << "[OpenGL Error] (" << errorStr << ") in context: " << context << std::endl;
		__debugbreak();
	}
}




FileReader::FileReader(
	std::ifstream&& _openedStream,
	const char* _commentMarker
) :
	m_FileStream(std::move(_openedStream)),
	m_CommentMarker(_commentMarker)
{}


FileReader::~FileReader() {
	m_FileStream.close();
}


bool FileReader::IsFileOpened() const {
	return m_FileStream.is_open();
}


bool FileReader::ReadNextContentLine(
	std::string& OUT_nextLine
) {
	if (!IsFileOpened()) {
		DEBUG_LOG("Trying to read from a non-opened file.");
		return false;
	}

	while (std::getline(m_FileStream, OUT_nextLine)) {
		if (OUT_nextLine.empty()) continue;

		size_t CommentIndex = OUT_nextLine.find(m_CommentMarker);

		if (CommentIndex == 0) continue;

		if (CommentIndex == std::string::npos) return true;
		else {
			OUT_nextLine.erase(OUT_nextLine.begin() + CommentIndex, OUT_nextLine.end());
			return true;
		}
	}

	return false;
}

bool FileReader::IsEOF() const {
	return m_FileStream.eof();
}


FileReader FileReader::OpenFile(
	const char* _fileDirectory,
	const char* _commentMarker
) {
	std::ifstream FileStream(_fileDirectory);
	return FileReader(std::move(FileStream), _commentMarker);
}




Tokeniser::Tokeniser(
	const std::string& _string
) :
	m_InitialString(_string)
{
	SplitString();
}


void Tokeniser::SplitString() {

	size_t First = 0;
	bool InEmptyInterval = true;

	m_Tokens.clear();

	for (size_t i = 0; i < m_InitialString.size(); i++) {

		char CurrChar = m_InitialString[i];

		if (InEmptyInterval) {
			if (!(CurrChar == ' ' || CurrChar == '\t')) {
				InEmptyInterval = false;
				First = i;
			}
			continue;
		}

		else {
			if (CurrChar == ' ' || CurrChar == '\t') {
				InEmptyInterval = true;
				m_Tokens.emplace_back(m_InitialString.c_str() + First, i - First);
			}
			else if (i == m_InitialString.size() - 1) {
				m_Tokens.emplace_back(m_InitialString.c_str() + First, i - First + 1);
			}
			continue;
		}
	}
}


std::string_view Tokeniser::GetToken(
	const size_t _tokenNumber
) const {
	DEBUG_ASSERT(_tokenNumber < m_Tokens.size(), "Indexing tokens out of bounds.");
	return m_Tokens[_tokenNumber];
}


void Tokeniser::LoadString(
	const std::string& _string
) {
	m_InitialString = _string;
	SplitString();
}




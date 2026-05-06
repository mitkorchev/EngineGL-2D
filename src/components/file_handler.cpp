#include "file_handler.h"




FileHandler::FileHandler(
	const char* _baseDir
) :
	m_DirectoryOfMain(_baseDir)
{}


FileHandler::FileHandler(
	const fs::path& _baseDir
) :
	m_DirectoryOfMain(_baseDir)
{}


bool FileHandler::FileExists(
	const char* _relativePathFromMain
) const {
	fs::path p = m_DirectoryOfMain / fs::path(_relativePathFromMain);
	return fs::exists(p);
}


void FileHandler::GetAllContentLinesConcatenatedWithDelimFromFileAt(
	const char* _relativePathFromMain,
	std::string& OUT_concatenatedLines,
	char _delimiter
) const {
	DEBUG_ASSERT(FileExists(_relativePathFromMain), "File at [~/%s] doesn't exist.", _relativePathFromMain);

	std::ifstream Stream;
	Stream.open(m_DirectoryOfMain / fs::path(_relativePathFromMain), std::ios::in);
	DEBUG_ASSERT(Stream.is_open(), "File at [~/%s] exists but can't be opened.", _relativePathFromMain);

	std::string CurrentLine;
	OUT_concatenatedLines.clear();
	
	while (!std::getline(Stream, CurrentLine).eof()) {

		if (CurrentLine.empty()) continue;

		size_t IndexOfComment = IndexWhereCommentStarts(CurrentLine);
		if (IndexOfComment == 0) continue;

		OUT_concatenatedLines
			.append(CurrentLine.substr(0, IndexOfComment));
		OUT_concatenatedLines += _delimiter;
	}

	Stream.close();
}


StringIndices FileHandler::GetFirstElementIndexOfLineWithinConcatenatedContentLines(
	const std::string& _concatenatedLines,
	uint32_t _contentLineIndex,
	char _delimiter
) const {

	size_t First = 0, Last = 0;
	uint32_t LineCounter = 0;

	while(true) {
		First = Last;
		Last = _concatenatedLines.find(_delimiter, First + 1);

		if (LineCounter == _contentLineIndex) {
			First++;
			break;
		}

		DEBUG_ASSERT(Last != std::string::npos, "Concatenated lines for file missing delimiter or indexing out of bounds.");
		LineCounter++;
	}

	StringIndices Result;
	Result.m_FirstIndex = First;
	Result.m_LastIndex = Last;

	return Result;
}


size_t FileHandler::IndexWhereCommentStarts(
	const std::string& _line
) const {
	return _line.find(c_CommentMarker, 0);
}
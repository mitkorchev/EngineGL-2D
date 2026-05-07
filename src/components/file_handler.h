#pragma once


#include "../common/common.h"




struct StringIndices {
	size_t m_FirstIndex = std::string::npos;
	size_t m_LastIndex = std::string::npos;

	size_t GetDifference() const { 
		return 
			m_FirstIndex == std::string::npos || m_LastIndex == std::string::npos ?
			std::string::npos :
			m_LastIndex - m_FirstIndex;
	}
};



class FileHandler {

	fs::path m_DirectoryOfMain = fs::path("/");

private:

	const char* c_CommentMarker = "//";

public:

	FileHandler() {}


	FileHandler(
		const char* _baseDir
	);


	FileHandler(
		const fs::path& _baseDir
	);


	bool FileExists(
		const char* _relativePathFromMain
	) const;


	void GetAllContentLinesConcatenatedWithDelimFromFileAt(
		const char* _relativePathFromMain,
		std::string& OUT_concatenatedLines,
		char _delimiter = '\n'
	) const;


	StringIndices GetFirstElementIndexOfLineWithinConcatenatedContentLines(
		const std::string& _concatenatedLines,
		uint32_t _contentLineIndex,
		char _delimiter = '\n'
	) const;

private:

	size_t IndexWhereCommentStarts(
		const std::string& _line
	) const;

};




#pragma once

#include <string>

class FileReader {

    std::ifstream m_FileStream;


    std::string m_CommentMarker;


    FileReader(
        std::ifstream&& _openedStream,
        const char* _commentMarker
    );

public:

    bool ReadNextContentLine(
        std::string& OUT_nextLine
    );


    bool IsFileOpened() const;


    bool IsEOF() const;


    static FileReader OpenFile(
        const char* _fileDirectory,
        const char* _commentMarker = "//"
    );


    ~FileReader();

};


class Tokeniser {

    std::string m_InitialString;


    std::vector<std::string_view> m_Tokens;

private:

    void SplitString();

public:

    Tokeniser() {}


    Tokeniser(
        const std::string& _string
    );


    std::string_view GetToken(
        const size_t _tokenNumber
    ) const;


    void LoadString(
        const std::string& _string
    );

public:

    size_t GetTokenCount() const { return m_Tokens.size(); }

};


class u32FileReader {

    std::ifstream m_FileStream;


    std::u32string m_CommentMarker;


    u32FileReader(
        std::ifstream&& _openedStream,
        const char32_t* _commentMarker
    );

public:

    bool ReadNextContentLine(
        std::u32string& OUT_nextLine
    );


    bool IsFileOpened() const;


    bool IsEOF() const;


    static FileReader OpenFile(
        const char* _fileDirectory,
        const char32_t* _commentMarker = U"//"
    );


    ~u32FileReader();

};


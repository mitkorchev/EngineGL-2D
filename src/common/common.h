#pragma once

/*
File for storing common utilities.
These include:
    - marcos for debugging, logging and assertions
    - profiler
    - collection of debugging functions for OpenGL
    - Flag class, with 32 possible flags

*/


// -- PROJECT SETTINGS -- //

#define GLEW_STATIC


#define DEBUG__CODE 1           // use for debug-build-only code snippets
#define DEBUG__ALLOW_ASSERTS 1

// -- END PROJECT SETTINGS -- //



#include <cmath>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <filesystem>
#include <format>


#include "../dependancies/GL/glew.h"
#include "../dependancies/GLFW/glfw3.h"

#include "../dependancies/glm/glm.hpp"
#include "../dependancies/glm/gtc/matrix_transform.hpp"
#include "../dependancies/glm/gtc/type_ptr.hpp"


#include "io_util.h"



namespace fs = std::filesystem;


#define FLOAT_COMPARE_TOLERANCE 0.0001f

#ifdef DEBUG__CODE

#define DEBUG__CODE__(__debug_variant, __release_variant)\
__debug_variant


#define BREAK_IF(cond_) if (cond_) __debugbreak();

#else

#define DEBUG__CODE__(__debug_variant, __release_variant)\
__release_variant

#endif


//  Conditions changed to that so ASSERT macroes
//  can freely use debug-only variables
#if (DEBUG__ALLOW_ASSERTS && DEBUG__CODE)

#define DEBUG_ASSERT(x_, msg_, ...) \
    if (!(x_)) { \
        fprintf(stderr, "%s : %d\nASSERT FAILED: " msg_ "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        __debugbreak(); \
    }

#define DEBUG_WARN(x_, msg_, ...) \
    if (!(x_)) { \
        fprintf(stderr, "%s : %d\nWARNING: " msg_ "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#define DEBUG_LOG(msg_, ...) \
    fprintf(stderr, "LOG: " msg_ "\n", ##__VA_ARGS__)

#else

#define DEBUG_ASSERT(x_, msg_, ...)
#define DEBUG_WARN(x_, msg_, ...)
#define DEBUG_LOG(msg_, ...)

#endif


//          COMMON VALUE DEFINITINOS


typedef unsigned int ID;



inline unsigned short gc_ui16ErrorCode = 0xffff;
inline unsigned int gc_ui32ErrorCode = 0xffffffff;
inline unsigned long long U64_ERROR_CODE = 0xFFFFFFFFFFFFFFFF;

#define U64_ERROR 0xFFFFFFFFFFFFFFFFui64


//          UTILITY METHODS AND DIAGNOSTICS


void CheckGLErrors(const char* context = "OpenGL");


bool fEqual(float _val1, float _val2);


bool FastStringCompare(const char* _str1, const char* _str2);


struct GLdiagnostics {

    static bool IsBuffer(
        unsigned int _bufferName,
        bool true_if_printing_result = false
    ) {

        bool IsBuffer = glIsBuffer(_bufferName) == GL_TRUE;
        
        if (true_if_printing_result) {

            if (IsBuffer) {
                fprintf(stdout, "[%u] is a valid buffer.\n", _bufferName);
            }
            else {
                fprintf(stdout, "[%u] is NOT a valid buffer.\n", _bufferName);
            }
        }

        return IsBuffer;
    }


    static bool IsVertexArray(
        unsigned int _vaoName,
        bool true_if_printing_result = false
    ) {

        bool IsVAO = glIsVertexArray(_vaoName) == GL_TRUE;

        if (true_if_printing_result) {

            if (IsVAO) {
                fprintf(stdout, "[%u] is a valid VAO.\n", _vaoName);
            }
            else {
                fprintf(stdout, "[%u] is NOT a valid VAO.\n", _vaoName);
            }
        }

        return IsVAO;
    }

};


struct FlagTracker {
    
    FlagTracker(
        const unsigned int _initFlags = 0
    ) {
        m_Flags |= _initFlags;
    }

    bool CheckFlag(
        const unsigned int _flag
    ) const {
        return m_Flags & _flag;
    }


    void SetFlag(
        const unsigned int _flag
    ) {
        m_Flags |= _flag;
    }


    void ClearFlag(
        const unsigned int _flag
    ) {
        m_Flags &= ~_flag;
    }


    bool CheckAndClearFlag(
        const unsigned int _flag
    ) {
        bool Result = CheckFlag(_flag);
        ClearFlag(_flag);
        return Result;
    }


private:

    unsigned int m_Flags = 0;

};


struct Random {

    static std::mt19937& engine() {
        static std::mt19937 eng{ std::random_device{}() };
        return eng;
    }


    static int IntRange(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }


    static uint32_t UInt32Range(uint32_t min, uint32_t max) {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(engine());
    }

};


struct Profiler {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    static inline TimePoint s_Start;
    static inline TimePoint s_End;


    static void Start() {
        s_Start = Clock::now();
    }

    
    static float Stop() {
        s_End = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(s_End - s_Start).count();
        return static_cast<float>(duration) / 1000.f;
    }


    static long long ElapsedMicroseconds() {
        return std::chrono::duration_cast<std::chrono::microseconds>(s_End - s_Start).count();
    }


    static double ElapsedMilliseconds() {
        return std::chrono::duration<double, std::milli>(s_End - s_Start).count();
    }
};



bool PointInRect(const glm::vec2& p, const glm::vec2& pos, const glm::vec2& size);
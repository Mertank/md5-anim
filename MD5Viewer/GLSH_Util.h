#ifndef GLSH_UTIL_H_
#define GLSH_UTIL_H_

#include <string>
#include <vector>
#include <sstream>  // stringstream
#include <cstdlib>  // rand and srand
#include <ctime>    // time for random seed

namespace glsh {

//
// Random numbers
//

inline void InitRandom()
{
    std::srand((unsigned)time(NULL) ^ (unsigned)std::rand());
}

inline void InitRandom(unsigned seed)
{
    std::srand(seed);
}

inline float UnitRandom()
{
    return std::rand() / (RAND_MAX + 1.0f);
}

inline float Random(float lower, float upper)
{
    return lower + UnitRandom() * (upper - lower);
}

inline int RandomInt(int lower, int upper)
{
    return lower + (int)(UnitRandom() * (upper - lower));
}

inline int RandomInt(int upper)
{
    return RandomInt(0, upper);
}


//
// File handling stuff
//

std::string ReadTextFile(const std::string& fname);


//
// string handling stuff
//

//
// break string up into non-whitespace tokens
//
std::vector<std::string> Tokenize(const std::string& str);

//
// Split a string using a character delimeter
//
// Split("ab&c&def", '&')  -->  { "ab", "c", "def" }
// Split("c&", '&')        -->  { "c", "" }
// Split("&c", '&')        -->  { "", "c" }
// Split("&", '&')         -->  { "", "" }
// Split("&&", '&')        -->  { "", "", "" }
// Split("abc", '&')       -->  { "abc" }
// Split("", '&')          -->  { }
//
std::vector<std::string> Split(const std::string& str, char delimiter);

template <typename T>
std::string ToString(const T& value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

template <typename T>
T FromString(const std::string& str)
{
    std::istringstream ss(str);
    T value;
    ss >> value;
    return value;
}

inline bool StringBeginsWith(const std::string& s, const std::string& prefix)
{
    return s.compare(0, prefix.length(), prefix) == 0;
}

// magic
inline bool IsPowerOf2(int x)
{
    return x > 0 && !(x & (x - 1));
}

}

#endif

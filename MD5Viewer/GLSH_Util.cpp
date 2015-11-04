#include "GLSH_Util.h"

#include <fstream>
#include <stdexcept>

namespace glsh {

std::string ReadTextFile(const std::string& fname)
{
    std::ifstream f(fname.c_str());

    if (!f.good()) {
        std::stringstream errmsg;
        errmsg << "Failed to open " << fname << " for reading";
        throw std::runtime_error(errmsg.str());
    }

    std::stringstream ss;
    std::string line;
    while (std::getline(f, line))
        ss << line << '\n';

    if (!f.eof()) {
        std::stringstream errmsg;
        errmsg << "Error reading " << fname;
        throw std::runtime_error(errmsg.str());
    }

    return ss.str();
}

std::vector<std::string> Split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;

    if (s.empty()) {
        return tokens;  // empty list of tokens
    }

    std::string::size_type pos = 0;
    std::string::size_type end = s.find_first_of(delimiter, pos);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(pos, end - pos));
        pos = end + 1;
        end = s.find_first_of(delimiter, pos);
    }

    tokens.push_back(s.substr(pos));

    return tokens;
}

std::vector<std::string> Tokenize(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<std::string> tokens;

    for (;;) {
        std::string s;
        if (!(ss >> s))
            break;
        tokens.push_back(s);
    }

    return tokens;
}

}

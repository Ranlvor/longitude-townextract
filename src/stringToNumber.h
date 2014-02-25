#ifndef STRINGTONUMBER_H
#define STRINGTONUMBER_H
#include <sstream>

template <typename T>
T StringToNumber ( const std::string &Text )//Text not by const reference so that the function can be used with a
{                               //character array as argument
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

#endif // STRINGTONUMBER_H

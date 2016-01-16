#ifndef DEFLATE_H_INCLUDED
#define DEFLATE_H_INCLUDED

#include <string>

std::string compress_string(const std::string& str);
std::string decompress_string(const std::string& str);

#endif // DEFLATE_H_INCLUDED

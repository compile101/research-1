#pragma once
#include <string>
#include <vector>

std::string base64Encode(const std::string& input);
std::vector<std::string> chunkData(const std::string& data, size_t chunkSize = 200);



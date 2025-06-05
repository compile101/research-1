#pragma once
#include <string>

std::string dnsQueryTXT(const std::string& domain);
bool dnsSendChunk(const std::string& domain, const std::string& chunk);



#include "exfil_utils.h"
#include <windows.h>
#include <wincrypt.h>

#pragma comment(lib, "crypt32.lib")

// === Base64 encode using Windows Crypto API ===
std::string base64Encode(const std::string& input) {
    DWORD inputLen = static_cast<DWORD>(input.length());
    DWORD outputLen = 0;

    if (!CryptBinaryToStringA(reinterpret_cast<const BYTE*>(input.c_str()),
        inputLen,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        nullptr, &outputLen)) {
        return "";
    }

    std::string output(outputLen, '\0');
    if (!CryptBinaryToStringA(reinterpret_cast<const BYTE*>(input.c_str()),
        inputLen,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &output[0], &outputLen)) {
        return "";
    }

    output.resize(outputLen - 1); // Trim null terminator
    return output;
}

// === Chunk string into fixed-size DNS-safe segments ===
std::vector<std::string> chunkData(const std::string& data, size_t chunkSize) {
    std::vector<std::string> chunks;
    for (size_t i = 0; i < data.size(); i += chunkSize) {
        chunks.push_back(data.substr(i, chunkSize));
    }
    return chunks;
}

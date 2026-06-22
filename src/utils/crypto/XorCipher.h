#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H

#include <string>
#include <cstddef>

namespace XorCipher
{

    inline const std::string DEFAULT_KEY = "AEDS3-PUC-2026";

    inline void apply(char *data, size_t len, const std::string &key = DEFAULT_KEY)
    {
        if (key.empty())
            return;
        for (size_t i = 0; i < len; i++)
            data[i] = static_cast<char>(
                static_cast<unsigned char>(data[i]) ^
                static_cast<unsigned char>(key[i % key.size()]));
    }
}

#endif

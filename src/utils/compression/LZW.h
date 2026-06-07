#ifndef LZW_H
#define LZW_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cstddef>

class LZW
{
private:
    static const int MAX_CODE = 65535;

    static void putU16(std::vector<unsigned char> &out, uint16_t v)
    {
        out.push_back(static_cast<unsigned char>(v & 0xFF));
        out.push_back(static_cast<unsigned char>((v >> 8) & 0xFF));
    }

public:
    static std::vector<unsigned char> compress(const std::vector<unsigned char> &input)
    {
        std::vector<unsigned char> out;
        if (input.empty())
            return out;

        std::unordered_map<std::string, int> dict;
        dict.reserve(1 << 16);
        for (int i = 0; i < 256; i++)
            dict[std::string(1, static_cast<char>(i))] = i;
        int nextCode = 256;

        std::string current;
        current += static_cast<char>(input[0]);

        for (size_t i = 1; i < input.size(); i++)
        {
            char c = static_cast<char>(input[i]);
            std::string combined = current + c;
            if (dict.find(combined) != dict.end())
            {
                current = combined;
            }
            else
            {
                putU16(out, static_cast<uint16_t>(dict[current]));
                if (nextCode <= MAX_CODE)
                    dict[combined] = nextCode++;
                current = std::string(1, c);
            }
        }
        putU16(out, static_cast<uint16_t>(dict[current]));
        return out;
    }

    static std::vector<unsigned char> decompress(const std::vector<unsigned char> &input)
    {
        std::vector<unsigned char> out;
        if (input.size() < 2)
            return out;

        std::unordered_map<int, std::string> dict;
        dict.reserve(1 << 16);
        for (int i = 0; i < 256; i++)
            dict[i] = std::string(1, static_cast<char>(i));
        int nextCode = 256;

        size_t pos = 0;
        auto readCode = [&](int &code) -> bool
        {
            if (pos + 1 >= input.size())
                return false;
            code = input[pos] | (static_cast<int>(input[pos + 1]) << 8);
            pos += 2;
            return true;
        };

        int code;
        readCode(code);
        std::string previous = dict[code];
        out.insert(out.end(), previous.begin(), previous.end());

        while (readCode(code))
        {
            std::string entry;
            if (dict.find(code) != dict.end())
            {
                entry = dict[code];
            }
            else if (code == nextCode)
            {
                entry = previous + previous[0];
            }
            else
            {
                break;
            }

            out.insert(out.end(), entry.begin(), entry.end());

            if (nextCode <= MAX_CODE)
                dict[nextCode++] = previous + entry[0];

            previous = entry;
        }

        return out;
    }
};

#endif

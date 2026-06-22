#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <string>
#include <vector>
#include <array>
#include <cstddef>

namespace BoyerMoore
{

    inline std::array<int, 256> buildBadChar(const std::string &pattern)
    {
        std::array<int, 256> last;
        last.fill(-1);
        for (size_t i = 0; i < pattern.size(); i++)
            last[static_cast<unsigned char>(pattern[i])] = static_cast<int>(i);
        return last;
    }

    inline std::vector<int> search(const std::string &text, const std::string &pattern)
    {
        std::vector<int> matches;
        int n = static_cast<int>(text.size());
        int m = static_cast<int>(pattern.size());
        if (m == 0 || m > n)
            return matches;

        std::array<int, 256> last = buildBadChar(pattern);

        int s = 0;
        while (s <= n - m)
        {
            int j = m - 1;

            while (j >= 0 && pattern[j] == text[s + j])
                j--;

            if (j < 0)
            {

                matches.push_back(s);
                s += 1;
            }
            else
            {
                int lastOcc = last[static_cast<unsigned char>(text[s + j])];
                int shift = j - lastOcc;
                s += (shift > 0 ? shift : 1);
            }
        }
        return matches;
    }

    inline bool contains(const std::string &text, const std::string &pattern)
    {
        return !search(text, pattern).empty();
    }
}

#endif

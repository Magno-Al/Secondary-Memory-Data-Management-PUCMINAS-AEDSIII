#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>
#include <cstddef>

namespace KMP
{

    inline std::vector<int> computeLPS(const std::string &pattern)
    {
        std::vector<int> lps(pattern.size(), 0);
        int len = 0;
        for (size_t i = 1; i < pattern.size();)
        {
            if (pattern[i] == pattern[len])
            {
                lps[i++] = ++len;
            }
            else if (len > 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i++] = 0;
            }
        }
        return lps;
    }

    inline std::vector<int> search(const std::string &text, const std::string &pattern)
    {
        std::vector<int> matches;
        if (pattern.empty() || pattern.size() > text.size())
            return matches;

        std::vector<int> lps = computeLPS(pattern);
        size_t i = 0;
        size_t j = 0;
        while (i < text.size())
        {
            if (text[i] == pattern[j])
            {
                i++;
                j++;
                if (j == pattern.size())
                {
                    matches.push_back(static_cast<int>(i - j));
                    j = lps[j - 1];
                }
            }
            else if (j > 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i++;
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

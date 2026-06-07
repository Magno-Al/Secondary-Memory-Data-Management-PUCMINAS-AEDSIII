#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <vector>
#include <queue>
#include <cstdint>
#include <cstddef>
#include <array>

class Huffman
{
private:
    struct Node
    {
        int symbol;
        uint64_t freq;
        Node *left;
        Node *right;

        Node(int s, uint64_t f, Node *l = nullptr, Node *r = nullptr)
            : symbol(s), freq(f), left(l), right(r) {}
    };

    struct Compare
    {
        bool operator()(Node *a, Node *b) const
        {
            if (a->freq != b->freq)
                return a->freq > b->freq;
            return a->symbol > b->symbol;
        }
    };

    static Node *buildTree(const std::array<uint64_t, 256> &freq)
    {
        std::priority_queue<Node *, std::vector<Node *>, Compare> pq;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
                pq.push(new Node(i, freq[i]));
        }

        if (pq.empty())
            return nullptr;

        if (pq.size() == 1)
        {
            Node *only = pq.top();
            pq.pop();
            return new Node(-1, only->freq, only, nullptr);
        }

        while (pq.size() > 1)
        {
            Node *a = pq.top();
            pq.pop();
            Node *b = pq.top();
            pq.pop();
            pq.push(new Node(-1, a->freq + b->freq, a, b));
        }
        return pq.top();
    }

    static void freeTree(Node *n)
    {
        if (!n)
            return;
        freeTree(n->left);
        freeTree(n->right);
        delete n;
    }

    static void buildCodes(Node *n, std::vector<bool> &prefix, std::array<std::vector<bool>, 256> &codes)
    {
        if (!n)
            return;
        if (!n->left && !n->right)
        {
            codes[n->symbol] = prefix.empty() ? std::vector<bool>{false} : prefix;
            return;
        }
        prefix.push_back(false);
        buildCodes(n->left, prefix, codes);
        prefix.pop_back();

        prefix.push_back(true);
        buildCodes(n->right, prefix, codes);
        prefix.pop_back();
    }

    static void putU16(std::vector<unsigned char> &out, uint16_t v)
    {
        out.push_back(static_cast<unsigned char>(v & 0xFF));
        out.push_back(static_cast<unsigned char>((v >> 8) & 0xFF));
    }
    static void putU64(std::vector<unsigned char> &out, uint64_t v)
    {
        for (int i = 0; i < 8; i++)
            out.push_back(static_cast<unsigned char>((v >> (8 * i)) & 0xFF));
    }
    static uint16_t getU16(const std::vector<unsigned char> &in, size_t &pos)
    {
        uint16_t v = in[pos] | (static_cast<uint16_t>(in[pos + 1]) << 8);
        pos += 2;
        return v;
    }
    static uint64_t getU64(const std::vector<unsigned char> &in, size_t &pos)
    {
        uint64_t v = 0;
        for (int i = 0; i < 8; i++)
            v |= static_cast<uint64_t>(in[pos + i]) << (8 * i);
        pos += 8;
        return v;
    }

public:
    static std::vector<unsigned char> compress(const std::vector<unsigned char> &input)
    {
        std::vector<unsigned char> out;

        std::array<uint64_t, 256> freq{};
        for (unsigned char c : input)
            freq[c]++;

        putU64(out, static_cast<uint64_t>(input.size()));

        uint16_t numSymbols = 0;
        for (int i = 0; i < 256; i++)
            if (freq[i] > 0)
                numSymbols++;
        putU16(out, numSymbols);

        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
            {
                out.push_back(static_cast<unsigned char>(i));
                putU64(out, freq[i]);
            }
        }

        if (input.empty())
            return out;

        Node *root = buildTree(freq);
        std::array<std::vector<bool>, 256> codes;
        std::vector<bool> prefix;
        buildCodes(root, prefix, codes);

        unsigned char acc = 0;
        int bits = 0;
        for (unsigned char c : input)
        {
            for (bool bit : codes[c])
            {
                acc = static_cast<unsigned char>((acc << 1) | (bit ? 1 : 0));
                bits++;
                if (bits == 8)
                {
                    out.push_back(acc);
                    acc = 0;
                    bits = 0;
                }
            }
        }
        if (bits > 0)
        {
            acc = static_cast<unsigned char>(acc << (8 - bits));
            out.push_back(acc);
        }

        freeTree(root);
        return out;
    }

    static std::vector<unsigned char> decompress(const std::vector<unsigned char> &input)
    {
        std::vector<unsigned char> out;
        if (input.size() < 10)
            return out;

        size_t pos = 0;
        uint64_t originalSize = getU64(input, pos);
        uint16_t numSymbols = getU16(input, pos);

        std::array<uint64_t, 256> freq{};
        for (uint16_t i = 0; i < numSymbols; i++)
        {
            unsigned char sym = input[pos++];
            uint64_t f = getU64(input, pos);
            freq[sym] = f;
        }

        if (originalSize == 0)
            return out;

        Node *root = buildTree(freq);

        out.reserve(originalSize);
        Node *cur = root;
        uint64_t produced = 0;

        for (size_t i = pos; i < input.size() && produced < originalSize; i++)
        {
            unsigned char byte = input[i];
            for (int b = 7; b >= 0 && produced < originalSize; b--)
            {
                bool bit = (byte >> b) & 1;
                cur = bit ? cur->right : cur->left;
                if (cur->left == nullptr && cur->right == nullptr)
                {
                    out.push_back(static_cast<unsigned char>(cur->symbol));
                    produced++;
                    cur = root;
                }
            }
        }

        freeTree(root);
        return out;
    }
};

#endif

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include <iomanip>
#include <cmath>

void input(std::unordered_map<char, int> &freq)
{
    std::ifstream file("附件2.哈夫曼编码输入文本-23.txt");

    if (!file.is_open())
    {
        std::cout << "无法打开文件！" << std::endl;
        return;
    }

    char c;
    while (file.get(c))
    {
        if (freq.find(c) == freq.end())
        {
            freq[c] = 1;
        }
        else
        {
            freq[c]++;
        }
    }

    file.close();
}

class HuffmanNode
{
public:
    char character;
    int frequency;
    HuffmanNode *left;
    HuffmanNode *right;
    // 构造函数
    HuffmanNode(char c, int freq) : character(c), frequency(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes // 用于优先队列的比较函数, 使得优先队列中的节点按照频率从小到大排列
{
    bool operator()(HuffmanNode *a, HuffmanNode *b)
    {
        return a->frequency > b->frequency;
    }
};

HuffmanNode *buildTree(std::unordered_map<char, int> &freq)
{
    std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, CompareNodes> pq;
    for (auto &p : freq)
    {
        pq.push(new HuffmanNode(p.first, p.second));
    }

    while (pq.size() > 1)
    {
        HuffmanNode *left = pq.top();
        pq.pop();
        HuffmanNode *right = pq.top();
        pq.pop();
        HuffmanNode *parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    return pq.top();
}

void generateCodes(HuffmanNode *root, std::string code, std::unordered_map<char, std::string> &codes)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->left == nullptr && root->right == nullptr)
    {
        codes[root->character] = code;
        return;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

void printFrequenciesAndCodes(std::unordered_map<char, int> &freq, std::unordered_map<char, std::string> &codes)
{
    int totalBitsHuffman = 0;
    int totalBitsFixed = 0;
    int bitLength = ceil(log2(freq.size())); // 定长编码的二进制数的位数
    int count = 0;                           // 用于计算定长编码的二进制数

    std::cout << std::left << std::setw(8) << "字符" << std::setw(8) << "频率" << std::setw(16) << "哈夫曼编码" << std::setw(16) << "定长编码" << std::endl;
    for (auto &p : freq)
    {
        char character = p.first;
        int frequency = p.second;
        
        std::string huffmanCode = codes[character];
        std::string fixedCode = std::bitset<8>(count++).to_string();
        fixedCode = fixedCode.substr(fixedCode.length() - bitLength, bitLength);

        totalBitsHuffman += frequency * huffmanCode.length();
        totalBitsFixed += frequency * bitLength;

        if (character == '\n') // 换行符
        {
            std::cout << std::left << std::setw(8) << "\\n" << std::setw(8) << frequency << std::setw(16) << huffmanCode << std::setw(16) << fixedCode << std::endl;
            continue;
        }

        std::cout << std::left << std::setw(8) << character << std::setw(8) << frequency << std::setw(16) << huffmanCode << std::setw(16) << fixedCode << std::endl;
    }

    std::cout << "哈夫曼编码存储比特数：" << totalBitsHuffman << "比特" << std::endl;
    std::cout << "定长编码存储比特数：" << totalBitsFixed << "比特" << std::endl;
    std::cout << "哈夫曼编码相较定长编码节省空间" << (1 - (double)totalBitsHuffman / totalBitsFixed) * 100 << "%" << std::endl;
}

void encode(std::unordered_map<char, int> &freq, std::unordered_map<char, std::string> &codes)
{
    std::ifstream file("附件2.哈夫曼编码输入文本-23.txt");

    if (!file.is_open())
    {
        std::cout << "无法打开文件！" << std::endl;
        return;
    }

    std::ofstream fileEncoded("附件2.哈夫曼编码输出文本-23.txt");

    if (!fileEncoded.is_open())
    {
        std::cout << "无法打开文件！" << std::endl;
        return;
    }

    char c;
    while (file.get(c))
    {
        if (c == '\n') // 控制符不参与编码
        {
            fileEncoded << c;
            continue;
        }
        fileEncoded << codes[c];
    }

    file.close();
    fileEncoded.close();
}

int main()
{
    std::unordered_map<char, int> freq;
    input(freq);
    HuffmanNode *root = buildTree(freq);
    std::unordered_map<char, std::string> codes;
    generateCodes(root, "", codes);
    printFrequenciesAndCodes(freq, codes);
    encode(freq, codes);
    return 0;
}

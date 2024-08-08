#include<bits/stdc++.h>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr) 
        : ch(ch), freq(freq), left(left), right(right) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void generateHuffmanCodes(Node* root, const string& str, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = str;
    }

    generateHuffmanCodes(root->left, str + "0", huffmanCodes);
    generateHuffmanCodes(root->right, str + "1", huffmanCodes);
}

unordered_map<char, string> buildHuffmanTree(const string& text) {
    unordered_map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }

    Node* root = pq.top();
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    return huffmanCodes;
}

void saveHuffmanCodes(const unordered_map<char, string>& huffmanCodes, const string& huffmanCodesFilePath) {
    ofstream file(huffmanCodesFilePath);
    if (!file) {
        cerr << "Error: Could not open Huffman codes file for writing." << endl;
        return;
    }
    for (const auto& pair : huffmanCodes) {
        file << pair.first << " " << pair.second << endl;
    }
    file.close();
    cout << "Huffman codes saved to: " << huffmanCodesFilePath << endl;
}

unordered_map<string, char> loadHuffmanCodes(const string& huffmanCodesFilePath) {
    unordered_map<string, char> reverseHuffmanCodes;
    ifstream file(huffmanCodesFilePath);
    if (!file) {
        cerr << "Error: Could not open Huffman codes file for reading." << endl;
        return reverseHuffmanCodes;
    }

    char ch;
    string code;
    while (file.get(ch)) {
        file >> code;
        reverseHuffmanCodes[code] = ch;
        file.ignore(numeric_limits<streamsize>::max(), '\n'); // Handle remaining line
    }

    file.close();
    cout << "Huffman codes loaded from: " << huffmanCodesFilePath << endl;
    return reverseHuffmanCodes;
}

void compressFile(const string& inputFilePath, const string& compressedFilePath, const string& huffmanCodesFilePath) {
    ifstream inputFile(inputFilePath, ios::binary);
    if (!inputFile) {
        cerr << "Error: Could not open input file: " << inputFilePath << endl;
        return;
    }

    string text = "";
    char ch;
    while (inputFile.get(ch)) {
        text += ch;
    }
    inputFile.close();

    unordered_map<char, string> huffmanCodes = buildHuffmanTree(text);
    saveHuffmanCodes(huffmanCodes, huffmanCodesFilePath);

    string compressedBinary = "";
    for (char ch : text) {
        compressedBinary += huffmanCodes[ch];
    }

    int padding = 8 - (compressedBinary.size() % 8);
    for (int i = 0; i < padding; ++i) {
        compressedBinary += "0";
    }

    ofstream compressedFile(compressedFilePath, ios::binary);
    if (!compressedFile) {
        cerr << "Error: Could not open compressed file for writing: " << compressedFilePath << endl;
        return;
    }
    compressedFile.write(reinterpret_cast<char*>(&padding), sizeof(padding));

    for (size_t i = 0; i < compressedBinary.size(); i += 8) {
        bitset<8> byte(compressedBinary.substr(i, 8));
        char byteChar = static_cast<char>(byte.to_ulong());
        compressedFile.put(byteChar);
    }

    compressedFile.close();
    cout << "File compressed and saved to: " << compressedFilePath << endl;
}

string decompressFile(const string& compressedFilePath, const string& huffmanCodesFilePath) {
    unordered_map<string, char> reverseHuffmanCodes = loadHuffmanCodes(huffmanCodesFilePath);

    ifstream compressedFile(compressedFilePath, ios::binary);
    if (!compressedFile) {
        cerr << "Error: Could not open compressed file: " << compressedFilePath << endl;
        return "";
    }

    int padding;
    compressedFile.read(reinterpret_cast<char*>(&padding), sizeof(padding));

    char byteChar;
    string compressedBinary = "";
    while (compressedFile.get(byteChar)) {
        bitset<8> byteBits(static_cast<unsigned long>(byteChar));
        compressedBinary += byteBits.to_string();
    }

    compressedFile.close();

    compressedBinary = compressedBinary.substr(0, compressedBinary.size() - padding);
    string decompressedContent = "";
    string code = "";
    for (char bit : compressedBinary) {
        code += bit;
        if (reverseHuffmanCodes.count(code)) {
            decompressedContent += reverseHuffmanCodes[code];
            code = "";
        }
    }

    return decompressedContent;
}

string getFileNameBase(const string& filePath) {
    size_t lastSlash = filePath.find_last_of("\\/");
    string fileName = filePath.substr(lastSlash + 1);
    size_t compressedPos = fileName.find("_compressed.bin");
    if (compressedPos != string::npos) {
        return fileName.substr(0, compressedPos);
    }
    return fileName;
}

int main() {
    string choice;
    cout << "Enter 'c' to compress or 'd' to decompress: ";
    cin >> choice;
    cin.ignore();

    if (choice == "c") {
        string inputFilePath;
        cout << "Enter the file path to compress: ";

        getline(cin,inputFilePath);
        string baseFileName = getFileNameBase(inputFilePath);
        string compressedFilePath = baseFileName + "_compressed.bin";
        string huffmanCodesFilePath = baseFileName + "_HuffmanCodes.txt";

        compressFile(inputFilePath, compressedFilePath, huffmanCodesFilePath);
    } 
    else if (choice == "d") {
        string compressedFilePath;
        cout << "Enter the compressed file path to decompress: ";
        getline(cin, compressedFilePath);

        cout<<compressedFilePath<<endl;

        string baseFileName = getFileNameBase(compressedFilePath);
        string huffmanCodesFilePath = baseFileName + "_HuffmanCodes.txt";

        string decompressedContent = decompressFile(compressedFilePath, huffmanCodesFilePath);
        if (!decompressedContent.empty()) {
            ofstream decompressedFile(baseFileName + "_decompressed.txt");
            decompressedFile << decompressedContent;
            decompressedFile.close();
            cout << "File decompressed and saved to: " << baseFileName + "_decompressed.txt" << endl;
        } else {
            cout << "Decompression failed." << endl;
        }
    } 
    else {
        cout << "Invalid choice." << endl;
    }

    return 0;
}

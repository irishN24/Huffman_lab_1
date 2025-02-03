#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <bitset>

using namespace std;

class HuffmanTree {
private:
    class Node { //класс для узла дерева:символ, его частота и его потомки
    private:
        string m_symbols; //строка символов
        int m_frecuency; // частота 
        Node* m_Lchild; // левый потомок
        Node* m_Rchild; // правый потомок
    public:
        Node(string symbols, int frecuency): m_symbols(symbols), m_frecuency(frecuency), m_Lchild(nullptr), m_Rchild(nullptr){} //коструктор
        string getSymb() const { 
            return m_symbols; 
        }
        int getFrec() const {
            return m_frecuency;
        }
        Node* getL_Child() const { 
            return m_Lchild; 
        }
        Node* getR_Child() const {
            return m_Rchild;
        }
        void setL_Child(Node* node) { //установка левого потомка
            m_Lchild = node;
        }
        void setR_Child(Node* node) { //установка правого потомка
            m_Rchild = node;
        }
    };
    Node* m_root; //указатель на корень
    void buildT(const string& text); //строит дерево Хаффмана на частотах
    void createCode(Node* node, string code, unordered_map<char, string>& codes) const; // генерация уодов для символов
    void deleteTree(Node* node); //удаление узлов, начиная с node

public:
    HuffmanTree() : m_root(nullptr) {} //конструктор
    ~HuffmanTree() {                    //деструктор
        deleteTree(m_root);
    }
    void build(const string& text); //построение дерева Хаффмана
    string encode(const string& text) const; //кодирование
    string decode(const string& text) const; // декодирование
};

//построение дерева Хаффмана
void HuffmanTree::buildT(const string& text) {
    if (text.empty()) {
        return;
    }

    unordered_map<char, int> frecuencyM; //ассоциативный массив(символ : частота) 
    for (char ch : text) {
        frecuencyM[ch]++;
    }

    list<Node*> nodes; //список для указателей на узлы
                        //  [Node(символ, частота)...]
    for (auto& pair : frecuencyM) {
        nodes.push_back(new Node(string(1, pair.first), pair.second));
    }
    //сортировка узлов по возрастанию частот
    nodes.sort([](Node* a, Node* b) { return a->getFrec() < b->getFrec(); });

    //построение дерева
    while (nodes.size() > 1) {
        //берём два узла с наименьшими частотами
        Node* left = nodes.front();
        nodes.pop_front();
        Node* right = nodes.front();
        nodes.pop_front();

        //создаём родителя и добавляем его в список
        Node* parent = new Node(left->getSymb() + right->getSymb(), left->getFrec() + right->getFrec());
        parent->setL_Child(left);
        parent->setR_Child(right);
        nodes.push_back(parent);
        //сортировка 
        nodes.sort([](Node* a, Node* b) { return a->getFrec() < b->getFrec();});
    }
    //после выхода из цикла узел-родитель становится корнем дерева
    m_root = nodes.front();
}

void HuffmanTree::build(const string& text) {
    buildT(text);
}
//генерация кода для каждого символа(с помощью рекурсии)
void HuffmanTree::createCode(Node* node, string code, unordered_map<char, string>& codes) const {
    //если нет потомков у корня, сохраняется code в массиве codes, и рекурсия завершается
    if (node->getL_Child() == nullptr && node->getR_Child() == nullptr) {
        codes[node->getSymb()[0]] = code;
        return;
    }
    //рекурсия
    createCode(node->getL_Child(), code + "0", codes);
    createCode(node->getR_Child(), code + "1", codes);
}
void HuffmanTree::deleteTree(Node* node) {
    if (node) {
        deleteTree(node->getL_Child());
        deleteTree(node->getR_Child());
        delete node;
    }
}
//кодирование
string HuffmanTree::encode(const string& text) const {
    //создание кодов для символов
    unordered_map<char, string> codes;
    createCode(m_root, "", codes);

    //кодирование
    string encodeTxT; //кодированный текст
    for (char ch : text) {
        encodeTxT += codes[ch];
    }
    return encodeTxT;
}
//декодирование
string HuffmanTree::decode(const string& text) const {
    string decodedTxT; //декодированный текст
    Node* current = m_root;

    //декодирование
    for (char bit : text) {
        if (bit == '0') {
            current = current->getL_Child();
        }
        else {
            current = current->getR_Child();
        }
        if (current->getL_Child() == nullptr && current->getR_Child() == nullptr) {
            decodedTxT += current->getSymb();
            current = m_root;
        }
    }
    return decodedTxT;
}
//чтение из файла
string readFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error open file: " << filename << endl;
        return "";
    }
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return text;
}
//перевод бит в байты(для кодирования и сжатия)
vector<uint8_t> Bits_Bytes(const string& bit) {
    vector<uint8_t> bytes;
    uint8_t byte = 0;
    int counter = 0;
    for (char bit : bit) {
        byte = (byte << 1) | (bit == '1' ? 1 : 0);
        counter++;
        if (counter == 8) {
            bytes.push_back(byte);
            byte = 0;
            counter = 0;
        }
    }
    if (counter > 0) {
        byte <<= (8 - counter);
        bytes.push_back(byte);
    }
    return bytes;
}
//перевод байтов в биты(для декодирования)
string Bytes_Bits(const vector<uint8_t>& bytes) {
    string bits;
    for (uint8_t byte : bytes) {
        for (int i = 7; i >= 0; i--) {
            bits += (byte & (1 << i)) ? '1' : '0';
        }
    }
    return bits;
}

//запись закодированного текста в бинарный файл
void writeBinToFile(const string& F_name, const vector<uint8_t>& bytes) {
    ofstream file(F_name, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening the file for writing: " << F_name << "\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    file.close();
}
//чтение бинарного файла для декодирования
vector<uint8_t> readBinFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error open file: " << filename << endl;
        return {};
    }
    vector<uint8_t> bytes((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return bytes;
}
//запись в файл
void writeToFile(const string& F_name, const string& text) {
    ofstream file(F_name);
    if (!file.is_open()) {
        cerr << "Error opening the file for writing: " << F_name << "\n";
        return;
    }
    file << text;
    file.close();
}
int main(){
    HuffmanTree huffman_Tree;

    //чтение текста из файла
    string inputFile = "original.txt";
    string text = readFromFile(inputFile);
    if (text.empty()) {
        cerr << "The file is empty or the file could not be read." << "\n";
        return 1;
    }
    //построение дерева Хаффмана
    huffman_Tree.build(text);

    //кодирование
    string encodedTxT = huffman_Tree.encode(text);
    //сжатие и запись закодированного текста в бинарный файл
    vector<uint8_t> bytes = Bits_Bytes(encodedTxT);
    string encodeName = "encode.bin";
    writeBinToFile(encodeName, bytes);
    cout << "The encoded text is written to a bynary file: " << encodeName << "\n";

    //чтение бинарного файла
    vector<uint8_t> readBytes = readBinFromFile(encodeName);
    string readBits = Bytes_Bits(readBytes);
    //декодирование и запись текста в файл
    string decodeTxT = huffman_Tree.decode(readBits);
    string decodeName = "decode.txt";
    writeToFile(decodeName, decodeTxT);
    cout << "The decoded text is written to a file: " << decodeName << "\n";

    //удаление дерева происходит автоматически
    return 0;
}   


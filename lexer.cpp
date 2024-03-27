#include <iostream>
#include <fstream>
#include <vector>
#include <map> 
#include <string>
#include <regex>
using namespace std;

std::map<std::string, bool> reserved_keywords = {
    {"auto", true},
    {"break", true},
    {"case", true},
    {"char", true},
    {"const", true},
    {"continue", true},
    {"default", true},
    {"do", true},
    {"double", true},
    {"else", true},
    {"enum", true},
    {"extern", true},
    {"float", true},
    {"for", true},
    {"goto", true},
    {"if", true},
    {"int", true},
    {"long", true},
    {"register", true},
    {"return", true},
    {"short", true},
    {"signed", true},
    {"sizeof", true},
    {"static", true},
    {"struct", true},
    {"switch", true},
    {"typedef", true},
    {"union", true},
    {"unsigned", true},
    {"void", true},
    {"volatile", true},
    {"while", true}

};

bool is_reserved(const std::string& identifier) {
    // the find function returns an iterator containing the index in which it was found,
    // when not found, it returns the index of the element after the last
    // the end function also returns this , so when they're not equal (the find returned a different index)
    // the return value is true.
    return reserved_keywords.find(identifier) != reserved_keywords.end();           
}

enum TokenType {
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    PUNCTUATION,
    CONSTANT,
    COMMENT,
    CHAR_LITERAL,
    FORMAT_SPECIFIER,
    PREPROCESSOR,
    WHITESPACE,
    ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int value;
    int symbol_table_index;
};

std::ostream& operator<<(std::ostream& out, const Token& token) {
std::string str;
str += "<";
switch (token.type) {
    case KEYWORD:
str += "reserved keyword, name = " + token.lexeme;
break;
case IDENTIFIER:
str += "identifier, name = " + token.lexeme +
", symbol table index #" + std::to_string(token.symbol_table_index);
break;
case CONSTANT:
str += "number, value = " + std::to_string(token.value);
break;
case ADD_OP:
str += "addition arithmetic operator";
break;
case SUB_OP:
str += "subtract arithmetic operator";
break;
case MUL_OP:
str += "multiply arithmetic operator";
break;
}

    
vector<string> readFileChunks(const string& filename, streamsize chunkSize) {
    vector<string> chunks;
    ifstream file(filename, ios::binary);
    
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return chunks;
    }

    char buffer[chunkSize];
    while (file.read(buffer, chunkSize)) {
        chunks.push_back(std::string(buffer, file.gcount()));
    }

    if (file.gcount() > 0) {
        chunks.push_back(std::string(buffer, file.gcount()));
    }

    file.close();
    return chunks;
}

int main() {
    string filename = "your_file_path.txt";
    streamsize chunkSize = 1024; // Specify your desired chunk size

    vector<string> chunks = readFileChunks(filename, chunkSize);

    // Process each chunk as needed
    for (const auto& chunk : chunks) {
        // Process the chunk
        cout << "Chunk: " << chunk << endl;
    }

    return 0;
}

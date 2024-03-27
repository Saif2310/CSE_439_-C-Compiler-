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
    ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    INCREMENT,
    DECREMENT,
    ARROW,
    LOGICAL_AND,
    LOGICAL_OR,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
    EQUAL,
    NOT_EQUAL,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NOT,
    COMPLEMENT,
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
case ASSIGN:
            str += "assignment operator";
            break;
        case ADD_ASSIGN:
            str += "addition assignment operator";
            break;
        case SUB_ASSIGN:
            str += "subtraction assignment operator";
            break;
        case MUL_ASSIGN:
            str += "multiplication assignment operator";
            break;
        case DIV_ASSIGN:
            str += "division assignment operator";
            break;
        case MOD_ASSIGN:
            str += "modulo assignment operator";
            break;
        case INCREMENT:
            str += "increment operator";
            break;
        case DECREMENT:
            str += "decrement operator";
            break;
        case ARROW:
            str += "arrow operator";
            break;
        case LOGICAL_AND:
            str += "logical AND operator";
            break;
        case LOGICAL_OR:
            str += "logical OR operator";
            break;
        case LESS_THAN_OR_EQUAL:
            str += "less than or equal operator";
            break;
        case GREATER_THAN_OR_EQUAL:
            str += "greater than or equal operator";
            break;
        case EQUAL:
            str += "equality operator";
            break;
        case NOT_EQUAL:
            str += "not equal operator";
            break;
        case BITWISE_AND:
            str += "bitwise AND operator";
            break;
        case BITWISE_OR:
            str += "bitwise OR operator";
            break;
        case BITWISE_XOR:
            str += "bitwise XOR operator";
            break;
        case BITWISE_NOT:
            str += "bitwise NOT operator";
            break;
        case COMPLEMENT:
            str += "complement operator";
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

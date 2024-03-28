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
    LOGICAL_NOT,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
    EQUAL,
    NOT_EQUAL,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NOT,
    COMPLEMENT,

    INTEGER_CONSTANT,
    FLOATING_CONSTANT,
    CHARACTER_CONSTANT,
    STRING_CONSTANT,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOT,
    ARROW_OPERATOR,
    COMMA,
    SEMICOLON,
    COLON,
    ELLIPSIS,
    HASH,
    INCLUDE,
    DEFINE,
    UNDEF,
    IFDEF,
    IFNDEF,
    ELIF,
    ELSE,
    ENDIF,
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int int_value;  
    float float_value;  
    char char_value;  
    std::string str_value;  
    int symbol_table_index;  
};

std::ostream& operator<<(std::ostream& out, const Token& token) {
    std::string str;
    str += "<";
    switch (token.type) {
        case IDENTIFIER:
            str += "identifier, name = " + token.lexeme +
                   ", symbol table index #" + std::to_string(token.symbol_table_index);
            break;
        case INTEGER_CONSTANT:
            str += "integer constant, value = " + std::to_string(token.int_value);
            break;
        case FLOATING_CONSTANT:
            str += "floating-point constant, value = " + std::to_string(token.float_value);
            break;
        case CHARACTER_CONSTANT:
            str += "character constant, value = '" + std::string(1, token.char_value) + "'";
            break;
        case STRING_CONSTANT:
            str += "string constant, value = \"" + token.str_value + "\"";
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
        case LEFT_PAREN:
            str += "left parenthesis";
            break;
        case RIGHT_PAREN:
            str += "right parenthesis";
            break;
        case LEFT_BRACKET:
            str += "left bracket";
            break;
        case RIGHT_BRACKET:
            str += "right bracket";
            break;
        case LEFT_BRACE:
            str += "left brace";
            break;
        case RIGHT_BRACE:
            str += "right brace";
            break;
        case DOT:
            str += "dot operator";
            break;
        case COMMA:
            str += "comma";
            break;
        case SEMICOLON:
            str += "semicolon";
            break;
        case COLON:
            str += "colon";
            break;
        case ELLIPSIS:
            str += "ellipsis";
            break;
        case HASH:
            str += "hash";
            break;
        case INCLUDE:
            str += "include";
            break;
        case DEFINE:
            str += "define";
            break;
        case UNDEF:
            str += "undef";
            break;
        case IFDEF:
            str += "ifdef";
            break;
        case IFNDEF:
            str += "ifndef";
            break;
        case ELIF:
            str += "elif";
            break;
        case ELSE:
            str += "else";
            break;
        case ENDIF:
            str += "endif";
            break;
        case END_OF_FILE:
            str += "end-of-file";
            break;
        case ERROR:
            str += "erroneous token, content = " + token.lexeme;
            break;
        default:
            str += "unexpected token";
            break;
    }
    str += ">";
    out << str;
    return out;
}

    
vector<string> readFileChunks(const string& filename, streamsize chunkSize) {
    vector<string> chunks;
    ifstream file(filename, ios::binary);
    
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return chunks;
    }

    char buffer[1024]; //chunkSize
    while (file.read(buffer, chunkSize)) {
        chunks.push_back(std::string(buffer, file.gcount()));
    }

    if (file.gcount() > 0) {
        chunks.push_back(std::string(buffer, file.gcount()));
    }

    file.close();
    return chunks;
}

typedef std::vector<std::pair<std::string, std::vector<int>>> symbol_table_t;
symbol_table_t symbol_table;

int add_to_symbol_table(const std::string& token_name, int offset) {
    int index = 0;
    for (const auto& symbol : symbol_table) {
        if (token_name == symbol.first) {
            symbol_table[index].second.push_back(offset);
            return index;
        }
        index++;
    }

    symbol_table.push_back({ token_name, { offset } });
    return index;
}

std::ostream& operator<<(std::ostream& out, const symbol_table_t& st) {
    std::string str = "\n\nSYMBOL TABLE\n\n";
    int index = 0;
    for (const auto& symbol : st) {
        str += std::to_string(index++) + "- Symbol: " + symbol.first + ", at offset(s): ";
        for (auto offset : symbol.second) {
            str += std::to_string(offset) + " ";
        }
        str += "\n";
    }
    out << str;
    return out;
}

bool get_next_token_regex(std::string& buffer, Token*& token) {
    static std::regex my_regex("\\s*([a-zA-Z_][a-zA-Z0-9_]*|\\d+(\\.\\d+)?([eE][-+]?\\d+)?)|(==|!=|<=|>=|&&|\\|\\||[+\\-*/%&|<>=!^~])|([(){}[\\],;.:])|([\"'])(?:(?=(\\\\?))\\2.)*?\\7");
    static std::sregex_iterator my_regex_iterator(buffer.begin(), buffer.end(), my_regex);
    static std::sregex_iterator end;

    if (my_regex_iterator == end) {
        return false; // No more tokens
    }

    std::smatch match = *my_regex_iterator;
    std::string token_str = match.str();
    int token_length = token_str.length();
    token->lexeme_begin = static_cast<int>(match.position());
    token->lexeme = token_str;

    if (std::regex_match(token_str, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
        if (is_reserved(token_str)) {
            token->type = KEYWORD;
        } else {
            token->type = IDENTIFIER;
        }
    } else if (std::regex_match(token_str, std::regex("\\d+(\\.\\d+)?([eE][-+]?\\d+)?"))) {
    if (token_str.find('.') != std::string::npos) {
        token->type = FLOATING_CONSTANT;
        token->float_value = std::stof(token_str); // Convert to float
    } else {
        token->type = INTEGER_CONSTANT;
        token->int_value = std::stoi(token_str); // Convert to int
    }
} else if (token_length == 1) {
        switch (token_str[0]) {
            case '=': token->type = ASSIGN; break;
            case '+': token->type = ADD_ASSIGN; break;
            case '-': token->type = SUB_ASSIGN; break;
            case '*': token->type = MUL_ASSIGN; break;
            case '/': token->type = DIV_ASSIGN; break;
            case '%': token->type = MOD_ASSIGN; break;
            case '<': token->type = LEFT_SHIFT; break;
            case '>': token->type = RIGHT_SHIFT; break;
            case '&': token->type = BITWISE_AND; break;
            case '|': token->type = BITWISE_OR; break;
            case '^': token->type = BITWISE_XOR; break;
            case '!': token->type = LOGICAL_NOT; break;
            case '~': token->type = BITWISE_NOT; break;
            case '(': token->type = LEFT_PAREN; break;
            case ')': token->type = RIGHT_PAREN; break;
            case '[': token->type = LEFT_BRACKET; break;
            case ']': token->type = RIGHT_BRACKET; break;
            case '{': token->type = LEFT_BRACE; break;
            case '}': token->type = RIGHT_BRACE; break;
            case ',': token->type = COMMA; break;
            case ';': token->type = SEMICOLON; break;
            case ':': token->type = COLON; break;
            case '.': token->type = DOT; break;
            case '"': token->type = STRING_CONSTANT; break;
            case '\'': token->type = CHARACTER_CONSTANT; break;
            default: token->type = ERROR; break;
        }
    } else {
        token->type = ERROR;
    }

    ++my_regex_iterator;
    return true;
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

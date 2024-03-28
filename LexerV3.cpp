#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>

using namespace std;

// Reserved keywords
std::map<std::string, bool> reserved_keywords = {
    {"auto", true},     {"break", true},   {"case", true},     {"char", true},
    {"const", true},    {"continue", true},{"default", true},  {"do", true},
    {"double", true},   {"else", true},    {"enum", true},     {"extern", true},
    {"float", true},    {"for", true},     {"goto", true},     {"if", true},
    {"int", true},      {"long", true},    {"register", true}, {"return", true},
    {"short", true},    {"signed", true},  {"sizeof", true},   {"static", true},
    {"struct", true},   {"switch", true},  {"typedef", true},  {"union", true},
    {"unsigned", true}, {"void", true},    {"volatile", true}, {"while", true}
};

bool is_reserved(const std::string& identifier) {
    return reserved_keywords.find(identifier) != reserved_keywords.end();
}

enum TokenType {
    IDENTIFIER, KEYWORD, ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
    LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT, ARROW, LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
    LESS_THAN_OR_EQUAL, GREATER_THAN_OR_EQUAL, EQUAL, NOT_EQUAL, BITWISE_AND, BITWISE_OR, BITWISE_XOR,
    BITWISE_NOT, COMPLEMENT, INTEGER_CONSTANT, FLOATING_CONSTANT, CHARACTER_CONSTANT, STRING_CONSTANT,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE, DOT, COMMA, SEMICOLON,
    COLON, ELLIPSIS, HASH, INCLUDE, DEFINE, UNDEF, IFDEF, IFNDEF, ELIF, ELSE, ENDIF, END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int int_value;
    float float_value;
    char char_value;
    std::string str_value;
    int symbol_table_index;
    int line_number;
    int column_number;
    int lexeme_begin;
};

std::ostream& operator<<(std::ostream& out, const Token& token) {
    std::string str;
    str += "<";
    switch (token.type) {
    case IDENTIFIER:
        str += "identifier, name = " + token.lexeme +
            ", symbol table index #" + std::to_string(token.symbol_table_index) +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
        break;
    case INTEGER_CONSTANT:
        str += "integer constant, value = " + std::to_string(token.int_value) +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
        break;
    case FLOATING_CONSTANT:
        str += "floating-point constant, value = " + std::to_string(token.float_value) +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
        break;
    case CHARACTER_CONSTANT:
        str += "character constant, value = '" + std::string(1, token.char_value) + "'" +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
        break;
    case STRING_CONSTANT:
        str += "string constant, value = \"" + token.str_value + "\"" +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
        break;
        // Other token types...
    case ERROR:
        str += "erroneous token, content = " + token.lexeme +
            ", line " + std::to_string(token.line_number) + ", column " + std::to_string(token.column_number);
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

    char buffer[1024];
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

int add_to_symbol_table(const std::string& token_name, int offset, int line_number, int column_number) {
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

bool get_next_token_regex(std::string& buffer, Token* token) {
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
    token->line_number = 1; // Update line and column numbers
    token->column_number = token->lexeme_begin + 1;
    if (std::regex_match(token_str, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
        if (is_reserved(token_str)) {
            token->type = KEYWORD;
        }
        else {
            token->type = IDENTIFIER;
            token->symbol_table_index = add_to_symbol_table(token_str, token->lexeme_begin, token->line_number, token->column_number);
        }
    }
    else if (std::regex_match(token_str, std::regex("\\d+(\\.\\d+)?([eE][-+]?\\d+)?"))) {
        if (token_str.find('.') != std::string::npos) {
            token->type = FLOATING_CONSTANT;
            token->float_value = std::stof(token_str);
        }
        else {
            token->type = INTEGER_CONSTANT;
            token->int_value = std::stoi(token_str);
        }
    }
    else if (token_length == 1) {
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
    }
    else {
        token->type = ERROR;
    }

    ++my_regex_iterator;
    return true;
}

int main() {
    string filename = "C:\\Users\\mdzhs\\OneDrive\\Documents\\Programming Projects\\LexicalAnalyzer\\test1.txt";
    streamsize chunkSize = 1024;

    vector<string> chunks = readFileChunks(filename, chunkSize);

    // Process each chunk
    for (const auto& chunk : chunks) {
        std::string buffer = chunk;
        Token token;
        while (get_next_token_regex(buffer, &token)) {
            if (token.type == ERROR) {
                std::cerr << "Lexer Error: Unrecognized token " << token.lexeme << " at line " << token.line_number << ", column " << token.column_number << std::endl;
                // Handle error, such as skipping the token or stopping further processing
            }
            else {
                std::cout << "Token: " << token << std::endl;
            }
        }
    }

    std::cout << symbol_table << std::endl;

    return 0;
}

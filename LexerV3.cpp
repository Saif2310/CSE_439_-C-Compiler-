#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>

using namespace std;

// Reserved keywords
std::map<std::string, bool> reserved_keywords = {
    {"auto", true}, {"break", true}, {"case", true}, {"char", true},
    {"const", true}, {"continue", true}, {"default", true}, {"do", true},
    {"double", true}, {"else", true}, {"enum", true}, {"extern", true},
    {"float", true}, {"for", true}, {"goto", true}, {"if", true},
    {"int", true}, {"long", true}, {"register", true}, {"return", true},
    {"short", true}, {"signed", true}, {"sizeof", true}, {"static", true},
    {"struct", true}, {"switch", true}, {"typedef", true}, {"union", true},
    {"unsigned", true}, {"void", true}, {"volatile", true}, {"while", true}
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
    COLON, ELLIPSIS, HASH, INCLUDE, DEFINE, UNDEF, IFDEF, IFNDEF, ELIF, ELSE, ENDIF, END_OF_FILE, ERROR, PREPROCESSOR_INCLUDE,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int int_value;
    float float_value;
    char char_value;
    std::string str_value;
    int symbol_table_index;
    int line_number = 1; // Initialize line number to 1
    int column_number;
    int lexeme_begin;
    std::string string_value;
};



vector<string> readFileChunks(const string& filename) {
    vector<string> chunks;
    ifstream file(filename);
    string line;
    const int chunkSize = 1024; // You can adjust chunk size as needed

    if (file.is_open()) {
        while (getline(file, line)) {
            for (size_t i = 0; i < line.size(); i += chunkSize) {
                chunks.push_back(line.substr(i, chunkSize));
            }
        }
        file.close();
    }

    return chunks;
}
// Symbol table typedef
typedef vector<pair<string, vector<int>>> symbol_table_t;

// Global symbol table
symbol_table_t symbol_table;

// Function to add a token to the symbol table
int add_to_symbol_table(const std::string& token_name, int offset, int line_number, int column_number) {
    symbol_table.push_back(make_pair(token_name, vector<int>{offset, line_number, column_number}));
    return symbol_table.size() - 1; // Return index of the added token in the symbol table
}

// Overload of operator<< for Token struct
std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << "Type: " << token.type << ", Lexeme: " << token.lexeme << ", Line: " << token.line_number << ", Column: " << token.column_number;
    // Output more details of the token as needed
    return out;
}

// Overload of operator<< for symbol_table_t
std::ostream& operator<<(std::ostream& out, const symbol_table_t& st) {
    for (const auto& entry : st) {
        out << "Token: " << entry.first << ", Occurrences: ";
        for (int i : entry.second) {
            out << i << " ";
        }
        out << endl;
    }
    return out;
}

bool get_next_token_regex(std::string& buffer, Token* token) {
    static std::regex my_regex("\\s*([a-zA-Z_][a-zA-Z0-9_]*|\\d+(\\.\\d+)?([eE][-+]?\\d+)?|'(\\\\.|[^\\\\])'|\"(\\\\.|[^\"])*\")|"
        "(==|!=|<=|>=|&&|\\|\\||[+\\-*/%&|<>=!^~])|"
        "([(){}[\\],;.:])");
    std::smatch match;

    // Check if the buffer is empty
    if (buffer.empty()) {
        return false; // No more tokens
    }

    if (!std::regex_search(buffer, match, my_regex)) {
        buffer.clear(); // Clear the buffer to indicate no more tokens
        return false; // No more tokens
    }

    std::string token_str = match.str();
    int token_length = token_str.length();
    token->lexeme_begin = static_cast<int>(match.position());
    token->lexeme = token_str;

    // Update line number for each token (assuming newlines are `\n`)
    size_t newline_pos = token_str.find('\n');
    if (newline_pos != string::npos) {
        token->line_number++;
        token->column_number = static_cast<int>(newline_pos);
    }
    else {
        token->column_number = token->lexeme_begin;
    }

    // Determine token type
    if (is_reserved(token_str)) {
        token->type = KEYWORD;
    }
    else if (std::regex_match(token_str, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
        token->type = IDENTIFIER;
    }
    else if (std::regex_match(token_str, std::regex("\\d+(\\.\\d+)?([eE][-+]?\\d+)?"))) {
        if (token_str.find('.') != std::string::npos || token_str.find('e') != std::string::npos || token_str.find('E') != std::string::npos) {
            token->type = FLOATING_CONSTANT;
            token->float_value = std::stof(token_str);
        }
        else {
            token->type = INTEGER_CONSTANT;
            token->int_value = std::stoi(token_str);
        }
    }
    else if (std::regex_match(token_str, std::regex("'(\\\\.|[^\\\\])'")) && token_str.length() == 3) {
        token->type = CHARACTER_CONSTANT;
        token->char_value = token_str[1];
    }
    else if (std::regex_match(token_str, std::regex("\"(\\\\.|[^\"])*\"")) && token_str.length() >= 2) {
        token->type = STRING_CONSTANT;
        token->str_value = token_str.substr(1, token_str.length() - 2);
    }
    else {
        // Handle other token types (operators, punctuation, etc.)
        // Add more cases as needed
        if (token_str == "=") {
            token->type = ASSIGN;
        }
        else if (token_str == "+") {
            token->type = ADD_ASSIGN;
        }
        else if (token_str == "-") {
            token->type = SUB_ASSIGN;
        }
        else if (token_str == "*") {
            token->type = MUL_ASSIGN;
        }
        else if (token_str == "/") {
            token->type = DIV_ASSIGN;
        }
        else if (token_str == "%") {
            token->type = MOD_ASSIGN;
        }
        else if (token_str == "<<") {
            token->type = LEFT_SHIFT;
        }
        else if (token_str == ">>") {
            token->type = RIGHT_SHIFT;
        }
        else if (token_str == "++") {
            token->type = INCREMENT;
        }
        else if (token_str == "--") {
            token->type = DECREMENT;
        }
        else if (token_str == "->") {
            token->type = ARROW;
        }
        else if (token_str == "&&") {
            token->type = LOGICAL_AND;
        }
        else if (token_str == "||") {
            token->type = LOGICAL_OR;
        }
        else if (token_str == "!") {
            token->type = LOGICAL_NOT;
        }
        else if (token_str == "<=") {
            token->type = LESS_THAN_OR_EQUAL;
        }
        else if (token_str == ">=") {
            token->type = GREATER_THAN_OR_EQUAL;
        }
        else if (token_str == "==") {
            token->type = EQUAL;
        }
        else if (token_str == "!=") {
            token->type = NOT_EQUAL;
        }
        else if (token_str == "&") {
            token->type = BITWISE_AND;
        }
        else if (token_str == "|") {
            token->type = BITWISE_OR;
        }
        else if (token_str == "^") {
            token->type = BITWISE_XOR;
        }
        else if (token_str == "~") {
            token->type = BITWISE_NOT;
        }
        else if (token_str == "!") {
            token->type = COMPLEMENT;
        }
        else if (token_str == "(") {
            token->type = LEFT_PAREN;
        }
        else if (token_str == ")") {
            token->type = RIGHT_PAREN;
        }
        else if (token_str == "[") {
            token->type = LEFT_BRACKET;
        }
        else if (token_str == "]") {
            token->type = RIGHT_BRACKET;
        }
        else if (token_str == "{") {
            token->type = LEFT_BRACE;
        }
        else if (token_str == "}") {
            token->type = RIGHT_BRACE;
        }
        else if (token_str == ".") {
            token->type = DOT;
        }
        else if (token_str == ",") {
            token->type = COMMA;
        }
        else if (token_str == ";") {
            token->type = SEMICOLON;
        }
        else if (token_str == ":") {
            token->type = COLON;
        }
        else if (token_str == "...") {
            token->type = ELLIPSIS;
        }
        else {
            token->type = ERROR;
        }
    }

    buffer = match.suffix();
    return true;
}


int main(int argc, char* argv[]) {
    std::string filename = "test1.txt"; // argv[1]

    // Read file content in chunks for better memory management
    vector<string> chunks = readFileChunks(filename);

    // Symbol table for storing identifiers and their occurrences
    symbol_table_t symbol_table;

    // Process each chunk of the file
    for (const auto& chunk : chunks) {
        std::string buffer = chunk;
        Token token;
        while (get_next_token_regex(buffer, &token)) {
            if (token.type == ERROR) {
                std::cerr << "Lexer Error: Unrecognized token " << token.lexeme << " at line " << token.line_number << ", column " << token.column_number << std::endl;
                // Handle error (e.g., skip token, provide more informative message)
            }
            else {
                std::cout << "Token: " << token << std::endl;

                // Further processing based on token type (optional)
                switch (token.type) {
                case IDENTIFIER:
                    // Check if identifier is a keyword
                    if (is_reserved(token.lexeme)) {
                        token.type = KEYWORD;
                    }
                    else {
                        // Add identifier to symbol table
                        int symbol_table_index = add_to_symbol_table(token.lexeme, token.lexeme_begin, token.line_number, token.column_number);
                        token.symbol_table_index = symbol_table_index;
                    }
                    break;
                case INTEGER_CONSTANT:
                case FLOATING_CONSTANT:
                case CHARACTER_CONSTANT:
                case STRING_CONSTANT:
                    // Further processing for numeric or string literals (optional)
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Print symbol table (optional)
    std::cout << symbol_table;

    return 0;
}

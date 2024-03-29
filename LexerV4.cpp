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

bool is_reserved(const std::string& lexeme) {
    // Add your list of reserved keywords here
    static const std::vector<std::string> reserved_keywords = { "if", "else", "while", "for", "int", "float", "char", "return" };
    for (const auto& keyword : reserved_keywords) {
        if (lexeme == keyword) {
            return true;
        }
    }
    return false;
}

enum TokenType {
    IDENTIFIER, KEYWORD, ASSIGN, ADD, SUB, MUL, DIV, MOD,
    LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT, ARROW, LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
    LESS_THAN, GREATER_THAN, LESS_THAN_OR_EQUAL, GREATER_THAN_OR_EQUAL, EQUAL, NOT_EQUAL, BITWISE_AND, BITWISE_OR, BITWISE_XOR,
    BITWISE_NOT, COMPLEMENT, INTEGER_CONSTANT, FLOATING_CONSTANT, CHARACTER_CONSTANT, STRING_CONSTANT,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE, DOT, COMMA, SEMICOLON,
    COLON, ELLIPSIS, HASH, INCLUDE, DEFINE, UNDEF, IFDEF, IFNDEF, ELIF, ELSE, ENDIF, END_OF_FILE, ERROR, PREPROCESSOR_INCLUDE,
};

struct Token {
    int type;
    std::string lexeme;
    int line_number;
    int column_number;
    int lexeme_begin;
    int symbol_table_index;
    int int_value;
    float float_value;
    char char_value;
    std::string str_value;
};

vector<string> readFileChunks(const string& filename) {
    vector<string> chunks;
    ifstream file(filename);
    string line;
    int line_number = 1;
    int column_number = 1;

    if (file.is_open()) {
        while (getline(file, line)) {
            for (size_t i = 0; i < line.size(); i++) {
                chunks.push_back(string(1, line[i]));
                column_number++;
                if (line[i] == '\n') {
                    line_number++;
                    column_number = 1;
                }
            }
            chunks.push_back("\n"); // Add a newline character after each line
            line_number++;
            column_number = 1;
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
    out << "Token: Type: " << token.type << ", Lexeme: " << token.lexeme << ", Line: " << token.line_number << ", Column: " << token.column_number;
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

bool get_next_token(std::string& buffer, Token* token) {
    // Utilize a simple character-by-character parsing approach
    if (buffer.empty()) {
        return false; // No more tokens
    }

    // Skip whitespace
    while (!buffer.empty() && isspace(buffer[0])) {
        if (buffer[0] == '\n') {
            token->line_number++;
            token->column_number = 1;
            cout << "Skipping newline, line: " << token->line_number << ", column: " << token->column_number << endl;
        }
        else {
            token->column_number++;
            cout << "Skipping whitespace, column: " << token->column_number << endl;
        }
        buffer.erase(0, 1);
    }

    if (buffer.empty()) {
        return false; // No more tokens
    }

    token->lexeme_begin = token->column_number;
    //cout << "Parsing token, line: " << token->line_number << ", column: " << token->lexeme_begin << endl;

    // Determine the token type
    if (isalpha(buffer[0]) || buffer[0] == '_') {
        // Identifier or keyword
        size_t i = 1;
        while (i < buffer.size() && (isalnum(buffer[i]) || buffer[i] == '_')) {
            i++;
        }
        token->lexeme = buffer.substr(0, i);
        buffer.erase(0, i);
        token->column_number += static_cast<int>(i);

        if (is_reserved(token->lexeme)) {
            token->type = KEYWORD;
        }
        else {
            token->type = IDENTIFIER;
            int symbol_table_index = add_to_symbol_table(token->lexeme, token->lexeme_begin, token->line_number, token->column_number - static_cast<int>(token->lexeme.length()));
            token->symbol_table_index = symbol_table_index;
        }
    }
    else if (isdigit(buffer[0])) {
        // Integer or floating-point constant
        size_t i = 1;
        bool is_float = false;
        while (i < buffer.size() && (isdigit(buffer[i]) || buffer[i] == '.')) {
            if (buffer[i] == '.') {
                is_float = true;
            }
            i++;
        }
        token->lexeme = buffer.substr(0, i);
        buffer.erase(0, i);
        token->column_number += static_cast<int>(i);

        if (is_float) {
            token->type = FLOATING_CONSTANT;
            token->float_value = stof(token->lexeme);
        }
        else {
            token->type = INTEGER_CONSTANT;
            token->int_value = stoi(token->lexeme);
        }
    }
    else if (buffer[0] == '\'') {
        // Character constant
        size_t i = 1;
        if (i < buffer.size() && buffer[i] != '\'') {
            token->char_value = buffer[i];
            i++;
            if (i < buffer.size() && buffer[i] == '\'') {
                i++;
                token->type = CHARACTER_CONSTANT;
                token->lexeme = buffer.substr(0, i);
                buffer.erase(0, i);
                token->column_number += static_cast<int>(i);
            }
            else {
                token->type = ERROR;
                token->lexeme = "Unterminated character constant";
                buffer.erase(0, i);
                token->column_number += static_cast<int>(i);
            }
        }
        else {
            token->type = ERROR;
            token->lexeme = "Empty character constant";
            buffer.erase(0, 1);
            token->column_number += 1;
        }
    }
    else if (buffer[0] == '"') {
        // String constant
        size_t i = 1;
        while (i < buffer.size() && buffer[i] != '"') {
            i++;
        }
        if (i < buffer.size() && buffer[i] == '"') {
            token->str_value = buffer.substr(1, i - 1);
            i++;
            token->type = STRING_CONSTANT;
            token->lexeme = buffer.substr(0, i);
            buffer.erase(0, i);
            token->column_number += static_cast<int>(i);
        }
        else {
            token->type = ERROR;
            token->lexeme = "Unterminated string constant";
            buffer.erase(0, 1);
            token->column_number += 1;
        }
    }
    else {
        // Single-character tokens
        switch (buffer[0]) {
        case '=':
            token->type = ASSIGN;
            token->lexeme = "=";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '.':
            token->type = DOT;
            token->lexeme = ".";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '#':
            token->type = HASH;
            token->lexeme = ".";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '+':
            token->type = ADD;
            token->lexeme = "+";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '-':
            token->type = SUB;
            token->lexeme = "-";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '*':
            token->type = MUL;
            token->lexeme = "*";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '/':
            token->type = DIV;
            token->lexeme = "/";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '%':
            token->type = MOD;
            token->lexeme = "%";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '<':
            token->type = LESS_THAN;
            token->lexeme = "<";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '>':
            token->type = GREATER_THAN;
            token->lexeme = ">";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '(':
            token->type = LEFT_PAREN;
            token->lexeme = "(";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case ')':
            token->type = RIGHT_PAREN;
            token->lexeme = ")";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '{':
            token->type = LEFT_BRACE;
            token->lexeme = "{";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '}':
            token->type = RIGHT_BRACE;
            token->lexeme = "}";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case ';':
            token->type = SEMICOLON;
            token->lexeme = ";";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case ',':
            token->type = COMMA;
            token->lexeme = ",";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        
        default:
            token->type = ERROR;
            token->lexeme = "Unknown token";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        }
    }

    return true;
}

int main() {
    std::vector<std::string> file_chunks = readFileChunks("test1.txt");
    std::string buffer;
    for (const auto& chunk : file_chunks) {
        buffer += chunk;
    }

    Token token;
    token.line_number = 1;
    token.column_number = 1;

    while (get_next_token(buffer, &token)) {
        std::cout << token << std::endl;
    }

    std::cout << symbol_table << std::endl;

    return 0;
}

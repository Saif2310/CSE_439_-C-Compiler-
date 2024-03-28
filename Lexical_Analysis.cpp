#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
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
bool is_reserved(const std::string &identifier)
{
    // the find function returns an iterator containing the index in which it was found,
    // when not found, it returns the index of the element after the last
    // the end function also returns this , so when they're not equal (the find returned a different index)
    // the return value is true.
    return reserved_keywords.find(identifier) != reserved_keywords.end();
}
enum TokenType
{
    COMMENT,
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

struct Token
{
    TokenType type;
    std::string lexeme;
};

std::string tokenTypeToString(TokenType type)
{
    static std::map<TokenType, std::string> tokenTypeMap = {
        {COMMENT, "COMMENT"},
        {IDENTIFIER, "IDENTIFIER"},
        {KEYWORD, "KEYWORD"},
        {ASSIGN, "ASSIGN"},
        {ADD_ASSIGN, "ADD_ASSIGN"},
        {SUB_ASSIGN, "SUB_ASSIGN"},
        {MUL_ASSIGN, "MUL_ASSIGN"},
        {DIV_ASSIGN, "DIV_ASSIGN"},
        {MOD_ASSIGN, "MOD_ASSIGN"},
        {LEFT_SHIFT, "LEFT_SHIFT"},
        {RIGHT_SHIFT, "RIGHT_SHIFT"},
        {INCREMENT, "INCREMENT"},
        {DECREMENT, "DECREMENT"},
        {ARROW, "ARROW"},
        {LOGICAL_AND, "LOGICAL_AND"},
        {LOGICAL_OR, "LOGICAL_OR"},
        {LOGICAL_NOT, "LOGICAL_NOT"},
        {LESS_THAN_OR_EQUAL, "LESS_THAN_OR_EQUAL"},
        {GREATER_THAN_OR_EQUAL, "GREATER_THAN_OR_EQUAL"},
        {EQUAL, "EQUAL"},
        {NOT_EQUAL, "NOT_EQUAL"},
        {BITWISE_AND, "BITWISE_AND"},
        {BITWISE_OR, "BITWISE_OR"},
        {BITWISE_XOR, "BITWISE_XOR"},
        {BITWISE_NOT, "BITWISE_NOT"},
        {COMPLEMENT, "COMPLEMENT"},
        {INTEGER_CONSTANT, "INTEGER_CONSTANT"},
        {FLOATING_CONSTANT, "FLOATING_CONSTANT"},
        {CHARACTER_CONSTANT, "CHARACTER_CONSTANT"},
        {STRING_CONSTANT, "STRING_CONSTANT"},
        {LEFT_PAREN, "LEFT_PAREN"},
        {RIGHT_PAREN, "RIGHT_PAREN"},
        {LEFT_BRACKET, "LEFT_BRACKET"},
        {RIGHT_BRACKET, "RIGHT_BRACKET"},
        {LEFT_BRACE, "LEFT_BRACE"},
        {RIGHT_BRACE, "RIGHT_BRACE"},
        {DOT, "DOT"},
        {ARROW_OPERATOR, "ARROW_OPERATOR"},
        {COMMA, "COMMA"},
        {SEMICOLON, "SEMICOLON"},
        {COLON, "COLON"},
        {ELLIPSIS, "ELLIPSIS"},
        {HASH, "HASH"},
        {INCLUDE, "INCLUDE"},
        {DEFINE, "DEFINE"},
        {UNDEF, "UNDEF"},
        {IFDEF, "IFDEF"},
        {IFNDEF, "IFNDEF"},
        {ELIF, "ELIF"},
        {ELSE, "ELSE"},
        {ENDIF, "ENDIF"},
        {END_OF_FILE, "END_OF_FILE"},
        {ERROR, "ERROR"}};

    auto it = tokenTypeMap.find(type);
    if (it != tokenTypeMap.end())
    {
        return it->second;
    }
    return "UNKNOWN";
}

void read_file(std::string filename, std::string &buffer)
{
    std::ifstream file(filename);
    std::stringstream ss;
    ss << file.rdbuf(); // Read the entire file into the stringstream
    file.close();
    buffer = ss.str(); // Copy the stringstream contents to the buffer
}
// Define token patterns
std::vector<std::pair<TokenType, std::string>> patterns = {
    {COMMENT, ("\\/\\/.*|\\/\\*([\\s\\S]*?)\\*\\/")},
    {INCLUDE, "include"},
    {DEFINE, "define"},
    {UNDEF, "undef"},
    {IFDEF, "ifdef"},
    {IFNDEF, "ifndef"},
    {ELIF, "elif"},
    {ELSE, "else"},
    {ENDIF, "endif"},
    {KEYWORD, "\\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\\b"},
    {IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*"},
    {INTEGER_CONSTANT, "\\d+"},
    // {KEYWORD, "(auto|break|sizeof|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)"},
    {ASSIGN, "="},
    {ADD_ASSIGN, "\\+="},
    {SUB_ASSIGN, "-="},
    {MUL_ASSIGN, "\\*="},
    {DIV_ASSIGN, "/="},
    {MOD_ASSIGN, "%="},
    {LEFT_SHIFT, "<<"},
    {RIGHT_SHIFT, ">>"},
    {INCREMENT, "\\+\\+"},
    {DECREMENT, "--"},
    {ARROW, "->"},
    {LOGICAL_AND, "&&"},
    {LOGICAL_OR, "\\|\\|"},
    {LOGICAL_NOT, "!"},
    {LESS_THAN_OR_EQUAL, "<="},
    {GREATER_THAN_OR_EQUAL, ">="},
    {EQUAL, "=="},
    {NOT_EQUAL, "!="},
    {BITWISE_AND, "&"},
    {BITWISE_OR, "\\|"},
    {BITWISE_XOR, "\\^"},
    {BITWISE_NOT, "~"},
    {COMPLEMENT, "-"},
    {FLOATING_CONSTANT, "\\d+\\.\\d+([eE][-+]?\\d+)?"},
    {CHARACTER_CONSTANT, "'.'"},
    {STRING_CONSTANT, "\"[^\"]*\""},
    {LEFT_PAREN, "\\("},
    {RIGHT_PAREN, "\\)"},
    {LEFT_BRACKET, "\\["},
    {RIGHT_BRACKET, "\\]"},
    {LEFT_BRACE, "\\{"},
    {RIGHT_BRACE, "\\}"},
    {DOT, "\\."},
    {ARROW_OPERATOR, "#"},
    {COMMA, ","},
    {SEMICOLON, ";"},
    {COLON, ":"},
    {ELLIPSIS, "\\.{3}"},
    {HASH, "#"},
    {END_OF_FILE, ""},
    {ERROR, ""}
    // Add more token patterns as needed
};

int main()
{
    // Read file into buffer
    std::string filename = "input.txt";
    std::string buffer;
    read_file(filename, buffer);

    std::vector<Token> tokens;
    size_t i = 0;
    while (i < buffer.size())
    {
        bool matched = false;
        for (const auto &pattern : patterns)
        {
            std::regex reg(pattern.second);
            std::smatch match;
            if (std::regex_search(buffer.cbegin() + i, buffer.cend(), match, reg))
            {
                std::string lexeme = match.str();
                i += lexeme.size();

                // Check if the lexeme already has a token type
                bool already_exists = std::any_of(tokens.begin(), tokens.end(),
                                                  [&](const Token &token)
                                                  { return token.lexeme == lexeme; });

                if (!already_exists)
                {
                    tokens.push_back({pattern.first, lexeme});
                    matched = true;
                    break; // Exit the inner loop to prioritize the current pattern
                }
            }
        }

        if (!matched)
        {
            // If no pattern matched, skip the current character
            ++i;
        }
    }
    // Print tokens
    for (const auto &token : tokens)
    {
        if (token.type != END_OF_FILE && token.type != ERROR)
        {
            std::cout << "Token: " << token.lexeme << " Type: " << tokenTypeToString(token.type) << std::endl;
        }
    }

    return 0;
}
std::map<std::string, Token> symbolTable;
void add_to_symbol_table(Token &token)
{
    symbolTable.insert(std::make_pair(token.lexeme, token));
}

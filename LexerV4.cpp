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
    {"while", true}};

bool is_reserved(const std::string &lexeme)
{
    static const std::vector<std::string> reserved_keywords = {"if", "else", "while", "for", "int", "float", "char", "return"};
    for (const auto &keyword : reserved_keywords)
    {
        if (lexeme == keyword)
        {
            return true;
        }
    }
    return false;
}

enum TokenType
{
    IDENTIFIER,
    KEYWORD,
    ASSIGN,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    INCREMENT,
    DECREMENT,
    ARROW,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    LESS_THAN,
    GREATER_THAN,
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
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOT,
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
    ERROR,
    PREPROCESSOR_INCLUDE,
    COMMENT,
};

struct Token
{
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

vector<string> readFileChunks(const string &filename)
{
    vector<string> chunks;
    ifstream file(filename);
    string line;
    int line_number = 1;
    int column_number = 1;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            for (size_t i = 0; i < line.size(); i++)
            {
                chunks.push_back(string(1, line[i]));
                column_number++;
                if (line[i] == '\n')
                {
                    line_number++;
                    column_number = 1;
                }
            }
            chunks.push_back("\n");
            line_number++;
            column_number = 1;
        }
        file.close();
    }

    return chunks;
}

typedef vector<pair<string, vector<int>>> symbol_table_t;

symbol_table_t symbol_table;

int add_to_symbol_table(const std::string &token_name, int offset, int line_number, int column_number)
{
    symbol_table.push_back(make_pair(token_name, vector<int>{offset, line_number, column_number}));
    return symbol_table.size() - 1;
}

std::ostream &operator<<(std::ostream &out, const Token &token)
{
    out << "Token: Type: " << token.type << ", Lexeme: " << token.lexeme << ", Line: " << token.line_number << ", Column: " << token.column_number;
    return out;
}

std::ostream &operator<<(std::ostream &out, const symbol_table_t &st)
{
    for (const auto &entry : st)
    {
        out << "Token: " << entry.first << ", Occurrences: ";
        for (int i : entry.second)
        {
            out << i << " ";
        }
        out << endl;
    }
    return out;
}

bool get_next_token(std::string &buffer, Token *token)
{
    if (buffer.empty())
    {
        return false;
    }

    while (!buffer.empty() && isspace(buffer[0]))
    {
        if (buffer[0] == '\n')
        {
            token->line_number++;
            token->column_number = 1;
        }
        else
        {
            token->column_number++;
        }
        buffer.erase(0, 1);
    }

    if (buffer.empty())
    {
        return false;
    }

    token->lexeme_begin = token->column_number;
<<<<<<< HEAD
    if (buffer[0] == '/' && buffer.size() > 1 && buffer[1] == '/')
    {
        token->type = COMMENT;
        // Handle single-line comments
        size_t endOfLine = buffer.find('\n');
        if (endOfLine == string::npos)
        {
            // Comment until the end of buffer if no newline found
            buffer.clear();
        }
        else
        {
            // Skip the comment until the newline character
            buffer.erase(0, endOfLine);
        }
    }
    else if (buffer[0] == '/' && buffer.size() > 1 && buffer[1] == '*')
    {
        // Handle multi-line comments
        size_t endOfComment = buffer.find("*/", 2);
        if (endOfComment == string::npos)
        {
            // Comment until the end of buffer if end comment marker not found
            buffer.clear();
        }
        else
        {
            // Skip the comment, including the closing marker
            buffer.erase(0, endOfComment + 2);
        }
        buffer.erase(0, 1);
    }
    if (isalpha(buffer[0]) || buffer[0] == '_')
    {
=======
    //cout << "Parsing token, line: " << token->line_number << ", column: " << token->lexeme_begin << endl;

    // Determine the token type
    if (isalpha(buffer[0]) || buffer[0] == '_') {
        // Identifier or keyword
>>>>>>> b262bcb6358c7e99cc284824ba0df353a9890f45
        size_t i = 1;
        while (i < buffer.size() && (isalnum(buffer[i]) || buffer[i] == '_'))
        {
            i++;
        }
        token->lexeme = buffer.substr(0, i);
        buffer.erase(0, i);
        token->column_number += static_cast<int>(i);

        if (is_reserved(token->lexeme))
        {
            token->type = KEYWORD;
        }
        else
        {
            token->type = IDENTIFIER;
            int symbol_table_index = add_to_symbol_table(token->lexeme, token->lexeme_begin, token->line_number, token->column_number - static_cast<int>(token->lexeme.length()));
            token->symbol_table_index = symbol_table_index;
        }
    }
    else if (isdigit(buffer[0]))
    {
        size_t i = 1;
        bool is_float = false;
        while (i < buffer.size() && (isdigit(buffer[i]) || buffer[i] == '.'))
        {
            if (buffer[i] == '.')
            {
                is_float = true;
            }
            i++;
        }
        token->lexeme = buffer.substr(0, i);
        buffer.erase(0, i);
        token->column_number += static_cast<int>(i);

        if (is_float)
        {
            token->type = FLOATING_CONSTANT;
            token->float_value = stof(token->lexeme);
        }
        else
        {
            token->type = INTEGER_CONSTANT;
            token->int_value = stoi(token->lexeme);
        }
    }
    else if (buffer[0] == '\'')
    {
        size_t i = 1;
        if (i < buffer.size() && buffer[i] != '\'')
        {
            token->char_value = buffer[i];
            i++;
            if (i < buffer.size() && buffer[i] == '\'')
            {
                i++;
                token->type = CHARACTER_CONSTANT;
                token->lexeme = buffer.substr(0, i);
                buffer.erase(0, i);
                token->column_number += static_cast<int>(i);
            }
            else
            {
                token->type = ERROR;
                token->lexeme = "Unterminated character constant";
                buffer.erase(0, i);
                token->column_number += static_cast<int>(i);
            }
        }
        else
        {
            token->type = ERROR;
            token->lexeme = "Empty character constant";
            buffer.erase(0, 1);
            token->column_number += 1;
        }
    }
    else if (buffer[0] == '"')
    {
        size_t i = 1;
        while (i < buffer.size() && buffer[i] != '"')
        {
            i++;
        }
        if (i < buffer.size() && buffer[i] == '"')
        {
            token->str_value = buffer.substr(1, i - 1);
            i++;
            token->type = STRING_CONSTANT;
            token->lexeme = buffer.substr(0, i);
            buffer.erase(0, i);
            token->column_number += static_cast<int>(i);
        }
        else
        {
            token->type = ERROR;
            token->lexeme = "Unterminated string constant";
            buffer.erase(0, 1);
            token->column_number += 1;
        }
    }
    else
    {
        switch (buffer[0])
        {
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
<<<<<<< HEAD
        case '#':
            token->type = HASH;
            token->lexeme = "#";
=======
        
        default:
            token->type = ERROR;
            token->lexeme = "Unknown token";
>>>>>>> b262bcb6358c7e99cc284824ba0df353a9890f45
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '[':
            token->type = LEFT_SQUARE_BRACKET;
            token->lexeme = "[";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case ']':
            token->type = RIGHT_SQUARE_BRACKET;
            token->lexeme = "]";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        case '.':
            token->type = DOT;
            token->lexeme = ".";
            buffer.erase(0, 1);
            token->column_number += 1;
            break;
        default:
            token->type = COMMENT;
            //     token->str_value;
            //     token->lexeme = "Unknown token";
            buffer.erase(0, 1);
            //     token->column_number += 1;
            break;
        }
    }

    return true;
}

int main()
{
    std::vector<std::string> file_chunks = readFileChunks("test3.txt");
    std::string buffer;
    for (const auto &chunk : file_chunks)
    {
        buffer += chunk;
    }

    Token token;
    token.line_number = 1;
    token.column_number = 1;

    while (get_next_token(buffer, &token))
    {
        std::cout << token << std::endl;
    }

    std::cout << symbol_table << std::endl;

    return 0;
}

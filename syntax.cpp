#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include "LexerV4.cpp"
#include <cstdarg> // For va_list, va_start, va_end
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;
// Statement----> Assignment ";" | Expression ";"
// void parseStatement()
// {
//     if (lookAheadAndMatch(Token token))
//     {
//         parseDeclarationStatement();
//     }
//     else
//     {
//         parseExpressionStatement();
//     }
// }

// void parseExpressionStatement()
// {
//     parseExpression();
//     expect(SEMICOLON); // Expecting a semicolon at the end of an expression statement
// }
// void parseAssignmentStatement{

// }
//     lookAheadAndMatch(std::initializer_list<TokenType> expectedTypes)
// {
//     for (auto type : expectedTypes)
//     {
//         if (currentToken.type == type)
//         {
//             return true;
//         }
//     }
//     return false;
// }
bool advance()
{
    return get_next_token(buffer, &current_token);
}

// Helper function to match expected tokens
bool match(TokenType expected)
{
    if (current_token.type == expected)
    {
        advance();
        return true;
    }
    else
    {
        cerr << "Syntax error: Expected token type " << expected << " but found " << current_token.type << endl;
        return false;
    }
}

// Forward declarations of parsing functions
void parseProgram();
void parseStatement();
void parseExpression();
void parseVariableDeclaration();
void parseIfStatement();
void parseWhileLoop();
void parseFunctionCall();

// Parsing function for a program (a series of statements)
void parseProgram()
{
    while (current_token.type != END_OF_FILE)
    {
        parseStatement();
    }
}

// Parse a generic statement
void parseStatement()
{
    switch (current_token.type)
    {
    case INT:
    case FLOAT:
    case CHAR:
        parseVariableDeclaration();
        break;
    case IF:
        parseIfStatement();
        break;
    case WHILE:
        parseWhileLoop();
        break;
    case IDENTIFIER:         // Assuming function call or assignment
        parseFunctionCall(); // Simplified, normally you'd distinguish between assignment or a call
        break;
    default:
        cerr << "Unhandled statement type: " << current_token.type << endl;
        advance(); // Skip unhandled tokens
        break;
    }
}

// Parse expressions (this is highly simplified)
void parseExpression()
{
    // Assume expressions are single identifiers or constants for simplicity
    if (current_token.type == IDENTIFIER || current_token.type == INTEGER_CONSTANT || current_token.type == FLOATING_CONSTANT)
    {
        advance(); // Consume the identifier or constant
    }
    else
    {
        throw runtime_error("Expected expression, found unknown token");
    }
}

// Parse variable declaration
void parseVariableDeclaration()
{
    // Consume type
    advance(); // Assuming the type is the current token
    if (!match(IDENTIFIER))
    {
        throw runtime_error("Expected identifier in variable declaration");
    }
    if (match(ASSIGN))
    {
        parseExpression();
    }
    match(SEMICOLON);
}

// Parse if statements
void parseIfStatement()
{
    match(IF);
    match(LEFT_PAREN);
    parseExpression();
    match(RIGHT_PAREN);
    parseStatement(); // Then branch
    if (current_token.type == ELSE)
    {
        match(ELSE);
        parseStatement(); // Else branch
    }
}

// Parse while loops
void parseWhileLoop()
{
    match(WHILE);
    match(LEFT_PAREN);
    parseExpression();
    match(RIGHT_PAREN);
    parseStatement();
}

// Parse function calls
void parseFunctionCall()
{
    match(IDENTIFIER);
    match(LEFT_PAREN);
    if (current_token.type != RIGHT_PAREN)
    {
        parseExpression(); // Parse the first argument
        while (current_token.type == COMMA)
        {
            match(COMMA);
            parseExpression();
        }
    }
    match(RIGHT_PAREN);
    match(SEMICOLON);
}
#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include <fstream>
#include <map>
#include "lexer.cpp"
#include <sstream> // Include the <sstream> header

using namespace std;

// arithmetic exp --> ezay hnndaha mn hetatnya
// boolean exp x>y

// symbol table

class parse_tree_node
{
public:
    bool is_terminal;
    std::string label;
    std::deque<parse_tree_node *> children;
    parse_tree_node(std::string l, bool t) : label(l), is_terminal(t) {}
};

// A function to uniquely identify the node in graphviz dot style
std::ostream &operator<<(std::ostream &out, parse_tree_node *node)
{
    out << "\"" << (void *)node << "\" [label=\"" << node->label << "\"]";
    return out;
}

// Global "buf" buffer to store text in
std::string buf;

// Global "tok" token to lookahead with
Token *tok;
Token *tok1;
void visualize_parse_tree(parse_tree_node *root);
void visualize_parse_tree_aux(parse_tree_node *node, std::ostream &out);
bool match_input(int token_type, std::string *name = nullptr, int *value = nullptr);

parse_tree_node *recursively_descend_with_lookahead();
parse_tree_node *if_statement();
parse_tree_node *stmt();
parse_tree_node *expression();
parse_tree_node *declaration();
parse_tree_node *assignment_stmt();
parse_tree_node *if_statement_aux();
parse_tree_node *else_statement();
parse_tree_node *while_statement();
parse_tree_node *initialize_stmt();
parse_tree_node *Arithmetic_stmt();
parse_tree_node *variable_op();
parse_tree_node *Boolean_expression();
parse_tree_node *Parse_choose();

bool match_input(int token_type, std::string *name, int *value)
{
    // cout<< "  AL MO4KLA HNA  "<<endl;

    static int buffer_offset = get_next_token(buf, tok);
    tok1 = tok;

    if (token_type >= 59)
    {
        if (token_type == 59 && (tok->type != ERROR || tok->lexeme != ","))
            return false;
    }
    else if (tok->type != token_type)
    {
        cout << tok->type << "    " << endl;
        cout << token_type << "    " << endl;

        cout << "  AL MO4KLA HNA 1   " << endl;
        return false;
    }

    if (name != nullptr && tok->lexeme != *name)
        return false;

    std::cout << "MATCH::> " << *tok << std::endl;
    buffer_offset = get_next_token(buf, tok);
    return true;
}
parse_tree_node *recursively_descend_with_lookahead()
{
    // // Peek ahead to see the next token type without consuming it
    // // Depending on the next token type and lexeme, choose which parsing function to call
    // if (tok->type == KEYWORD)
    // {
    //     if (tok->lexeme == "if")
    //     {
    //         cout << "d5l hna       ";
    //         return if_statement();
    //     }
    //     else if (tok->lexeme == "while")
    //     {
    //         return while_statement();
    //     }
    //     else if (tok->lexeme == "int" || tok->lexeme == "float" || tok->lexeme == "char")
    //     {
    //         return initialize_stmt();
    //     }
    // }
    // else if (tok->type == IDENTIFIER)
    // {
    //     // Check if it's an assignment or variable initialization
    //     return assignment_stmt();
    // }

    // // If none of the specific cases match, return null or an appropriate error node
    // return nullptr;
    return if_statement();
}

parse_tree_node *variable()
{
    parse_tree_node *current_node = new parse_tree_node("Variable", false);
    Token current = *tok;
    if (!match_input(IDENTIFIER))
    {
        return nullptr;
    }
    else
    {

        current_node->children.push_back(new parse_tree_node("identifier: " + current.lexeme, true));
    }
    return current_node;
}
parse_tree_node *Arithmetic_stmt()
{
    // cout<<"STUCK HNA";

    parse_tree_node *current_node = new parse_tree_node("Arithmetic_stmt", false);
    parse_tree_node *current_node2 = variable_op();
    if (!current_node2)
    {
        return nullptr;
    }
    current_node->children.push_back(current_node2);
    if (match_input(ADD))
    {
        current_node->children.push_back(new parse_tree_node("+", true));
        parse_tree_node *current_node3 = Arithmetic_stmt();
        if (!current_node3)
        {
            return nullptr;
        }
        current_node->children.push_back(current_node3);
    }
    else if (match_input(SUB))
    {
        current_node->children.push_back(new parse_tree_node("-", true));
        parse_tree_node *current_node3 = Arithmetic_stmt();
        if (!current_node3)
        {
            return nullptr;
        }
        current_node->children.push_back(current_node3);
    }
    else if (match_input(ASTREK))
    {
        current_node->children.push_back(new parse_tree_node("*", true));
        parse_tree_node *current_node3 = Arithmetic_stmt();
        if (!current_node3)
        {
            return nullptr;
        }
        current_node->children.push_back(current_node3);
    }
    else if (match_input(DIV))
    {
        current_node->children.push_back(new parse_tree_node("/", true));
        parse_tree_node *current_node3 = Arithmetic_stmt();
        if (!current_node3)
        {
            return nullptr;
        }
        current_node->children.push_back(current_node3);
    }

    return current_node;
}
parse_tree_node *variable_op()
{
    parse_tree_node *current_node = new parse_tree_node("variable_op", false);

    if (!match_input(IDENTIFIER) && !match_input(INTEGER_CONSTANT))
    {
        return nullptr;
    }
    else
    {
        Token current = *tok;
        if (tok1->type == IDENTIFIER)
        {
            current_node->children.push_back(new parse_tree_node("identifier" + current.lexeme, true));
        }
        else
        {
            current_node->children.push_back(new parse_tree_node("Number" + current.lexeme, true));
        }
    }
    return current_node;
}
parse_tree_node *initialize_stmt()
{
    // ASSIGNMENTS
    parse_tree_node *current_node = new parse_tree_node("initialize_stmt", false);
    parse_tree_node *current_node1 = variable();
    current_node->children.push_back(current_node1);
    cout << "HN- INTIALIZE -- >     " << tok->type << "  " << tok1->type << endl;
    if (match_input(ASSIGN))
    {
        current_node->children.push_back(new parse_tree_node("=", true));
        if (match_input(INTEGER_CONSTANT))
        {
            Token prev = *tok1;
            current_node->children.push_back(new parse_tree_node(prev.lexeme, true));
        }
        else if (match_input(STRING_CONSTANT))
        {
            current_node->children.push_back(new parse_tree_node("STRING", true));
        }
        else
        {
            parse_tree_node *current_node4 = variable();
            current_node->children.push_back(current_node4);
        }
    }
    else
    {
        cout << "NULL INSTIALIze\n";
        return nullptr;
    }

    if (match_input(SEMICOLON))
    {
        current_node->children.push_back(new parse_tree_node(";", true));
    }
    else
    {
        return nullptr;
    }
    return current_node;
}
parse_tree_node *assignment_stmt()
{

    parse_tree_node *current_node = new parse_tree_node("Assignment_stmt", false);
    current_node->children.push_back(new parse_tree_node("=", true));

    if (match_input(INTEGER_CONSTANT))
    {
        Token prev = *tok1;
        current_node->children.push_back(new parse_tree_node(prev.lexeme, true));
    }
    else if (match_input(STRING_CONSTANT))
    {
        current_node->children.push_back(new parse_tree_node("STRING", true));
    }
    else
    {
        parse_tree_node *current_node4 = variable();
        current_node->children.push_back(current_node4);
    }
    if (!match_input(SEMICOLON))
    {
        // Failed to match a semicolon
        return nullptr;
    }
    else
    {
        current_node->children.push_back(new parse_tree_node(";", true));
    }

    return current_node;
}
parse_tree_node *Declar_stmt()
{
    parse_tree_node *current_node = new parse_tree_node("Declar_stmt", false);
    parse_tree_node *current_node2 = variable();
    if (!current_node2)
    {
        return nullptr;
    }
    current_node->children.push_back(current_node2);

    if (match_input(COMMA))
    {
        current_node->children.push_back(new parse_tree_node(",", true));
        parse_tree_node *current_node3 = Declar_stmt();
        if (!current_node3)
        {
            return nullptr;
        }
        current_node->children.push_back(current_node3);
    }

    if (match_input(ASSIGN))
    {
        parse_tree_node *node = assignment_stmt();
        if (node)
        {
            current_node->children.push_back(node);
        }
    }
    return current_node;
}
parse_tree_node *declaration()
{

    parse_tree_node *current_node = new parse_tree_node("Declaration", false);

    // Attempt to match a data type
    if (!match_input(KEYWORD))
    {
        return nullptr;
    }
    else
    {
        current_node->children.push_back(new parse_tree_node("Data Type", true));
    }

    parse_tree_node *current_node2 = Declar_stmt();
    Token current = *tok;

    if (!current_node2)
    {
        return nullptr;
    }
    else
    {
        current_node->children.push_back(current_node2);
    }

    // cout << current.type << endl;
    if (!match_input(SEMICOLON))
    {
        // Failed to match a semicolon
        return nullptr;
    }
    else
    {
        current_node->children.push_back(new parse_tree_node(";", true));
    }

    // Declaration successfully parsed
    return current_node;
}
parse_tree_node *if_statement()
{

    /*
    selection stmt: if_stmt → if ( expr )  { if_stmt } | if expr  stmt-else else if_stmt | stmt
    stmt-else → if expr  stmt-else else stmt-else | stmt
    */

    parse_tree_node *current_node = new parse_tree_node("if_stmt", false);
    string if_str = string("if");
    // cout<<KEYWORD;
    if (match_input(KEYWORD, &if_str))
        current_node->children.push_back(new parse_tree_node("if", true));
    else
    {
        return nullptr;
    }

    if (match_input(LEFT_PAREN))
    {
        current_node->children.push_back(new parse_tree_node("bracket_left", true));
    }

    else
    {
        return nullptr;
    }

    // make if(what kind of expression will be done here)!!!!!!!!!!!!
    parse_tree_node *if_exp = expression();

    if (if_exp)
    {
        current_node->children.push_back(if_exp);
    }
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_PAREN))
        current_node->children.push_back(new parse_tree_node("bracket_right", true));
    else
    {
        return nullptr;
    }

    if (match_input(LEFT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_left", true));
    else
    {
        return nullptr;
    }

    // make if(what kind of statements will be done here)!!!!!!!!!!!!
    if (match_input(KEYWORD, &if_str))
    {
        parse_tree_node *newnode = if_statement_aux();
        if (newnode)
        {
            current_node->children.push_back(newnode);
        }
        else
        {
            return nullptr;
        }
    }
    // not implemented fully
    Token current = *tok;
    // cout << current.type << endl; // identifier
    parse_tree_node *if_stmts;
    if (current.type == KEYWORD)
    {
        if_stmts = declaration();
    }
    else if (current.type == IDENTIFIER)
    {

        if_stmts = initialize_stmt();
    }

    if (if_stmts)
    {
        current_node->children.push_back(if_stmts);
    }
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_right", true));
    else
    {
        return nullptr;
    }

    string else_str = string("else");
    if (match_input(KEYWORD, &else_str))
    {

        parse_tree_node *newnode = else_statement();
        if (newnode)
            current_node->children.push_back(newnode);
        else
            return nullptr;
    }

    return current_node;
}
parse_tree_node *else_statement()
{

    string if_str = string("if");

    parse_tree_node *current_node = new parse_tree_node("else_statement", false);

    current_node->children.push_back(new parse_tree_node("else", true));

    if (match_input(LEFT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_left", true));
    else
    {
        return nullptr;
    }

    // not implemented fully

    if (match_input(KEYWORD, &if_str))
    {
        parse_tree_node *newnode = if_statement_aux();
        if (newnode)
        {
            current_node->children.push_back(newnode);
        }
        else
        {

            return nullptr;
        }
    }
    Token current = *tok;
    parse_tree_node *if_stmtss;
    cout << "INSTANCE  ::  " << current.type << endl;

    if (!(current.type == RIGHT_BRACKET))
    {
        Token current = *tok;
        // std::cout << current.type << endl; // identifier

        if (current.type == KEYWORD)
        {
            //  cout  << current.type << endl;

            if_stmtss = declaration();
        }
        else if (current.type == IDENTIFIER)
        {
            if_stmtss = initialize_stmt();
        }

        if (if_stmtss)
        {
            current_node->children.push_back(if_stmtss);
        }
        else
        {
            return nullptr;
        }
    }
    if (match_input(RIGHT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_right", true));
    else
    {
        return nullptr;
    }

    return current_node;
}

parse_tree_node *if_statement_aux()
{

    /*
    selection stmt: if_stmt → if ( expr )  { if_stmt } | if expr  stmt-else else if_stmt | stmt
    stmt-else → if expr  stmt-else else stmt-else | stmt
    */

    string if_str = string("if");
    parse_tree_node *current_node = new parse_tree_node("if_stmt", false);
    current_node->children.push_back(new parse_tree_node("if", true));

    if (match_input(LEFT_PAREN))
        current_node->children.push_back(new parse_tree_node("bracket_left", true));
    else
    {
        return nullptr;
    }

    // make if(what kind of expression will be done here)!!!!!!!!!!!!
    parse_tree_node *if_exp = expression();

    if (if_exp)
    {
        current_node->children.push_back(if_exp);
    }
    else
    {
        return nullptr;
    }

    if (match_input(LEFT_PAREN))
        current_node->children.push_back(new parse_tree_node("bracket_right", true));
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_PAREN))
        current_node->children.push_back(new parse_tree_node("brace_left", true));
    else
    {
        return nullptr;
    }

    // make if(what kind of statements will be done here)!!!!!!!!!!!!

    if (match_input(KEYWORD, &if_str))
    {
        parse_tree_node *newnode = if_statement_aux();
        if (newnode)
        {
            current_node->children.push_back(newnode);
        }
        else
        {
            return nullptr;
        }
    }

    Token current = *tok;
    // cout << current.type << endl; // identifier
    parse_tree_node *if_stmts;
    if (current.type == KEYWORD)
    {
        if_stmts = declaration();
    }
    else if (current.type == IDENTIFIER)
    {

        if_stmts = initialize_stmt();
    }

    if (if_stmts)
    {
        current_node->children.push_back(if_stmts);
    }
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_right", true));
    else
    {
        return nullptr;
    }

    string else_str = string("else");
    if (match_input(KEYWORD, &else_str))
    {
        parse_tree_node *newnode = else_statement();
        if (newnode)
            current_node->children.push_back(newnode);
        else
            return nullptr;
    }

    return current_node;
}

// STATEMENT IS NOT IMPLEMENTED YETTTTTTT

parse_tree_node *stmt()
{
    // Create a new node for this non-terminal
    parse_tree_node *current_node = new parse_tree_node("stmt", false);

    return current_node;
}
parse_tree_node *expression()
{
    // Create a new node for this non-terminal
    parse_tree_node *current_node = new parse_tree_node("expression", false);
    Token current;

    // Attempt to match an identifier
    if (!match_input(IDENTIFIER))
    {
        return nullptr;
    }
    else
    {

        current = *tok;

        current_node->children.push_back(new parse_tree_node("identifier " + current.lexeme, true));
    }

    // Attempt to match an operator
    if (!match_input(ADD) && !match_input(SUB) && !match_input(DIV) && !match_input(LESS_THAN) && !match_input(GREATER_THAN) && !match_input(LESS_THAN_OR_EQUAL) && !match_input(GREATER_THAN_OR_EQUAL) && !match_input(EQUAL) && !match_input(NOT_EQUAL) && !match_input(LOGICAL_AND) && !match_input(LOGICAL_OR))
    {
        return nullptr;
    }
    else
    {

        current_node->children.push_back(new parse_tree_node("operation" + current.lexeme, true));
    }

    // Attempt to match an identifier
    if (!match_input(IDENTIFIER) && !match_input(INTEGER_CONSTANT))
    {

        return nullptr;
    }
    else if (tok1->type == IDENTIFIER)
    {
        current = *tok1;
        // cout << current.type << endl;
        current_node->children.push_back(new parse_tree_node("identifier: " + current.lexeme, true));
    }
    else
    {
        // cout << tok->type << "\n";
        current_node->children.push_back(new parse_tree_node("number", true));
    }

    return current_node;
}
parse_tree_node *Boolean_expression()
{
    // Create a new node for this non-terminal
    parse_tree_node *current_node = new parse_tree_node("Boolean_expression", false);

    parse_tree_node *current_node2 = expression();
    if (!current_node)
    {
        return nullptr;
    }
    else
    {
        current_node->children.push_back(current_node2);
    }

    if (!match_input(SEMICOLON))
    {
        // Failed to match a semicolon
        return nullptr;
    }
    else
    {
        current_node->children.push_back(new parse_tree_node(";", true));
    }

    return current_node;
}
parse_tree_node *while_statement()
{
    parse_tree_node *current_node = new parse_tree_node("while_stmt", false);
    string while_str = string("while");
    if (match_input(KEYWORD, &while_str))
        current_node->children.push_back(new parse_tree_node("while", true));
    else
    {
        return nullptr;
    }

    if (match_input(LEFT_PAREN))
        current_node->children.push_back(new parse_tree_node("bracket_left", true));
    else
    {
        return nullptr;
    }

    parse_tree_node *while_exp = expression();

    if (while_exp)
    {
        current_node->children.push_back(while_exp);
    }
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_PAREN))
        current_node->children.push_back(new parse_tree_node("R_bracket", true));
    else
    {
        return nullptr;
    }

    if (match_input(LEFT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_left", true));
    else
    {
        return nullptr;
    }

    // not implemented fully
    Token current = *tok;
    // cout << current.type << endl; // identifier
    parse_tree_node *if_stmts;
    if (current.type == KEYWORD)
    {
        if_stmts = declaration();
    }
    else if (current.type == IDENTIFIER)
    {

        if_stmts = initialize_stmt();
    }

    if (if_stmts)
    {
        current_node->children.push_back(if_stmts);
    }
    else
    {
        return nullptr;
    }

    if (match_input(RIGHT_BRACKET))
        current_node->children.push_back(new parse_tree_node("brace_right", true));
    else
    {
        return nullptr;
    }

    return current_node;
}

void visualize_parse_tree(parse_tree_node *root)
{

    std::ofstream dot_file_out("parse_tree.dot", std::ios::trunc);

    // TODO: Would be nice if you decorated your parse tree, lookup how to do so in graphviz dot notation
    dot_file_out << "digraph G {\n";
    visualize_parse_tree_aux(root, dot_file_out);
    dot_file_out << "}";
    dot_file_out.close();

    // Convert the dot file into a SVG vector image
    // system("dot -Tsvg ./parse_tree.dot -oparse_tree.svg");

    // Open the vector file
    // TODO: Integrate this with your GUI...
    // You'll need to lookup how to open an .svg file inside Qt or whatever GUI framework you are using
    // system("parse_tree.svg");
}

void visualize_parse_tree_aux(parse_tree_node *node, std::ostream &out)
{

    // Need to uniquely identify each node in graphviz
    out << "  " << node << (node->is_terminal ? "[fontcolor=\"red\"][color=\"red\"];\n" : ";\n");

    for (auto child : node->children)
    {

        bool child_has_children = child->children.size() > 0;
        if (child->is_terminal || (!child->is_terminal && child_has_children))
        {

            // Need to uniquely identify each child node too
            out << "  " << child << ";\n";

            // Create edge between node and child unique addresses
            out << "  \"" << (void *)node << "\" -> \"" << (void *)child << "\";\n";

            // Recursively build rest of tree
            visualize_parse_tree_aux(child, out);
        }
    }
}
int main()
{
    // Read input code from a file
    std::ifstream input_file("test4.txt");
    if (!input_file.is_open())
    {
        std::cerr << "Failed to open input file\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string input_code = buffer.str();

    // Set up lexer with the input code
    buf = input_code;
    tok = new Token();

    // Build the parse tree
    parse_tree_node *root = recursively_descend_with_lookahead();
    cout << "RUN";

    // Visualize the parse tree
    visualize_parse_tree(root);

    // Clean up
    delete tok;
    input_file.close();

    return 0;
}
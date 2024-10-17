#include <iostream>
#include <cstdio>
#include <cctype>
#include <string>
#include <vector>

using namespace std;

// Token Types
typedef enum {
    CLASS,
    FUNCTION,
    IDENTIFIER,
    STRING_LITERAL,
    OPEN_CURLY,
    CLOSE_CURLY,
    OPEN_PAREN,
    CLOSE_PAREN,
    SEMI,
    PRINT
} TokenType;

// Token Structure
struct Token {
    TokenType type;
    string value;
};

// Lexer functions
bool is_separator(char c) {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == ';';
}

void print_token(const Token& token) {
    cout << "Token(";
    switch (token.type) {
        case CLASS: cout << "CLASS, value: " << token.value; break;
        case FUNCTION: cout << "FUNCTION, value: " << token.value; break;
        case IDENTIFIER: cout << "IDENTIFIER, value: " << token.value; break;
        case STRING_LITERAL: cout << "STRING_LITERAL, value: " << token.value; break;
        case OPEN_CURLY: cout << "OPEN_CURLY"; break;
        case CLOSE_CURLY: cout << "CLOSE_CURLY"; break;
        case OPEN_PAREN: cout << "OPEN_PAREN"; break;
        case CLOSE_PAREN: cout << "CLOSE_PAREN"; break;
        case SEMI: cout << "SEMI"; break;
        case PRINT: cout << "PRINT"; break;
        default: cout << "UNKNOWN"; break;
    }
    cout << ")\n";
}

// Lexer function
void Lexer(FILE *file, vector<Token> &tokens) {
    char current_char = fgetc(file);
    while (current_char != EOF) {
        if (isspace(current_char)) {
            current_char = fgetc(file);
            continue;
        }
        if (is_separator(current_char)) {
            Token token;
            switch (current_char) {
                case '{': token = {OPEN_CURLY, "{"}; break;
                case '}': token = {CLOSE_CURLY, "}"}; break;
                case '(': token = {OPEN_PAREN, "("}; break;
                case ')': token = {CLOSE_PAREN, ")"}; break;
                case ';': token = {SEMI, ";"}; break;
            }
            tokens.push_back(token);
            current_char = fgetc(file);
            continue;
        }
        if (isalpha(current_char)) {
            string word = "";
            while (isalpha(current_char)) {
                word += current_char;
                current_char = fgetc(file);
            }
            if (word == "class") {
                tokens.push_back({CLASS, word});
            } else if (word == "function") {
                tokens.push_back({FUNCTION, word});
            } else if (word == "print") {
                tokens.push_back({PRINT, word});
            } else {
                tokens.push_back({IDENTIFIER, word});
            }
            continue;
        }
        if (current_char == '"') {
            string str_literal = "";
            current_char = fgetc(file);
            while (current_char != '"' && current_char != EOF) {
                str_literal += current_char;
                current_char = fgetc(file);
            }
            tokens.push_back({STRING_LITERAL, str_literal});
            current_char = fgetc(file);
            continue;
        }
        current_char = fgetc(file);
    }
}

// Parser function
void Parser(vector<Token> &tokens) {
    int pos = 0;
    if (tokens[pos].type == CLASS) {
        cout << "Parsing CLASS: " << tokens[pos].value << "\n";
        pos++;
        if (tokens[pos].type == IDENTIFIER && tokens[pos].value == "Main") {
            cout << "Parsing CLASS IDENTIFIER: " << tokens[pos].value << "\n";
            pos++;
            if (tokens[pos].type == OPEN_CURLY) {
                cout << "Parsing OPEN_CURLY: " << tokens[pos].value << "\n";
                pos++;
                if (tokens[pos].type == FUNCTION) {
                    cout << "Parsing FUNCTION: " << tokens[pos].value << "\n";
                    pos++;
                    if (tokens[pos].type == IDENTIFIER && tokens[pos].value == "Main") {
                        cout << "Parsing FUNCTION IDENTIFIER: " << tokens[pos].value << "\n";
                        pos++;
                        if (tokens[pos].type == OPEN_PAREN) {
                            cout << "Parsing OPEN_PAREN: " << tokens[pos].value << "\n";
                            pos++;
                            if (tokens[pos].type == CLOSE_PAREN) {
                                cout << "Parsing CLOSE_PAREN: " << tokens[pos].value << "\n";
                                pos++;
                                if (tokens[pos].type == OPEN_CURLY) {
                                    cout << "Parsing OPEN_CURLY: " << tokens[pos].value << "\n";
                                    pos++;
                                    if (tokens[pos].type == PRINT) {
                                        cout << "Parsing PRINT: " << tokens[pos].value << "\n";
                                        pos++;
                                        if (tokens[pos].type == OPEN_PAREN) {
                                            cout << "Parsing OPEN_PAREN: " << tokens[pos].value << "\n";
                                            pos++;
                                            if (tokens[pos].type == STRING_LITERAL) {
                                                cout << "Parsing STRING_LITERAL: " << tokens[pos].value << "\n";
                                                pos++;
                                                if (tokens[pos].type == CLOSE_PAREN) {
                                                    cout << "Parsing CLOSE_PAREN: " << tokens[pos].value << "\n";
                                                    pos++;
                                                    if (tokens[pos].type == SEMI) {
                                                        cout << "Parsing SEMI: " << tokens[pos].value << "\n";
                                                        cout << "Valid print statement\n";
                                                    } else {
                                                        cout << "Error: Expected ';'\n";
                                                    }
                                                } else {
                                                    cout << "Error: Expected ')'\n";
                                                }
                                            } else {
                                                cout << "Error: Expected STRING_LITERAL\n";
                                            }
                                        } else {
                                            cout << "Error: Expected '('\n";
                                        }
                                    }
                                    if (tokens[pos].type == CLOSE_CURLY) {
                                        cout << "Parsing CLOSE_CURLY: " << tokens[pos].value << "\n";
                                    } else {
                                        cout << "Error: Expected '}'\n";
                                    }
                                }
                            }
                        }
                    }
                }
                if (tokens[pos].type == CLOSE_CURLY) {
                    cout << "Parsing CLOSE_CURLY: " << tokens[pos].value << "\n";
                } else {
                    cout << "Error: Expected '}'\n";
                }
            }
        }
    }
}

int main() {
    FILE *file;
    file = fopen("main.eld", "r");
    if (!file) {
        cerr << "Error opening file!\n";
        return 1;
    }
    vector<Token> tokens;
    Lexer(file, tokens);
    fclose(file);

    for (const Token &token : tokens) {
        print_token(token);
    }

    Parser(tokens);

    return 0;
}

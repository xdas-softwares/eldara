#include <iostream>
#include <cstdio>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

// Define assembly instruction templates
const string ASM_HEADER = ".386\n.model flat, stdcall\noption casemap :none\n\n"
                          "include \\masm32\\include\\kernel32.inc\n"
                          "include \\masm32\\include\\masm32.inc\n"
                          "includelib \\masm32\\lib\\kernel32.lib\n"
                          "includelib \\masm32\\lib\\masm32.lib\n\n"
                          ".data\n";
const string ASM_CODE_START = ".code\nmain:\n";
const string ASM_CODE_END = "    invoke ExitProcess, 0\nend main\n";

// Assembly Instructions Structure
struct AssemblyInstruction {
    string instruction;
    string operand;  // For constants or identifiers
};

vector<AssemblyInstruction> assembly_code;
vector<string> data_section;

// Code Generator for Assembly
void generate_asm_code(const string &instruction, const string &operand = "") {
    assembly_code.push_back({instruction, operand});
}

void generate_data_section(const string &label, const string &data) {
    data_section.push_back(label + " db \"" + data + "\", 0");
}

// Lexer functions
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
    PRINT_TOKEN
} TokenType;

struct Token {
    TokenType type;
    string value;
};

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
        case PRINT_TOKEN: cout << "PRINT"; break;
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
                tokens.push_back({PRINT_TOKEN, word});
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
        pos++;
        if (tokens[pos].type == IDENTIFIER && tokens[pos].value == "Main") {
            pos++;
            if (tokens[pos].type == OPEN_CURLY) {
                pos++;
                if (tokens[pos].type == FUNCTION) {
                    pos++;
                    if (tokens[pos].type == IDENTIFIER && tokens[pos].value == "Main") {
                        pos++;
                        if (tokens[pos].type == OPEN_PAREN) {
                            pos++;
                            if (tokens[pos].type == CLOSE_PAREN) {
                                pos++;
                                if (tokens[pos].type == OPEN_CURLY) {
                                    pos++;
                                    while (tokens[pos].type == PRINT_TOKEN) {
                                        pos++;
                                        if (tokens[pos].type == OPEN_PAREN) {
                                            pos++;
                                            if (tokens[pos].type == STRING_LITERAL) {
                                                string label = "message" + to_string(pos);
                                                generate_data_section(label, tokens[pos].value);
                                                generate_asm_code("invoke StdOut, addr " + label);
                                                pos++;
                                                if (tokens[pos].type == CLOSE_PAREN) {
                                                    pos++;
                                                    if (tokens[pos].type == SEMI) {
                                                        pos++;
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
                                        pos++;
                                    } else {
                                        cout << "Error: Expected '}'\n";
                                    }
                                }
                            }
                        }
                    }
                }
                if (tokens[pos].type == CLOSE_CURLY) {
                    pos++;
                } else {
                    cout << "Error: Expected '}'\n";
                }
            }
        }
    }
}

// Function to save the assembly code to a file
void save_asm_to_file(const string &filename) {
    ofstream outfile(filename);
    if (!outfile) {
        cerr << "Error: Could not open file for writing!\n";
        return;
    }

    outfile << ASM_HEADER;

    for (const auto &data : data_section) {
        outfile << "    " << data << "\n";
    }

    outfile << "\n" << ASM_CODE_START;

    for (const auto &instruction : assembly_code) {
        outfile << "    " << instruction.instruction;
        if (!instruction.operand.empty()) {
            outfile << ", " << instruction.operand;
        }
        outfile << "\n";
    }

    outfile << ASM_CODE_END;
    outfile.close();
    cout << "Assembly code saved to " << filename << "\n";
}

int main() {
    FILE *file = fopen("main.eld", "r");
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

    // Save the generated assembly code
    save_asm_to_file("output.asm");

    return 0;
}

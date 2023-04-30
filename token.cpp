#include "token.hpp"
#include <cctype>
#include "iostream"

std::unordered_map <std::string, TokenType> Token::predefined_tokens = {
    {"program", PROGRAM}, {"var", VAR}, {"const", CONST}, {"type", TYPE}, {"function", FUNCTION}, 
    {"return", RETURN}, {"begin", BEGIN}, {"end", END}, {":=:", SWAP}, {":=", ASSIGN}, {"output", OUTPUT},
    {"if", IF}, {"then", THEN}, {"else", ELSE}, {"while", WHILE}, {"do", DO}, {"case", CASE}, {"of", OF},
    {"..", DOTS}, {"otherwise", OTHERWISE}, {"repeat", REPEAT}, {"for", FOR}, {"until", UNTIL},
    {"loop", LOOP}, {"pool", POOL}, {"exit", EXIT}, {"<=", LEQ}, {"<>", NEQ}, {"<", LE}, {">=", GEQ}, 
    {">", GE}, {"=", EQ}, {"mod", MOD}, {"and", AND}, {"or", OR}, {"not", NOT}, {"read", READ}, 
    {"succ", SUCC}, {"pred", PRED}, {"chr", CHR}, {"ord", ORD}, {"eof", EOFT},
    {"{", BLKBEGIN}, {":", COLON}, {";", SEMICOLON}, {".", PERIOD}, 
    {",", COMMA}, {"{", OPENBRKT}, {"}", CLSBRKT}, {"+", PLUS}, {"-", MINUS}, {"*", MULT}, {"/", DIVIDE}
};

Token::Token(TokenType type, std::string value) {
    this->type = type;
    this->value = value;
}

TokenType Token::getType(){
    return type;
}

std::string Token::getValue(){
    return value;
}

TokenType Token::identifyNonPredefinedTokenType(char c){
    
    if (isalpha(c) || c=='_') {
        return IDENTIFER;
    }
    if (isdigit(c)) {
        return INTEGER;
    }
    if (c=='\''){
        return CHAR;
    }
    if (c=='"'){
        return STRING;
    }
    if (c=='#'){
        return COMMENT_1;
    }
    if (c=='{'){
        return COMMENT_2;
    }
    std::cout << "Unexpected character encountered: token could not be identified.\n";
    exit(0);
}
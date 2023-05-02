#include "token.hpp"
#include <cctype>
#include "iostream"

std::unordered_map <TokenType, std::string> Token::predefined_tokens = {
    {PROGRAM, "program"}, 
    {VAR, "var"},
    {CONST, "const"},
    {TYPE, "type"}, 
    {FUNCTION, "function"}, 
    {RETURN, "return"}, 
    {BEGIN, "begin"}, 
    {END, "end"}, 
    {SWAP, ":=:"}, 
    {ASSIGN, ":="}, 
    {OUTPUT, "output"}, 
    {IF, "if"}, 
    {THEN, "then"}, 
    {ELSE, "else"}, 
    {WHILE, "while"}, 
    {DO, "do"}, 
    {CASE, "case"}, 
    {OF, "of"}, 
    {DOTS, ".."}, 
    {OTHERWISE, "otherwise"}, 
    {REPEAT, "repeat"}, 
    {FOR, "for"}, 
    {UNTIL, "until"}, 
    {LOOP, "loop"}, 
    {POOL, "pool"}, 
    {EXIT, "exit"}, 
    {LEQ, "<="}, 
    {NEQ, "<>"}, 
    {LE, "<"}, 
    {GEQ, ">="}, 
    {GE, ">"}, 
    {EQ, "="}, 
    {MOD, "mod"}, 
    {AND, "and"}, 
    {OR, "or"}, 
    {NOT, "not"}, 
    {READ, "read"}, 
    {SUCC, "succ"}, 
    {PRED, "pred"}, 
    {CHR, "chr"}, 
    {ORD, "ord"}, 
    {EOFT, "eof"}, 
    {COLON, ":"}, 
    {SEMICOLON, ";"}, 
    {PERIOD, "."}, 
    {COMMA, ","}, 
    {OPENBRKT, "("}, 
    {CLSBRKT, ")"}, 
    {PLUS, "+"}, 
    {MINUS, "-"}, 
    {MULT, "*"}, 
    {DIVIDE, "/"}
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
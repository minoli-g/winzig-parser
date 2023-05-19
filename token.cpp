#include "token.hpp"
#include <cctype>
#include <unordered_set>
#include <stdexcept>
#include "iostream"

std::unordered_map <TokenType, std::string> Token::predefined_tokens = {
    {TokenType::PROGRAM, "program"}, 
    {TokenType::VAR, "var"},
    {TokenType::CONST, "const"},
    {TokenType::TYPE, "type"}, 
    {TokenType::FUNCTION, "function"}, 
    {TokenType::RETURN, "return"}, 
    {TokenType::BEGIN, "begin"}, 
    {TokenType::END, "end"}, 
    {TokenType::SWAP, ":=:"}, 
    {TokenType::ASSIGN, ":="}, 
    {TokenType::OUTPUT, "output"}, 
    {TokenType::IF, "if"}, 
    {TokenType::THEN, "then"}, 
    {TokenType::ELSE, "else"}, 
    {TokenType::WHILE, "while"}, 
    {TokenType::DO, "do"}, 
    {TokenType::CASE, "case"}, 
    {TokenType::OF, "of"}, 
    {TokenType::DOTS, ".."}, 
    {TokenType::OTHERWISE, "otherwise"}, 
    {TokenType::REPEAT, "repeat"}, 
    {TokenType::FOR, "for"}, 
    {TokenType::UNTIL, "until"}, 
    {TokenType::LOOP, "loop"}, 
    {TokenType::POOL, "pool"}, 
    {TokenType::EXIT, "exit"}, 
    {TokenType::LEQ, "<="}, 
    {TokenType::NEQ, "<>"}, 
    {TokenType::LE, "<"}, 
    {TokenType::GEQ, ">="}, 
    {TokenType::GE, ">"}, 
    {TokenType::EQ, "="}, 
    {TokenType::MOD, "mod"}, 
    {TokenType::AND, "and"}, 
    {TokenType::OR, "or"}, 
    {TokenType::NOT, "not"}, 
    {TokenType::READ, "read"}, 
    {TokenType::SUCC, "succ"}, 
    {TokenType::PRED, "pred"}, 
    {TokenType::CHR, "chr"}, 
    {TokenType::ORD, "ord"}, 
    {TokenType::EOFT, "eof"}, 
    {TokenType::COLON, ":"}, 
    {TokenType::SEMICOLON, ";"}, 
    {TokenType::PERIOD, "."}, 
    {TokenType::COMMA, ","}, 
    {TokenType::OPENBRKT, "("}, 
    {TokenType::CLSBRKT, ")"}, 
    {TokenType::PLUS, "+"}, 
    {TokenType::MINUS, "-"}, 
    {TokenType::MULT, "*"}, 
    {TokenType::DIVIDE, "/"}
};

Token::Token(TokenType type, std::string value) {
    this->type = type;
    this->value = value;
}

Token::Token(TokenType type){
    std::unordered_set<TokenType> not_predefined = {
        TokenType::IDENTIFER, 
        TokenType::INTEGER, 
        TokenType::CHAR, 
        TokenType::COMMENT_1, 
        TokenType::COMMENT_2
    };
    if (not_predefined.count(type)){
        throw std::runtime_error("String argument required to construct non-predefined token");
    }
    else {
        this->type = type;
        this->value = predefined_tokens[type];
    }
}

TokenType Token::getType(){
    return type;
}

std::string Token::typeToString(TokenType type){
    std::string x = str(type);
    return x;
}

std::string Token::getValue(){
    return value;
}

TokenType Token::identifyNonPredefinedTokenType(char c){
    
    if (isalpha(c) || c=='_') {
        return TokenType::IDENTIFER;
    }
    if (isdigit(c)) {
        return TokenType::INTEGER;
    }
    if (c=='\''){
        return TokenType::CHAR;
    }
    if (c=='"'){
        return TokenType::STRING;
    }
    if (c=='#'){
        return TokenType::COMMENT_1;
    }
    if (c=='{'){
        return TokenType::COMMENT_2;
    }
    throw std::runtime_error("Unexpected character ("+
                            std::string(1,c) + ") encountered: token could not be identified.\n");
}
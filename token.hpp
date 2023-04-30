#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>

enum TokenType {
    // define all the types of tokens that will be used.
    // Whitespace and Newline are not included as they will be discarded during the lexical analysis
    IDENTIFER, INTEGER, CHAR, STRING, COMMENT_1, COMMENT_2,
    PROGRAM, VAR, CONST, TYPE, FUNCTION, RETURN, BEGIN, END, SWAP, ASSIGN, OUTPUT, IF,
    THEN, ELSE, WHILE, DO, CASE, OF, DOTS, OTHERWISE, REPEAT, FOR, UNTIL, LOOP, POOL,
    EXIT, LEQ, NEQ, GEQ, GE, LE, EQ, MOD, AND, OR, NOT, READ, SUCC, PRED, CHR, ORD, EOFT,
    BLKBEGIN, COLON, SEMICOLON, PERIOD, COMMA, OPENBRKT, CLSBRKT, PLUS, MINUS, MULT, DIVIDE
};

class Token {
    private:
        TokenType type;
        std::string value;

    public:
        static std::unordered_map<std::string, TokenType> predefined_tokens;

        Token(TokenType type, std::string value);
        static TokenType identifyNonPredefinedTokenType(char c);

        TokenType getType();
        std::string getValue();
};

#endif
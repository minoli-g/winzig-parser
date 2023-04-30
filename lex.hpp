#ifndef LEX_H
#define LEX_H

#include <string>
#include <vector>
#include "token.hpp"
#include <unordered_set>

class Lexer {

    private:
        std::string content;
        std::string::iterator position;
        std::vector<Token> tokens;

        static std::unordered_set<char> whitespaces;

    public:
        Lexer(std::string content);
        std::vector<Token> parse();

        bool positionValid();

        void consumeWhitespaceIfPresent();
        bool consumePredefinedTokenIfPresent();
        void consumeIdentifier();
        void consumeInteger();
        void consumeChar();
        void consumeCommentOne();
        void consumeCommentTwo();
};

#endif
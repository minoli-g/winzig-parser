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
        void parse();

        std::vector<Token> getTokenSequence();

        bool positionValid();

        void consumeWhitespaceIfPresent();
        bool consumePredefinedTokenIfPresent();
        void consumeIdentifier();
        void consumeInteger();
        void consumeChar();
        void consumeString();
        void consumeCommentOne();
        void consumeCommentTwo();
};

#endif
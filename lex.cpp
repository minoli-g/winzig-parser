#include "lex.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>

std::unordered_set<char> Lexer::whitespaces = {' ', '\t', '\n', '\f', '\r', '\v'};

Lexer::Lexer(std::string content){
    this->content = content;
    this->position = this->content.begin();
    tokens = { };
}

bool Lexer::positionValid(){
    if (position < content.end() ){
        return true;
    }
    return false;
}

void Lexer::parse(){

    while (positionValid()){
        if (! consumePredefinedTokenIfPresent() ){

            switch(Token::identifyNonPredefinedTokenType(*position)){
                case IDENTIFER:
                    consumeIdentifier();
                    break;
                case INTEGER:
                    consumeInteger();
                    break;
                case CHAR:
                    consumeChar();
                    break;
                case STRING:
                    consumeString();
                case COMMENT_1:
                    consumeCommentOne();
                    break;
                case COMMENT_2:
                    consumeCommentTwo();
                    break;
                default:
                    break;
            }
        }
        consumeWhitespaceIfPresent();
    }
}

void Lexer::consumeWhitespaceIfPresent(){
    while (positionValid() && whitespaces.count(*position)){
        position++ ;
    }
}

bool Lexer::consumePredefinedTokenIfPresent(){
    // Identify whether any of the predefined tokens match with the sequence of characters
    // following the current pointer location

    for (auto it: Token::predefined_tokens){
        std::string token_value = it.second;
        int token_length = token_value.size();

        if (position+token_length < content.end()){
            std::string content_slice (position, position+token_length);
            if (content_slice == token_value){
                position += token_length;
                tokens.push_back( Token(it.first) ); 
                return true;
            }
        }
    }
    return false;
}

void Lexer::consumeIdentifier(){
    // position iterator is on the first letter of the identifier, which was already checked
    std::string::iterator temp { position };
    position ++;

    while (positionValid() && (isalnum(*position) || *position=='_' )){
        position++;
    }
    tokens.push_back( Token(IDENTIFER, std::string(temp, position)) );
}

void Lexer::consumeInteger(){
    // position iterator is on the first letter of the integer, which was already checked
    std::string::iterator temp { position };
    position ++;

    while (positionValid() && isdigit(*position) ){
        position++;
    }
    tokens.push_back( Token(INTEGER, std::string(temp, position)) );
}

void Lexer::consumeChar(){
    // position iterator is on the first quote of the char, which was already checked
    std::string::iterator temp { position };

    position ++;
    if (positionValid() && *position =='\''){
        throw std::runtime_error("Invalid char token \n");
    }

    position ++;
    if (positionValid() && *position !='\''){
        throw std::runtime_error("Invalid char token \n");
    }

    if (!positionValid()){ 
        throw std::runtime_error("EOF reached while parsing char \n");
    }
    tokens.push_back( Token(CHAR, std::string(temp+1, temp+2)) );
    position ++;
}

void Lexer::consumeString(){
    // position iterator is on the first double quote of the string, which was already checked
    std::string::iterator temp { position };

    position ++;
    while (positionValid() && *position!='"'){
        position ++;
    }
    if (!positionValid()){ 
        throw std::runtime_error("EOF reached while parsing string \n");
    }
    if (*position == '"'){
        tokens.push_back( Token(STRING, std::string(temp+1, position)) );
        position++;
    }
}

void Lexer::consumeCommentOne(){
    // position iterator is on the first letter of the identifier, which was already checked
    std::string::iterator temp { position };
    position ++;

    while (positionValid() && *position != '\n'){
        position++;
    }
    // Whether it's now at EOF or a newline, the comment is complete
    tokens.push_back( Token(COMMENT_1, std::string(temp, position)) );
}

void Lexer::consumeCommentTwo(){
    // position iterator is on the first letter of the identifier, which was already checked
    std::string::iterator temp { position };
    position ++;

    while (positionValid() && *position!='}'){
        position++;
    }
    if (!positionValid()){ 
        throw std::runtime_error("EOF reached while parsing multiline comment \n");
    }
    if (*position == '}'){
        tokens.push_back( Token(COMMENT_2, std::string(temp, position+1)) );
        position++;
    }
}

std::vector<Token> Lexer::getTokenSequence(){
    return tokens;
}

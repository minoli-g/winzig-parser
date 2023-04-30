#include "lex.hpp"
#include <cctype>
#include <iostream>

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

std::vector<Token> Lexer::parse(){

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
    return tokens;
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
        std::string token_value = it.first;
        int token_length = token_value.size();

        if (position+token_length < content.end()){
            std::string content_slice (position, position+token_length);
            if (content_slice == token_value){
                position += token_length;
                tokens.push_back( Token(it.second, token_value) );  // TODO - replace constructor
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
    if (!positionValid()){  // End of file has been reached - that is okay
        tokens.push_back( Token(IDENTIFER, std::string(temp, position)) );
    }
    if (whitespaces.count(*position)){  // Token was terminated by a whitespace
        tokens.push_back( Token(IDENTIFER, std::string(temp, position)) );
        position++;
    }
    else {
        std::cout << "Invalid character encountered while parsing identifier token \n";
        exit(0);
    }
}

void Lexer::consumeInteger(){
    // position iterator is on the first letter of the integer, which was already checked
    std::string::iterator temp { position };
    position ++;

    while (positionValid() && isdigit(*position) ){
        position++;
    }
    if (!positionValid()){  // End of file has been reached - that is okay
        tokens.push_back( Token(INTEGER, std::string(temp, position)) );
    }
    if (whitespaces.count(*position)){
        tokens.push_back( Token(INTEGER, std::string(temp, position)) );
        position++;
    }
    else {
        std::cout << "Invalid non-digit character encountered while parsing integer token \n";
        exit(0);
    }
}

void Lexer::consumeChar(){
    // position iterator is on the first quote of the char, which was already checked
    std::string::iterator temp { position };

    position ++;
    if (positionValid() && *position =='\''){
        std::cout << "Invalid char token \n";
        exit(0);
    }

    position ++;
    if (positionValid() && *position !='\''){
        std::cout << "Invalid char token \n";
        exit(0);
    }

    if (!positionValid()){ 
        std::cout << "EOF reached while parsing char \n";
        exit(0);
    }
    tokens.push_back( Token(CHAR, std::string(temp+1, temp+2)) );
    position ++;
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
        std::cout << "EOF reached while parsing multiline comment \n";
        exit(0);
    }
    if (*position == '}'){
        tokens.push_back( Token(COMMENT_2, std::string(temp, position)) );
        position++;
    }
}


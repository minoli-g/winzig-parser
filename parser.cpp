#include "parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens){
    //TODO - Strip comment tokens from the list
    this->tokens = tokens;
    this->position = this->tokens.begin();
    this->stack = { };
}

bool Parser::positionValid(){
    if (position < tokens.end() ){
        return true;
    }
    return false;
}

Token Parser::peekNextToken(){
    if (position+1 < tokens.end()){
        return *(position+1);
    }
    throw std::runtime_error("Attempted to peek ahead at EOF");
}

// Consumes the token at the current position and push a tree node to stack if required.
void Parser::readToken(){

    Token t = *position;

    switch (t.getType()){
        case TokenType::IDENTIFER:
            TreeNode* tn = new TreeNode(TreeNodeType::IDENTIFER);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case INTEGER:
            TreeNode* tn = new TreeNode(TreeNodeType::INTEGER);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case CHAR:
            TreeNode* tn = new TreeNode(TreeNodeType::CHAR);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case STRING:
            TreeNode* tn = new TreeNode(TreeNodeType::STRING);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        default:
            // Do not construct tree nodes for other types of tokens
            break;
    }
    position ++;
}

// Consumes the token at the current position only if it is of the expected type
void Parser::readExpectedToken(TokenType type){
    if (peekNextToken().getType() != type){
        throw std::runtime_error(
            "Expected token of type " + 
            Token::typeToString(type) + 
            ", got token of type " +
            Token::typeToString(peekNextToken().getType())
        );
    }
    else {
        readToken();
    }
}

void Parser::buildTree(TreeNodeType type, int num_children){

    TreeNode* tn = new TreeNode(type);
    for (int i=0; i<num_children; ++i){
        tn->addChild(stack.top());
        stack.pop();
    }

}

// Parses the production
// Winzig -> 'program' Name ':' Consts Types Dclns SubProgs Body Name '.' => "program"
void Parser::parseWinzig(){
    readExpectedToken(TokenType::PROGRAM);
    parseName();
    readExpectedToken(TokenType::SEMICOLON);
    parseConsts();
    parseTypes();
    parseDclns();
    parseSubProgs();
    parseBody();
    parseName();
    readExpectedToken(TokenType::PERIOD); 
    buildTree(TreeNodeType::PROGRAM, 7);   
}

// Parse the productions
// Consts -> 'const' Const list ',' ';' => "consts"
//        ->                            => "consts";
void Parser::parseConsts(){
    if (peekNextToken().getType() != TokenType::CONST){
        buildTree(TreeNodeType::CONSTS, 0);
    }
    else {
        readExpectedToken(TokenType::CONST);
        parseConst();
        int n = 1;
        while (peekNextToken().getType() != TokenType::SEMICOLON){
            readExpectedToken(TokenType::COMMA);
            parseConst();
            n ++;
        }
        readExpectedToken(TokenType::SEMICOLON);
        buildTree(TreeNodeType::CONSTS, n);
    }
}

// Parses the productions
// Const -> Name '=' ConstValue => "const"
void Parser::parseConst(){
    parseName();
    readExpectedToken(TokenType::EQ);
    parseConstValue();
    buildTree(TreeNodeType::CONST, 2);
}

// Parses the productions
//ConstValue -> '<integer>'
//           -> '<char>'
//           -> Name;
void Parser::parseConstValue(){
    switch (peekNextToken().getType()){
        case TokenType::INTEGER:
            readExpectedToken(TokenType::INTEGER);
            break;
        case TokenType::CHAR:
            readExpectedToken(TokenType::CHAR);
            break;
        default:
            parseName();
            break;
    }
}

// Parses the productions
// Types -> 'type' (Type ';')+ => "types"
//       ->                    => "types";
void Parser::parseTypes(){
    if (peekNextToken().getType() == TokenType::TYPE){
        readExpectedToken(TokenType::TYPE);

        parseType();
        readExpectedToken(TokenType::SEMICOLON);

        int n=1;
        while (peekNextToken().getType() == TokenType::IDENTIFER){
            parseType();
            readExpectedToken(TokenType::SEMICOLON);
            n++;
        }

        buildTree(TreeNodeType::TYPES, n);
    }
    else {
        buildTree(TreeNodeType::TYPES, 0);
    }
}

// Parses the production
// Type -> Name '=' LitList => "type"
void Parser::parseType(){
    parseName();
    readExpectedToken(TokenType::EQ);
    parseLitList();
    buildTree(TreeNodeType::TYPE, 2);
}

// Parses the production
// LitList -> '(' Name list ',' ')' => "lit"
void Parser::parseLitList(){
    readExpectedToken(TokenType::OPENBRKT);
    parseName();

    int n = 1;
    while (peekNextToken().getType() != TokenType::SEMICOLON){
        readExpectedToken(TokenType::COMMA);
        parseName();
        n ++;
    }
    readExpectedToken(TokenType::CLSBRKT);
    buildTree(TreeNodeType::TYPE, n);
}

// Parses the production
// SubProgs -> Fcn* => "subprogs"
void Parser::parseSubProgs(){
    int n = 0;
    while (peekNextToken().getType()==TokenType::FUNCTION){
        parseFcn();
        n++;
    }
    buildTree(TreeNodeType::SUBPROGS, n);
}

// Parses the production
// Fcn -> 'function' Name '(' Params ')' ':' Name ';' Consts Types Dclns Body Name ';' => "fcn"
void Parser::parseFcn(){
    readExpectedToken(TokenType::FUNCTION);
    parseName();
    readExpectedToken(TokenType::OPENBRKT);
    parseParams();
    readExpectedToken(TokenType::CLSBRKT);
    readExpectedToken(TokenType::SEMICOLON);
    parseName();
    readExpectedToken(TokenType::SEMICOLON);
    parseConsts();
    parseTypes();
    parseDclns();
    parseBody();
    parseName();
    readExpectedToken(TokenType::SEMICOLON);
    buildTree(TreeNodeType::FCN, 8);
}



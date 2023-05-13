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

// Parses the production
// Params -> Dcln list ';' => "params"
void Parser::parseParams(){
    parseDcln();
    int n=1;
    while (peekNextToken().getType() == TokenType::IDENTIFER){
        parseDcln();
        n++;
    }
    buildTree(TreeNodeType::PARAMS, n);
}

// Parses the productions
// Dclns -> 'var' (Dcln ';')+ => "dclns"
//       ->                   => "dclns"
void Parser::parseDclns(){

    if (peekNextToken().getType() == TokenType::VAR){
        readExpectedToken(TokenType::VAR);
        parseDcln();
        readExpectedToken(TokenType::SEMICOLON);

        int n=1;
        while (peekNextToken().getType() == TokenType::IDENTIFER){
            parseDcln();
            readExpectedToken(TokenType::SEMICOLON);
            n++;
        }
        buildTree(TreeNodeType::DCLNS, n);
    }
    else {
        buildTree(TreeNodeType::DCLNS, 0);
    }
}

// Parses the production
// Dcln -> Name list ',' ':' Name => "var";
void Parser::parseDcln(){
    parseName();
    int n=1;
    while (peekNextToken().getType() == TokenType::COMMA){
        readExpectedToken(TokenType::COMMA);
        parseName();
        n++;
    }
    readExpectedToken(TokenType::SEMICOLON);
    parseName();
    buildTree(TreeNodeType::VAR, n+1);
}

// Parses the production
// Body -> 'begin' Statement list ';' 'end' => "block";
void Parser::parseBody(){
    readExpectedToken(TokenType::BEGIN);
    parseStatement();
    int n=1;
    while (peekNextToken().getType() == TokenType::SEMICOLON){
        readExpectedToken(TokenType::SEMICOLON);
        parseStatement();
        n++;
    }
    readExpectedToken(TokenType::END);
    buildTree(TreeNodeType::BLOCK, n);
}

// Parses the productions
// Statement -> Assignment
//           -> 'output' '(' OutExp list ',' ')'                         => "output"
//           -> 'if' Expression 'then' Statement ('else' Statement)?     => "if"
//           -> 'while' Expression 'do' Statement                        => "while"
//           -> 'repeat' Statement list ';' 'until' Expression           => "repeat"
//           -> 'for' '(' ForStat ';' ForExp ';' ForStat ')' Statement   => "for"
//           -> 'loop' Statement list ';' 'pool'                         => "loop"
//           -> 'case' Expression 'of' Caseclauses OtherwiseClause 'end' => "case"
//           -> 'read' '(' Name list ',' ')' => "read"
//           -> 'exit'                       => "exit"
//           -> 'return' Expression          => "return"
//           -> Body
//           ->                              => "<null>"
void Parser::parseStatement(){
    switch (peekNextToken().getType()){

        case TokenType::OUTPUT:
            readExpectedToken(TokenType::OUTPUT);
            readExpectedToken(TokenType::OPENBRKT);
            parseOutExp();
            int n = 1;
            while (peekNextToken().getType() == TokenType::COMMA){
                readExpectedToken(TokenType::COMMA);
                parseOutExp();
                n++;
            }
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::OUTPUT, n);
            break;

        case TokenType::IF:
            readExpectedToken(TokenType::IF);
            parseExpression();
            readExpectedToken(TokenType::THEN);
            parseStatement();
            int n = 2;
            if (peekNextToken().getType() == TokenType::ELSE){
                readExpectedToken(TokenType::ELSE);
                parseStatement();
                n ++;
            }
            buildTree(TreeNodeType::IF, n);
            break;

        case TokenType::WHILE:
            readExpectedToken(TokenType::WHILE);
            parseExpression();
            readExpectedToken(TokenType::DO);
            parseStatement();
            buildTree(TreeNodeType::WHILE, 2);
            break;
        
        case TokenType::REPEAT:
            readExpectedToken(TokenType::REPEAT);
            parseStatement();
            int n=1;
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                parseStatement();
                n ++;
            }
            readExpectedToken(TokenType::UNTIL);
            parseExpression();
            buildTree(TreeNodeType::REPEAT, n+1);
            break;

        case TokenType::FOR:
            readExpectedToken(TokenType::FOR);
            readExpectedToken(TokenType::OPENBRKT);
            parseForStat();
            readExpectedToken(TokenType::SEMICOLON);
            parseForExp();
            readExpectedToken(TokenType::SEMICOLON);
            parseForStat();
            readExpectedToken(TokenType::CLSBRKT);
            parseStatement();
            buildTree(TreeNodeType::FOR, 4);
            break;

        case TokenType::LOOP:
            readExpectedToken(TokenType::LOOP);
            parseStatement();
            int n=1;
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                parseStatement();
                n ++;
            }
            readExpectedToken(TokenType::POOL);
            buildTree(TreeNodeType::LOOP, n);
            break;

        case TokenType::CASE:
            readExpectedToken(TokenType::CASE);
            parseExpression();
            readExpectedToken(TokenType::OF);
            parseCaseclauses();
            parseOtherwiseClause();
            readExpectedToken(TokenType::END);
            buildTree(TreeNodeType::CASE, 3);
            break;

        case TokenType::READ:
            readExpectedToken(TokenType::READ);
            readExpectedToken(TokenType::OPENBRKT);
            parseName();

            int n=1;
            while (peekNextToken().getType() == TokenType::COMMA){
                readExpectedToken(TokenType::COMMA);
                parseName();
                n ++;
            }
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::READ, n);
            break;

        case TokenType::EXIT:
            readExpectedToken(TokenType::EXIT);
            buildTree(TreeNodeType::EXIT, 0);
            break;

        case TokenType::RETURN:
            readExpectedToken(TokenType::RETURN);
            parseExpression();
            buildTree(TreeNodeType::RETURN, 1);
            break;

        case TokenType::IDENTIFER:
            // All Assignment productions begin with this token
            parseAssignment();
            break;

        case TokenType::BEGIN:
            // All Body productions begin with this token
            parseBody();
            break;

        default:
            buildTree(TreeNodeType::NNULL, 0);
            break;
    }
}

// Parses the productions
// OutExp -> Expression => "integer"
//        -> StringNode => "string";
void Parser::parseOutExp(){
    if (peekNextToken().getType() == TokenType::STRING){
        parseStringNode();
        buildTree(TreeNodeType::STRING, 1);
    }
    else {
        parseExpression();
        buildTree(TreeNodeType::INTEGER, 1);
    }
}

#include "parser.hpp"
#include <stdexcept>
#include <unordered_set>

Parser::Parser(std::vector<Token> lexer_tokens){
    this->tokens = { };
    // Strip comment tokens from the list
    for (Token t: lexer_tokens){
        if (t.getType() != TokenType::COMMENT_1 || t.getType() != TokenType::COMMENT_2){
            this->tokens.push_back(t);
        }
    }
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
// Returns the number of tree nodes pushed to stack
int Parser::parseWinzig(){
    int tn = 0;
    readExpectedToken(TokenType::PROGRAM);
    tn += parseName();
    readExpectedToken(TokenType::SEMICOLON);
    tn += parseConsts();
    tn += parseTypes();
    tn += parseDclns();
    tn += parseSubProgs();
    tn += parseBody();
    tn += parseName();
    readExpectedToken(TokenType::PERIOD); 
    buildTree(TreeNodeType::PROGRAM, tn); 
    return 1;  
}

// Parse the productions
// Consts -> 'const' Const list ',' ';' => "consts"
//        ->                            => "consts";
// Returns the number of tree nodes pushed to stack
int Parser::parseConsts(){
    if (peekNextToken().getType() != TokenType::CONST){
        buildTree(TreeNodeType::CONSTS, 0);
        return 1;
    }
    else {
        readExpectedToken(TokenType::CONST);
        int tn = 0;
        tn += parseConst();
        while (peekNextToken().getType() != TokenType::SEMICOLON){
            readExpectedToken(TokenType::COMMA);
            tn += parseConst();
        }
        readExpectedToken(TokenType::SEMICOLON);
        buildTree(TreeNodeType::CONSTS, tn);
        return 1;
    }
}

// Parses the productions
// Const -> Name '=' ConstValue => "const"
// Returns the number of tree nodes pushed to stack
int Parser::parseConst(){
    int tn = 0;
    tn += parseName();
    readExpectedToken(TokenType::EQ);
    tn += parseConstValue();
    buildTree(TreeNodeType::CONST, tn);
    return 1;
}

// Parses the productions
//ConstValue -> '<integer>'
//           -> '<char>'
//           -> Name;
// Returns the number of tree nodes pushed to stack
int Parser::parseConstValue(){
    switch (peekNextToken().getType()){
        case TokenType::INTEGER:
            readExpectedToken(TokenType::INTEGER);
            return 1;
            break;
        case TokenType::CHAR:
            readExpectedToken(TokenType::CHAR);
            return 1;
            break;
        default:
            return parseName();
            break;
    }
}

// Parses the productions
// Types -> 'type' (Type ';')+ => "types"
//       ->                    => "types";
// Returns the number of tree nodes pushed to stack
int Parser::parseTypes(){
    if (peekNextToken().getType() == TokenType::TYPE){
        int tn = 0;
        readExpectedToken(TokenType::TYPE);

        tn += parseType();
        readExpectedToken(TokenType::SEMICOLON);

        while (peekNextToken().getType() == TokenType::IDENTIFER){
            tn += parseType();
            readExpectedToken(TokenType::SEMICOLON);
        }

        buildTree(TreeNodeType::TYPES, tn);
        return 1;
    }
    else {
        buildTree(TreeNodeType::TYPES, 0);
        return 1;
    }
}

// Parses the production
// Type -> Name '=' LitList => "type"
// Returns the number of tree nodes pushed to stack
int Parser::parseType(){
    int tn = 0;
    tn += parseName();
    readExpectedToken(TokenType::EQ);
    tn += parseLitList();
    buildTree(TreeNodeType::TYPE, tn);
    return 1;
}

// Parses the production
// LitList -> '(' Name list ',' ')' => "lit"
// Returns the number of tree nodes pushed to stack
int Parser::parseLitList(){
    int tn = 0;
    readExpectedToken(TokenType::OPENBRKT);
    tn += parseName();

    while (peekNextToken().getType() != TokenType::SEMICOLON){
        readExpectedToken(TokenType::COMMA);
        tn += parseName();
    }
    readExpectedToken(TokenType::CLSBRKT);
    buildTree(TreeNodeType::TYPE, tn);
    return 1;
}

// Parses the production
// SubProgs -> Fcn* => "subprogs"
// Returns the number of tree nodes pushed to stack
int Parser::parseSubProgs(){
    int tn = 0;
    while (peekNextToken().getType()==TokenType::FUNCTION){
        tn += parseFcn();
    }
    buildTree(TreeNodeType::SUBPROGS, tn);
    return 1;
}

// Parses the production
// Fcn -> 'function' Name '(' Params ')' ':' Name ';' Consts Types Dclns Body Name ';' => "fcn"
// Returns the number of tree nodes pushed to stack
int Parser::parseFcn(){
    int tn = 0;
    readExpectedToken(TokenType::FUNCTION);
    tn += parseName();
    readExpectedToken(TokenType::OPENBRKT);
    tn += parseParams();
    readExpectedToken(TokenType::CLSBRKT);
    readExpectedToken(TokenType::SEMICOLON);
    tn += parseName();
    readExpectedToken(TokenType::SEMICOLON);
    tn += parseConsts();
    tn += parseTypes();
    tn += parseDclns();
    tn += parseBody();
    tn += parseName();
    readExpectedToken(TokenType::SEMICOLON);
    buildTree(TreeNodeType::FCN, tn);
    return 1;
}

// Parses the production
// Params -> Dcln list ';' => "params"
// Returns the number of tree nodes pushed to stack
int Parser::parseParams(){
    int tn = 0;
    tn += parseDcln();
    while (peekNextToken().getType() == TokenType::IDENTIFER){
        tn += parseDcln();
    }
    buildTree(TreeNodeType::PARAMS, tn);
    return 1;
}

// Parses the productions
// Dclns -> 'var' (Dcln ';')+ => "dclns"
//       ->                   => "dclns"
// Returns the number of tree nodes pushed to stack
int Parser::parseDclns(){

    if (peekNextToken().getType() == TokenType::VAR){
        int tn = 0;
        readExpectedToken(TokenType::VAR);
        tn += parseDcln();
        readExpectedToken(TokenType::SEMICOLON);

        while (peekNextToken().getType() == TokenType::IDENTIFER){
            tn += parseDcln();
            readExpectedToken(TokenType::SEMICOLON);
        }
        buildTree(TreeNodeType::DCLNS, tn);
        return 1;
    }
    else {
        buildTree(TreeNodeType::DCLNS, 0);
        return 1;
    }
}

// Parses the production
// Dcln -> Name list ',' ':' Name => "var";
// Returns the number of tree nodes pushed to stack
int Parser::parseDcln(){
    int tn = 0;
    tn += parseName();
    while (peekNextToken().getType() == TokenType::COMMA){
        readExpectedToken(TokenType::COMMA);
        tn += parseName();
    }
    readExpectedToken(TokenType::SEMICOLON);
    tn += parseName();
    buildTree(TreeNodeType::VAR, tn);
    return 1;
}

// Parses the production
// Body -> 'begin' Statement list ';' 'end' => "block";
// Returns the number of tree nodes pushed to stack
int Parser::parseBody(){
    int tn = 0;
    readExpectedToken(TokenType::BEGIN);
    tn += parseStatement();
    
    while (peekNextToken().getType() == TokenType::SEMICOLON){
        readExpectedToken(TokenType::SEMICOLON);
        tn += parseStatement();
    }
    readExpectedToken(TokenType::END);
    buildTree(TreeNodeType::BLOCK, tn);
    return 1;
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
// Returns the number of tree nodes pushed to stack
int Parser::parseStatement(){
    switch (peekNextToken().getType()){

        case TokenType::OUTPUT:
            int tn = 0;
            readExpectedToken(TokenType::OUTPUT);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseOutExp();
            
            while (peekNextToken().getType() == TokenType::COMMA){
                readExpectedToken(TokenType::COMMA);
                tn += parseOutExp();
            }
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::OUTPUT, tn);
            return 1;
            break;

        case TokenType::IF:
            int tn = 0;
            readExpectedToken(TokenType::IF);
            tn += parseExpression();
            readExpectedToken(TokenType::THEN);
            tn += parseStatement();
            
            if (peekNextToken().getType() == TokenType::ELSE){
                readExpectedToken(TokenType::ELSE);
                tn += parseStatement();
            }
            buildTree(TreeNodeType::IF, tn);
            return 1;
            break;

        case TokenType::WHILE:
            int tn = 0;
            readExpectedToken(TokenType::WHILE);
            tn += parseExpression();
            readExpectedToken(TokenType::DO);
            tn += parseStatement();
            buildTree(TreeNodeType::WHILE, tn);
            return 1;
            break;
        
        case TokenType::REPEAT:
            int tn = 0;
            readExpectedToken(TokenType::REPEAT);
            tn += parseStatement();
            
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                tn += parseStatement();
            }
            readExpectedToken(TokenType::UNTIL);
            tn += parseExpression();
            buildTree(TreeNodeType::REPEAT, tn);
            break;

        case TokenType::FOR:
            int tn = 0;
            readExpectedToken(TokenType::FOR);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseForStat();
            readExpectedToken(TokenType::SEMICOLON);
            tn += parseForExp();
            readExpectedToken(TokenType::SEMICOLON);
            tn += parseForStat();
            readExpectedToken(TokenType::CLSBRKT);
            tn += parseStatement();
            buildTree(TreeNodeType::FOR, tn);
            return 1;
            break;

        case TokenType::LOOP:
            int tn = 0;
            readExpectedToken(TokenType::LOOP);
            tn += parseStatement();
            
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                tn += parseStatement();
            }
            readExpectedToken(TokenType::POOL);
            buildTree(TreeNodeType::LOOP, tn);
            return 1;
            break;

        case TokenType::CASE:
            int tn = 0;
            readExpectedToken(TokenType::CASE);
            tn += parseExpression();
            readExpectedToken(TokenType::OF);
            tn += parseCaseclauses();
            tn += parseOtherwiseClause();
            readExpectedToken(TokenType::END);
            buildTree(TreeNodeType::CASE, tn);
            return 1;
            break;

        case TokenType::READ:
            int tn = 0;
            readExpectedToken(TokenType::READ);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseName();

            while (peekNextToken().getType() == TokenType::COMMA){
                readExpectedToken(TokenType::COMMA);
                tn += parseName();
            }
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::READ, tn);
            return 1;
            break;

        case TokenType::EXIT:
            readExpectedToken(TokenType::EXIT);
            buildTree(TreeNodeType::EXIT, 0);
            return 1;
            break;

        case TokenType::RETURN:
            readExpectedToken(TokenType::RETURN);
            int tn = parseExpression();
            buildTree(TreeNodeType::RETURN, tn);
            return 1;
            break;

        case TokenType::IDENTIFER:
            // All Assignment productions begin with this token
            return parseAssignment();
            break;

        case TokenType::BEGIN:
            // All Body productions begin with this token
            return parseBody();
            break;

        default:
            buildTree(TreeNodeType::NNULL, 0);
            return 1;
            break;
    }
}

// Parses the productions
// OutExp -> Expression => "integer"
//        -> StringNode => "string";
// Returns the number of tree nodes pushed to stack
int Parser::parseOutExp(){
    if (peekNextToken().getType() == TokenType::STRING){
        int tn = parseStringNode();
        buildTree(TreeNodeType::STRING, tn);
        return 1;
    }
    else {
        int tn = parseExpression();
        buildTree(TreeNodeType::INTEGER, tn);
        return 1;
    }
}

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
    TreeNode* tn;

    switch (t.getType()){
        case TokenType::IDENTIFER:
            tn = new TreeNode(TreeNodeType::IDENTIFER);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case TokenType::INTEGER:
            tn = new TreeNode(TreeNodeType::INTEGER);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case TokenType::CHAR:
            tn = new TreeNode(TreeNodeType::CHAR);
            tn->addChild(new TreeNode(t.getValue()));
            stack.push(tn);
            break;

        case TokenType::STRING:
            tn = new TreeNode(TreeNodeType::STRING);
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
            "Expected token of type " + std::to_string((int) type) +
            ", got token of type " + std::to_string((int) peekNextToken().getType())
        );
    }
    else {
        readToken();
    }
}

void Parser::buildTree(TreeNodeType type, int num_children){

    TreeNode* tn = new TreeNode(type);
    std::stack<TreeNode*> temp = { };

    for (int i=0; i<num_children; ++i){
        temp.push(stack.top());
        stack.pop();
    }

    for (int i=0; i<num_children; ++i){
        tn->addChild(temp.top());
        temp.pop();
    }
    stack.push(tn);
}

TreeNode* Parser::returnFinalTree(){
    return stack.top();
}

// Parses the production
// Winzig -> 'program' Name ':' Consts Types Dclns SubProgs Body Name '.' => "program"
// Returns the number of tree nodes pushed to stack
int Parser::parseWinzig(){
    int tn = 0;
    readExpectedToken(TokenType::PROGRAM);
    tn += parseName();
    readExpectedToken(TokenType::COLON);
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
    readExpectedToken(TokenType::COLON);
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
    readExpectedToken(TokenType::COLON);
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

    int tn = 0;
    switch (peekNextToken().getType()){

        case TokenType::OUTPUT:
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

        case TokenType::IF:
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

        case TokenType::WHILE:
            readExpectedToken(TokenType::WHILE);
            tn += parseExpression();
            readExpectedToken(TokenType::DO);
            tn += parseStatement();
            buildTree(TreeNodeType::WHILE, tn);
            return 1;
        
        case TokenType::REPEAT:
            readExpectedToken(TokenType::REPEAT);
            tn += parseStatement();
            
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                tn += parseStatement();
            }
            readExpectedToken(TokenType::UNTIL);
            tn += parseExpression();
            buildTree(TreeNodeType::REPEAT, tn);
            return 1;

        case TokenType::FOR:
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

        case TokenType::LOOP:
            readExpectedToken(TokenType::LOOP);
            tn += parseStatement();
            
            while (peekNextToken().getType() == TokenType::SEMICOLON){
                readExpectedToken(TokenType::SEMICOLON);
                tn += parseStatement();
            }
            readExpectedToken(TokenType::POOL);
            buildTree(TreeNodeType::LOOP, tn);
            return 1;

        case TokenType::CASE:
            readExpectedToken(TokenType::CASE);
            tn += parseExpression();
            readExpectedToken(TokenType::OF);
            tn += parseCaseclauses();
            tn += parseOtherwiseClause();
            readExpectedToken(TokenType::END);
            buildTree(TreeNodeType::CASE, tn);
            return 1;

        case TokenType::READ:
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

        case TokenType::EXIT:
            readExpectedToken(TokenType::EXIT);
            buildTree(TreeNodeType::EXIT, 0);
            return 1;

        case TokenType::RETURN:
            readExpectedToken(TokenType::RETURN);
            tn += parseExpression();
            buildTree(TreeNodeType::RETURN, tn);
            return 1;

        case TokenType::IDENTIFER:
            // All Assignment productions begin with this token
            return parseAssignment();

        case TokenType::BEGIN:
            // All Body productions begin with this token
            return parseBody();

        default:
            buildTree(TreeNodeType::NNULL, 0);
            return 1;
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

// Parses the production
// StringNode -> '<string>'
// Returns the number of tree nodes pushed to stack
int Parser::parseStringNode(){
    readExpectedToken(TokenType::STRING);
    return 1;
}

// Parses the production
// Caseclauses-> (Caseclause ';')+
// Returns the number of tree nodes added to the stack
int Parser::parseCaseclauses(){
    int tn = 0;
    tn += parseCaseclause();
    readExpectedToken(TokenType::SEMICOLON);
    
    std::unordered_set<TokenType> select_set = {TokenType::INTEGER, TokenType::IDENTIFER, TokenType::CHAR};
    while (select_set.count(peekNextToken().getType())){
        tn += parseCaseclause();
        readExpectedToken(TokenType::SEMICOLON);
    }
    return tn;
}

// Parses the production
// Caseclause -> CaseExpression list ',' ':' Statement => "case_clause"
// Returns the number of tree nodes added to the stack
int Parser::parseCaseclause(){
    int tn = 0;
    tn += parseCaseExpression();

    while (peekNextToken().getType() == TokenType::COMMA){
        readExpectedToken(TokenType::COMMA);
        tn += parseCaseExpression();
    }
    readExpectedToken(TokenType::COLON);
    tn += parseStatement();
    buildTree(TreeNodeType::CASECLAUSE, tn);
    return 1;
}

// Parses the production
// CaseExpression -> ConstValue
//                -> ConstValue '..' ConstValue => ".."
// Returns the number of tree nodes added to the stack
int Parser::parseCaseExpression(){
    int tn = parseConstValue();
    if (peekNextToken().getType() == TokenType::DOTS){
        readExpectedToken(TokenType::DOTS);
        tn += parseConstValue();
        buildTree(TreeNodeType::DOTS, tn);
        return 1;
    }
    else {
        return tn;
    }
}

// Parses the production
// OtherwiseClause -> 'otherwise' Statement => "otherwise"
//                 -> 
// Returns the number of tree nodes added to the stack
int Parser::parseOtherwiseClause(){
    if (peekNextToken().getType() == TokenType::OTHERWISE){
        readExpectedToken(TokenType::OTHERWISE);
        int tn = parseStatement();
        buildTree(TreeNodeType::OTHERWISE, tn);
        return 1;
    }
    else {
        return 0;
    }
}

// Parses the productions
// Assignment -> Name ':=' Expression => "assign"
//            -> Name ':=:' Name      => "swap"
// Returns the number of tree nodes added to the stack
int Parser::parseAssignment(){
    int tn = parseName();
    if (peekNextToken().getType() == TokenType::ASSIGN){
        readExpectedToken(TokenType::ASSIGN);
        tn += parseExpression();
        buildTree(TreeNodeType::ASSIGN, tn);
        return 1;
    }
    else {
        readExpectedToken(TokenType::SWAP);
        tn += parseName();
        buildTree(TreeNodeType::SWAP, tn);
        return 1;
    }
}

// Parses the productions
// ForStat -> Assignment
//         ->             => "<null>"
// Returns the number of tree nodes added to the stack
int Parser::parseForStat(){
    // Select set of Assignment is { Identifier }
    if (peekNextToken().getType() == TokenType::IDENTIFER){
        return parseAssignment();
    }
    else{
        buildTree(TreeNodeType::NNULL, 0);
        return 1;
    }
}

// Parses the productions
// ForExp -> Expression
//        ->              => "true"
// Returns the number of tree nodes added to the stack
int Parser::parseForExp(){
    std::unordered_set<TokenType> select_set = {
        TokenType::MINUS, TokenType::PLUS, TokenType::NOT, TokenType::EOFT, TokenType::IDENTIFER,
        TokenType::INTEGER, TokenType::CHAR, TokenType::OPENBRKT, 
        TokenType::SUCC, TokenType::PRED, TokenType::CHR, TokenType::ORD
    };

    if (select_set.count(peekNextToken().getType())){
        return parseExpression();
    }
    else {
        buildTree(TreeNodeType::TRUE, 0);
        return 1;
    }
}

// Parses the productions
// Expression -> Term
//            -> Term '<=' Term  => "<="
//            -> Term '<' Term   => "<"
//            -> Term '>=' Term  => ">="
//            -> Term '>' Term   => ">"
//            -> Term '=' Term   => "="
//            -> Term '<>' Term  => "<>"
// Returns the number of tree nodes added to the stack
int Parser::parseExpression(){
    int tn = parseTerm();
    switch (peekNextToken().getType()){

        case TokenType::LEQ:
            readExpectedToken(TokenType::LEQ);
            tn += parseTerm();
            buildTree(TreeNodeType::LEQ, tn);
            return 1;
            break;

        case TokenType::LE:
            readExpectedToken(TokenType::LE);
            tn += parseTerm();
            buildTree(TreeNodeType::LE, tn);
            return 1;
            break;

        case TokenType::GEQ:
            readExpectedToken(TokenType::GEQ);
            tn += parseTerm();
            buildTree(TreeNodeType::GEQ, tn);
            return 1;
            break;

        case TokenType::GE:
            readExpectedToken(TokenType::GE);
            tn += parseTerm();
            buildTree(TreeNodeType::GE, tn);
            return 1;
            break;

        case TokenType::EQ:
            readExpectedToken(TokenType::EQ);
            tn += parseTerm();
            buildTree(TreeNodeType::EQ, tn);
            return 1;
            break;

        case TokenType::NEQ:
            readExpectedToken(TokenType::NEQ);
            tn += parseTerm();
            buildTree(TreeNodeType::NEQ, tn);
            return 1;
            break;

        default:
            // Just the Term on its own
            return tn;
            break;
    }

}

// Parses the productions
// Term -> Factor
//      -> Term '+' Factor  => "+"
//      -> Term '-' Factor  => "-"
//      -> Term 'or' Factor => "or"
// Returns the number of tree nodes added to the stack
int Parser::parseTerm(){
    int tn = parseFactor();
    std::unordered_set<TokenType> next_set = { TokenType::PLUS, TokenType::MINUS, TokenType::OR};
    
    while (next_set.count(peekNextToken().getType())){
        int p = 0;
        switch (peekNextToken().getType()){

            case TokenType::PLUS:
                readExpectedToken(TokenType::PLUS);
                p = parseFactor();
                buildTree(TreeNodeType::PLUS, tn+p);
                break;

            case TokenType::MINUS:
                readExpectedToken(TokenType::MINUS);
                p = parseFactor();
                buildTree(TreeNodeType::MINUS, tn+p);
                break;    

            case TokenType::OR:
                readExpectedToken(TokenType::OR);
                p = parseFactor();
                buildTree(TreeNodeType::OR, tn+p);
                break; 

            default:
                // This shouldn't happen
                throw std::runtime_error("Error occurred when parsing Term");
        }
    }
    return 1;
}

// Parses the productions
// Factor  -> Factor '*' Primary     => "*"
//         -> Factor '/' Primary    => "/"
//         -> Factor 'and' Primary  => "and"
//         -> Factor 'mod' Primary  => "mod"
//         -> Primary
// Returns the number of tree nodes added to the stack
int Parser::parseFactor(){
    int tn = parsePrimary();
    std::unordered_set<TokenType> next_set = { TokenType::MULT, TokenType::DIVIDE, TokenType::AND, TokenType::MOD};
    
    while (next_set.count(peekNextToken().getType())){
        int p = 0;
        switch (peekNextToken().getType()){

            case TokenType::MULT:
                readExpectedToken(TokenType::MULT);
                p = parsePrimary();
                buildTree(TreeNodeType::MULT, tn+p);
                break;

            case TokenType::DIVIDE:
                readExpectedToken(TokenType::DIVIDE);
                p = parsePrimary();
                buildTree(TreeNodeType::DIVIDE, tn+p);
                break;    

            case TokenType::AND:
                readExpectedToken(TokenType::AND);
                p = parsePrimary();
                buildTree(TreeNodeType::AND, tn+p);
                break;     

            case TokenType::MOD:
                readExpectedToken(TokenType::MOD);
                p = parsePrimary();
                buildTree(TreeNodeType::MOD, tn+p);
                break; 

            default:
                // This shouldn't happen
                throw std::runtime_error("Error occurred when parsing Factor");
        }
    }
    return 1;
}

// Parses the productions
// Primary -> '-' Primary   => "-"
//         -> '+' Primary
//         -> 'not' Primary => "not"
//         -> 'eof'         => "eof"
//         -> Name
//         -> '<integer>'
//         -> '<char>'
//         -> Name '(' Expression list ',' ')' => "call"
//         -> '(' Expression ')'
//         -> 'succ' '(' Expression ')' => "succ"
//         -> 'pred' '(' Expression ')' => "pred"
//         -> 'chr' '(' Expression ')'  => "chr"
//         -> 'ord' '(' Expression ')'  => "ord"
// Returns the number of tree nodes added to the stack
int Parser::parsePrimary(){
    int tn = 0;

    switch (peekNextToken().getType()){

        case TokenType::MINUS:
            readExpectedToken(TokenType::MINUS);
            tn += parsePrimary();
            buildTree(TreeNodeType::MINUS, tn);
            return 1;

        case TokenType::PLUS:
            readExpectedToken(TokenType::PLUS);
            return parsePrimary();

        case TokenType::NOT:
            readExpectedToken(TokenType::NOT);
            tn += parsePrimary();
            buildTree(TreeNodeType::NOT, tn);
            return 1;

        case TokenType::EOFT:
            readExpectedToken(TokenType::EOFT);
            buildTree(TreeNodeType::EOFT, 0);
            return 1;

        case TokenType::IDENTIFER:
            // All productions startng with Name
            tn += parseName();

            if (peekNextToken().getType() == TokenType::OPENBRKT){
                readExpectedToken(TokenType::OPENBRKT);
                tn += parseExpression();
                while (peekNextToken().getType() == TokenType::COMMA){
                    readExpectedToken(TokenType::COMMA);
                    tn += parseExpression();
                }
                readExpectedToken(TokenType::CLSBRKT);
                buildTree(TreeNodeType::CALL, tn);
                return 1;
            }
            else {
                return tn;
            }

        case TokenType::INTEGER:
            readExpectedToken(TokenType::INTEGER);
            return 1;

        case TokenType::CHAR:
            readExpectedToken(TokenType::CHAR);
            return 1;

        case TokenType::OPENBRKT:
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseExpression();
            readExpectedToken(TokenType::CLSBRKT);
            return tn;

        case TokenType::SUCC:
            readExpectedToken(TokenType::SUCC);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseExpression();
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::SUCC, tn);
            return 1;

        case TokenType::PRED:
            readExpectedToken(TokenType::PRED);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseExpression();
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::PRED, tn);
            return 1;

        case TokenType::CHR:
            readExpectedToken(TokenType::CHR);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseExpression();
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::CHR, tn);
            return 1;

        case TokenType::ORD:
            readExpectedToken(TokenType::ORD);
            readExpectedToken(TokenType::OPENBRKT);
            tn += parseExpression();
            readExpectedToken(TokenType::CLSBRKT);
            buildTree(TreeNodeType::ORD, tn);
            return 1;

        default:
            throw std::runtime_error("Error occurred during parsing Primary");
    }
}

// Parses the production
// Name -> '<identifier>'
// Returns the number of tree nodes added to the stack
int Parser::parseName(){
    readExpectedToken(TokenType::IDENTIFER);
    return 1;
}
#include "parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens){
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





#include "treenode.hpp"

std::unordered_map<TreeNodeType, std::string> TreeNode::type_string_map = {
    {TreeNodeType::IDENTIFER, "<identifier>"}, 
    {TreeNodeType::INTEGER, "<integer>"}, 
    {TreeNodeType::CHAR, "<char>"},
    {TreeNodeType::STRING, "<string>"},
    {TreeNodeType::PROGRAM, "program"},
    {TreeNodeType::CONSTS, "consts"},
    {TreeNodeType::CONST, "const"},
    {TreeNodeType::TYPES, "types"},
    {TreeNodeType::TYPE, "type"},
    {TreeNodeType::LIT, "lit"},
    {TreeNodeType::SUBPROGS, "subprogs"},
    {TreeNodeType::FCN, "fcn"},
    {TreeNodeType::PARAMS, "params"},
    {TreeNodeType::DCLNS, "dclns"},
    {TreeNodeType::VAR, "var"},
    {TreeNodeType::BLOCK, "block"},
    {TreeNodeType::OUTPUT, "output"},
    {TreeNodeType::IF, "if"},
    {TreeNodeType::WHILE, "while"},
    {TreeNodeType::REPEAT, "repeat"},
    {TreeNodeType::FOR, "for"},
    {TreeNodeType::LOOP, "loop"},
    {TreeNodeType::CASE, "case"},
    {TreeNodeType::READ, "read"},
    {TreeNodeType::EXIT, "exit"},
    {TreeNodeType::RETURN, "return"},
    {TreeNodeType::NNULL, "<null>"},
    {TreeNodeType::CASECLAUSE, "case_clause"},
    {TreeNodeType::TN_INTEGER, "integer"},
    {TreeNodeType::DOTS, ".."},
    {TreeNodeType::OTHERWISE, "otherwise"},
    {TreeNodeType::ASSIGN, "assign"},
    {TreeNodeType::SWAP, "swap"},
    {TreeNodeType::TRUE, "true"},
    {TreeNodeType::LEQ, "<="},
    {TreeNodeType::LE, "<"},
    {TreeNodeType::GEQ, ">="},
    {TreeNodeType::GE, ">"},
    {TreeNodeType::EQ, "="},
    {TreeNodeType::NEQ, "<>"},
    {TreeNodeType::PLUS, "+"},
    {TreeNodeType::MINUS, "-"},
    {TreeNodeType::OR, "or"},
    {TreeNodeType::MULT, "*"},
    {TreeNodeType::DIVIDE, "/"},
    {TreeNodeType::AND, "and"},
    {TreeNodeType::MOD, "mod"},
    {TreeNodeType::NOT, "not"},
    {TreeNodeType::EOFT, "eof"},
    {TreeNodeType::CALL, "call"},
    {TreeNodeType::SUCC, "succ"},
    {TreeNodeType::PRED, "pred"},
    {TreeNodeType::CHR, "chr"},
    {TreeNodeType::ORD, "ord"}
};

TreeNode::TreeNode(std::string value){
    this->value=value;
}


TreeNode::TreeNode(TreeNodeType type){
    this->type = type;
    this->value = type_string_map[type];
}

void TreeNode::addChild(TreeNode* child){
    this->children.push_back(child);
}

std::string TreeNode::pprintTree(int depth){
    std::string printStr = "";
    for (int i=0; i<depth; ++i){
        printStr.append(". ");
    }
    int n = this->children.size();

    printStr.append(this->value);
    printStr.append("(");
    printStr.append(std::to_string(n));
    printStr.append(")");

    for (int i=0; i<n; ++i){
        printStr.append("\n");
        printStr.append(this->children[i]->pprintTree(depth+1));
    }
    return printStr;
}
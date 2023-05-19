#include "treenode.hpp"

TreeNode::TreeNode(std::string value){
    this->value=value;
}


TreeNode::TreeNode(TreeNodeType type){
    this->type = type;
    // TODO - Set the value to be taken from the map
}

void TreeNode::addChild(TreeNode* child){
    this->children.push_back(child);
}

std::string TreeNode::pprintTree(int depth){
    std::string printStr = "";
    for (int i=0; i<depth; ++i){
        printStr.append(".");
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
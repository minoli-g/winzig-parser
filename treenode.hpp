#include <vector>
#include <string>

enum TreeNodeType {
    IDENTIFER, INTEGER, CHAR, STRING,
    PROGRAM, CONSTS, CONST, TYPES, TYPE, LIT, SUBPROGS, FCN, PARAMS, DCLNS,
    VAR, BLOCK, OUTPUT, IF, WHILE, REPEAT, FOR, LOOP, CASE, READ, EXIT, RETURN,
    NNULL, CASECLAUSE, DOTS, OTHERWISE, ASSIGN, SWAP, TRUE, LEQ, LE, GEQ, GE, 
    EQ, NEQ, PLUS, MINUS, OR, MULT, DIVIDE, AND, MOD, NOT, EOFT, CALL, SUCC, 
    PRED, CHR, ORD
};

class TreeNode {

    private:
        TreeNodeType type;
        std::string value;
        std::vector<TreeNode*> children;

    public:
        TreeNode(std::string value);
        TreeNode(TreeNodeType type);
        void addChild(TreeNode* child);

        std::string pprintTree();
};
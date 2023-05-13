#include <vector>
#include <stack>
#include "token.hpp"
#include "treenode.hpp"

class Parser {

    private:
        std::vector<Token> tokens;
        std::vector<Token>::iterator position;
        std::stack<TreeNode*> stack;

    public:
        Parser(std::vector<Token> tokens);

        bool positionValid();
        Token peekNextToken();

        void readToken();
        void readExpectedToken(TokenType type);

        void buildTree(TreeNodeType type, int num_children);

        // Functions which parse different types of nonterminals and return the number of tree nodes pushed onto the stack
        // Most often, the return value is 1
        // But a few nonterminals (CaseClauses, OtherwiseClause etc) do not always adhere to this

        int parseWinzig();
        int parseConsts();
        int parseConst();
        int parseConstValue();
        int parseTypes();
        int parseType();
        int parseLitList();
        int parseSubProgs();
        int parseFcn();
        int parseParams();
        int parseDclns();
        int parseDcln();
        int parseBody();
        int parseStatement();
        int parseOutExp();
        int parseStringNode();
        int parseCaseclauses();
        int parseCaseclause();
        int parseCaseExpression();
        int parseOtherwiseClause();
        int parseAssignment();
        int parseForStat();
        int parseForExp();
        int parseExpression();
        int parseTerm();
        int parseFactor();
        int parsePrimary();
        int parseName();
};
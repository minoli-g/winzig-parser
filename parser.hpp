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

        void parseWinzig();
        void parseConsts();
        void parseConst();
        void parseConstValue();
        void parseTypes();
        void parseType();
        void parseLitList();
        void parseSubProgs();
        void parseFcn();
        void parseParams();
        void parseDclns();
        void parseDcln();
        void parseBody();
        void parseStatement();
        void parseOutExp();
        void parseStringNode();
        void parseCaseclauses();
        void parseCaseclause();
        void parseCaseExpression();
        void parseOtherwiseClause();
        void parseAssignment();
        void parseForStat();
        void parseForExp();
        void parseExpression();
        void parseTerm();
        void parseFactor();
        void parsePrimary();
        void parseName();
};
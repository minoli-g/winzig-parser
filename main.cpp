#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "lex.hpp"
#include "token.hpp"
#include "parser.hpp"

int main(int argc, char *argv[]){

    std::cout << "Program started \n";
    
    std::string input_file_path; 
    std::ifstream file; 

    // Accept argument preceded by "--ast", "-ast", or none
    if ((argc==3) && ((std::string(argv[1])=="--ast") || (std::string(argv[1])=="-ast"))) {
        input_file_path = argv[2];
    }
    else if (argc==2){
        input_file_path = argv[1];
    }
    else{
        std::cout << "Error: Argument format incorrect. \n";
    }

    file.open(input_file_path);
    if (!file){
        std::cout << "Error: Could not read file. \n";
    }

    // Read the content of the file
    std::string content (std::istreambuf_iterator<char>{file}, {});
    file.close();

    // Convert the content into tokens
    Lexer lexer (content);
    std::vector<Token> token_sequence;

    try{
        lexer.parse();   
        token_sequence = lexer.getTokenSequence();
        std::cout << "Lexical analysis complete \n";

        std::ofstream token_seq_file("token_seq.txt");
        for (Token t: token_sequence){
            token_seq_file << static_cast<std::underlying_type<TokenType>::type>(t.getType()) << " : " <<t.getValue() << "\n";
        } 
        token_seq_file.close(); 
    }
    catch (const std::runtime_error& err){
        std::cout << err.what() << "\n"; 
        return 0;
    }

    Parser parser = Parser(token_sequence);
    parser.parseWinzig();
    TreeNode* ast = parser.returnFinalTree();
    std::cout << ast->pprintTree(0);

    
    return 0;
}
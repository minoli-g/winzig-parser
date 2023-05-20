#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "lex.hpp"
#include "token.hpp"
#include "parser.hpp"

int main(int argc, char *argv[]){

    //std::cout << "Program started \n";
    
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
        exit(1);
    }

    file.open(input_file_path);
    if (!file){
        std::cout << "Error: Could not read file. \n";
        exit(1);
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
        //std::cout << "Lexical analysis complete \n"; 
    }
    catch (const std::runtime_error& err){
        std::cout << err.what() << "\n"; 
        exit(1);
    }
    
    // Save lexer output tokens for debugging purposes
    // std::ofstream token_seq_file("token_seq.txt");
    // for (Token t: token_sequence){
    //     token_seq_file << static_cast<std::underlying_type<TokenType>::type>(t.getType()) << " : " <<t.getValue() << "\n";
    // } 
    // token_seq_file.close();

    // Parse the tokens into an AST
    Parser parser = Parser(token_sequence);

    try{
        parser.parseWinzig();
        TreeNode* ast = parser.returnFinalTree();
        std::cout << ast->pprintTree(0);

        // Save parser output to file
        // Removed as this happens automatically on running command
        // std::ofstream output_file("ast.txt");
        // output_file << ast->pprintTree(0);
        // output_file.close();
    }
    catch (const std::runtime_error& err){
        std::cout << err.what() << "\n"; 
        exit(1); 
    }

    exit(0);
}
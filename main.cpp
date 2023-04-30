#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "lex.hpp"
#include "token.hpp"

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
    std::vector<Token> token_sequence = lexer.parse();

    for (Token t: token_sequence){
        std::cout << t.getType() << " : " <<t.getValue() << "\n";
    }

    return 0;
}
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

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

    // Read the tokens in from the file
    std::vector<std::string> token_strings{ };
    while(file.good()){
        std::string next_token;              
        file >> next_token; 
        token_strings.push_back(next_token);
    }
    file.close();

    // Convert the tokens into enum types
    // Parse the enum list and return a tree
    // Print the tree / save to file

    return 0;
}
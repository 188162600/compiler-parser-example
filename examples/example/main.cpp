#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include<iostream>
#include<fstream>
#include<filesystem>

int main()
{
   std::filesystem::path current_path(__FILE__);
   std::filesystem::path folder_path=current_path.parent_path();
   std::filesystem::path input_path=folder_path/"code.h";
 
      std::ifstream file(input_path, std::ios::in);
       if (!file) {
        std::cerr << "Could not open the file: " << input_path << std::endl;
        return 1;
    }
        std::string code;
        std::string line;
        while(std::getline(file,line))
        {
            code+=line;
        }
        file.close();

    auto ast=clang::tooling::buildASTFromCode(code);
    ast->getASTContext().getTranslationUnitDecl()->dump();
    
    
   
}
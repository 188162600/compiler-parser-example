#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include<iostream>
#include<fstream>
#include<filesystem>
#include <clang/AST/RecursiveASTVisitor.h>
template<typename T>
class A
{
    public:
    template<typename T2>
    class B
    {

    };
};
using C=A<int>::B<int>;

class MyVisitor : public clang::RecursiveASTVisitor<MyVisitor> {
public:
    bool VisitNamedDecl(clang::NamedDecl *D) {
      D->dump();
        if (auto *Prev = D->getPreviousDecl()) {
            llvm::errs() << D << " was previously declared at ";
            Prev->getLocation().print(llvm::errs(), D->getASTContext().getSourceManager());
            llvm::errs() << "\n";
        }
        return true;
    }
};

void VisitAST(clang::ASTContext &Context) {
    MyVisitor Visitor;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}

// ... someplace else in your code
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
            code+=line+'\n';
        }
        file.close();

llvm::outs()<<code;
    auto ast=clang::tooling::buildASTFromCode(code);
   
ast.get()->getASTContext().getTranslationUnitDecl()->dump();

VisitAST(ast.get()->getASTContext());
   llvm::outs() << "Done\n";
}

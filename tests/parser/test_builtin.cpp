#include"src/tools/compilerinstance.h"

#include"tests/testingtools/testingtools.h"
using namespace hydro;

int main()
{
    llvm::outs()<<"start";
    llvm::outs().flush();
    CompilerInstance compiler_instance;
    compiler_instance.create_diagnostics();
    
    compiler_instance.create_target();
     auto file_manager=compiler_instance.create_file_manager();
    compiler_instance.create_source_manager(*file_manager);
    compiler_instance.create_preprocessor();
    compiler_instance.create_sema();
    compiler_instance.create_ast_context();
    compiler_instance.create_parser();
   
    auto& source_manager = compiler_instance.source_manager();
    
    auto&preprocessor=compiler_instance.preprocessor();
   
       preprocessor.initialize();
    
};

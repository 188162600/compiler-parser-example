#ifndef HYDROGEN_SRC_PARSER_PARSER
#define HYDROGEN_SRC_PARSER_PARSER

#include "src/lexer/lexer.h"
#include "src/semantics/sema.h"

HYDROGEN_BEGIN_NAMESPACE
class Parser
{
    class Impl;

public:
    Parser(Preprocessor &preprocessor, Sema &action) : preprocessor(&preprocessor), action(&action)
    {
        token = preprocessor.next_token();
    };
    clang::DiagnosticBuilder diag(clang::SourceLocation loc, DiagnosticKind);
    clang::DiagnosticBuilder diag(Token token, DiagnosticKind);
    bool parse_main_module();

   // bool parse_top_level_decl();
    
    
   
private:
    Preprocessor *preprocessor;
    Sema *action;
    Token token;
    llvm::DenseMap<clang::ModuleIdPath, clang::NamespaceDecl *> parsed_module;
    Precedence lhs_precedence;
    Precedence ambient_precedence;
    DiagnosticsEngine*diagnostics_engine;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_PARSER_PARSER */ 4

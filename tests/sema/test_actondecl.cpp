#include"src/semantics/sema.h"
#include "llvm/Support/Host.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/Builtins.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/StaticAnalyzer/Frontend/AnalysisConsumer.h"

int main()
{
    using namespace hydro;
    llvm::Triple target_triple=llvm::Triple(llvm::sys::getDefaultTargetTriple());
    std::shared_ptr<clang::TargetOptions> target_options(new clang::TargetOptions());
    target_options->Triple=target_triple.getTriple();
    clang::DiagnosticsEngine diagnostic_engine(llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(new clang::DiagnosticIDs()), new clang::DiagnosticOptions(), new clang::IgnoringDiagConsumer(), false);
   
    clang::TargetInfo *taarget_info=clang::TargetInfo::CreateTargetInfo(diagnostic_engine, target_options);
     LangOptions lang_options;
    LangOptions::set_lang_default(lang_options, target_triple);
    std::shared_ptr<clang::HeaderSearchOptions> header_search_options= std::shared_ptr<clang::HeaderSearchOptions> (new clang::HeaderSearchOptions());
    clang::FileSystemOptions file_system_options;
    clang::FileManager file_manager(file_system_options);
    clang::SourceManager source_manager(diagnostic_engine, file_manager);
    clang::HeaderSearch header_search(header_search_options, source_manager, diagnostic_engine, clang::LangOptions(), target_info);
    clang::ModuleLoader*module_loader=new clang::TrivialModuleLoader;
    Preprocessor preprocessor(diagnostic_engine,lang_options,header_search,source_manager,*module_loader);
    clang::LangOptions clang_lang_options;
    clang::Builtin::Context builtin_context;
    clang::SelectorTable selector_table;
    clang::ASTContext ast_context(clang_lang_options,source_manager,preprocessor.identifier_table(),selector_table,builtin_context,clang::TranslationUnitKind::TU_Complete);
    clang::ASTConsumer ast_consumer;
    Sema sema(preprocessor,ast_context,ast_consumer);


    sema.act_on_bool_literal(clang::SourceLocation(),true);
    sema.act_on_bool_literal(clang::SourceLocation(),false);
    sema.act_on_integer_literal(clang::SourceLocation(),rand());
    Token token;
    token.set_token_kind(TokenKind::NumericLiteral);

    sema.act_on_integer_literal()
  
    return 0;
}
#ifndef HYDROGEN_SRC_LEXER_PREPROCESSOR
#define HYDROGEN_SRC_LEXER_PREPROCESSOR
#include"src/basic/langoptions.h"
#include "src/basic/sourcelocation.h"
#include "src/basic/identifierpath.h"
#include"src/basic/diagnostickind.h"
#include"src/basic/diagnosticsengine.h"
#include"src/ast/binaryoperatorkind.h"
#include"src/ast/unaryoperatorkind.h"
#include"src/lexer/numericliteralprecison.h"
#include"src/lexer/token.h"

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Basic/FileEntry.h"
#include "clang/Basic/SourceManager.h"

HYDROGEN_BEGIN_NAMESPACE
class Lexer;
class Preprocessor
{
    public:
   
    Preprocessor(DiagnosticsEngine &diags, LangOptions &opts,clang::HeaderSearch&header_search, clang::SourceManager &source_manager,clang::ModuleLoader &module_loader, clang::IdentifierInfoLookup *IILookup=nullptr);;
    clang::DiagnosticBuilder diag(SourceLocation loc, DiagnosticKind error) const
    {
        return diags_.report(loc,error);
    
    };
    clang::SourceManager &source_manager()
    {
        return source_manager_;
    };
    llvm::Optional<clang::FileEntryRef>  	lookup_file (SourceLocation filename_loc, llvm::StringRef filename);
    clang::Module*lookup_precompiled_module(clang::ModuleIdPath);
  //  llvm::Optional<clang::FileEntryRef> 	lookup_file ( const IdentifierPath&path);
    LangOptions &lang_options()
    {
        return lang_options_;
    };
    
    clang::IdentifierTable &identifier_table()
    {
        return identifier_table_;
    };
    
    Lexer&current_lexer()
    {
        return *current_lexer_;
    };
    void initialize();
    NumericLiteralPrecision integer_precision();
    NumericLiteralPrecision floating_precision();
    Token&next_token()
    {
        current_lexer_->lex(token_);
        return token_;
    };
    
    // static bool is_magic_method_name(const clang::IdentifierInfo*method_name);
    // static bool is_private_name(const clang::IdentifierInfo*name);
    private:
    clang::TargetInfo*target_info_;
    DiagnosticsEngine &diags_;
    LangOptions &lang_options_;
    clang::HeaderSearch &header_search_;
    clang::SourceManager &source_manager_;
    clang::ModuleLoader &module_loader_;
    clang::IdentifierTable identifier_table_;
    std::vector<std::unique_ptr<Lexer>> file_lexer_stack_;
    Lexer*current_lexer_;
    Token token_;
};

HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_LEXER_PREPROCESSOR */

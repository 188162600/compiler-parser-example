#include"src/lexer/preprocessor.h"
#include"src/lexer/lexer.h"
#include"llvm/ADT/Optional.h"
#include "clang/Basic/FileEntry.h"
HYDROGEN_BEGIN_NAMESPACE
Preprocessor::Preprocessor(DiagnosticsEngine &diags, LangOptions &opts, clang::HeaderSearch &header_search, clang::SourceManager &source_manager, clang::ModuleLoader &module_loader, clang::IdentifierInfoLookup *IILookup)
    :diags_(diags),lang_options_(opts),header_search_(header_search),source_manager_(source_manager),module_loader_(module_loader),identifier_table_(IILookup)
{
 



}

void Preprocessor::initialize()
{
    auto main_file_id=source_manager_.getMainFileID();

    auto main_file_buffer=source_manager_.getBufferOrNone(main_file_id);
    assert(main_file_buffer.has_value());
    file_lexer_stack_.push_back(std::unique_ptr<Lexer>(new Lexer(main_file_id,main_file_buffer.value(),*this)));
    current_lexer_=file_lexer_stack_.back().get();
}

HYDROGEN_END_NAMESPACE

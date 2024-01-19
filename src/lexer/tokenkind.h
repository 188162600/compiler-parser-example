#ifndef HYDROGEN_LEXER_TOKENKIND
#define HYDROGEN_LEXER_TOKENKIND
#include "src/basic/global.h"
#include "llvm/ADT/StringRef.h"
HYDROGEN_BEGIN_NAMESPACE
class SemanticKind;

class TokenKind
{

public:
    enum EnumKind : uint32_t
    {
#define HYDROGEN_TOKEN(NAME) NAME,
#include "src/lexer/tokenkind.def"
    TokenKindCount
    };
    constexpr inline TokenKind(EnumKind enumKind=Unknown):enum_kind(enumKind){};
    constexpr inline llvm::StringRef spelling()const
    {
        return spellings_[enum_kind];
    };
    
    constexpr inline llvm::StringRef name()const
    {
        return names_[enum_kind];
    };
    
    constexpr bool is_keyword()const
    {
        return is_keywords_[enum_kind];
    }
    constexpr bool is_symbol()const
    {
        return is_symbols_[enum_kind];
    }
    constexpr inline operator EnumKind()const{return enum_kind;};
    constexpr bool is(TokenKind tokenKind)const
    {
        return enum_kind==tokenKind.enum_kind;
    }
    constexpr bool is_any_of(TokenKind tokenKind)const
    {
        return is(tokenKind);
    };
    template<typename... Args>
    constexpr bool is_any_of(TokenKind tokenKind,Args... args)const
    {
        return is(tokenKind) or is_any_of(args...);
    }
    

    EnumKind enum_kind;
    constexpr SemanticKind operator_kind();
    private:
    #define HYDROGEN_TOKEN(NAME) llvm::StringRef(),
    #define HYDROGEN_KEYWORD_TOKEN(NAME, SPELLING) llvm::StringRef(SPELLING),
    #define HYDROGEN_SYMBOL_TOKEN(NAME, SPELLING) llvm::StringRef(SPELLING),
    constexpr static inline llvm::StringRef spellings_[]={
#include "src/lexer/tokenkind.def"
    };
     #define HYDROGEN_TOKEN(NAME) llvm::StringRef(#NAME),
    constexpr static inline  llvm::StringRef names_[]={
#include "src/lexer/tokenkind.def"
    };
    constexpr static inline bool is_keywords_[]={
#define HYDROGEN_TOKEN(NAME) false,
#define HYDROGEN_SYMBOL_TOKEN(NAME, SPELLING) false,
#define HYDROGEN_KEYWORD_TOKEN(NAME, SPELLING) true,
#include "src/lexer/tokenkind.def"
    };
    constexpr static inline bool is_symbols_[]={
#define HYDROGEN_TOKEN(NAME) false,
#define HYDROGEN_SYMBOL_TOKEN(NAME, SPELLING) true,
#define HYDROGEN_KEYWORD_TOKEN(NAME, SPELLING) false,
#include "src/lexer/tokenkind.def"
    };


};

HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_LEXER_TOKENKIND */

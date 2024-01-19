#ifndef HYDROGEN_SRC_AST_BINARYOPERATORKIND
#define HYDROGEN_SRC_AST_BINARYOPERATORKIND

#include "src/ast/semantickind.h"
#include"src/lexer/tokenkind.h"
#include "clang/AST/Expr.h"
HYDROGEN_BEGIN_NAMESPACE
class BinaryOperatorKind
{
public:
    enum EnumKind
    {
        #define HYDROGEN_BINARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) NAME,
        #include "src/ast/binaryoperatorkind.def"
    };
    constexpr inline BinaryOperatorKind(EnumKind enumKind):enum_kind(enumKind){};
    constexpr inline TokenKind token_kind()const
    {
        return token_kinds_[enum_kind];
    };
    constexpr inline SemanticKind semantic_kind()const
    {
        return semantic_kinds_[enum_kind];
    };
    constexpr inline clang::BinaryOperatorKind opcode()const
    {
        return opcodes_[enum_kind];
    };
    constexpr inline operator EnumKind()const{return enum_kind;};
    EnumKind enum_kind;
    private:
    #define HYDROGEN_BINARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) TOKEN_KIND,
    constexpr static inline TokenKind token_kinds_[]={
        #include "src/ast/binaryoperatorkind.def"
    };
    #define HYDROGEN_BINARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) SEMANTIC_KIND,
    constexpr static inline SemanticKind semantic_kinds_[]={
        #include "src/ast/binaryoperatorkind.def"
    };
    #define HYDROGEN_BINARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) __VA_ARGS__,
    constexpr static inline clang::BinaryOperatorKind opcodes_[]={
        #include "src/ast/binaryoperatorkind.def"
    };
};

HYDROGEN_END_NAMESPACE



#endif /* HYDROGEN_SRC_AST_BINARYOPERATORKIND */


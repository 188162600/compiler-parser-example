#ifndef HYDROGEN_SRC_BASIC_UNARYOPERATORKIND
#define HYDROGEN_SRC_BASIC_UNARYOPERATORKIND

#include "src/ast/semantickind.h"
#include"src/lexer/tokenkind.h"
#include "clang/AST/Expr.h"
HYDROGEN_BEGIN_NAMESPACE
class UnaryOperatorKind
{
    public:
    enum EnumKind
    {
        #define HYDROGEN_UNARY_OPERATOR_KIND(NAME,SEMANTIC_KIND,TOKEN_KIND,CAN_OVERFLOW, ...) NAME,
        #include "src/ast/unaryoperatorkind.def"
    };
    constexpr inline UnaryOperatorKind(EnumKind enumKind):enum_kind(enumKind){};
    constexpr inline TokenKind token_kind()const
    {
        return token_kinds_[enum_kind];
    };
    constexpr inline SemanticKind semantic_kind()const
    {
        return semantic_kinds_[enum_kind];
    };
    constexpr inline clang::UnaryOperatorKind opcode()const
    {
        return opcodes_[enum_kind];
    };
    constexpr inline operator EnumKind()const{return enum_kind;};
    EnumKind enum_kind;
    private:
    #define HYDROGEN_UNARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) TOKEN_KIND,
    constexpr static inline TokenKind token_kinds_[]={
        #include "src/ast/unaryoperatorkind.def"
    };
    #define HYDROGEN_UNARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) SEMANTIC_KIND,
    constexpr static inline SemanticKind semantic_kinds_[]={
        #include "src/ast/unaryoperatorkind.def"
    };
    #define HYDROGEN_UNARY_OPERATOR_KIND(NAME, SEMANTIC_KIND, TOKEN_KIND, ...) __VA_ARGS__,
    constexpr static inline clang::UnaryOperatorKind opcodes_[]={
        #include "src/ast/unaryoperatorkind.def"
    };
    

};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_BASIC_UNARYOPERATORKIND */

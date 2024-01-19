#ifndef HYDROGEN_SRC_SEMANTICS_SEMANTICKIND
#define HYDROGEN_SRC_SEMANTICS_SEMANTICKIND
#include "src/lexer/tokenkind.h"
#include"src/basic/global.h"
#include "clang/AST/Expr.h"
#include"src/ast/precedence.h"

HYDROGEN_BEGIN_NAMESPACE
class SemanticKind
{
    public:

    enum EnumKind{
        Unknown,
#define HYDROGEN_DECL(NAME) NAME,
#define HYDROGEN_STMT(NAME) NAME,
#define HYDROGEN_EXPR(NAME,PRECEDENCE) NAME,
#include"src/ast/semantickind.def"
    };
    constexpr SemanticKind(EnumKind enum_kind=Unknown):enum_kind(enum_kind){};
    Precedence precedence()const;
    TokenKind binary_operator()const;
    TokenKind unary_operator()const;
    EnumKind enum_kind;
};

HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_SEMANTICKIND */

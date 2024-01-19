#ifndef HYDROGEN_SRC_SEMANTICS_DECLEXPRPAIR
#define HYDROGEN_SRC_SEMANTICS_DECLEXPRPAIR
#include"clang/AST/Decl.h"
#include"clang/AST/Expr.h"
#include "clang/Sema/Ownership.h"
#include "src/basic/sourcelocation.h"
#include"src/basic/global.h"
HYDROGEN_BEGIN_NAMESPACE
using DeclExprPair = std::pair<clang::Decl *, clang::Expr *>;
using DeclExprPairResult = clang::ActionResult<DeclExprPair>;
class ParsedExpr
{
    public:

    constexpr inline ParsedExpr(clang::Decl *decl, clang::Expr *expr)
        : decl(decl), expr(expr)
    {}
    constexpr inline ParsedExpr(SourceLocation loc,clang::Decl*decl)
        : decl(decl), expr(nullptr)
    {}
    constexpr inline ParsedExpr(clang::Expr *expr)
        : decl(nullptr), expr(expr)
    {}
    constexpr inline ParsedExpr()
        : decl(nullptr), expr(nullptr)
    {}
    constexpr inline ParsedExpr(const ParsedExpr&other) = default;
    constexpr bool is_empty() const
    {
        return !decl && !expr;
    }
    constexpr inline operator bool() const
    {
        return decl || expr;
    }
    constexpr inline bool operator!() const
    {
        return !decl && !expr;
    }
    constexpr ParsedExpr&operator=(const ParsedExpr&other) = default;

    clang::Decl *decl;
    clang::Expr *expr;
    SourceLocation loc;
};

using ParsedExprResult= clang::ActionResult<ParsedExpr>;
ParsedExprResult ParsedExprError()
{
    return ParsedExprResult(/*invalid*/ true);
}
ParsedExprResult ParsedExprError(const clang::StreamingDiagnostic &)
{
    return ParsedExprError();
};
ParsedExprResult ParsedExprEmpty()
{
    return ParsedExprResult(ParsedExpr(nullptr, nullptr));
}
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_DECLEXPRPAIR */

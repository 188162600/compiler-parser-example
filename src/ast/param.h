#ifndef HYDROGEN_SRC_SEMANTICS_PARAM
#define HYDROGEN_SRC_SEMANTICS_PARAM
#include "clang/AST/Expr.h"
#include"src/basic/identifierloc.h"

HYDROGEN_BEGIN_NAMESPACE
struct Param
{
    Param(IdentifierLoc name,
    clang::Expr *type,
    clang::Expr *defaultArg);

    IdentifierLoc name;
    clang::Expr *type;
    clang::Expr *defaultArg;
    clang::Decl*decl;
};

HYDROGEN_END_NAMESPACE



#endif /* HYDROGEN_SRC_SEMANTICS_PARAM */

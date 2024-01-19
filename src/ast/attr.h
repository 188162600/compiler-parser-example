#ifndef HYDROGEN_SRC_AST_ATTR
#define HYDROGEN_SRC_AST_ATTR
#include"src/basic/global.h"
#include "clang/AST/Attr.h"


HYDROGEN_BEGIN_NAMESPACE
enum class AttrKind
{
    Inline,
    NoInline,
    AlwaysInline,
    Constexpr,
    Static,
    Extern,

    CompileTimeOnly,
    YetNoSideEffect,
    NoSideEffect,
    Objective,


};

template<AttrKind Kind>
struct Attr
{
    static constexpr AttrKind kind = Kind;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_AST_ATTR */

#ifndef HYDROGEN_SRC_SEMANTICS_PARSEDATTR
#define HYDROGEN_SRC_SEMANTICS_PARSEDATTR

#include "clang/AST/Attr.h"
#include"src/AST/parsedexpr.h"
HYDROGEN_BEGIN_NAMESPACE
class ParsedAttr:public clang::Attr
{
};
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_SEMANTICS_PARSEDATTR */

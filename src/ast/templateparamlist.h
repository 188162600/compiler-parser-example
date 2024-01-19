#ifndef HYDROGEN_SRC_SEMANTICS_TEMPLATEPARAMLIST
#define HYDROGEN_SRC_SEMANTICS_TEMPLATEPARAMLIST
#include "clang/AST/Expr.h"
#include"src/ast/param.h"
HYDROGEN_BEGIN_NAMESPACE
struct TemplateParamList
{

    SourceLocation left_angle_loc;
    SourceLocation right_angle_loc;
    std::vector<Param> params;
};
HYDROGEN_END_NAMESPACE



#endif /* HYDROGEN_SRC_SEMANTICS_TEMPLATEPARAMLIST */

#ifndef HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLIST
#define HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLIST
#include "clang/AST/Expr.h"
#include "clang/Sema/ParsedAttr.h"
#include "src/basic/sourcelocation.h"
#include "src/ast/parsedexpr.h"
HYDROGEN_BEGIN_NAMESPACE
struct TemplateArgumentList
{

  SourceLocation left_angle_loc;
  SourceLocation right_angle_loc;
  std::vector<clang::TemplateArgumentLoc> args;
  std::vector<std::pair<clang::IdentifierLoc, clang::TemplateArgumentLoc>> kwargs;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLIST */

#ifndef HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLISTINFO
#define HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLISTINFO

#include "clang/AST/Expr.h"
#include"src/basic/sourcelocation.h"
#include"src/ast/parsedexpr.h"
HYDROGEN_BEGIN_NAMESPACE
class TemplateArgumentListInfo
{
    public:
    TemplateArgumentListInfo():is_set_(false){};
    TemplateArgumentListInfo(SourceLocation left_angle_loc,SourceLocation right_angle_loc,std::vector<ParsedExpr> args):left_angle_loc_(left_angle_loc),right_angle_loc_(right_angle_loc),args_(args),is_set_(true){};
    TemplateArgumentListInfo(const TemplateArgumentListInfo&other)=default;
    TemplateArgumentListInfo(TemplateArgumentListInfo&&other)=default;
    TemplateArgumentListInfo&operator=(const TemplateArgumentListInfo&other)=default;
    TemplateArgumentListInfo&operator=(TemplateArgumentListInfo&&other)=default;
    SourceLocation left_angle_loc()const{return left_angle_loc_;};
    SourceLocation right_angle_loc()const{return right_angle_loc_;};
    llvm::ArrayRef<ParsedExpr> args()const{return args_;};
    llvm::ArrayRef<std::pair<clang::IdentifierLoc,ParsedExpr>>kwargs()const{return kwargs_;};
    void set_left_angle_loc(SourceLocation left_angle_loc){left_angle_loc_=left_angle_loc;};
    void set_right_angle_loc(SourceLocation right_angle_loc){right_angle_loc_=right_angle_loc;};
    //void set_args(std::vector<ParsedExpr>&&args){args_=args;is_set_=true;};
    void add_arg(ParsedExpr arg){args_.push_back(arg);is_set_=true;};
    void add_kwarg(clang::IdentifierLoc identifier,ParsedExpr arg){kwargs_.push_back(std::make_pair(identifier,arg));is_set_=true;};
    bool is_set()const{return is_set_;};
    operator bool()const{return is_set_;}
    private:
  SourceLocation left_angle_loc_;
    SourceLocation right_angle_loc_;
    std::vector<ParsedExpr> args_;
    std::vector<std::pair<clang::IdentifierLoc,ParsedExpr>>kwargs_;
    bool is_set_;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_TEMPLATEARGUMENTLISTINFO */

#ifndef HYDROGEN_SRC_SEMANTICS_SEMA_20COPY
#define HYDROGEN_SRC_SEMANTICS_SEMA_20COPY
#ifndef HYDROGEN_SRC_SEMANTICS_SEMA
#define HYDROGEN_SRC_SEMANTICS_SEMA
#include "src/lexer/preprocessor.h"
#include "src/ast/attr.h"
#include "src/ast/owership.h"
#include "src/ast/parsedexpr.h"
#include "src/ast/templateparamlist.h"
#include "src/ast/templateargumentlist.h"
#include "src/ast/templateargumentlistinfo.h"
#include "src/semantics/parsedattr.h"
#include "src/semantics/scope.h"
#include "src/semantics/lookupresult.h"
#include "src/semantics/builtin.h"
#include "clang/Sema/Scope.h"

HYDROGEN_BEGIN_NAMESPACE

using ModuleDecl = clang::NamespaceDecl;
using PrimitiveDecl = clang::TypedefDecl;
class Sema
{
    class Impl;

public:
    class FunctionScopeInfo
    {
    public:
        llvm::SmallVector<clang::QualType, 8> params_type;
        llvm::SmallVector<clang::ReturnStmt *, 4> returns;
    };
    class ParsedModule
    {
    public:
        bool is_valid();
        bool is_invalid();

        SourceLocation loc;
        // clang::CXXScopeSpec scope_spec;
        ModuleDecl *module;
        clang::CXXScopeSpec scope_spec;
        clang::Scope *scope;
    };
    using ConditionResult = clang::ActionResult<std::pair<clang::VarDecl *, clang::Expr *>>;

    Sema(Preprocessor &pp, clang::ASTContext &ctxt, clang::ASTConsumer &consumer, clang::TranslationUnitKind TUKind = clang::TU_Complete, clang::CodeCompleteConsumer *CompletionConsumer = nullptr);
    clang::DiagnosticBuilder diag(SourceLocation Loc, DiagnosticKind DiagID);

    // DiagnosticBuilder Parser::Diag(const Token &Tok, DiagnosticKind DiagID) ;
    void initialize();
    clang::ASTContext &ast_context();

    void push_scope(Scope *scope, clang::DeclContext *context);
    void pop_scope();

    inline clang::Scope *current_scope() { return active_modules_scope_stack_.top(); };
    void set_current_scope(clang::Scope*scope);
    clang::IdentifierInfo *unary_operator_method_name(UnaryOperatorKind op);
    clang::IdentifierInfo *binary_operator_method_name(BinaryOperatorKind op);

    inline const Scope *current_scope() const { return active_modules_scope_stack_.top(); };
    inline const BuiltinContext *builtin_context() const { return &builtin_context_; };
    inline BuiltinContext *builtin_context() { return &builtin_context_; };
    inline llvm::ArrayRef<FunctionScopeInfo *> function_scopes() const;
    inline FunctionScopeInfo *current_function_scope_info() const;
    ConditionResult act_on_condition(Scope *S, ParsedExpr expr);
    ConditionResult act_on_condition(Scope *S, ParsedExprResult expr);

    clang::ActionResult<clang::TypeSourceInfo *> act_on_type_source_info(ParsedExpr type_expr, bool complain = true);
    clang::ActionResult<clang::QualType> act_on_type(ParsedExpr type_expr, bool complain = true);
    clang::ActionResult<clang::DeclContext *> act_on_module(ParsedExpr module_expr);
    clang::ActionResult<clang::Attr *> act_on_attribute(ParsedExpr expr);

    clang::ExprResult act_on_value_expr(clang::SourceLocation loc, clang::VarDecl *decl);
    clang::ExprResult act_on_type_expr(SourceLocation loc, clang::QualType);
    clang::ExprResult act_on_function_expr(SourceLocation loc, clang::FunctionDecl *decl);
    clang::ExprResult act_on_module_expr(SourceLocation loc, clang::DeclContext *decl);
    clang::ExprResult act_on_attribute_expr(SourceLocation loc, clang::Attr *attr);
    clang::ActionResult<clang::QualType> to_qualtype(ParsedExpr);
    clang::Expr *function_type_expr();
    clang::Expr *type_type_expr();
    clang::Expr *module_type_expr();
    clang::Expr *attribute_type_expr();
    clang::QualType any_type();
    clang::QualType type_type();
    clang::QualType function_type(llvm::ArrayRef<clang::QualType>, clang::QualType);
    clang::QualType module_type();
    clang::QualType attribute_type();
    clang::ExprResult act_on_implicit_cast_expr(clang::Expr *expr, clang::QualType type);

    bool complete_expr(ParsedExpr &expr);
    bool complete_decl(ParsedExpr &expr);
    bool try_complete_expr(ParsedExpr &expr);
    bool try_complete_decl(ParsedExpr &expr);
    bool try_complete_parsed_expr(ParsedExpr &expr);
    bool is_reserved_name(clang::IdentifierInfo *name);
    bool check_reserved_name(clang::IdentifierLoc name);
    bool is_local(Scope *scope, clang::Decl *decl);
    bool is_function(ParsedExpr expr);
    bool is_function_type(ParsedExpr expr);
    bool is_function_type(clang::QualType type);
    bool is_type(ParsedExpr expr);
    bool is_type_type(ParsedExpr expr);
    bool is_type_type(clang::QualType type);
    bool is_module(ParsedExpr expr);
    bool is_module_type(ParsedExpr expr);
    bool is_module_type(clang::QualType type);
    bool is_attribute(ParsedExpr expr);
    bool is_attribute_type(ParsedExpr expr);
    bool is_attribute_type(clang::QualType type);
    bool is_integral(ParsedExpr expr);
    bool is_integral_type(ParsedExpr expr);
    bool is_integral_type(clang::QualType type);
    bool is_hashable(ParsedExpr expr);
    bool is_hashable_type(ParsedExpr expr);
    bool is_hashable_type(clang::QualType type);
    bool is_constexpr(ParsedExpr expr);
    bool is_template(ParsedExpr expr);

    bool has_binary_operator(ParsedExpr lhs, ParsedExpr rhs, BinaryOperatorKind op);
    // bool has_lhs_defined_binary_operator(ParsedExpr lhs, ParsedExpr rhs, BinaryOperatorKind op);
    // bool has_rhs_defined_binary_operator(ParsedExpr lhs, ParsedExpr rhs, BinaryOperatorKind op);
    bool has_unary_operator(ParsedExpr expr, UnaryOperatorKind op);
    clang::StorageClass variable_storage_class();

    clang::StmtResult act_on_decl_stmt(clang::DeclGroupRef Decl, SourceLocation StartLoc, SourceLocation EndLoc);
    clang::StmtResult act_on_expr_stmt(ParsedExpr expr);
    clang::StmtResult act_on_block_stmt(SourceLocation indent, SourceLocation dedent, clang::Stmt *stmts);

    clang::StmtResult act_on_pass_stmt(SourceLocation);
    clang::StmtResult act_on_if_stmt(SourceLocation loc, ConditionResult cond, SourceLocation colon_loc, clang::Stmt *then, SourceLocation else_loc = SourceLocation(), clang::Stmt *else_stmt = nullptr);

    clang::StmtResult act_on_while_stmt(SourceLocation loc, ConditionResult cond, SourceLocation colon_loc, clang::Stmt *then);
    clang::StmtResult act_on_for_each_identifier(Scope *S, IdentifierLoc name);
    clang::StmtResult act_on_for_each_identifier(Scope *S, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type);
    clang::StmtResult act_on_for_each_stmt(Scope *scope, SourceLocation loc, clang::Stmt *var, SourceLocation in_loc, ParsedExpr iter, SourceLocation colon_loc, clang::Stmt *then);
    clang::StmtResult act_on_with_stmt(Scope *scope, SourceLocation loc, ParsedExpr expr, SourceLocation colon_loc, clang::Stmt *then);
    clang::StmtResult act_on_match_stmt(SourceLocation loc, ParsedExpr cond, SourceLocation colon_loc, llvm::ArrayRef<clang::Stmt *> cases);
    clang::StmtResult act_on_try_stmt(Scope *scope, SourceLocation loc, clang::Stmt *then, llvm::ArrayRef<clang::Stmt *> cases);

    clang::DeclResult act_on_except_identifier(Scope *S, ParsedExpr type, SourceLocation as, IdentifierLoc name);
    clang::StmtResult act_on_except_stmt(Scope *scope, SourceLocation loc, clang::Stmt *then);
    clang::StmtResult act_on_except_stmt(Scope *scope, SourceLocation loc, ParsedExpr exception_type, clang::Stmt *then);
    clang::StmtResult act_on_except_as_stmt(Scope *scope, SourceLocation loc, clang::Decl *except_var, clang::Stmt *then);

    clang::StmtResult act_on_case_stmt(Scope *scope, SourceLocation loc, ParsedExpr cond, SourceLocation colon_loc, clang::Stmt *then);
    clang::StmtResult act_on_case_default_stmt(Scope *scope, SourceLocation loc, SourceLocation colon_loc, clang::Stmt *then);
    clang::StmtResult act_on_break_stmt(Scope *scope, SourceLocation loc);
    clang::StmtResult act_on_continue_stmt(Scope *scope, SourceLocation loc);
    clang::StmtResult act_on_raise_stmt(Scope *scope, SourceLocation loc);
    clang::StmtResult act_on_raise_stmt(Scope *scope, SourceLocation loc, ParsedExpr value);
    clang::StmtResult act_on_yield_stmt(Scope *scope, SourceLocation loc, ParsedExpr value = ParsedExpr());
    clang::StmtResult act_on_yield_from_stmt(Scope *scope, SourceLocation loc, ParsedExpr value);

    clang::StmtResult act_on_return_stmt(Scope *scope, SourceLocation loc, ParsedExpr value = ParsedExpr());
    clang::StmtResult act_on_assert_stmt(Scope *scope, SourceLocation loc, ConditionResult cond, ParsedExpr message = ParsedExpr());

    // clang::DeclResult act_on_import_decl(Scope*scope,SourceLocation loc, llvm::ArrayRef< IdentifierPath>paths);
    // //a.b.c:IdentifierPath 直接转 a/b/c 然后用headersearch 用hashmap找 fixme llvm::ArrayRef<IdentifierLoc>names should be llvm::ArrayRef<IdentifierPath>names
    // clang::DeclGroup* act_on_import_from_decl(Scope*scope, SourceLocation loc,const IdentifierPath &path,llvm::ArrayRef<IdentifierLoc>names);
    // clang::DeclGroup* act_on_import_all_decl(Scope*scope, SourceLocation loc,const IdentifierPath&path);
    // clang::DeclGroup* act_on_import_as_decl(Scope*scope, SourceLocation loc,const IdentifierPath&path,llvm::ArrayRef<IdentifierLoc>names,llvm::ArrayRef<IdentifierLoc>as);

    ParsedModule lookup_module(SourceLocation import_loc, IdentifierPath path);

    bool lookup_name(Scope *scope, LookupResult &result);
    clang::DeclGroupRef act_on_non_local_decl(Scope *scope, SourceLocation loc, llvm::ArrayRef<IdentifierLoc> names);
    clang::DeclGroupRef act_on_global_decl(Scope *scope, SourceLocation loc, llvm::ArrayRef<IdentifierLoc> names);
    clang::ActionResult<clang::DeclContext *> act_on_module(Scope *scope, const ParsedModule &);
    clang::ActionResult<clang::DeclContext *> act_on_start_module(clang::DeclContext *module_context);
    clang::ActionResult<clang::DeclContext *> act_on_finish_module(clang::DeclContext *module_context);
    clang::DeclGroupRef act_on_non_local_decl(Scope *scope, SourceLocation loc, llvm::ArrayRef<IdentifierLoc> names);
    clang::DeclGroupRef act_on_global_decl(Scope *scope, SourceLocation loc, llvm::ArrayRef<IdentifierLoc> names);

    clang::DeclGroupRef act_on_import_decl(clang::Scope *scope, SourceLocation loc, llvm::MutableArrayRef<ParsedModule> modules);
    clang::DeclGroupRef act_on_import_as_decl(Scope *scope, SourceLocation loc, llvm::MutableArrayRef<ParsedModule> modules, SourceLocation as_loc, llvm::ArrayRef<IdentifierLoc> as);
    clang::DeclGroupRef act_on_import_from_decl(Scope *scope, SourceLocation loc, ParsedModule model, llvm::ArrayRef<IdentifierLoc> names);
    clang::DeclGroupRef act_on_import_from_as_decl(Scope *scope, SourceLocation loc, ParsedModule model, llvm::ArrayRef<IdentifierLoc> names, SourceLocation as_loc, llvm::ArrayRef<IdentifierLoc> as);
    clang::DeclGroupRef act_on_import_all_decl(Scope *scope, SourceLocation loc, llvm::MutableArrayRef<ParsedModule> modules);
    clang::DeclResult act_on_decl(Scope *scope, clang::Decl *decl);
    clang::DeclResult act_on_template_param(Scope *scope, IdentifierLoc name, SourceLocation colon_loc = SourceLocation(), ParsedExpr type = ParsedExpr(), SourceLocation equal_loc = SourceLocation(), ParsedExpr default_value = ParsedExpr());
    clang::TemplateParameterList *act_on_template_param_list(Scope *scope, SourceLocation LAngleLoc, llvm::MutableArrayRef<clang::NamedDecl *> Params, SourceLocation RAngleLoc);

    clang::DeclResult act_on_class_declarator(Scope *scope, SourceLocation loc, IdentifierLoc name, clang::TemplateParameterList *template_param_list, llvm::ArrayRef<ParsedExpr> decorators = {});
    // clang::DeclResult act_on_start_class_decl(Scope *scope, SourceLocation loc, IdentifierLoc name, clang::TemplateParameterList*template_param_list);
    clang::DeclResult act_on_start_class_decl(Scope *scope);
    clang::DeclResult act_on_class_bases(Scope *scope, clang::Decl *tag_decl, llvm::ArrayRef<ParsedExpr> args, llvm::ArrayRef<std::pair<clang::IdentifierLoc, ParsedExpr>> kwargs = {});
    clang::DeclResult act_on_finish_class_decl(Scope *scope);
    // enter template scope; act on template param;exist template scope; enter protetype scope; ;ActOnParamDeclarator ;act on function (declartor)/
    clang::DeclResult act_on_function_param(Scope *scope, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type, SourceLocation equal_loc = SourceLocation(), ParsedExpr default_value = ParsedExpr());

    clang::DeclResult act_on_function_declarator(Scope *scope, SourceLocation loc, IdentifierLoc name, clang::TemplateParameterList *template_params, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Decl *> params, ParsedExpr return_type = ParsedExpr(), llvm::ArrayRef<ParsedExpr> decorators = {});
    // clang::DeclResult act_on_function_signature(Scope*scope,llvm::ArrayRef<clang::Decl *>params,ParsedExpr return_type=ParsedExpr());
    clang::DeclResult act_on_start_function_decl(Scope *scope, clang::Decl *function_decl);
    clang::DeclResult act_on_finish_function_decl(Scope *scope, clang::Decl *function_decl, SourceLocation colon_loc, clang::Stmt *body);

    clang::DeclResult act_on_method_param(Scope *scope, clang::Decl *class_decl, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type, SourceLocation equal_loc = SourceLocation(), ParsedExpr default_value = ParsedExpr());
    clang::DeclResult act_on_method(Scope *scope, clang::Decl *class_decl, SourceLocation loc, IdentifierLoc name, clang::TemplateParameterList *template_params, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Decl *> params, ParsedExpr return_type = ParsedExpr(), llvm::ArrayRef<ParsedExpr> decorators = {});
    clang::DeclResult act_on_start_method_decl(Scope *scope, clang::Decl *decl);

    clang::DeclResult act_on_finish_method_function_decl(Scope *scope, clang::Decl *class_decl, clang::Decl *function_decl, SourceLocation colon_loc, clang::Stmt *body);

    clang::DeclResult act_on_variable_decl(Scope *scope, IdentifierLoc name, SourceLocation eqaul_loc, ParsedExpr init);
    clang::DeclResult act_on_variable_decl(Scope *scope, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type, SourceLocation eqaul_loc, ParsedExpr init);
    clang::DeclResult act_on_property_decl(Scope *scope, IdentifierLoc name, SourceLocation eqaul_loc, ParsedExpr init);
    clang::DeclResult act_on_property_decl(Scope *scope, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type, SourceLocation eqaul_loc, ParsedExpr init);
    //  clang::DeclResult act_on_variable_declarator(Scope *scope, IdentifierLoc name);
    // clang::DeclResult act_on_variable_declarator(Scope *scope, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type);
    // clang::DeclResult act_on_finish_variable_decl(Scope *scope, clang::Decl *decl, SourceLocation equal_loc, ParsedExpr init);

    // clang::DeclResult act_on_property_decl(Scope *scope, IdentifierLoc name, SourceLocation eqaul_loc, ParsedExpr init);
    // clang::DeclResult act_on_property_decl(Scope *scope, IdentifierLoc name, SourceLocation colon_loc, ParsedExpr type, SourceLocation eqaul_loc, ParsedExpr init);
    // clang::DeclResult act_on_finish_property_decl(Scope *scope, clang::Decl *decl);

    ParsedExprResult act_on_binary_operator_expr(SourceLocation loc, ParsedExpr lhs, ParsedExpr rhs, BinaryOperatorKind kind);

    ParsedExprResult act_on_unary_operator_expr(SourceLocation loc, ParsedExpr expr, SemanticKind, UnaryOperatorKind kind);
    // ParsedExprResult resolve_function_overload(llvm::ArrayRef<clang::Decl *> candidates, llvm::ArrayRef<clang::Expr *> args);
    // ParsedExprResult resolve_template_function_overload(llvm::ArrayRef<clang::Decl *> candidates, const TemplateArgumentList &template_args, llvm::ArrayRef<clang::Expr *> args);

    ParsedExprResult act_on_resolved_call_expr(ParsedExpr callee, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<ParsedExpr> args, llvm::ArrayRef<std::pair<clang::IdentifierLoc, ParsedExpr>> kwargs = {});
    ParsedExprResult act_on_call_expr(ParsedExpr callee, const TemplateArgumentListInfo &template_args, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<ParsedExpr> args, llvm::ArrayRef<std::pair<clang::IdentifierLoc, ParsedExpr>> kwargs = {});

    // remember to handle the case where object.name is callable but not method
    ParsedExprResult act_on_attr_call_expr(ParsedExpr object, SourceLocation dot_loc, IdentifierLoc name, const TemplateArgumentListInfo &template_args, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<ParsedExpr> args, llvm::ArrayRef<std::pair<clang::IdentifierLoc, ParsedExpr>> kwargs = {});

    // ParsedExprResult act_on_template_call_expr(clang::Expr *callee, const TemplateArgumentList &template_args, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Expr *> args);
    //  ParsedExprResult act_on_member_call_expr(clang::Expr *callee, IdentifierLoc name,SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Expr *> args);

    // ParsedExprResult act_on_template_member_call_expr(clang::Expr *callee, IdentifierLoc name,const TemplateArgumentList &template_args, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Expr *> args);
    clang::Decl *look_up_single_name(Scope *scope, IdentifierLoc name);
    // clang::Decl *look_up_single_name(ModuleDecl *module, IdentifierLoc name);
    //  ParsedExprResult act_on_name_expr(Scope *scope, IdentifierLoc name,bool complain=true);
    //  ParsedExprResult act_on_path_expr(Scope *scope, IdentifierPath path);
    ParsedExprResult act_on_template_expr(ParsedExpr object, const TemplateArgumentListInfo &template_args);
    // ParsedExprResult act_on_template_call_expr(ParsedExpr object, const TemplateArgumentList &template_args, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<ParsedExpr> args);

    // ParsedExprResult act_on_get_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());
    // ParsedExprResult act_on_get_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name, clang::NestedNameSpecifierLoc specifier_loc, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());

    // ParsedExprResult act_on_set_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name,  const TemplateArgumentListInfo &template_args ,ParsedExpr rhs);
    // ParsedExprResult act_on_get_item_expr(ParsedExpr lhs, SourceLocation left_square, llvm::ArrayRef<ParsedExpr>, SourceLocation right_sqaure);
    // ParsedExprResult act_on_set_item_expr(ParsedExpr lhs, SourceLocation left_square, llvm::ArrayRef<ParsedExpr>, SourceLocation right_sqaure, SourceLocation equal_loc, ParsedExpr value);
    ParsedExprResult act_on_get_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name);
    ParsedExprResult act_on_get_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name, clang::NestedNameSpecifierLoc specifier_loc);

    ParsedExprResult act_on_set_attr_expr(ParsedExpr lhs, SourceLocation loc, IdentifierLoc name, ParsedExpr rhs);
    ParsedExprResult act_on_get_item_expr(ParsedExpr lhs, SourceLocation left_square, llvm::ArrayRef<ParsedExpr>, SourceLocation right_sqaure);
    ParsedExprResult act_on_set_item_expr(ParsedExpr lhs, SourceLocation left_square, llvm::ArrayRef<ParsedExpr>, SourceLocation right_sqaure, SourceLocation equal_loc, ParsedExpr value);

    ParsedExprResult act_on_if_expr(ConditionResult ConditionResult, SourceLocation if_loc, ParsedExpr true_expr, SourceLocation else_loc, ParsedExpr false_expr);
    ParsedExprResult act_on_lambda_expr(SourceLocation loc, SourceLocation left_paren, SourceLocation right_paren, llvm::ArrayRef<clang::Decl *> params, SourceLocation arrow_loc, ParsedExpr return_type, ParsedExpr body);

    // ParsedExprResult act_on_tuple_expr(llvm::ArrayRef<ParsedExpr> elements);
    ParsedExprResult act_on_tuple_expr(SourceLocation left_paren_loc, SourceLocation right_paren_loc, llvm::ArrayRef<ParsedExpr> elements, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());
    ParsedExprResult act_on_list_expr(SourceLocation left_square_loc, SourceLocation right_square_loc, llvm::ArrayRef<ParsedExpr> elements, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());

    ParsedExprResult act_on_dict_expr(SourceLocation left_brace_loc, SourceLocation right_brace_loc, llvm::ArrayRef<ParsedExpr> elements, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());
    ParsedExprResult act_on_set_expr(SourceLocation left_brace_loc, SourceLocation right_brace_loc, llvm::ArrayRef<ParsedExpr> elements, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());
    ParsedExprResult act_on_range_expr(SourceLocation left_parent, SourceLocation right_paren, ParsedExpr start, ParsedExpr stop, ParsedExpr step, const TemplateArgumentListInfo &template_args = TemplateArgumentListInfo());
    // ParsedExprResult act_on_slice_expr(ParsedExpr start, ParsedExpr stop, ParsedExpr step,const TemplateArgumentListInfo& template_args = TemplateArgumentListInfo());

    // ParsedExprResult act_on_real_literal(const Token &token);
    // ParsedExprResult act_on_integer_literal(const Token &token);
    ParsedExprResult act_on_integer_literal(SourceLocation location, std::int64_t value);
    // clang::QualType numeric_type_for_precision(NumericLiteralPrecision precision);
    // const llvm::fltSemantics &float_semantics_for_precision(NumericLiteralPrecision precision);
    ParsedExprResult act_on_numeric_literal(const Token &token);
    // {
    //     auto spelling = llvm::StringRef(token.literal_data().begin(), token.literal_data().size());
    //     NumericLiteralParser parser(spelling, diagnostics_engine_);
    //     if (parser.had_error())
    //         return clang::ExprError();
    //     if (parser.is_fractional())
    //     {
    //         clang::QualType type = numeric_type_for_precision(parser.precision());
    //         llvm::APFloat value(float_semantics_for_precision(parser.precision()));
    //         if (parser.get_float_value(value) == llvm::APFloat::opStatus::opOK)
    //             return clang::FloatingLiteral::Create(ast_context_, value, true, type, token.location());
    //         return clang::ExprError();
    //     }
    //     clang::QualType type = numeric_type_for_precision(parser.precision());
    //     llvm::APInt value;
    //     if (parser.get_integer_value(value) == false)
    //         return clang::ExprError();
    //     return clang::IntegerLiteral::Create(ast_context_, value, type, token.location());
    // };
    ParsedExprResult act_on_string_literal(llvm::ArrayRef<Token> tokens);
    // {
    //     if(tokens.empty())
    //         return clang::ExprError();
    //     TextualLiteralParser parser(tokens,diagnostics_engine_);
    //     if (parser.had_error())
    //         return clang::ExprError();
    //     auto text=parser.text();
    //     auto string_literal_type=ast_context_.getPointerType(ast_context_.Char8Ty);
    //     auto loc=tokens.begin()->location();
    //     auto string_literal = clang::StringLiteral::Create(ast_context_,text,clang::StringLiteral::StringKind::UTF8, false,string_literal_type, loc);
    //     auto length_literal = clang::IntegerLiteral::Create(ast_context_, llvm::APInt(sizeof(text.size())*8,text.size()), builtin_context_.int_type.qualtype, loc);
    //     return act_on_constructor_call_expr(loc,builtin_context_.str_type.type_decl,{string_literal,length_literal});
    // };

    ParsedExprResult act_on_bytes_literal(const Token &token);
    ParsedExprResult act_on_string_literal(SourceLocation location, std::string_view);
    ParsedExprResult act_on_format_argument(ParsedExpr string, ParsedExpr argument);

    ParsedExprResult act_on_joined_string(ParsedExpr lhs, ParsedExpr rhs);
    ParsedExprResult act_on_bool_literal(SourceLocation location, bool);

    ParsedExprResult act_on_none_expr(SourceLocation location);
    ParsedExprResult act_on_assertion_error_expr(SourceLocation location, ParsedExpr message);
    // clang::Expr *act_on_name_expr(IdentifierLoc location);

    ParsedExprResult act_on_has_next_expr(SourceLocation loc, ParsedExpr iter);
    ParsedExprResult act_on_next_expr(SourceLocation loc, ParsedExpr iter);
    ParsedExprResult act_on_len_expr(SourceLocation loc, ParsedExpr iter);

    inline ConditionResult ConditionError()
    {
        return ConditionResult(/*invalid*/ true);
    }
    inline ConditionResult ConditionEmpty()
    {
        return ConditionResult(std::make_pair(nullptr, nullptr));
    }

private:
    static constexpr size_t linear_search_threshold = 4;
    clang::DeclResult DeclError();
    using ModuleDecl = clang::NamespaceDecl;
    Preprocessor &preprocessor_;
    clang::ASTContext &ast_context_;
    std::stack<Scope *> active_modules_scope_stack_;
    Scope *scope_;
    BuiltinContext builtin_context_;
    clang::DeclContext *loaded_module_context_;
    llvm::DenseMap<std::vector<clang::IdentifierInfo *>, clang::DeclContext *> loaded_modules;
    DiagnosticsEngine &diagnostics_engine_;
    bool in_builtin_scope_;

private:
    clang::Sema *sema_;
    bool CheckUsingDeclRedeclaration(SourceLocation UsingLoc,
                                     const clang::CXXScopeSpec &SS,
                                     clang::SourceLocation NameLoc,
                                     const clang::LookupResult &Prev);

    bool CheckUsingDeclQualifier(clang::SourceLocation UsingLoc,
                                 const clang::CXXScopeSpec &SS,
                                 const clang::DeclarationNameInfo &NameInfo,
                                 SourceLocation NameLoc,
                                 const LookupResult *R = nullptr,
                                 const clang::UsingDecl *UD = nullptr);

    clang::Decl *act_on_import_as_decl(clang::Scope *scope, clang::AccessSpecifier AS, SourceLocation loc, IdentifierLoc alias_name, clang::CXXScopeSpec &SS, IdentifierLoc identifier);
    clang::Decl *build_import_as_decl(clang::Scope *scope, clang::AccessSpecifier AS, SourceLocation loc, IdentifierLoc alias_name, clang::CXXScopeSpec &SS, IdentifierLoc identifier);
    
    bool 	CheckUsingDeclRedeclaration (SourceLocation UsingLoc, bool& HasTypenameKeyword, const clang::CXXScopeSpec &SS, SourceLocation NameLoc, const LookupResult &Previous);
};

HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_SEMANTICS_SEMA */

#endif /* HYDROGEN_SRC_SEMANTICS_SEMA_20COPY */

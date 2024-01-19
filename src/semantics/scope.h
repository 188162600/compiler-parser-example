#ifndef HYDROGEN_SRC_SEMANTICS_SCOPE
#define HYDROGEN_SRC_SEMANTICS_SCOPE

#include"src/basic/global.h"
#include<variant>
#include "clang/AST/DeclBase.h"
#include <unordered_map>
HYDROGEN_BEGIN_NAMESPACE

class Scope:public clang::Scope
{
public:

    enum ScopeFlags
    {
        DeclScope=0x01,
        StmtScope=0x02,
        ControlScope=0x04,

        TemplateParamScope=0x08|DeclScope,
        FunctionSignatureScope=0x10|DeclScope,
        FunctionScope=0x20|DeclScope|StmtScope,
        ClassScope=0x40|DeclScope,
        ModuleScope=0x80|DeclScope,
        WhileScope=0x100|StmtScope|ControlScope,
        ForScope=0x200|StmtScope|ControlScope,
        IfScope=0x400|StmtScope|ControlScope,
        ElifScope=0x800|StmtScope|ControlScope,
        ElseScope=0x1000|StmtScope|ControlScope,
        MatchScope=0x2000|StmtScope|ControlScope,
        CaseScope=0x4000|StmtScope|ControlScope,
        TryScope=0x8000|StmtScope|ControlScope,
        ExceptScope=0x10000|StmtScope|ControlScope,
        FinallyScope=0x20000|StmtScope|ControlScope,

    };
    Scope(ScopeFlags type,Scope*scope);
    using expr_range = llvm::iterator_range<std::vector<clang::Expr *>::iterator>;
    void set_entity(clang::DeclContext *declContext);
    bool is(ScopeFlags type)const;
  
    ScopeFlags scope_flags()const;

    void set_parent(Scope*scope);
    clang::DeclContext *entity() const;
    clang::ArrayRef<clang::Stmt *> stmts()const;
    Scope *parent() const;
    Scope*global()const;
    
    void add(clang::Decl *decl);
    size_t template_depth()const;
    size_t template_param_count()const;
    void init(Scope*scope);


    // clang::NestedNameSpecifier * representative_path()const;
    // clang::NestedNameSpecifierLoc representative_path_with_loc_in_context(clang::ASTContext*context)const;

    void update_nrvo_candidate (clang::VarDecl *VD);
    void apply_nrvo_candidate();
    void add_directive_module_import(clang::DeclContext*module_context);
    bool is_valid()const;
    clang::CXXScopeSpec&scope_spec();

    // void increment_template_argument_count();
    // bool function_signature_scope_no_param()const;
    // void set_function_signature_scope_no_param(bool no_param);
    // void add_directive_module_import(clang::DeclContext*module_context);
    //care using namespace ...;
//     void 	PushUsingDirective (UsingDirectiveDecl *UDir)
 
// using_directives_range 	using_directives ()

    ScopeType getScopeType();
    bool is(ScopeType);

private:
    std::vector<clang::Decl*>decls;
    clang::VarDecl*nrvo_candidate;
    
 
};
HYDROGEN_END_NAMESPACE


#endif /* HYDROGEN_SRC_SEMANTICS_SCOPE */

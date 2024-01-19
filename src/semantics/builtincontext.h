#ifndef HYDROGEN_SRC_SEMANTICS_BUILTINCONTEXT
#define HYDROGEN_SRC_SEMANTICS_BUILTINCONTEXT
#include"src/basic/global.h"
#include"clang/AST/Expr.h"
HYDROGEN_BEGIN_NAMESPACE
class BuiltinContext
{
public:
    struct TypeData
    {
        clang::IdentifierInfo*name;
        ParsedExpr object;

        // clang::QualType qualtype;

        // clang::RecordDecl* type_decl;
        // clang::Expr* type_expr;
        bool initialized()const
        {
           return bool(object);
        };
        
    };
    struct MethodData
    {
        bool initialized()const
        {
            return method_name!=nullptr;
        };
       clang::IdentifierInfo* method_name;
    };
    struct FunctionData
    {
        bool initialized()const
        {
            return function_name!=nullptr;
        };
        clang::IdentifierInfo* function_name;
    };
    
    TypeData compiler_ptr_type;
    TypeData int_type;
    TypeData int8_type;
    TypeData int16_type;
    TypeData int32_type;
    TypeData int64_type;
    TypeData int128_type;
    TypeData uint_type;
    TypeData uint8_type;
    TypeData uint16_type;
    TypeData uint32_type;
    TypeData uint64_type;
    TypeData uint128_type;
    TypeData float_type;
    TypeData float32_type;
    TypeData float64_type;
    TypeData float128_type;
    TypeData bool_type;
    TypeData str_type;
    TypeData slice_type;
    TypeData range_type;
    TypeData list_type;
    TypeData bytes_type;
    TypeData tuple_type;
    TypeData dict_type;
    TypeData set_type;
    TypeData function_type;
    TypeData template_function_type;
    TypeData type_data_type;
    TypeData type_type;
    TypeData template_type_type;
    TypeData module_type;
    TypeData none_type;
    TypeData any_type;
    TypeData error_type;
    TypeData assertion_error_type;

    MethodData init_method;
    MethodData len_method;
    MethodData next_method;
    MethodData has_next_method;
    MethodData iter_method;
    MethodData call_method;
    MethodData not_method;
    MethodData getitem_method;
    MethodData setitem_method;
    
    clang::IdentifierInfo* name_self;






};
HYDROGEN_END_NAMESPACE


#endif /* HYDROGEN_SRC_SEMANTICS_BUILTINCONTEXT */

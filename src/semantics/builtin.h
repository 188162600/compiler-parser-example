#ifndef HYDROGEN_SRC_SEMANTICS_BUILTIN
#define HYDROGEN_SRC_SEMANTICS_BUILTIN
class BuiltinContext
{
public:
    struct TypeData
    {
        clang::QualType qualtype;
        clang::RecordDecl* type_decl;
        clang::Expr* type_expr;
        
    };
    struct MethodData
    {
       clang::IdentifierInfo* method_name;
    };
    TypeData int_type;
    TypeData int8_type;
    TypeData int16_type;
    TypeData int32_type;
    TypeData int64_type;
    TypeData int128_type;
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
    TypeData tuple_type;
    TypeData dict_type;
    TypeData set_type;
    TypeData function_type;
    TypeData template_function_type;
    TypeData type_type;
    TypeData template_type_type;
    TypeData module_type;
    TypeData none_type;
    TypeData any_type;
    TypeData error_type;
    TypeData assertion_error_type;
    MethodData len_method;
    MethodData next_method;
    MethodData has_next_method;
    MethodData iter_method;





};

#endif /* HYDROGEN_SRC_SEMANTICS_BUILTIN */

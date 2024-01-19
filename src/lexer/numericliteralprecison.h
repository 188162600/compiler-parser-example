#ifndef HYDROGEN_SRC_LEXER_NUMERICLITERALPRECISON
#define HYDROGEN_SRC_LEXER_NUMERICLITERALPRECISON
#include"src/basic/global.h"
#include "llvm/ADT/StringRef.h"
HYDROGEN_BEGIN_NAMESPACE
class NumericLiteralPrecision
{
public:
    enum EnumKind
    {
    
#define HYDROGEN_INTEGER_LITERAL_PRECISION(NAME, SUFFIX) NAME,
#define HYDROGEN_FLOATING_LITERAL_PRECISION(NAME, SUFFIX) NAME,
        #include "src/lexer/numericliteralprecison.def"
        NumericLiteralPrecisionCount
    };
    constexpr inline NumericLiteralPrecision(EnumKind enum_kind):enum_kind(enum_kind){};
    constexpr inline llvm::StringRef suffix()const
    {
        return suffixes_[enum_kind];
    };
    constexpr inline bool is_frac()const
    {
        return is_fracs_[enum_kind];
    };
    constexpr inline operator EnumKind()const{return enum_kind;};
    constexpr bool is(NumericLiteralPrecision numeric_literal_precision)const
    {
        return enum_kind==numeric_literal_precision.enum_kind;
    }
    
    EnumKind enum_kind;
private:
    constexpr static inline llvm::StringRef suffixes_[]={
#define HYDROGEN_INTEGER_LITERAL_PRECISION(NAME, SUFFIX) llvm::StringRef(SUFFIX),
#define HYDROGEN_FLOATING_LITERAL_PRECISION(NAME, SUFFIX) llvm::StringRef(SUFFIX),
#include "src/lexer/numericliteralprecison.def"
    };
    constexpr static inline bool is_fracs_[]={
#define HYDROGEN_INTEGER_LITERAL_PRECISION(NAME, SUFFIX) false,
#define HYDROGEN_FLOATING_LITERAL_PRECISION(NAME, SUFFIX) true,
#include "src/lexer/numericliteralprecison.def"
    };
};
HYDROGEN_END_NAMESPACE




#endif /* HYDROGEN_SRC_LEXER_NUMERICLITERALPRECISON */

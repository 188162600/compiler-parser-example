#ifndef HYDROGEN_SRC_LEXER_STRINGLITERALENCODING
#define HYDROGEN_SRC_LEXER_STRINGLITERALENCODING
#include"src/basic/global.h"
#include "llvm/ADT/StringRef.h"
HYDROGEN_BEGIN_NAMESPACE
class StringLiteralEncoding
{
public:
    enum EnumKind
    {
        #define HYDROGEN_STRING_LITERAL_ENCODING(NAME, SUFFIX) NAME,
        #include "src/lexer/stringliteralencoding.def"
        StringLiteralEncodingCount
    };
    constexpr inline StringLiteralEncoding(EnumKind enum_kind):enum_kind(enum_kind){};
    constexpr inline llvm::StringRef suffix()const
    {
        return suffixes_[enum_kind];
    };
    constexpr inline operator EnumKind()const{return enum_kind;};
    constexpr bool is(StringLiteralEncoding string_literal_encoding)const
    {
        return enum_kind==string_literal_encoding.enum_kind;
    }
    
    EnumKind enum_kind;
private:
    constexpr static inline llvm::StringRef suffixes_[]={
        #define HYDROGEN_STRING_LITERAL_ENCODING(NAME, SUFFIX) llvm::StringRef(SUFFIX),
        #include "src/lexer/stringliteralencoding.def"
    };
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_LEXER_STRINGLITERALENCODING */

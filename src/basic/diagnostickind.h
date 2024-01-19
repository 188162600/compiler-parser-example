#ifndef HYDROGEN_SRC_BASIC_DIAGNOSTICKIND
#define HYDROGEN_SRC_BASIC_DIAGNOSTICKIND
#include"src/basic/global.h"
#include "clang/Basic/DiagnosticIDs.h"

HYDROGEN_BEGIN_NAMESPACE
class DiagnosticKind
{
public:

    enum Level
    {
        Ignored=clang::DiagnosticIDs::Ignored,
        Note=clang::DiagnosticIDs::Note,
        Remark=clang::DiagnosticIDs::Remark,
        Warning=clang::DiagnosticIDs::Warning,
        Error=clang::DiagnosticIDs::Error,
        Fatal=clang::DiagnosticIDs::Fatal

    };
    enum KindEnum
    {
        #define HYDROGEN_DIAGNOSTIC_KIND(NAME, LEVEL, DESC) NAME,
        #include"src/basic/diagnostickind.def"
        NUM_DIAGNOSTIC_KINDS
    };
    inline constexpr DiagnosticKind(KindEnum kind)
    : enum_kind(kind)
    {

    }
    Level level();
    
    KindEnum enum_kind;

};
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_BASIC_DIAGNOSTICKIND */

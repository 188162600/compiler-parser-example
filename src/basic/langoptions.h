#ifndef HYDROGEN_BASIC_LANGOPTIONS
#define HYDROGEN_BASIC_LANGOPTIONS
#include"src/basic/global.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Specifiers.h"

HYDROGEN_BEGIN_NAMESPACE
class LangOptions
{
public:
   

    enum class ABI
    {

    };
    enum LangStandard
    {
        UnspecifiedLangStandard,
        Hydrogen_0_1,

    };
    clang::FPOptionsOverride fp_feature;
    clang::StorageClass storage_class;
 
    bool use_FPIntrin;
    llvm::RoundingMode rounding_mode;
    static void set_lang_default(LangOptions &Opts, const llvm::Triple &T, LangStandard standard=UnspecifiedLangStandard);
};

HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_BASIC_LANGOPTIONS */

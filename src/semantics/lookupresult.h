#ifndef HYDROGEN_SRC_SEMANTICS_LOOKUPRESULT
#define HYDROGEN_SRC_SEMANTICS_LOOKUPRESULT

#include "src/basic/identifierloc.h"

HYDROGEN_BEGIN_NAMESPACE
class Sema;
class LookupResult
{
public:
    enum LookupResultKind
    {
        NotFound = 0,
        NotFoundInCurrentInstantiation,
        Found,
        FoundOverloaded,
        FoundUnresolvedValue,
        Ambiguous
    };
    enum LookupNameKind
    {
        LookupValueName,
        LookupFunctionName,
        LookupFunctionTemplateName,
        LookupMethodName,
        LookupMethodTemplateName,
        LookupClassName,
        LookupClassTemplateName,
        LookupModuleName,
        Unknown

    };
    enum RedeclarationKind

    {
        NotForRedeclaration,
        ForVisibleRedeclaration,
    };
    LookupResult(Sema &sema, IdentifierLoc name, RedeclarationKind redeclaration_kind = NotForRedeclaration);
    void resolve_kind();
    
    template <class DeclClass>
    DeclClass *as_single() const;
    const clang::UnresolvedSetImpl &as_unresolved_set() const;
    clang::NamedDecl*representative_decl()const;
    bool is_empty() const;
    LookupNameKind lookup_name_kind() const;
    bool is_single_result()const;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_LOOKUPRESULT */

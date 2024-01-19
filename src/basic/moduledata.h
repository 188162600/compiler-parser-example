#ifndef HYDROGEN_SRC_RESOLVE_MODULEDATA
#define HYDROGEN_SRC_RESOLVE_MODULEDATA
#include"src/basic/global.h"
#include"src/basic/identifierloc.h"
HYDROGEN_BEGIN_NAMESPACE
using ModuleIdPath = std::vector<IdentifierLoc>;
struct ModuleData 
{
    clang::DeclContext  *module_decl_context;
    



};
HYDROGEN_END_NAMESPACE
class ModuleId
{
public:
};
#endif /* HYDROGEN_SRC_RESOLVE_MODULEDATA */
            
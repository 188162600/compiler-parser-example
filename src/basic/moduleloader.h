#ifndef HYDROGEN_SRC_BASIC_MODULELOADER
#define HYDROGEN_SRC_BASIC_MODULELOADER
#include "src/basic/global.h"
#include "src/basic/sourcelocation.h"
#include "src/basic/identifierpath.h"
HYDROGEN_BEGIN_NAMESPACE
class ModuleId
{
public:
bool is_invalid()const;
bool is_valid()const;
};
class ModuleLoader
{
    public:
    ModuleId look_up(SourceLocation import_loc,IdentifierPath path);

};
class ModuleManager
{
    public:
      ModuleId look_up(SourceLocation import_loc,IdentifierPath path);

}
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_BASIC_MODULELOADER */

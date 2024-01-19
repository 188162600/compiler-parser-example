#ifndef HYDROGEN_SRC_BASIC_GLOBAL
#define HYDROGEN_SRC_BASIC_GLOBAL

#include<string_view>
#include<span>
#include<vector>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<span>
#include<random>
#include<array>
#define HYDROGEN_BEGIN_NAMESPACE namespace hydro{
#define HYDROGEN_END_NAMESPACE };
#define HYDROGEN_DEBUG
#ifdef HYDROGEN_DEBUG
#define debug_assert(EXPRESSION) assert(EXPRESSION) 
#else
#define debug_assert(EXPRESSION) 
#endif


#endif /* HYDROGEN_SRC_BASIC_GLOBAL */

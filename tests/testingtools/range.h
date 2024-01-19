#ifndef HYDROGEN_TESTS_TESTINGTOOLS_RANGE
#define HYDROGEN_TESTS_TESTINGTOOLS_RANGE

#include"src/basic/global.h"
HYDROGEN_BEGIN_NAMESPACE
template<typename T>
struct Range
{
     Range()=default;
    Range(T&& first, T&& last):first(first), last(last){};
    T first;
    T last;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_TESTS_TESTINGTOOLS_RANGE */

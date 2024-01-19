#ifndef HYDROGEN_TESTS_TESTINGTOOLS_RANDOM
#define HYDROGEN_TESTS_TESTINGTOOLS_RANDOM

#include"src/basic/global.h"
#include"tests/testingtools/range.h"
#include<set>
HYDROGEN_BEGIN_NAMESPACE
int random_int(int min, int max)
{
    return rand() % (max - min + 1) + min;
}
template<std::integral T>
T random_int(Range<T> range)
{
    return random_int(range.first, range.last);
}
template<std::integral T>
std::vector<T>random_ints(size_t count, T min, T max)
{
    std::vector<T> ints;
    for (size_t i = 0; i < count; ++i)
    {
        ints.push_back(random_int(min, max));
    }
    return ints;
}
template<std::integral T>
std::vector<T>random_unique_ints(size_t count, T min, T max)
{
    assert(count <= max - min + 1);
    std::set<T> ints;

    for (size_t i = 0; i < count; i++)
    {
        while (true)
        {
            
            T random_int = random_int(min, max);
            if (ints.find(random_int) == ints.end())
            {
                ints.insert(random_int);
                break;
            }
        }
        
        ints.push_back(random_int);
    }
    return std::vector<T>(ints);
}
template<std::integral T>
std::vector<T>random_unique_ints(size_t count, Range<T> range)
{
    return random_unique_ints(count, range.first, range.last);
}
template<typename Iterator>
auto random_element(Iterator first, Iterator last)
{
    auto size = std::distance(first, last);
    auto random_index = random_int(0, size - 1);
    auto random_it = std::next(first, random_index);
    return *random_it;
}
template<typename InputIterator,typename OutputIterator>
void random_sample(InputIterator first,InputIterator last,OutputIterator out_first,OutputIterator out_last)
{
    auto size = std::distance(first, last);
    for(auto it = out_first;it!=out_last;++it)
    {
        auto random_index = random_int(0, size - 1);
        auto random_it = std::next(first, random_index);
        *it = *random_it;
    }
    
    

} 
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_TESTS_TESTINGTOOLS_RANDOM */

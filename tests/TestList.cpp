#include "../part2/List.h"
#include <type_traits>

int main () {
    List<int, double, char> l;
    static_assert(l.size == 3, "Size of List<int, double, char> should be 3");
    static_assert(std::is_same<decltype(l)::head, int>::value, "Head of List<int, double, char> should be int");
    static_assert(std::is_same<PrependList<int, decltype(l)>::list, List<int, int, double, char>>::value, "PrependList<float, List<int, double, char>> should be List<float, int, double, char>");
    static_assert(std::is_same<GetAtIndex<2, decltype(l)>::value, char>::value, "GetAtIndex<0, List<int, double, char>> should be int");
    return 0;
}

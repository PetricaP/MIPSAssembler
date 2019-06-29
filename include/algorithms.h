#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <algorithm>
#include <type_traits>

namespace pp {

template <typename IT>
struct it_value_type
{
    typedef typename std::iterator_traits<IT>::value_type elem;
};

template <typename Container>
struct it_value_type<std::back_insert_iterator<Container>>
{
    typedef typename Container::value_type elem;
};

template <typename Container>
struct it_value_type<std::front_insert_iterator<Container>>
{
    typedef typename Container::value_type elem;
};

template <typename FIterator>
inline bool contains(FIterator begin, FIterator end, typename FIterator::value_type const &value) {
    return std::find(begin, end, value) != end;
}

template <typename FIterator, typename Predicate>
inline bool contains_which(FIterator begin, FIterator end, Predicate p) {
    return std::find_if(begin, end, p) != end;
}

template <typename FIterator, typename Predicate>
inline bool contains_which_not(FIterator begin, FIterator end, Predicate p) {
    return std::find_if_not(begin, end, p) != end;
}

template <typename FIterator1, typename FIterator2>
std::pair<FIterator1, FIterator2> find_first_of(FIterator1 begin1, FIterator1 end1,
                                                FIterator2 begin2, FIterator2 end2) {
    while(begin1 != end1) {
        for(auto i2 = begin2; i2 != end2; ++i2) {
            if(*begin1 == *i2) {
                return std::make_pair(begin1, i2);
            }
        }
        ++begin1;
    }
    return std::make_pair(end1, end2);
}

template <typename FIterator1, typename FIterator2, typename FIterator3>
void split(FIterator1 begin1, FIterator1 end1, FIterator2 begin2, FIterator2 end2, FIterator3 out) {
    using Container = typename it_value_type<FIterator3>::elem;
    auto last = begin1;
    auto next = ::pp::find_first_of(begin1, end1, begin2, end2);
    for(; next.first != end1; ) {
        if(last != next.first) {
            Container c;
            std::copy(last, next.first, std::back_inserter(c));
            *out++ = std::move(c);
        }

        last = next.first + 1;
        next = ::pp::find_first_of(last, end1, begin2, end2);
    }
}

}

#endif // ALGORITHMS_H_

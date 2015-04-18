#ifndef AQ_SORT_IMPL_SELECT_PIVOT_H
#define AQ_SORT_IMPL_SELECT_PIVOT_H

#include <cassert>
#include <cstddef>

#ifdef AQ_SORT_RANDOM_PIVOT
#include "random_number_generator.h"
#endif

namespace aq
{
    namespace impl
    {
#ifdef AQ_SORT_RANDOM_PIVOT
        // random pivot
        inline std::size_t select_pivot(std::size_t start, std::size_t n, impl::random_number_generator* const rng)
        {
            // avoid unnecessary calling performance penalty
            assert (n > 0);

            return start + rng->operator()() % n;
        }
#else
        // median-of-3 pivot
        template<typename Comp>
        inline std::size_t select_pivot(std::size_t start, std::size_t n, Comp* const comp)
        {
            // avoid unnecessary calling performance penalty
            assert (n > 0);

            std::size_t left   = start;
            std::size_t middle = start + (n >> 1);
            std::size_t right  = start + n - 1;

            if ((*comp)(left, right)) {
                if ((*comp)(middle, left)) 
                    return left;
                else if ((*comp)(middle, right))
                    return middle;
                return right;
            }

            if ((*comp)(left, middle))
                return left;
            else if ((*comp)(right, middle))
                return middle;
            return right;
        }
#endif
    }
}

#endif

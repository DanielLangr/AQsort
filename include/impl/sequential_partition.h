#ifndef AQ_SORT_IMPL_SEQUENTIAL_PARTITION_H
#define AQ_SORT_IMPL_SEQUENTIAL_PARTITION_H

#include <cassert>
#include <cstddef>

namespace aq
{
    namespace impl
    {
        template<typename Comp, typename Swap>
        inline std::size_t sequential_partition(
                std::size_t start, std::size_t n, std::size_t pivot, Comp* const comp, Swap* const swap)
        {
            // avoid unnecessary calling performance penalty
            assert (n > 0);

            std::size_t left  = start;
            std::size_t right = start + n - 1;

            while (left < right) {
                while ((left < right) && ((*comp)(left, pivot)))
                    left++;
                while ((left < right) && (!(*comp)(right, pivot)))
                    right--;
                if (left < right) {
                    (*swap)(left, right);
                    left++;
                 // if (left < right) // cannot lead to underflow since left >= 1 and right >= left here
                    right--;
                }
            }
            if ((*comp)(left, pivot))
                left++;

            // return number of elements less than pivot
            return left - start;
        }
    }
}

#endif

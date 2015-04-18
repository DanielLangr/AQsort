#ifndef AQ_SORT_IMPL_SEQUENTIAL_SORT_H
#define AQ_SORT_IMPL_SEQUENTIAL_SORT_H

#include <cassert>
#include <cstddef>

#ifdef AQ_SORT_RANDOM_PIVOT
#include "random_number_generator.h"
#endif

#include "select_pivot.h"
#include "sequential_partition.h"

namespace aq
{
    namespace impl
    {
        template<typename Comp, typename Swap>
        inline void insert_sort(std::size_t start, std::size_t n, Comp* const comp, Swap* const swap)
        {
            // avoid unnecessary calling performance penalty
            assert (n > 1);

            // find minimal element
            std::size_t temp = 0;
            for (std::size_t i = 1; i < n; ++i)
                if ((*comp)(start + i, start + temp)) 
                    temp = i;

            (*swap)(start, start + temp);

            temp = 0;
            while (++temp < n)
                for (std::size_t j = temp; (*comp)(start + j, start + j - 1); j--) 
                    (*swap)(start + j - 1, start + j);
        }

        template<typename Comp, typename Swap>
#ifdef AQ_SORT_RANDOM_PIVOT
        void quick_sort(std::size_t start, std::size_t n, Comp* const comp, Swap* const swap,
                impl::random_number_generator* const rng)
#else
        void quick_sort(std::size_t start, std::size_t n, Comp* const comp, Swap* const swap)
#endif
        {
            // avoid unnecessary calling performance penalty
            assert (n > 0);

            // quick sort need to be invoked with at least 2 elements
            assert (AQ_SORT_INSERT_SORT_THRESHOLD > 1);

            // if length is below threshold then use insert sort
            if (n <= AQ_SORT_INSERT_SORT_THRESHOLD) {
                insert_sort(start, n, comp, swap);
                return;
            }

            // choose pivot and put it at the end (position n - 1)
#ifdef AQ_SORT_RANDOM_PIVOT
            // random pivot
            std::size_t pivot = select_pivot(start, n, rng);
#else
            // median-of-3 pivot
            std::size_t pivot = select_pivot(start, n, comp);
#endif
            // swap to the end
            (*swap)(pivot, start + n - 1);
            pivot = start + n - 1;

            // partitioning
            std::size_t less_than = sequential_partition(start, n, pivot, comp, swap);

            assert(less_than >= 0);
            assert(less_than <= n);

            // swap pivot to its final position
            (*swap)(start + less_than, pivot);

            // do not process equal-to-pivot elements
            std::size_t greater_than = n - less_than - 1;
            while ((greater_than > 0) &&
                    ((*comp)(start + less_than, start + n - greater_than) == false) &&
                    ((*comp)(start + n - greater_than, start + less_than) == false))
                greater_than--;

#ifdef AQ_SORT_RANDOM_PIVOT
            if (less_than > 1)
                quick_sort(start,                    less_than,    comp, swap, rng);
         // if (n > less_than + 2)
            if (greater_than > 1)
             // quick_sort(start + less_than + 1, n - less_than - 1, comp, swap, rng);
                quick_sort(start + n - greater_than, greater_than, comp, swap, rng);
#else
            if (less_than > 1)
                quick_sort(start,                 less_than,       comp, swap);
         // if (n > less_than + 2)
            if (greater_than > 1)
             // quick_sort(start + less_than + 1, n - less_than - 1, comp, swap);
                quick_sort(start + n - greater_than, greater_than, comp, swap);
#endif
        }

        template<typename Comp, typename Swap>
        inline void sequential_sort(std::size_t start, std::size_t n, Comp* const comp, Swap* const swap)
        {
            // nothing-to-sort check
            if (n <= 1)
                return;

#ifdef AQ_SORT_RANDOM_PIVOT
            impl::random_number_generator rng;
            quick_sort(start, n, comp, swap, &rng);
#else
            quick_sort(start, n, comp, swap);
#endif
        }

        template<typename Comp, typename Swap>
        inline void sequential_sort(std::size_t n, Comp* const comp, Swap* const swap)
        {
            sequential_sort(0, n, comp, swap);
        }
    }
}

#endif

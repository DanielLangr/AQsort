#ifndef AQ_SORT_H
#define AQ_SORT_H

#include <cstddef>

// threshold for using insert sort instead of quick sort in sequential sorting
#ifndef AQ_SORT_INSERT_SORT_THRESHOLD 
#define AQ_SORT_INSERT_SORT_THRESHOLD (16)
#endif

#include "impl/sequential_sort.h"

// blocks size used by paralell partitioning algorithm
#ifndef AQ_PARALLEL_PARTITION_BLOCK_SIZE
#define AQ_PARALLEL_PARTITION_BLOCK_SIZE (1024)
#endif

#ifdef _OPENMP
#include "impl/parallel_sort.h"
#endif

/* 
 * Passing instance references to OpenMP tasks as shared variables can
 * lead to segmentation fault (Intel compiler). Thus, passing as constant pointers.
 */

namespace aq
{
#ifdef _OPENMP
    template <typename Comp, typename Swap>
 // inline void parallel_sort(std::size_t length, Comp& comp, Swap& swap)
    inline void parallel_sort(std::size_t length, Comp* const comp, Swap* const swap)
    {
        impl::parallel_sort(length, comp, swap);
    }
#endif

    template <typename Comp, typename Swap>
 // inline void sequential_sort(std::size_t length, Comp& comp, Swap& swap)
    inline void sequential_sort(std::size_t length, Comp* const comp, Swap* const swap)
    {
        impl::sequential_sort(length, comp, swap);
    }

    template <typename Comp, typename Swap>
 // inline void sort(std::size_t length, Comp& comp, Swap& swap)
    inline void sort(std::size_t length, Comp* const comp, Swap* const swap)
    {
#ifdef _OPENMP
        parallel_sort(length, comp, swap);
#else
        sequential_sort(length, comp, swap);
#endif
    }
}

#endif

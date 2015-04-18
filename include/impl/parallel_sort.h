#ifndef AQ_SORT_IMPL_PARALLEL_SORT_H
#define AQ_SORT_IMPL_PARALLEL_SORT_H

#ifdef _OPENMP
#include <omp.h>
#endif

#include <cassert>
#include <cstddef>

#include "parallel_partition.h"

#ifdef AQ_SORT_RANDOM_PIVOT
#include "random_number_generator.h"
#endif 

#include "sequential_sort.h"


namespace aq
{
    namespace impl
    {
        template<typename Comp, typename Swap>
#ifdef AQ_SORT_RANDOM_PIVOT
        void parallel_quick_sort(std::size_t total_n, std::size_t total_P, std::size_t start, std::size_t n,
                Comp* const comp, Swap* const swap, impl::random_number_generator* const rng)
#else
        void parallel_quick_sort(std::size_t total_n, std::size_t total_P, std::size_t start, std::size_t n,
                Comp* const comp, Swap* const swap)
#endif
        {
            // avoid unnecessary calling performance penalty
            assert (n > 1); 

            // actual number of threads for this chunk
            std::size_t P = total_P * n / total_n;
            if (P < 1)
                P = 1;

            if (P < 2) {
                // sequential sort
#pragma omp task untied firstprivate(comp, swap)
                sequential_sort(start, n, comp, swap);
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

            // parallel partitioning using P threads
            std::size_t less_than = parallel_partition(start, n, pivot, comp, swap, P);

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
#pragma omp task untied firstprivate(comp, swap, rng)
                parallel_quick_sort(total_n, total_P, start,                    less_than,    comp, swap, rng);
         // if (n > less_than + 2)
            if (greater_than > 1)
//#pragma omp task untied firstprivate(comp, swap, rng)
             // parallel_quick_sort(total_n, total_P, start + less_than + 1, n - less_than - 1, comp, swap, rng);
                parallel_quick_sort(total_n, total_P, start + n - greater_than, greater_than, comp, swap, rng);
#else
            if (less_than > 1)
#pragma omp task untied firstprivate(comp, swap)
                parallel_quick_sort(total_n, total_P, start,                    less_than,    comp, swap);
         // if (n > less_than + 2)
            if (greater_than > 1)
//#pragma omp task untied firstprivate(comp, swap)
             // parallel_quick_sort(total_n, total_P, start + less_than + 1, n - less_than - 1, comp, swap, rng);
                parallel_quick_sort(total_n, total_P, start + n - greater_than, greater_than, comp, swap);
#endif
        }

        // must be executed inside a parallel region by all threads
        template<typename Comp, typename Swap>
        void parallel_sort_parallel(std::size_t n, Comp* const comp, Swap* const swap, std::size_t P)
        {
#ifdef AQ_SORT_RANDOM_PIVOT
            impl::random_number_generator rng; // for all threads
#endif

#ifdef AQ_SORT_RANDOM_PIVOT
#pragma omp master
            parallel_quick_sort(n, P, 0, n, comp, swap, &rng);
#else
#pragma omp master
            parallel_quick_sort(n, P, 0, n, comp, swap      );
#endif

#pragma omp taskwait
        }
        
        template<typename Comp, typename Swap>
        void parallel_sort(std::size_t n, Comp* const comp, Swap* const swap)
        {
            int nested_orig = omp_get_nested();
            omp_set_nested(1);

            // nested is a must for parallel algorithm
            if (omp_get_nested() == 0) {
                sequential_sort(0, n, comp, swap);
                omp_set_nested(nested_orig);
                return;
            }

            std::size_t P = omp_get_num_threads();

            if (P > 1) {
                // already in parallel region
                parallel_sort_parallel(n, comp, swap, P);
#pragma omp barrier
            }
            else {
#pragma omp parallel shared(P)
                {
#pragma omp single
                    P = omp_get_num_threads();

                    // at least 2 threads is a must
                    if (P < 2) {
#pragma omp master
                        sequential_sort(0, n, comp, swap);
                    }
                    else {
                        parallel_sort_parallel(n, comp, swap, P);
                    }
                } // implied barrier at the end of parallel construct
            }

            omp_set_nested(nested_orig);
        }
    }
}

#endif

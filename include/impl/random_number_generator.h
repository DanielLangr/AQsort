#ifndef AQ_SORT_IMPL_RANDOM_NUMBER_GENERATOR_H
#define AQ_SORT_IMPL_RANDOM_NUMBER_GENERATOR_H

#ifdef _OPENMP
#include <omp.h>
#endif

#include <cstdlib>
#include <ctime>

#ifdef AQ_SORT_RNG_USE_BOOST
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/type_traits/make_signed.hpp>
#else
#include <random>
#include <type_traits>
#endif

namespace aq
{
    namespace impl
    {
        struct random_number_generator {
#ifdef AQ_SORT_RNG_USE_BOOST
            typedef boost::random::mt19937_64::result_type result_type;
            typedef boost::make_signed<result_type>::type signed_result_type;

            boost::random::mt19937_64 gen;
            boost::random::uniform_int_distribution<result_type> dist;
#else
            typedef std::mt19937_64::result_type result_type;
            typedef std::make_signed<result_type>::type signed_result_type;

            std::mt19937_64 gen;
            std::uniform_int_distribution<result_type> dist;
#endif

            random_number_generator()
            {
                std::time_t seconds = std::time(NULL);
                signed_result_type thread_correction;
#ifdef _OPENMP
                thread_correction = (seconds * 181) * ((omp_get_thread_num() - 83) * 359);
#else 
                thread_correction = (seconds * 181) * ((0                    - 83) * 359);
#endif
                result_type seed = std::abs(thread_correction) % 104729;
                gen.seed(seed);
            }

            result_type operator()() { return dist(gen); }
        };
    }
}

#endif

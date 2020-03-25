#ifndef PSTL_PARTITION_H
#define PSTL_PARTITION_H

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

//--------------------------------------------
// simplified code of PSTL partition
//--------------------------------------------
namespace mypstl {

// PSTL uses                `parallel_reduce <auto_partitioner>`
// but TBB does not provide `parallel_deterministic_reduce <auto_partitioner>`
// to compare the performance we use the same partitioner for two versions
typedef tbb::simple_partitioner partitioner_t;

// in pstl/internal/unseq_backend_simd.h
template <class Iterator1, class DifferenceType, class Iterator2>
Iterator2 pstl_simd_swap(Iterator1 first, DifferenceType n, Iterator2 second) noexcept {
    #pragma vector nontemporal
    #pragma omp simd
    for (DifferenceType i = 0; i < n; ++i)
        std::iter_swap(first + i, second + i);
    return second + n;
}

template <class Iterator>
Iterator pstl_simd_rotate(Iterator first, Iterator middle, Iterator last) noexcept {
    auto n = last - first;
    auto m = middle - first;
    const Iterator ret = first + (last - middle);

    bool is_left = (m <= n / 2);
    if (!is_left)
        m = n - m;

    while (n > 1 && m > 0) {
        const auto m_2 = m * 2;
        if (is_left) {
            for (; last - first >= m_2; first += m) {
                pstl_simd_swap(first, m, first + m);
            }
        }
        else {
            for (; last - first >= m_2; last -= m) {
                pstl_simd_swap(last - m, m, last - m_2);
            }
        }
        is_left = !is_left;
        m = n % m;
        n = last - first;
    }

    return ret;
}

template<typename Iterator>
struct PartitionRange {
    typedef Iterator iterator;
    Iterator begin, pivot, end;
};

//--------------------------------------------
// parallel partition
//--------------------------------------------
template<typename Range>
Range merge_partitioned_range(Range val1, Range val2) {

    typedef tbb::blocked_range<typename Range::iterator> input_range_t;

    auto size1 = val1.end - val1.pivot;
    auto size2 = val2.pivot - val2.begin;
    auto new_begin = val2.begin - (val1.end - val1.begin);

    if (val1.end == val1.pivot) {
        return {new_begin, val2.pivot, val2.end};
    }
    
    else if (size2 > size1) {
        tbb::parallel_for(input_range_t(val1.pivot, val1.pivot + size1),
            [val1, val2, size1](const input_range_t& r) {
                auto i = r.begin();
                pstl_simd_swap(i, r.size(), (val2.pivot - size1) + (i - val1.pivot));
            });
        return {new_begin, val2.pivot - size1, val2.end};
    }

    else {
        tbb::parallel_for(input_range_t(val1.pivot, val1.pivot + size2),
            [val1, val2](const input_range_t& r) {
                auto i = r.begin();
                pstl_simd_swap(i, r.size(), val2.begin + (i - val1.pivot));
            });
        return {new_begin, val1.pivot + size2, val2.end};
    }
}

template<typename Iterator, typename Predicate>
Iterator partition(Iterator begin, Iterator end, Predicate pred, 
    std::size_t grain_size = 1000 /* additional parameter for tuning */) { 

    typedef tbb::blocked_range<Iterator> input_range_t;
    typedef PartitionRange<Iterator>     output_range_t;

    output_range_t init {end, end, end};

    return tbb::parallel_reduce(input_range_t(begin, end, grain_size), init, 

        [&pred] (const input_range_t& range, const output_range_t& left) {
            Iterator i = range.begin(), j = range.end(), pivot = std::partition(i, j, pred);
            return merge_partitioned_range(left, {i, pivot, j});
        },

        [](const output_range_t& left, const output_range_t& right) {
            return merge_partitioned_range(left, right);
        },

        partitioner_t()
    ).pivot;
}


//--------------------------------------------
// modified partition using parallel_deterministic_reduce
//--------------------------------------------
template<typename Iterator, typename Predicate>
Iterator partition_deterministic(Iterator begin, Iterator end, Predicate pred, 
    std::size_t grain_size = 1000 /* additional parameter for tuning */) { 

    typedef tbb::blocked_range<Iterator> input_range_t;
    typedef PartitionRange<Iterator>     output_range_t;

    output_range_t init {end, end, end};

    return tbb::parallel_deterministic_reduce(input_range_t(begin, end, grain_size), init, 

        [&pred] (const input_range_t& range, const output_range_t& left) {
            Iterator i = range.begin(), j = range.end(), pivot = std::partition(i, j, pred);
            return merge_partitioned_range(left, {i, pivot, j});
        },

        [](const output_range_t& left, const output_range_t& right) {
            return merge_partitioned_range(left, right);
        },

        partitioner_t()
    ).pivot;
}


//--------------------------------------------
// parallel stable_partition
//--------------------------------------------
template<typename Range>
Range stable_merge_partitioned_range(Range val1, Range val2) {

    auto size1 = val1.end - val1.pivot;
    auto new_begin = val2.begin - (val1.end - val1.begin);

    if (val1.end != val1.pivot) {
        pstl_simd_rotate(val1.pivot, val2.begin, val2.pivot);
    }

    return {new_begin, val2.pivot - size1, val2.end};
}

template<typename Iterator, typename Predicate>
Iterator stable_partition(Iterator begin, Iterator end, Predicate pred, 
    std::size_t grain_size = 1000 /* additional parameter for tuning */) { 

    typedef tbb::blocked_range<Iterator> input_range_t;
    typedef PartitionRange<Iterator>     output_range_t;

    output_range_t init {end, end, end};

    return tbb::parallel_reduce(input_range_t(begin, end, grain_size), init, 

        [&pred] (const input_range_t& range, const output_range_t& left) {
            Iterator i = range.begin(), j = range.end(), pivot = std::stable_partition(i, j, pred);
            return stable_merge_partitioned_range(left, {i, pivot, j});
        },

        [](const output_range_t& left, const output_range_t& right) {
            return stable_merge_partitioned_range(left, right);
        },

        partitioner_t()
    ).pivot;
}


//--------------------------------------------
// modified stable_partition using parallel_deterministic_reduce
//--------------------------------------------
template<typename Iterator, typename Predicate>
Iterator stable_partition_deterministic(Iterator begin, Iterator end, Predicate pred, 
    std::size_t grain_size = 1000 /* additional parameter for tuning */) { 

    typedef tbb::blocked_range<Iterator> input_range_t;
    typedef PartitionRange<Iterator>     output_range_t;

    output_range_t init {end, end, end};

    return tbb::parallel_deterministic_reduce(input_range_t(begin, end, grain_size), init, 

        [&pred] (const input_range_t& range, const output_range_t& left) {
            Iterator i = range.begin(), j = range.end(), pivot = std::stable_partition(i, j, pred);
            return stable_merge_partitioned_range(left, {i, pivot, j});
        },

        [](const output_range_t& left, const output_range_t& right) {
            return stable_merge_partitioned_range(left, right);
        },

        partitioner_t()
    ).pivot;
}

}

#endif
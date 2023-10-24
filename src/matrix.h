#pragma once

#include <iostream>
#include <cassert>
#include <numeric>
#include <vector>
#include <ranges>

#include "elements_buffer.h"
#include "vector.h"
#include "iterators.h"
#include "matrix_traits.h"
#include "matrix_line.h"


namespace ranges = std::ranges;
namespace views = std::views;


namespace hwmx {
    template<typename T, bool is_lazy, typename Traits> class Matrix;

    size_t gaussian_elimination(Matrix<double, false, MatrixTraits<double>>& matrix) noexcept;


    class non_square_matrix_error : public std::runtime_error {
    public:
        non_square_matrix_error() throw()
            : std::runtime_error("Matrix is non square.") {};
    };


    template<typename T, bool is_lazy = false, typename Traits = MatrixTraits<T>>
    class Matrix : private ElementsBuf_<T, is_lazy> {
        template<bool is_one_line = false>
        using RMI = RowMajorIterator<T, is_one_line>;

        template<bool is_one_line = false>
        using CMI = ColMajorIterator<T, is_one_line>;

        template<bool is_col_major = false>
        using II = MatrixGeneralIterator<T, false, is_col_major>;

        template<bool is_one_line = false>
        using const_RMI = RowMajorIterator<const T, is_one_line>;

        template<bool is_one_line = false>
        using const_CMI = ColMajorIterator<const T, is_one_line>;

        template<bool is_col_major = false>
        using const_II = MatrixGeneralIterator<const T, false, is_col_major>;

        size_t x, y;

        using ElementsBuf_<T, is_lazy>::data;
        using ElementsBuf_<T, is_lazy>::size;

        using ElementsBuf_<T, is_lazy>::swap;
    public:
        using elements_type = T;

        Matrix(size_t x, size_t y, T val = T{}):
            x(x), y(y), ElementsBuf_<T, is_lazy>(x*y, val) {}

        Matrix(const Matrix& rhs) : x(rhs.x), y(rhs.y), ElementsBuf_<T, is_lazy>(rhs) {}

        Matrix& operator=(const Matrix& rhs) {
            Matrix tmp{ rhs };

            std::swap(*this, tmp);
            return *this;
        }

        Matrix(Matrix&& rhs) : ElementsBuf_<T, is_lazy>(std::move(rhs)) {}

        Matrix& operator=(Matrix&& rhs) {
            swap(rhs);
            return *this;
        }

        template<typename IT>
        Matrix(size_t x, size_t y, const IT& first, const IT& second):
            x(x), y(y), ElementsBuf_<T, is_lazy>(x*y, first, second) {}

        static Matrix eye(size_t n) {
            Matrix m{ n, n };
            for (int i = 0; i < n; i++)
                m.set_value(i, i, 1);

            return m;
        }

        static Matrix iota(size_t n) {
            auto elements = views::iota(0u, (n * n));
            return Matrix{ n, n, elements.begin(), elements.end(),  };
        }


        // const

        size_t cols() const noexcept { return y; }
        size_t rows() const noexcept { return x; }

        T get_value(size_t ix, size_t iy) const {
            return data[ix * y + iy];
        }

        const T& const_ref_value(size_t ix, size_t iy) const {
            return data[ix * y + iy];
        }

        void print() const noexcept {
            for (size_t i = 0; i < x; i++) {
                row(i).print();
                std::cout << std::endl;
            }
        }

        double det() const {
            if (x != y)
                throw non_square_matrix_error{};

            Matrix<double> m{ x, y };
            std::transform(
                cbegin(), cend(), m.begin(),
                [](T el) { return Traits::to_double(el); }
            );
            size_t count_swaps = gaussian_elimination(m);
            
            std::vector<double> diag(x);
            for (size_t i = 0; i < x; i++)
                diag[i] = m[i][i];
            
            return std::reduce(
                diag.begin(), diag.end(),
                count_swaps % 2 ? -1.0 : 1.0,
                std::multiplies<double>()
            );
        }

        const Col<const Matrix> col(size_t iy) const { return Col<const Matrix>{ this, iy }; }
        const Row<const Matrix> row(size_t ix) const { return Row<const Matrix>{ this, ix }; }

        const Row<const Matrix> operator[](size_t ix) const { return row(ix); }


        // mutable

        Col<Matrix> col(size_t iy) { return Col<Matrix>{ this, iy }; }
        Row<Matrix> row(size_t ix) { return Row<Matrix>{ this, ix }; }

        Row<Matrix> operator[](size_t ix) { return row(ix); }

        void set_value(size_t ix, size_t iy, T value) {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            data[ix * y + iy] = value;
        }

        T& ref_value(size_t ix, size_t iy) {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return data[ix * y + iy];
        }

        void swap_rows(size_t ix1, size_t ix2) {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            std::swap_ranges(
                data + ix1 * y,
                data + (ix1 + 1) * y,
                data + ix2 * y
            );
        }


        // iterators

        template<bool is_col_major = false>
        II<is_col_major> item_iter(size_t ix, size_t iy) noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return II<is_col_major>{ data, ix, iy, x, y };
        }

        template<bool is_col = false>
        MatrixGeneralIterator<T, true, is_col> line_begin(size_t ln) noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, 0, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, 0, x, y };
        }

        template<bool is_col = false>
        MatrixGeneralIterator<T, true, is_col> line_end(size_t ln) noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, x, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, y, x, y };
        }

        RMI<> begin() noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return item_iter(0, 0);
        }
        RMI<> end() noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return item_iter(x, 0);
        }

        CMI<> col_begin() const noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();
            
            return item_iter<true>(0, 0);
        }
        CMI<> col_end() const noexcept {
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return item_iter<true>(0, y);
        }


        // const_iterators

        template<bool is_col_major = false>
        const_II<is_col_major> item_citer(size_t ix, size_t iy) const noexcept {
            return const_II<is_col_major>{ data, ix, iy, x, y };
        }

        template<bool is_col = false>
        MatrixGeneralIterator<const T, true, is_col> line_cbegin(size_t ln) const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<const T, true, is_col>{ data, 0, ln, x, y };
            else
                return MatrixGeneralIterator<const T, true, is_col>{ data, ln, 0, x, y };
        }

        template<bool is_col = false>
        MatrixGeneralIterator<const T, true, is_col> line_cend(size_t ln) const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<const T, true, is_col>{ data, x, ln, x, y };
            else
                return MatrixGeneralIterator<const T, true, is_col>{ data, ln, y, x, y };
        }

        const_RMI<> cbegin() const noexcept { return item_citer(0, 0); }
        const_RMI<> cend() const noexcept { return item_citer(x, 0); }

        const_CMI<> col_cbegin() const noexcept { return item_citer<true>(0, 0); }
        const_CMI<> col_cend() const noexcept { return item_citer<true>(0, y); }
    };

    template<typename T, typename Traits = MatrixTraits<T>>
    using LazyMatrix = Matrix<T, true, Traits>;
}
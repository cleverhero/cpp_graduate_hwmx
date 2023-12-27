#pragma once

#include <iostream>
#include <cassert>
#include <numeric>
#include <vector>
#include <ranges>
#include <optional>

#include "hwmx.h"
#include "elements_buffer.h"
#include "matrix_line.h"
#include "iterators.h"


namespace ranges = std::ranges;


namespace hwmx {
    template<typename T>
    class IBaseMatrix {
    public:
        // const

        virtual size_t cols() const noexcept = 0;
        virtual size_t rows() const noexcept = 0;

        virtual T get_value(size_t ix, size_t iy) const = 0;
        virtual const T& const_ref_value(size_t ix, size_t iy) const = 0;

        virtual const Row<const T> operator[](size_t ix) const = 0;

        virtual const Col<const T> col(size_t iy) const = 0;
        virtual const Row<const T> row(size_t ix) const = 0;

        // mutable

        virtual Row<T> operator[](size_t ix) = 0;

        virtual Col<T> col(size_t iy) = 0;
        virtual Row<T> row(size_t ix) = 0;

        virtual T& ref_value(size_t ix, size_t iy) = 0;

        virtual void set_value(size_t ix, size_t iy, T value) = 0;

        // dtor

        virtual ~IBaseMatrix() {};
    };


    template<typename T, bool is_lazy = false>
    class Matrix final: 
        private ElementsBuf_<T, is_lazy>,
        public IBaseMatrix<T>
    {
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

        Matrix(Matrix&& rhs) : 
            ElementsBuf_<T, is_lazy>(std::move(rhs)),
            x(rhs.x), y(rhs.y) {}

        Matrix& operator=(Matrix&& rhs) {
            swap(rhs);
            x = rhs.x;
            y = rhs.y;
            return *this;
        }

        template<typename IT>
        Matrix(size_t x, size_t y, const IT& first):
            x(x), y(y), ElementsBuf_<T, is_lazy>(x*y, first) {}

        template<double_like U = T>
        static Matrix<U, is_lazy> eye(size_t n) {
            Matrix<U, is_lazy> m{ n, n };
            for (int i = 0; i < n; i++)
                m.set_value(i, i, 1);

            return m;
        }

        template<double_like U = T>
        requires std::convertible_to<size_t, U>
        static Matrix<U, is_lazy> iota(size_t n) {
            auto elements = ranges::iota_view<size_t, size_t>(0, n * n);

            return Matrix<U, is_lazy>{
                n, n, elements.begin()
            };
        }


        // const

        size_t cols() const noexcept { 
            return y; 
        }
        size_t rows() const noexcept { 
            return x;
        }

        T get_value(size_t ix, size_t iy) const {
            return data[ix * y + iy];
        }

        const T& const_ref_value(size_t ix, size_t iy) const {
            return data[ix * y + iy];
        }

        const Col<const T> col(size_t iy) const { return Col<const T>{ data, iy, x, y }; }
        const Row<const T> row(size_t ix) const { return Row<const T>{ data, ix, x, y }; }

        const Row<const T> operator[](size_t ix) const { return row(ix); }


        // mutable

        Col<T> col(size_t iy) { 
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return Col<T>{ data, iy, x, y };
        }

        Row<T> row(size_t ix) { 
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return Row<T>{ data, ix, x, y };
        }

        Row<T> operator[](size_t ix) { 
            if constexpr (is_lazy)
                ElementsBuf_<T, is_lazy>::cow();

            return row(ix); 
        }

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


    template<typename T>
    LazyMatrix<T> operator*(const IBaseMatrix<T>& lhs, const IBaseMatrix<T>& rhs) {
        LazyMatrix<T> result{lhs.rows(), rhs.cols()};

        for (int i = 0; i < lhs.rows(); i++)
            for (int j = 0; j < rhs.cols(); j++) {
                auto row = lhs.row(i);
                auto col = rhs.col(j);

                result.set_value(i, j, row * col);
            }

        return result;
    }
}
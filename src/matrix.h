#pragma once

#include <iostream>
#include <cassert>
#include <numeric>
#include <vector>
#include <ranges>

#include "elements_buffer.h"
#include "vector.h"
#include "iterators.h"
#include "matrix_line.h"


namespace ranges = std::ranges;
namespace views = std::views;


namespace hwmx {
    template<typename T>
    class MatrixTraits {
    public:
        static double to_double(T value) noexcept {
            return static_cast<double>(value);
        }
    };


    template<typename T, typename Traits> class Matrix;

    size_t gaussian_elimination(Matrix<double, MatrixTraits<double>>& matrix);


    template<typename T, typename Traits = MatrixTraits<T>>
    class Matrix : private ElementsBuf<T> {
        template<bool is_one_line = false>
        using RMI = RowMajorIterator<T, is_one_line>;

        template<bool is_one_line = false>
        using CMI = ColMajorIterator<T, is_one_line>;

        template<bool is_col_major = false>
        using II = MatrixGeneralIterator<T, false, is_col_major>;

        size_t x, y;

        using ElementsBuf<T>::data;
        using ElementsBuf<T>::size;

        using ElementsBuf<T>::swap;

    public:
        Matrix(size_t x, size_t y, T val = T{}):
            x(x), y(y), ElementsBuf<T>(x*y, val) {}

        Matrix(const Matrix& rhs) : x(rhs.x), y(rhs.y), ElementsBuf<T>(rhs) {}

        Matrix& operator=(const Matrix& rhs) {
            Matrix tmp{ rhs };

            std::swap(*this, tmp);
            return *this;
        }

        Matrix(Matrix&& rhs) : ElementsBuf<T>(std::move(rhs)) {}

        Matrix& operator=(Matrix&& rhs) {
            swap(rhs);
            return *this;
        }

        template<typename IT>
        Matrix(size_t x, size_t y, const IT& first, const IT& second):
            x(x), y(y), ElementsBuf<T>(x*y, first, second) {}

        Row<T> operator[](size_t ix) const { return row(ix); }

        void set_value(size_t ix, size_t iy, T value) noexcept {
            data[ix * y + iy] = value;
        }

        T get_value(size_t ix, size_t iy) const noexcept {
            return data[ix * y + iy];
        }

        size_t cols() const noexcept { return y; }
        size_t rows() const noexcept { return x; }

        void swap_rows(size_t ix1, size_t ix2) {
            std::swap_ranges(
                data + ix1 * y,
                data + (ix1 + 1) * y,
                data + ix2 * y
            );
        }

        double det() {
            assert(x == y);

            Matrix<double> m{ x, y };
            std::transform(
                begin(), end(), m.begin(),
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

        static Matrix eye(size_t n) {
            Matrix m{ n, n };
            auto to_n_view = views::iota((size_t)0, n);
            auto set_one = [&m](size_t i) { m.set_value(i, i, 1); };
            ranges::for_each(to_n_view, set_one);

            return m;
        }

        static Matrix iota(size_t n) {
            auto elements = views::iota((size_t)0, (n * n));
            return Matrix{ n, n, elements.begin(), elements.end(),  };
        }

        void print() const {
            for (size_t i = 0; i < x; i++) {
                row(i).print();
                std::cout << std::endl;
            }
        }

        Col<T> col(size_t iy) const noexcept { return Col<T>{ data, iy, x, y }; }
        Row<T> row(size_t ix) const noexcept { return Row<T>{ data, ix, x, y }; }

        template<bool is_col_major = false>
        II<is_col_major> item_iter(size_t ix, size_t iy) const noexcept {
            return II<is_col_major>{ data, ix, iy, x, y };
        }

        RMI<> begin() const noexcept { return item_iter(0, 0); }
        RMI<> end() const noexcept { return item_iter(x, 0); }

        CMI<> col_begin() const noexcept { return item_iter<true>(0, 0); }
        CMI<> col_end() const noexcept { return item_iter<true>(0, y); }
    };
}
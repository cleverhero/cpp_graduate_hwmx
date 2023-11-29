#pragma once

#include <algorithm>


namespace hwmx {
    template<typename T, bool is_lazy, typename Traits> class Matrix;
    template<typename T, bool is_lazy> class Vector;

    // MatrixLine -- proxy col or row of matrix that don't own resources.
    // It's can convert to Vector via copying resources and
    // do inplace math operations.
    template<typename T, bool is_col = false>
    class MatrixLine {
        T* data;
        size_t ln;
        size_t x, y;

    public:
        MatrixLine(T* data, size_t ln, size_t x, size_t y):
            data(data), ln(ln), x(x), y(y) {}
        MatrixLine(const MatrixLine& rhs) = delete;
        MatrixLine(MatrixLine&& rhs) :
            data(rhs.data), ln(rhs.ln), x(rhs.x), y(rhs.y) {
            rhs.data = nullptr;
        };


        // const

        size_t size() const noexcept {
            if constexpr (is_col)
                return x;
            else
                return y;
        }

        T get_value(size_t ind) const {
            if constexpr(is_col)
                return data[ind * y + ln];
            else
                return data[ln * y + ind];
        }

        const T& const_ref_value(size_t ind) const {
            if constexpr(is_col)
                return data[ind * y + ln];
            else
                return data[ln * y + ind];
        }

        void print() const noexcept {
            for (size_t i = 0; i < size(); i++)
                std::cout << const_ref_value(i) << ' ';
        }

        Vector<T> operator*(const T& rhs) const {
            Vector<T> res = *this;

            res *= rhs;
            return res;
        }

        operator Vector<T>() const {
            return Vector<T>{ size(), cbegin() };
        }


        // mutable

        T& operator[](size_t ind) {
            if constexpr(is_col)
                return data[ind * y + ln];
            else
                return data[ln * y + ind];
        }

        MatrixLine& operator-=(const Vector<T>& rhs) {
            auto it1 = begin();
            auto it2 = rhs.cbegin();
            auto ite = end();

            while (it1 != ite) {
                *it1 -= *it2;
                it1++; it2++;
            }

            return *this;
        }


        // iters

        MatrixGeneralIterator<T, true, is_col> begin() noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, 0, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, 0, x, y };
        }

        MatrixGeneralIterator<T, true, is_col> end() noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, x, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, y, x, y };
        }


        // const_iters

        MatrixGeneralIterator<const T, true, is_col> cbegin() const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<const T, true, is_col>{ data, 0, ln, x, y };
            else
                return MatrixGeneralIterator<const T, true, is_col>{ data, ln, 0, x, y };
        }

        MatrixGeneralIterator<const T, true, is_col> cend() const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<const T, true, is_col>{ data, x, ln, x, y };
            else
                return MatrixGeneralIterator<const T, true, is_col>{ data, ln, y, x, y };
        }
    };

    template<typename M>
    using Row = MatrixLine<M, false>;

    template<typename M>
    using Col = MatrixLine<M, true>;

    template<typename T, bool is_col1, bool is_col2>
    auto operator*(const MatrixLine<T, is_col1>& lhs, const MatrixLine<T, is_col2>& rhs) {
        return std::transform_reduce(
            lhs.cbegin(), lhs.cend(), rhs.cbegin(), T{}, std::plus{},
            [](auto lv, auto rv) { return lv * rv; }
        );
    }
}
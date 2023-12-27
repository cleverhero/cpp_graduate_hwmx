#pragma once

#include "hwmx.h"
#include "elements_buffer.h"


namespace hwmx {
    // Vector is necessary to locate MatrixLine in own memory.
    template<typename T, bool is_lazy = false>
    class Vector: private ElementsBuf_<T, is_lazy> {
        using II = RowMajorIterator<T, true>;
        using const_II = RowMajorIterator<const T, true>;

        using ElementsBuf_<T, is_lazy>::data;
        using ElementsBuf_<T, is_lazy>::size;

        using ElementsBuf_<T, is_lazy>::swap;

    public:
        Vector(size_t size, T val = T{}) : ElementsBuf_<T, is_lazy>(size, val) {}

        template<typename IT>
        Vector(size_t size, const IT& first):
            ElementsBuf_<T, is_lazy>(size, first) {}

        Vector& operator=(const Vector& rhs) {
            Vector tmp{ rhs };

            std::swap(*this, tmp);
            return *this;
        }

        Vector(const Vector& rhs) : ElementsBuf_<T, is_lazy>(rhs) {}

        Vector(Vector&& rhs) noexcept : ElementsBuf_<T, is_lazy>(std::move(rhs)) {}

        Vector& operator=(Vector&& rhs) noexcept {
            swap(rhs);
            return *this;
        }        


        // const

        T get_value(size_t ind) const { return data[ind]; }

        void print() const noexcept {
            for (size_t i = 0; i < size; i++)
                std::cout << get_value(i) << ' ';
        }

        template<typename U>
        requires requires(T t, U u) {
            ( t *= u );
        }
        Vector<T> operator*(U&& rhs) const {
            Vector<T> res{ *this };

            res *= rhs;
            return res;
        }


        //mutable

        T& operator[](size_t ind) { return data[ind]; }

        Vector& operator*=(const T& rhs) noexcept {
            for (auto& el : *this)
                el *= rhs;

            return *this;
        }


        // iters

        II begin() noexcept { return II{ data, 0, 0, 0, size }; }
        II end() noexcept { return II{ data, 0, size, 0, size }; }


        // const_iters

        const_II cbegin() const noexcept { return const_II{ data, 0, 0, 0, size }; }
        const_II cend() const noexcept { return const_II{ data, 0, size, 0, size }; }
    };
}
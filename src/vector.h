#pragma once

#include "elements_buffer.h"
#include "iterators.h"


namespace hwmx {
    template<typename T, bool is_col> class MatrixLine;


    // Vector is necessary to locate MatrixLine in own memory.
    template<typename T>
    class Vector: private ElementsBuf<T> {
        using II = RowMajorIterator<T, true>;

        using ElementsBuf<T>::data;
        using ElementsBuf<T>::size;

        using ElementsBuf<T>::swap;

    public:
        Vector(size_t size, T val = T{}) : ElementsBuf<T>(size, val) {}

        Vector(const Vector& rhs) : ElementsBuf<T>(rhs) {}

        Vector& operator=(const Vector& rhs) {
            Vector tmp{ rhs };

            std::swap(*this, tmp);
            return *this;
        }

        Vector(Vector&& rhs) noexcept : ElementsBuf<T>(std::move(rhs)) {}

        Vector& operator=(Vector&& rhs) noexcept {
            swap(rhs);
            return *this;
        }

        template<bool is_col>
        Vector(const MatrixLine<T, is_col>& rhs): 
            ElementsBuf<T>(rhs.size(), rhs.begin(), rhs.end()) {}

        template<typename IT>
        Vector(size_t size, const IT& first, const IT& second):
            ElementsBuf<T>(size, first, second) {}

        T& operator[](size_t ind) const noexcept {
            return data[ind];
        }

        void print() const {
            for (size_t i = 0; i < size; i++)
                std::cout << data[i] << ' ';
        }

        II begin() const noexcept { return II{ data, 0, 0, 0, size }; }
        II end() const noexcept { return II{ data, 0, size, 0, size }; }

        Vector& operator*=(const T& rhs) noexcept {
            for (auto& el : *this)
                el *= rhs;

            return *this;
        }
    };
}
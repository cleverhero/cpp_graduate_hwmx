#pragma once

#include <iterator>
#include <stddef.h>


namespace hwmx {
    // Experiment with CRTP pattern.
    // I assumed that I would need more that one general iterator class,
    // Then MixShiftable would have helped me to avoid repetitive code.
    // But MixShiftable is useful to incapsulate boilerplate code, i think.
    template<typename Derived>
    class MixShiftable {
        using difference_type = std::ptrdiff_t;

    public:
        Derived& operator+=(const difference_type& k) {
            Derived* self = static_cast<Derived*>(this);
            self->shift(k);
            return *self;
        }

        Derived& operator-=(const difference_type& k) {
            Derived* self = static_cast<Derived*>(this);
            self->shift(-k);
            return *self;
        }

        Derived& operator++() {
            Derived* self = static_cast<Derived*>(this);
            self->shift(1);
            return *self;
        }

        Derived& operator--() {
            Derived* self = static_cast<Derived*>(this);
            self->shift(-1);
            return *self;
        }

        Derived operator++(int) {
            Derived* self = static_cast<Derived*>(this);
            Derived tmp(*self);
            self->shift(1);
            return tmp;
        }

        Derived operator--(int) {
            Derived* self = static_cast<Derived*>(this);
            Derived tmp(*self);
            self->shift(-1);
            return tmp;
        }

        Derived operator+(const difference_type& k) const {
            const Derived* self = static_cast<const Derived*>(this);
            Derived derived_copy = Derived(*self);
            derived_copy.shift(k);
            return derived_copy;
        }

        Derived operator-(const difference_type& k) const {
            const Derived* self = static_cast<const Derived*>(this);
            Derived derived_copy = Derived(*self);
            derived_copy.shift(-k);
            return derived_copy;
        }
    };


    // Class to iterate over matrix.
    // Options is_one_line and is_col_major are needed to slightly change the logic of iteration.
    // E.g. MatrixGeneralIterator<T, true, true> -- iterator to iterate over one certain column of matrix.
    template<typename T, bool is_one_line = false, bool is_col_major = false>
    class MatrixGeneralIterator 
        : public MixShiftable<MatrixGeneralIterator<T, is_one_line, is_col_major>>
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

    private:
        pointer data;
        size_t x, y;
        size_t ix, iy;

    private:
        void shift_for_col(const difference_type& kx) noexcept {
            if constexpr (!is_one_line) {
                auto ix_tmp = ix;
                ix = (ix_tmp + kx) % x;
                iy += (ix_tmp + kx) / x;
            }
            else {
                ix += kx;
            }
        }

        void shift_for_row(const difference_type& ky) noexcept {
            if constexpr (!is_one_line) {
                auto iy_tmp = iy;
                iy = (iy_tmp + ky) % y;
                ix += (iy_tmp + ky) / y;
            }
            else {
                iy += ky;
            }
        }

        difference_type diff_for_col(const MatrixGeneralIterator& rhs) const noexcept {
            return (iy - rhs.iy) * x + (ix - rhs.ix);
        }

        difference_type diff_for_row(const MatrixGeneralIterator& rhs) const noexcept {
            return (ix - rhs.ix) * y + (iy - rhs.iy);
        }

    public:
        MatrixGeneralIterator(pointer data, size_t ix, size_t iy, size_t x, size_t y)
            : data(data), x(x), y(y), ix(ix), iy(iy) {}
        MatrixGeneralIterator(const MatrixGeneralIterator& rhs) 
            : data(rhs.data), x(rhs.x), y(rhs.y), ix(rhs.ix), iy(rhs.iy) {}

        void shift(const difference_type& k) noexcept {
            if constexpr (!is_col_major)
                shift_for_row(k);
            else
                shift_for_col(k);
        }
        difference_type operator-(const MatrixGeneralIterator& rhs) const noexcept {
            if constexpr (!is_col_major)
                return diff_for_row(rhs);
            else
                return diff_for_col(rhs);
        }

    public:
        reference operator*() { return *(data + ix * y + iy); }
        reference operator[](const difference_type& k) { return *(*this + k); }
        pointer operator->() { return data + ix * y + iy; }
        auto operator<=>(const MatrixGeneralIterator& rhs) const = default;
        bool operator==(const MatrixGeneralIterator& rhs) const {
            return (
                ix == rhs.ix &&
                iy == rhs.iy &&
                data == rhs.data &&
                x == rhs.x &&
                y == rhs.y
            );
        };
    };
}
#pragma once


namespace hwmx {
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

        size_t size() const noexcept {
            if constexpr (is_col)
                return x;
            else
                return y;
        }

        T& operator[](size_t ind) const noexcept {
            if constexpr(is_col)
                return data[ind * y + ln];
            else
                return data[ln * y + ind];
        }

        MatrixGeneralIterator<T, true, is_col> begin() const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, 0, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, 0, x, y };
        }

        MatrixGeneralIterator<T, true, is_col> end() const noexcept {
            if constexpr (is_col)
                return MatrixGeneralIterator<T, true, is_col>{ data, x, ln, x, y };
            else
                return MatrixGeneralIterator<T, true, is_col>{ data, ln, y, x, y };
        }

        void print() const {
            for (size_t i = 0; i < size(); i++)
                std::cout << (*this)[i] << ' ';
        }

        Vector<T> operator*(const T& rhs) const {
            Vector<T> res{ *this };

            res *= rhs;
            return res;
        }

        MatrixLine& operator-=(const Vector<T>& rhs) {
            auto it1 = begin();
            auto it2 = rhs.begin();
            auto ite = end();

            while (it1 != ite) {
                *it1 -= *it2;
                it1++; it2++;
            }

            return *this;
        }
    };


    template<typename T>
    using Row = MatrixLine<T, false>;

    template<typename T>
    using Col = MatrixLine<T, true>;
}
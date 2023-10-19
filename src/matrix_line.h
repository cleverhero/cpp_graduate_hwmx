#pragma once


namespace hwmx {
    template<typename T, bool is_lazy, typename Traits> class Matrix;
    template<typename T, bool is_lazy> class Vector;

    // MatrixLine -- proxy col or row of matrix that don't own resources.
    // It's can convert to Vector via copying resources and
    // do inplace math operations.
    template<typename M, bool is_col = false>
    class MatrixLine {
        using T = typename M::elements_type;

        M* matrix_ptr;
        size_t ln;

    public:
        MatrixLine(M* matrix_ptr, size_t ln): matrix_ptr(matrix_ptr), ln(ln) {}
        MatrixLine(const MatrixLine& rhs) = delete;
        MatrixLine(MatrixLine&& rhs): matrix_ptr(rhs.matrix_ptr), ln(rhs.ln) {
            rhs.matrix_ptr = nullptr;
        };


        // const

        size_t size() const noexcept {
            if constexpr (is_col)
                return matrix_ptr->rows();
            else
                return matrix_ptr->cols();
        }

        T get_value(size_t ind) const {
            if constexpr(is_col)
                return matrix_ptr->get_value(ind, ln);
            else
                return matrix_ptr->get_value(ln, ind);
        }

        const T& const_ref_value(size_t ind) const {
            if constexpr(is_col)
                return matrix_ptr->const_ref_value(ind, ln);
            else
                return matrix_ptr->const_ref_value(ln, ind);
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
            return Vector<T>{ size(), cbegin(), cend() };
        }


        // mutable

        T& operator[](size_t ind) {
            if constexpr(is_col)
                return matrix_ptr->ref_value(ind, ln);
            else
                return matrix_ptr->ref_value(ln, ind);
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
            return matrix_ptr->template line_begin<is_col>(ln);
        }

        MatrixGeneralIterator<T, true, is_col> end() noexcept {
            return matrix_ptr->template line_end<is_col>(ln);
        }


        // const_iters

        MatrixGeneralIterator<const T, true, is_col> cbegin() const noexcept {
            return matrix_ptr->template line_cbegin<is_col>(ln);
        }

        MatrixGeneralIterator<const T, true, is_col> cend() const noexcept {
            return matrix_ptr->template line_cend<is_col>(ln);
        }
    };


    template<typename M>
    using Row = MatrixLine<M, false>;

    template<typename M>
    using Col = MatrixLine<M, true>;
}
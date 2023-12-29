#pragma once

#include <algorithm>

#include "hwmx.h"
#include "matrix.h"
#include "vector.h"
#include "matrix_line.h"


namespace hwmx {
    template<double_like T, bool is_lazy>
    Vector<double> solve(const Matrix<T, is_lazy>& matrix) {
        if (matrix.cols() != matrix.rows() + 1)
            throw non_linear_equation_system_error{};

        Matrix<double> m{ matrix.rows(), matrix.cols(), matrix.cbegin() };
        gaussian_elimination(m, true);

        for (int i = m.rows() - 1; i >= 0; i--) {
            m[i] /= +m[i][i];
            for (int j = 0; j < i; j++)
                m[j] -= m[i] * m[j][i];
        }
        
        return m.col(m.cols() - 1);
    }

    template<double_like T, bool is_lazy>
    double det(const Matrix<T, is_lazy>& matrix) {
        if (matrix.cols() != matrix.rows())
            throw non_square_matrix_error{};

        Matrix<double> m{ matrix.rows(), matrix.cols(), matrix.cbegin() };
        size_t count_swaps = gaussian_elimination(m);
        
        std::vector<double> diag(matrix.rows());
        for (size_t i = 0; i < matrix.rows(); i++)
            diag[i] = m[i][i];
        
        return std::reduce(
            diag.begin(), diag.end(),
            count_swaps % 2 ? -1.0 : 1.0,
            std::multiplies<double>()
        );
    }

    template<printable T, bool is_lazy>
    void print(const Matrix<T, is_lazy>& matrix, std::ostream& out) noexcept {
        for (size_t i = 0; i < matrix.rows(); i++)
            print(matrix.row(i));
    }

    template<printable T, bool is_lazy>
    void print(const MatrixLine<T, is_lazy>& line, std::ostream& out) noexcept {
        for (auto it = line.cbegin(); it != line.cend(); ++it)
            out << *it << ' ';
        out << std::endl;
    }

    template<printable T, bool is_lazy>
    void print(const Vector<T, is_lazy>& vector, std::ostream& out) noexcept {
        for (auto it = vector.cbegin(); it != vector.cend(); ++it)
            out << *it << ' ';
        out << std::endl;
    }
}
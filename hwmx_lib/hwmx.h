#pragma once

#include <iostream>
#include <concepts>


namespace hwmx {
    // concepts

    template<typename T> 
    concept double_like = std::convertible_to<T, double>;

    template<typename T>
    concept printable = requires(T t, std::ostream& out) { 
        { out << t } -> std::same_as<std::ostream&>;
    };


    // exceptions

    class non_linear_equation_system_error : public std::runtime_error {
    public:
        non_linear_equation_system_error() throw()
            : std::runtime_error("Matrix is not linear equation system.") {};
    };

    class non_square_matrix_error : public std::runtime_error {
    public:
        non_square_matrix_error() throw()
            : std::runtime_error("Matrix is non square.") {};
    };

    class column_of_zeros_error: public std::runtime_error {
    private:
        size_t column_ind;

    public:
        column_of_zeros_error(size_t ind) throw(): 
            std::runtime_error("Column " + std::to_string(ind) + " is consist of zeros."),
            column_ind(ind) {};

        size_t get_column_ind() {
            return column_ind;
        }
    };


    // types
    
    template<typename T, bool is_lazy> class Matrix;

    template<typename T>
    using LazyMatrix = Matrix<T, true>;

    template<typename T, bool is_lazy> class Vector;
    
    template<typename T, bool is_col> class MatrixLine;

    template<typename M> using Row = MatrixLine<M, false>;
    template<typename M> using Col = MatrixLine<M, true>;

    template<typename T, bool is_one_line, bool is_col_major> class MatrixGeneralIterator;

    template<typename T, bool is_one_line>
    using RowMajorIterator = MatrixGeneralIterator<T, is_one_line, false>;

    template<typename T, bool is_one_line>
    using ColMajorIterator = MatrixGeneralIterator<T, is_one_line, true>;

    template<typename T> class MatrixChain;

    // Algorithms

    size_t gaussian_elimination(Matrix<double, false>& matrix, bool throw_column_of_zeros = false);

    template<double_like T, bool is_lazy>
    Vector<double, false> solve(const Matrix<T, is_lazy>& matrix);

    template<double_like T, bool is_lazy>
    double det(const Matrix<T, is_lazy>& matrix);

    template<printable T, bool is_lazy>
    void print(const Matrix<T, is_lazy>& matrix, std::ostream& out = std::cout) noexcept;

    template<printable T, bool is_lazy>
    void print(const MatrixLine<T, is_lazy>& line, std::ostream& out = std::cout) noexcept;

    template<printable T, bool is_lazy>
    void print(const Vector<T, is_lazy>& vector, std::ostream& out = std::cout) noexcept;
}
#include <math.h>
#include <algorithm>

#include "hwmx.h"
#include "matrix.h"


namespace hwmx {
    size_t gaussian_elimination(Matrix<double>& matrix, bool throw_column_of_zeros) {
        const double EPS = std::pow(10, -8);
        auto abs_pred = [](double a, double b) { return std::abs(a) < std::abs(b); };
        size_t count_swaps = 0;
        for (int i = 0; i < matrix.rows(); ++i) {
            auto col_i = matrix.col(i);
            auto max_el = std::max_element(col_i.begin() + i, col_i.end(), abs_pred);
            auto k = std::distance(col_i.begin(), max_el);

            if (std::abs(*max_el) < EPS)
                if (throw_column_of_zeros)
                    throw column_of_zeros_error(i);
                else
                    continue;

            if (i != k) {
                matrix.swap_rows(i, k);
                count_swaps++;
            }

            for (int j = i + 1; j < matrix.rows(); j++) {
                if (std::abs(matrix[j][i]) < EPS) continue;

                double multiplier = matrix[j][i] / matrix[i][i];
                matrix[j] -= matrix[i] * multiplier;
            }
        }

        return count_swaps;
    };
}
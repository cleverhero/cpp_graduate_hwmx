#include <vector>
#include <algorithm>

#include "matrix.h"


namespace hwmx {
    template<typename T>
    class MatrixChain final {
    private:
        struct OrderInfoCell {
            size_t mul_count;
            size_t sep;

            OrderInfoCell(size_t mc = 0, size_t s = 0): mul_count(mc), sep(s) {}
        };
        using OrderInfo = std::vector<std::vector<OrderInfoCell>>;
        using MatrixT = IBaseMatrix<T>;

    private:
        size_t cols(size_t ind) const { return stack[ind]->cols(); }
        size_t rows(size_t ind) const { return stack[ind]->rows(); }

        void make_order_info() {
            order_info = OrderInfo(stack.size());
            for (auto& dp_line : order_info.value())
                dp_line.resize(stack.size());

            for (int i = stack.size() - 1; i >= 0; i--)
                for (int j = i + 1; j < stack.size(); j++) {                        
                    auto get_value = [&, this](size_t k) -> size_t {
                        return 
                            order_info.value()[i][k].mul_count + 
                            order_info.value()[k + 1][j].mul_count + 
                            rows(i) * cols(k) * cols(j);
                    };

                    size_t k = *ranges::min_element(
                        views::iota(i, j), std::less{}, get_value
                    );

                    order_info.value()[i][j] = OrderInfoCell{ get_value(k), k };
                }
        }

        void print_order(size_t l, size_t r) {
            if (l == r) {
                std::cout << l;
                return;
            }

            size_t k = order_info.value()[l][r].sep;

            std::cout << '(';
            print_order(l, k);
            std::cout << " * ";
            print_order(k + 1, r);
            std::cout << ')';
        }

        LazyMatrix<T> optimal_multiply(size_t l, size_t r) {
            if (r - l == 1)
                return (*stack[l]) * (*stack[r]);

            size_t k = order_info.value()[l][r].sep;

            if (l == k)
                return *(stack[l]) * optimal_multiply(k + 1, r);
            
            if (k + 1 == r)
                return optimal_multiply(l, k) * (*stack[r]);
            
            return optimal_multiply(l, k) * optimal_multiply(k + 1, r);
        }
    public:
        MatrixChain(): stack() {}

        MatrixChain(std::initializer_list<const MatrixT*>&& il):
            stack(
                std::forward<std::initializer_list<const MatrixT*>>(il)
            )
        {}

        void add(const MatrixT* p) {
            order_info.reset();
            stack.push_back(p);
        }

        void print_order() {
            if (!order_info) make_order_info();

            print_order(0, stack.size() - 1);
            std::cout << std::endl;
        }

        LazyMatrix<T> optimal_multiply() {
            if (!order_info) make_order_info();

            return optimal_multiply(0, stack.size() - 1);
        }

        LazyMatrix<T> sequential_multiply() const {
            return std::accumulate(
                stack.begin(), stack.end(),
                LazyMatrix<T>::eye(rows(0)),
                [](auto lhs, auto rhs) { return lhs * (*rhs); }
            );
        }
    
    private:
        std::vector<const MatrixT*> stack;
        std::optional<OrderInfo> order_info = std::nullopt;
    };
}
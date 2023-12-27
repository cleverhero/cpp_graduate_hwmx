#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <optional>
#include <queue>


#include "hwmx.h"
#include "matrix.h"
#include "algorithm.h"


namespace {
using InputLine = std::tuple<size_t, size_t, double, double>;
using index_t = size_t;
using intensity_t = double;
using Edge = std::pair<index_t, index_t>;
using Loop = std::vector<Edge>;

struct EdgeInfo {
    double resistense;
    double voltage;
};


class Graph final {
private:
    int size;
    hwmx::Matrix<bool> incidence_matrix;

    bool find_loop_(
        index_t node,
        std::vector<int>& colors, 
        std::vector<int>& parents,
        index_t& start_loop, index_t& finish_loop
    ) const noexcept {
        colors[node] = 1;
        auto& edges = incidence_matrix[node];

        for (index_t i = 0; i < size; i++) {
            if (!edges[i]) continue;
            if (parents[node] == i) continue;

            if (colors[i] == 0) {
                parents[i] = node;
                if (find_loop_(i, colors, parents, start_loop, finish_loop))
                    return true;
            }
    
            if (colors[i] == 1) {
                start_loop = i;
                finish_loop = node;
                return true;
            }
        }

        colors[node] = 2;
        return false;
    }

public:
    Graph(int n): size(n), incidence_matrix(n, n) {}

    void add_edge(index_t l, index_t r) noexcept {
        incidence_matrix.set_value(l, r, true);
        incidence_matrix.set_value(r, l, true);
    }

    bool check_edge(index_t l, index_t r) const noexcept {
        return incidence_matrix[l][r];
    }

    void remove_edge(index_t l, index_t r) noexcept {
        incidence_matrix.set_value(l, r, false);
        incidence_matrix.set_value(r, l, false);
    }

    Loop find_loop() const noexcept {
        std::vector<int> colors(size, 0);
        std::vector<int> parents(size, -1);
        index_t start_loop;
        index_t finish_loop;

        find_loop_(0, colors, parents, start_loop, finish_loop);

        index_t curr = finish_loop;
        Loop loop;
        while (curr != start_loop) {
            loop.push_back({curr, parents[curr]});
            curr = parents[curr];
        }
        loop.push_back({curr, finish_loop});

        return loop;
    }

    Graph get_spanning_tree() const noexcept {
        Graph spanning_tree(size);

        std::queue<index_t> q;
        std::vector<bool> used(size);
        
        q.push(0); used[0] = true;
        while(!q.empty()) {
            index_t curr = q.front(); q.pop();
            auto& edges = incidence_matrix[curr];

            for (index_t i = 0; i < size; i++) {
                if (!edges[i]) continue;

                if (!used[i]) {
                    spanning_tree.add_edge(curr, i);

                    q.push(i); used[i] = true;
                }
            }
        }

        return spanning_tree;
    }

    std::vector<Edge> edges() const noexcept {
        std::vector<Edge> res;

        for (index_t i = 0; i < size; i++)
            for (index_t j = i; j < size; j++)
                if (incidence_matrix[i][j]) 
                    res.push_back({i, j});

        return res;
    }

    std::vector<Loop> get_independent_loops() const noexcept {
        std::vector<Loop> loops;

        Graph spanning_tree = get_spanning_tree();
        
        for (index_t i = 0; i < size; i++) {
            auto& edges = incidence_matrix[i];

            for (index_t j = i; j < size; j++) {
                if (!edges[j]) continue;
                if (spanning_tree.check_edge(i, j)) continue;
                
                spanning_tree.add_edge(i, j);

                loops.push_back( spanning_tree.find_loop() );

                spanning_tree.remove_edge(i, j);
            }
        }

        return loops;
    }
};


int is_edge_in_loop(Edge edge, Loop& loop) {
    if (std::find(loop.begin(), loop.end(), edge) != loop.end())
        return 1;

    auto rev_edge = std::make_pair(edge.second, edge.first);
    if (std::find(loop.begin(), loop.end(), rev_edge) != loop.end())
        return -1;

    return 0;
}


class ElectricCircuit final {
private:
    using EdgeInfoOpt = std::optional<EdgeInfo>;
    using intensity_opt_t = std::optional<intensity_t>;

    Graph graph;
    hwmx::Matrix<EdgeInfoOpt> circuit_info;

public:
    ElectricCircuit(int n, const std::vector<InputLine>& input_data) :
        graph(n),
        circuit_info(n, n)
    {
        for (auto& [n1, n2, r, v] : input_data) {
            graph.add_edge(n1, n2);

            circuit_info.set_value(n1, n2, EdgeInfo(r, v));
            circuit_info.set_value(n2, n1, EdgeInfo(r, -v));
        }
    }

    hwmx::Matrix<intensity_opt_t> get_intensity_matrix() {
        std::vector<Loop> loops = graph.get_independent_loops();

        size_t n = loops.size();
        hwmx::Matrix<double> equations(n, n + 1);

        for (size_t i = 0; i < n; i++) {
            auto& loop = loops[i];

            for (auto edge : loop) {
                auto& edge_info = circuit_info[edge.first][edge.second].value();
                equations[i][i] += edge_info.resistense;
                equations[i][n] += edge_info.voltage;

                for (size_t j = i + 1; j < n; j++) {
                    auto& other_loop = loops[j];
                    int edge_in_loop_factor = is_edge_in_loop(edge, other_loop);
                    
                    equations[i][j] += edge_in_loop_factor * edge_info.resistense;
                    equations[j][i] += edge_in_loop_factor * edge_info.resistense;
                }
            }
        }

        auto solution = hwmx::solve(equations);

        hwmx::Matrix<intensity_opt_t> intensity_matrix{circuit_info.rows(), circuit_info.cols()};
        for (Edge edge : graph.edges()) {
            index_t row = edge.first, col = edge.second;

            intensity_matrix[row][col] = 0;
            intensity_matrix[col][row] = 0;

            for (int loop_i = 0; loop_i < n; loop_i++) {
                auto& loop = loops[loop_i];
                int edge_in_loop_factor = is_edge_in_loop(edge, loop);

                intensity_matrix[row][col].value() += edge_in_loop_factor * solution[loop_i];
                intensity_matrix[col][row].value() -= edge_in_loop_factor * solution[loop_i];
            }
        }

        return intensity_matrix;
    }
};

void prepare_input(std::string& line) {
    line.replace(line.find("--"), 2, "");
    line.replace(line.find(";"), 1, "");
    line.replace(line.find(","), 1, "");

    if (line.find("V") != std::string::npos)
        line.replace(line.find("V"), 1, "");
    else
        line += " 0";
}


std::ostream & operator<<(std::ostream & s, std::optional<double> el) {
    if (el.has_value())
        return s << el.value();

    return s << "n.o";
}
};

namespace hwmx {

void find_intensity_list(std::istream& in, std::ostream& out) {
    size_t n = 0;
    std::vector<InputLine> input_data;

    std::string line;
    while (std::getline(in, line)) {
        prepare_input(line);
        std::stringstream ss{line};

        size_t n1, n2; double r, v;
        ss >> n1 >> n2 >> r >> v;

        n = std::max(n, std::max(n1, n2));
        input_data.push_back({n1 - 1, n2 - 1, r, v});
    }

    ElectricCircuit ec(n, input_data);
    auto res = ec.get_intensity_matrix();

    for (InputLine line : input_data) {
        index_t lhs = std::get<0>(line), rhs = std::get<1>(line);
        out << lhs + 1 << " -- " << rhs + 1 << "; " << res[lhs][rhs] << " A" << std::endl;
    }
}

}
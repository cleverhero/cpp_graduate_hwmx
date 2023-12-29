#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <unordered_set>
#include <optional>
#include <queue>


#include "hwmx.h"
#include "matrix.h"
#include "algorithm.h"


namespace {
using InputLine = std::tuple<size_t, size_t, double, double>;
using intensity_t = double;

using node_id_t = size_t;
using Edge = std::pair<node_id_t, node_id_t>;
Edge empty_edge{0, 0};

using edge_id_t = int;
using Loop = std::vector<edge_id_t>;


struct EdgeInfo {
    double resistense;
    double voltage;
};


class EdgePool {
private:
    std::vector<Edge> edges;
    std::vector<EdgeInfo> edges_info;

public:
    EdgePool(): edges{empty_edge}, edges_info{{0, 0}} {}

    edge_id_t registrate(node_id_t l, node_id_t r, double resistense, double voltage) {
        edges.push_back({l, r});
        edges_info.push_back({resistense, voltage});

        return edges.size() - 1;
    }

    Edge get_edge(edge_id_t edge_id) {
        if (edge_id < 0) {
            Edge edge = edges[-edge_id];
            return {edge.second, edge.first};
        }

        return edges[edge_id];
    }

    EdgeInfo get_edge_info(edge_id_t edge_id) {
        if (edge_id < 0) {
            EdgeInfo edge_info = edges_info[-edge_id];
            return {edge_info.resistense, -edge_info.voltage};
        }

        return edges_info[edge_id];
    }

    size_t edges_number() {
        return edges.size() - 1;
    }
} edge_pool;


class Graph final {
private:
    int size;

    std::vector<std::unordered_set<edge_id_t>> edges_by_node;

    bool find_loop_(
        node_id_t node_id,
        std::vector<int>& colors, 
        std::vector<edge_id_t>& edge_to,
        node_id_t& start_loop, node_id_t& finish_loop
    ) const noexcept {
        colors[node_id] = 1;
        auto& edges = edges_by_node[node_id];

        for (edge_id_t edge_id : edges) {
            if (edge_to[node_id] == -edge_id) continue;

            Edge edge = edge_pool.get_edge(edge_id);
            node_id_t next_node_id = edge.second;

            if (colors[next_node_id] == 0) {
                edge_to[next_node_id] = edge_id;
                if (find_loop_(next_node_id, colors, edge_to, start_loop, finish_loop))
                    return true;
            }
    
            if (colors[next_node_id] == 1) {
                start_loop = next_node_id;
                finish_loop = node_id;

                edge_to[next_node_id] = edge_id;
                return true;
            }
        }

        colors[node_id] = 2;
        return false;
    }

public:
    Graph(int n):
        size(n),
        edges_by_node(n)
    {}

    void add_edge(edge_id_t edge_id) noexcept {
        auto [l, r] = edge_pool.get_edge(edge_id);

        edges_by_node[l].insert(edge_id);
        edges_by_node[r].insert(-edge_id);
    }

    bool check_edge(edge_id_t edge_id) const noexcept {
        auto [l, r] = edge_pool.get_edge(edge_id);

        return (edges_by_node[l].find(edge_id) != edges_by_node[l].end());
    }

    void remove_edge(edge_id_t edge_id) noexcept {
        auto [l, r] = edge_pool.get_edge(edge_id);

        edges_by_node[l].erase(edge_id);
        edges_by_node[r].erase(-edge_id);
    }

    Loop find_loop() const noexcept {
        std::vector<int> colors(size, 0);
        std::vector<edge_id_t> edge_to(size, 0);
        node_id_t start_loop;
        node_id_t finish_loop;

        for (node_id_t start_node = 0; start_node < size; start_node++)
            if (find_loop_(start_node, colors, edge_to, start_loop, finish_loop))
                break;

        node_id_t curr = finish_loop;
        Loop loop{-edge_to[start_loop]};
        while (curr != start_loop) {
            loop.push_back(-edge_to[curr]);

            edge_id_t edge_id = edge_to[curr];
            Edge edge = edge_pool.get_edge(edge_id);
            curr = edge.first;
        }

        return loop;
    }

    Graph get_spanning_tree() const noexcept {
        Graph spanning_tree{size};

        std::queue<node_id_t> q;
        std::vector<bool> used(size);
        
        for (node_id_t start_node = 0; start_node < size; start_node++) {
            if (used[start_node])
                continue;

            q.push(start_node); used[start_node] = true;
            while(!q.empty()) {
                node_id_t curr_node_id = q.front(); q.pop();
                auto& edges = edges_by_node[curr_node_id];

                for (edge_id_t edge_id : edges) {
                    Edge edge = edge_pool.get_edge(edge_id);
                    node_id_t next_node_id = edge.second;

                    if (!used[next_node_id]) {
                        spanning_tree.add_edge(edge_id);

                        q.push(next_node_id); used[next_node_id] = true;
                    }
                }
            }
        }
        
        return spanning_tree;
    }

    std::vector<Loop> get_independent_loops() const noexcept {
        std::vector<Loop> loops;

        Graph spanning_tree = get_spanning_tree();
        
        for (node_id_t node_id = 0; node_id < size; node_id++) {
            auto& edges = edges_by_node[node_id];

            for (edge_id_t edge_id : edges) {
                if (edge_id < 0) continue;
                Edge edge = edge_pool.get_edge(edge_id);
                node_id_t next_node_id = edge.second;

                if (spanning_tree.check_edge(edge_id)) continue;
                
                spanning_tree.add_edge(edge_id);

                loops.push_back( spanning_tree.find_loop() );

                spanning_tree.remove_edge(edge_id);
            }
        }

        return loops;
    }
};


int is_edge_in_loop(edge_id_t edge_id, Loop& loop) {
    if (std::find(loop.begin(), loop.end(), edge_id) != loop.end())
        return 1;

    if (std::find(loop.begin(), loop.end(), -edge_id) != loop.end())
        return -1;

    return 0;
}

std::ostream& operator<<(std::ostream& out, const Loop& loop) {
    for (edge_id_t edge_id : loop) {
        Edge edge = edge_pool.get_edge(edge_id);
        out << '(' << edge.first + 1 << " " << edge.second + 1 << ") ";
    }

    return out;
}

class bad_circuit_error: public std::runtime_error {
private:
    Loop bad_loop;

public:
    bad_circuit_error(const Loop& bad_loop_) throw():
        std::runtime_error("Bad circuit."),
        bad_loop(bad_loop_) {};

    Loop& get_bad_loop() {
        return bad_loop;
    }
};

std::vector<double> get_intensity_vector(Graph& graph) {
    std::vector<Loop> loops = graph.get_independent_loops();

    size_t n = loops.size();
    hwmx::Matrix<double> equations(n, n + 1);

    for (size_t i = 0; i < n; i++) {
        auto& loop = loops[i];

        for (edge_id_t edge_id : loop) {
            EdgeInfo edge_info = edge_pool.get_edge_info(edge_id);
            equations[i][i] += edge_info.resistense;
            equations[i][n] += edge_info.voltage;

            for (size_t j = i + 1; j < n; j++) {
                auto& other_loop = loops[j];
                int edge_in_loop_factor = is_edge_in_loop(edge_id, other_loop);

                equations[i][j] += edge_in_loop_factor * edge_info.resistense;
                equations[j][i] += edge_in_loop_factor * edge_info.resistense;
            }
        }
    }

    hwmx::Vector<double> solution{n};

    try {
        solution = hwmx::solve(equations);
    }
    catch(hwmx::column_of_zeros_error& exc) {
        Loop& bad_loop = loops[exc.get_column_ind()];
        throw bad_circuit_error{bad_loop};
    }

    size_t m = edge_pool.edges_number() + 1;
    std::vector<double> intensity_by_edge_id(m, 0.0);
    for (edge_id_t edge_id = 1; edge_id < m; edge_id++) {
        for (size_t i = 0; i < n; i++) {
            auto& loop = loops[i];
            int edge_in_loop_factor = is_edge_in_loop(edge_id, loop);

            intensity_by_edge_id[edge_id] += edge_in_loop_factor * solution[i];
        }
    }

    return intensity_by_edge_id;
}

void prepare_input(std::string& line) {
    line.replace(line.find("--"), 2, "");
    line.replace(line.find(";"), 1, "");
    line.replace(line.find(","), 1, "");

    if (line.find("V") != std::string::npos)
        line.replace(line.find("V"), 1, "");
    else
        line += " 0";
}

size_t read_edges(std::istream& in, std::vector<edge_id_t>& input_edges) {
    size_t n = 0;

    std::string line;
    while (std::getline(in, line)) {
        prepare_input(line);
        std::stringstream ss{line};

        size_t n1, n2; double r, v;
        ss >> n1 >> n2 >> r >> v;
        edge_id_t edge_id = edge_pool.registrate(n1 - 1, n2 - 1, r, v);

        n = std::max(n, std::max(n1, n2));
        input_edges.push_back(edge_id);
    }

    return n;
}
};

namespace hwmx {

void resolve_intensity_puzzle(std::istream& in, std::ostream& out) {
    std::vector<edge_id_t> input_edges;
    size_t nodes_number = read_edges(in, input_edges);

    Graph graph(nodes_number);
    for (auto edge_id : input_edges)
        graph.add_edge(edge_id);

    std::vector<double> intensity_by_edge_id;
    try {
        intensity_by_edge_id = get_intensity_vector(graph);
    }
    catch (bad_circuit_error& exc) {
        out << "Bad circuit." << std::endl;
        out << "Loop: " << exc.get_bad_loop() << " has zero resistance." << std::endl;
        out << "Impossible to get result." << std::endl;
        return;
    }

    for (auto edge_id : input_edges) {
        auto [l, r] = edge_pool.get_edge(edge_id);
        out << l + 1 << " -- " << r + 1 << "; " << intensity_by_edge_id[edge_id] << " A" << std::endl;
    }
}
}
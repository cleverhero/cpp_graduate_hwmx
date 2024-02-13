#include <iostream>
#include <vector>
#include <variant>
#include <sstream>


#include "graph.h"
#include "matrix.h"
#include "algorithm.h"


namespace {
    struct InputEdgeInfo {
        std::string n1;
        std::string n2;
        double r;
        double v;
    };

    class bad_token_exception : public std::runtime_error {
    public:
        bad_token_exception(const std::string& message) throw()
            : std::runtime_error(message) {};
    };

    enum class TokenType {
        NODE,
        DASH,
        RESISTANCE,
        VOLTAGE,
        END
    };

    struct Token {
        TokenType type;
        std::variant<std::string, double> value;

    public:
        double get_double() { return std::get<double>(value); }
        std::string get_string() { return std::get<std::string>(value); }
    };

    bool is_space_char(char c) {
        return ( isspace(c) || c == ';' || c == ',' );
    }

    class Reader {
        int state = 0;
        std::istream& in;

        Token read_token() {
            std::istreambuf_iterator<char> it{in}, end;

            while (is_space_char(*it) && (it != end)) it++;
            if (it == end) {
                if (state == 1)
                    throw bad_token_exception{ "Parsing error. Expected DASH, found EOF." };
                if (state == 2)
                    throw bad_token_exception{ "Parsing error. Expected NODE, found EOF." };
                if (state == 3)
                    throw bad_token_exception{ "Parsing error. Expected RESISTANCE, found EOF." };
                    
                return { TokenType::END, "" };
            }

            std::string token;
            while (!is_space_char(*it) && (it != end)) {
                token += *it;
                it++;
            }

            if (state == 0) {
                state++;
                return { TokenType::NODE, token };
            }
            else if (state == 1) {
                state++;
                if (token != "--")
                    throw bad_token_exception{ "Parsing error. Expected DASH, found " + token + "." };
                return { TokenType::DASH, token };
            }
            else if (state == 2) {
                state++;
                return { TokenType::NODE, token };
            }
            else if (state == 3) {
                state++;
                try {
                    return { TokenType::RESISTANCE, std::stod(token) };
                }
                catch (std::invalid_argument& exc) {
                    throw bad_token_exception{ "Parsing error. Expected RESISTANCE, found " + token + "." };
                }
            }
            else if (state == 4) {
                if (token.back() == 'V') {
                    std::string token_copy = token;
                    token_copy.erase( token_copy.end() - 1 );

                    try {
                        state = 0;
                        return { TokenType::VOLTAGE, std::stod(token_copy) };
                    }
                    catch (std::invalid_argument& exc) {
                        state = 1;
                        return { TokenType::NODE, token };
                    }
                }
                else {
                    state = 1;
                    return { TokenType::NODE, token };
                }
            }

            return { TokenType::END, "" };
        }

    public:
        Reader(std::istream& in_): in(in_) {}

        std::vector<InputEdgeInfo> read_input() {
            std::vector<Token> tokens;
            while (true) {
                Token token = read_token();
                if (token.type == TokenType::END)
                    break;

                tokens.push_back(token);
            }

            std::vector<InputEdgeInfo> input_edges;
            size_t i = 0;
            size_t token_number = tokens.size();
            while (i < tokens.size()) {
                std::string n1 = tokens[i].get_string();
                i++;

                i++;

                std::string n2 = tokens[i].get_string();
                i++;

                double r = tokens[i].get_double();
                i++;

                double v = 0;
                if (tokens[i].type == TokenType::VOLTAGE) {
                    v = tokens[i].get_double();
                    i++;
                }

                input_edges.push_back({n1, n2, r, v});
            }

            return input_edges;
        }
    };
};


namespace {
    namespace graph = hwmx::graph;
    using intensity_t = double;
    using resistance_t = double;
    using voltage_t = double;

    struct NodeInfo {
        std::string name;
    };

    struct EdgeInfo {
        resistance_t resistense;
        voltage_t voltage;

        EdgeInfo operator-() const noexcept {
            return {resistense, -voltage};
        }
    };

    class ElectricalCircuit final : public graph::Graph<NodeInfo, EdgeInfo> {
    private:
        std::unordered_map<std::string, graph::node_id_t> node_name2id;

    public:
        std::vector<graph::edge_id_t> edge_ids_by_input_order;

    private:
        graph::node_id_t registrate_and_add_input_node(std::string node_name) {
            auto iter = node_name2id.find(node_name);
            if (iter != node_name2id.end())
                return iter->second;

            NodeInfo info{node_name};
            graph::node_id_t node_id = registrate_and_add_node(info);
            node_name2id.insert({node_name, node_id});

            return node_id;
        }

        void registrate_and_add_input_edge(const InputEdgeInfo& input_edge) {
            graph::node_id_t l_id = registrate_and_add_input_node(input_edge.n1);
            graph::node_id_t r_id = registrate_and_add_input_node(input_edge.n2);

            EdgeInfo edge_info{input_edge.r, input_edge.v};
            graph::edge_id_t edge_id = registrate_and_add_edge(l_id, r_id, edge_info);
            edge_ids_by_input_order.push_back(edge_id);
        }

    public:
        ElectricalCircuit(const std::vector<InputEdgeInfo>& input_edges)
            : Graph()
        {
            for (const InputEdgeInfo& input_edge : input_edges)
                registrate_and_add_input_edge(input_edge);
        }

        std::unordered_map<graph::edge_id_t, intensity_t> get_intensity_map() {
            std::vector<graph::Loop> loops = get_independent_loops();

            size_t n = loops.size();
            hwmx::Matrix<double> equations(n, n + 1);

            for (size_t i = 0; i < n; i++) {
                auto& loop = loops[i];

                for (graph::edge_id_t edge_id : loop) {
                    EdgeInfo edge_info = get_edge_info(edge_id);
                    equations[i][i] += edge_info.resistense;
                    equations[i][n] += edge_info.voltage;

                    for (size_t j = i + 1; j < n; j++) {
                        auto& other_loop = loops[j];
                        int edge_in_loop_factor = graph::is_edge_in_loop(edge_id, other_loop);

                        equations[i][j] += edge_in_loop_factor * edge_info.resistense;
                        equations[j][i] += edge_in_loop_factor * edge_info.resistense;
                    }
                }
            }

            hwmx::Vector<intensity_t> solution{n};
            try {
                solution = hwmx::solve(equations);
            }
            catch(hwmx::column_of_zeros_error& exc) {
                graph::Loop& bad_loop = loops[exc.get_column_ind()];
                throw graph::bad_loop_error{bad_loop};
            }

            size_t m = edges.size() + 1;
            std::unordered_map<graph::edge_id_t, intensity_t> intensity_by_edge_id;
            for (graph::edge_id_t edge_id : edges) {
                for (size_t i = 0; i < n; i++) {
                    auto& loop = loops[i];
                    int edge_in_loop_factor = graph::is_edge_in_loop(edge_id, loop);

                    intensity_by_edge_id[edge_id] += edge_in_loop_factor * solution[i];
                }
            }

            return intensity_by_edge_id;
        }
    
        std::pair<std::string, std::string> nodes_names_by_edge_id(const graph::edge_id_t edge_id) {
            graph::node_id_t l_id = get_left_node_id(edge_id);
            graph::node_id_t r_id = get_right_node_id(edge_id);

            return std::make_pair(
                get_node_info(l_id).name,
                get_node_info(r_id).name
            );
        }

        std::string loop_to_string(const graph::Loop& loop) {
            if (loop.size() == 0)
                return "";

            auto node_names = nodes_names_by_edge_id(loop[0]);
            std::stringstream ss;
            ss << nodes_names_by_edge_id(loop[0]).first;
            for (graph::edge_id_t edge_id : loop)
                ss << " -- " << nodes_names_by_edge_id(edge_id).second;

            return ss.str();
        }
    };

    void resolve_intensity_puzzle(std::istream& in, std::ostream& out) {
        Reader reader(in);

        std::vector<InputEdgeInfo> input_edges;
        try {
            input_edges = reader.read_input();
        }
        catch (bad_token_exception& exc) {
            out << "Bad input." << std::endl;
            out << exc.what() << std::endl;
            return;
        }
        

        ElectricalCircuit graph{input_edges};
        std::unordered_map<graph::edge_id_t, intensity_t> intensity_by_edge_id;
        try {
            intensity_by_edge_id = graph.get_intensity_map();
        }
        catch (graph::bad_loop_error& exc) {
            out << "Bad circuit." << std::endl;
            out << "Loop: " << graph.loop_to_string(exc.get_bad_loop());
            out << " has zero resistance." << std::endl;
            out << "Impossible to get result." << std::endl;
            return;
        }

        for (size_t i = 0; i < input_edges.size(); i++) {
            const InputEdgeInfo& input_edge = input_edges[i];
            graph::edge_id_t edge_id = graph.edge_ids_by_input_order[i];

            out << input_edge.n1 << " -- " << input_edge.n2 << "; ";
            out << intensity_by_edge_id[edge_id] << " A" << std::endl;
        }
    }
}


int main() {
    resolve_intensity_puzzle(std::cin, std::cout);

    return 0;
}
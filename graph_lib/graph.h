#include <iostream>
#include <vector>
#include <queue>
#include <iterator>
#include <unordered_map>
#include <unordered_set>


namespace hwmx::graph {
    using node_id_t = size_t;
    using edge_id_t = long long;

    using Loop = std::vector<edge_id_t>;


    class bad_loop_error: public std::runtime_error {
    private:
        Loop bad_loop;

    public:
        bad_loop_error(const Loop& bad_loop_) throw():
            std::runtime_error("Bad circuit."),
            bad_loop(bad_loop_) {};

        Loop& get_bad_loop() {
            return bad_loop;
        }
    };


    template<typename Info>
    class Node {
        std::tuple<node_id_t, Info> data;

    public:
        Node(node_id_t id, const Info& info) noexcept : data({id, info}) {}

        node_id_t get_id() const noexcept { return std::get<0>(data); }
        Info get_info() const noexcept { return std::get<1>(data); }
    };


    template<typename Info>
    class Edge {
        std::tuple<edge_id_t, node_id_t, node_id_t, Info> data;

    public:
        template<typename InfoT>
        Edge(edge_id_t id, node_id_t l_id, node_id_t r_id, InfoT&& info) noexcept
         : data({id, l_id, r_id, std::forward<InfoT>(info)}) {}

        edge_id_t get_id() const noexcept { return std::get<0>(data); }
        node_id_t get_left_node_id() const noexcept { return std::get<1>(data); }
        node_id_t get_right_node_id() const noexcept { return std::get<2>(data); }
        Info get_info() const noexcept { return std::get<3>(data); }

        Edge operator-() const noexcept { 
            return {-get_id(), get_right_node_id(), get_left_node_id(), -get_info()}; 
        }
    };


    template<typename Info>
    class NodesPool {
    private:
        using NodeType = Node<Info>;
        std::vector<NodeType> nodes;

    public:
        node_id_t registrate(const Info& node_info) {
            nodes.emplace_back(nodes.size(), node_info);

            return nodes.size() - 1;
        }

        bool contains(node_id_t node_id) const noexcept {
            return (node_id < nodes.size());
        }

        Info get_node_info(node_id_t node_id) const { return nodes[node_id].get_info(); }
        size_t nodes_number() const noexcept { return nodes.size(); }

        node_id_t id_begin() const noexcept { return 0; }
        node_id_t id_end() const noexcept { return nodes.size(); }
    };


    template<typename Info>
    class EdgesPool {
    private:
        using EdgeType = Edge<Info>;
        std::vector<EdgeType> edges;

        EdgeType get_edge(edge_id_t edge_id) const {
            // Necessary transform edge_id to vector index
            // Negative edge_id is reversed Edge
            // 'edge_id - 1' because edge_id start from 1
            if (edge_id < 0)
                return -edges[-edge_id - 1];

            return edges[edge_id - 1];
        }

    public:
        edge_id_t registrate(node_id_t l_id, node_id_t r_id, const Info& edge_info) {
            edges.emplace_back(edges.size() + 1, l_id, r_id, edge_info);

            // Return ID from 1
            return edges.size();
        }

        node_id_t get_left_node_id(edge_id_t edge_id) const {
            return get_edge(edge_id).get_left_node_id(); 
        }

        node_id_t get_right_node_id(edge_id_t edge_id) const {
            return get_edge(edge_id).get_right_node_id();
        }

        Info get_info(edge_id_t edge_id) const {
            return get_edge(edge_id).get_info();
        }

        size_t edges_number() const noexcept {
            return edges.size();
        }

        edge_id_t first_id() const noexcept { return 1; }
        edge_id_t last_id() const noexcept { return edges.size() + 1; }
    };



    int is_edge_in_loop(edge_id_t edge_id, Loop& loop);


    template<typename NodeInfo, typename EdgeInfo>
    class BaseGraph {
    protected:
        using EdgesSet = std::unordered_set<edge_id_t>;

        NodesPool<NodeInfo> nodes_pool;
        EdgesPool<EdgeInfo> edges_pool;

        std::unordered_set<node_id_t> nodes;
        std::unordered_set<
            edge_id_t,
            decltype( [](edge_id_t edge_id) { return std::hash<edge_id_t>{}(std::abs(edge_id)); } ),
            decltype( [](edge_id_t lhs, edge_id_t rhs) { return std::abs(lhs) == std::abs(rhs); } )
        > edges;
 
        std::unordered_map<node_id_t, EdgesSet> edges_by_node;

    public:
        BaseGraph() = default;

        BaseGraph(NodesPool<NodeInfo> npool, EdgesPool<EdgeInfo> epool)
            : nodes_pool(std::move(npool)), edges_pool(std::move(epool)) {}

        node_id_t registrate_node(const NodeInfo& node_info) {
            return nodes_pool.registrate(node_info);
        }

        bool add_node(node_id_t node_id) {
            auto [it, inserted] = nodes.insert(node_id);
            return inserted;
        }

        template<std::input_iterator It>
        void add_nodes(It start, It finish) {
            nodes.insert(start, finish);
        }

        node_id_t registrate_and_add_node(const NodeInfo& node_info) {
            node_id_t node_id = registrate_node(node_info);
            add_node(node_id);

            return node_id;
        }

        edge_id_t registrate_edge(node_id_t l_id, node_id_t r_id, const EdgeInfo& edge_info) {
            return edges_pool.registrate(l_id, r_id, edge_info);
        }

        edge_id_t add_edge(edge_id_t edge_id) {
            auto [it, inserted] = edges.insert(edge_id);
            if (!inserted)
                return edge_id;
            
            node_id_t l_id = edges_pool.get_left_node_id(edge_id);
            node_id_t r_id = edges_pool.get_right_node_id(edge_id);

            edges_by_node[l_id].insert(edge_id);
            if (l_id != r_id)
                edges_by_node[r_id].insert(-edge_id);

            return edge_id;
        }

        edge_id_t registrate_and_add_edge(node_id_t l_id, node_id_t r_id, const EdgeInfo& edge_info) {
            edge_id_t edge_id = registrate_edge(l_id, r_id, edge_info);
            return add_edge(edge_id);
        }

        bool check_edge(edge_id_t edge_id) const noexcept {
            return (edges.find(edge_id) != edges.end());
        }

        NodeInfo get_node_info(node_id_t node_id) const noexcept {
            return nodes_pool.get_node_info(node_id);
        }

        EdgeInfo get_edge_info(edge_id_t edge_id) const noexcept {
            return edges_pool.get_info(edge_id);
        }

        node_id_t get_left_node_id(edge_id_t edge_id) const noexcept {
            return edges_pool.get_left_node_id(edge_id);
        }

        node_id_t get_right_node_id(edge_id_t edge_id) const noexcept {
            return edges_pool.get_right_node_id(edge_id);
        }

        bool remove_edge(edge_id_t edge_id) noexcept {
            auto iter = edges.find(edge_id);
            if (iter == edges.end())
                return false;

            edges.erase(iter);

            node_id_t l_id = edges_pool.get_left_node_id(edge_id);
            node_id_t r_id = edges_pool.get_right_node_id(edge_id);

            edges_by_node[l_id].erase(edge_id);
            edges_by_node[r_id].erase(-edge_id);

            return true;
        }

    public:
        virtual ~BaseGraph() = default;
    };


    template<typename NodeInfo, typename EdgeInfo>
    class Graph: public BaseGraph<NodeInfo, EdgeInfo> {
    protected:
        using BaseGraph<NodeInfo, EdgeInfo>::nodes_pool;
        using BaseGraph<NodeInfo, EdgeInfo>::edges_pool;

        using BaseGraph<NodeInfo, EdgeInfo>::nodes;
        using BaseGraph<NodeInfo, EdgeInfo>::edges;
        using BaseGraph<NodeInfo, EdgeInfo>::edges_by_node;

    private:
        bool find_loop_(
            node_id_t node_id,
            std::unordered_map<node_id_t, int>& colors, 
            std::unordered_map<node_id_t, edge_id_t>& edge_to,
            node_id_t& start_loop, node_id_t& finish_loop
        ) const noexcept {
            colors[node_id] = 1;
            for (edge_id_t edge_id : edges_by_node.find(node_id)->second) {
                if (edge_to[node_id] == -edge_id) continue;

                node_id_t next_node_id = edges_pool.get_right_node_id(edge_id);

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
        Graph() = default;

        Graph(NodesPool<NodeInfo> npool, EdgesPool<EdgeInfo> epool)
            : BaseGraph<NodeInfo, EdgeInfo>(std::move(npool), std::move(epool)) {}

        Loop find_loop() const noexcept {
            std::unordered_map<node_id_t, int> colors;
            std::unordered_map<node_id_t, edge_id_t> edge_to;
            node_id_t start_loop;
            node_id_t finish_loop;

            for (node_id_t start_node : nodes)
                if (find_loop_(start_node, colors, edge_to, start_loop, finish_loop))
                    break;

            node_id_t curr = finish_loop;
            Loop loop{-edge_to[start_loop]};
            while (curr != start_loop) {
                loop.push_back(-edge_to[curr]);

                edge_id_t edge_id = edge_to[curr];
                curr = edges_pool.get_left_node_id(edge_id);
            }

            return loop;
        }

        Graph get_spanning_tree() const noexcept {
            Graph spanning_tree{nodes_pool, edges_pool};
            spanning_tree.add_nodes(nodes.begin(), nodes.end());

            std::queue<node_id_t> q;
            std::unordered_set<node_id_t> used_nodes;
            
            for (node_id_t start_node : nodes) {
                if (used_nodes.contains(start_node))
                    continue;

                q.push(start_node); used_nodes.insert(start_node);
                while(!q.empty()) {
                    node_id_t curr_node_id = q.front(); q.pop();
                   
                    for (edge_id_t edge_id : edges_by_node.find(curr_node_id)->second) {
                        node_id_t next_node_id = edges_pool.get_right_node_id(edge_id);
                        if (used_nodes.contains(next_node_id))
                            continue;

                        spanning_tree.add_edge(edge_id);
                        q.push(next_node_id); used_nodes.insert(next_node_id);
                    }
                }
            }
            
            return spanning_tree;
        }

        std::vector<Loop> get_independent_loops() const noexcept {
            std::vector<Loop> loops;

            Graph spanning_tree = get_spanning_tree();

            for (edge_id_t edge_id : edges) {
                if (spanning_tree.check_edge(edge_id)) continue;
                    
                spanning_tree.add_edge(edge_id);
                loops.push_back( spanning_tree.find_loop() );
                spanning_tree.remove_edge(edge_id);
            }

            return loops;
        }
    };
}
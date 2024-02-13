#include "graph.h"


namespace hwmx::graph {
    int is_edge_in_loop(edge_id_t edge_id, Loop& loop) {
        if (std::find(loop.begin(), loop.end(), edge_id) != loop.end())
            return 1;

        if (std::find(loop.begin(), loop.end(), -edge_id) != loop.end())
            return -1;

        return 0;
    }
}
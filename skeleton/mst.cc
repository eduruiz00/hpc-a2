#include <chrono>
#include <vector>

#include <cstdio>

using namespace std;

#include "matrix.h"
#include "trees.h"

vector<Node> nodes;
vector<Edge> edges;
vector<SpanningTree> spanning_trees;

int
main(int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "usage: %s <filename>\n", argv[0]);
      return -1;
    }

  int n_rows, n_cols;
  bool ok(false);

  ok = load_matrix_market(argv[1], n_rows, n_cols, nodes, edges);
  if (!ok)
    {
      fprintf(stderr, "failed to load matrix.\n");
      return -1;
    }

  /* For debugging, can be removed when implementation is finished. */
  //dump_nonzeros(n_rows, values, col_ind, row_ptr_begin, row_ptr_end);

  for (Node node : nodes) {
    spanning_trees.push_back(SpanningTree(vector<Node> {node}));
  }

  auto factorization_start_time = std::chrono::high_resolution_clock::now();

  /* Perform MST Boruvka’s algorithm here */
  fprintf(stderr, "Size before: %lu\n", spanning_trees.size());

  vector<Edge> edges_node0 = get_edges_from_node(nodes[0], edges);

  SpanningTree& spt = spanning_trees[0];
  Node node_conn(1); // Create a default spanning tree which will be used
  Edge conn_edge = spt.find_minimum_edge(edges, node_conn);
  SpanningTree connectionSpanningTree = find_spanning_tree_of_node(node_conn, spanning_trees);
  spt.join_spanning_tree(connectionSpanningTree, conn_edge);
  remove_spanning_tree_from_list(spanning_trees, connectionSpanningTree);

  // TODO: Iterate until we have one spanning tree
  // TODO: Check in the same iteration

  fprintf(stderr, "Size after: %lu\n", spanning_trees.size());

  for (Node node : spt.nodes) {
    fprintf(stderr, "Vertex: %i\n", node.vertex);
  }

  for (Edge edge : edges_node0) {
    fprintf(stderr, "Node 1: %i, Node 2: %i, Connection: %f\n", edge.node1.vertex, edge.node2.vertex, edge.connection);
  }


  /*

  vector<Connection> connections_iteration;

  for (int i = 0; i < components.size(); i++) {
    Component &component = components[i];
    int min_index = std::min_element(component.connections.begin(), component.connections.end()) - component.connections.begin();
    Component &neighbor = component.neighbors[min_index];
    if(component.node < neighbor.node) {
      connections_iteration.push_back(Connection(component, neighbor));
    } else {
      connections_iteration.push_back(Connection(neighbor, component));
    }
    component.neighbors.erase(component.neighbors.begin() + min_index);
    int node_in_neighbor = component.FindIndexNeighborNode(neighbor);
    neighbor.neighbors.erase(neighbor.neighbors.begin() + node_in_neighbor);
  }

  */


  

  auto factorization_end_time = std::chrono::high_resolution_clock::now();
  
  
  auto solve_start_time = std::chrono::high_resolution_clock::now();
  
  /* Compute all 5 solution vectors here */

  auto solve_end_time = std::chrono::high_resolution_clock::now();
  
  /* Compute relative errors here */
  
  std::chrono::duration<double> factorization_elapsed_time = factorization_end_time - factorization_start_time;
  std::chrono::duration<double> solve_elapsed_time = solve_end_time - solve_start_time;
  
  
  /* Print results */
  fprintf(stdout, "%.20f\n", factorization_elapsed_time.count());
  fprintf(stdout, "%.20f\n", solve_elapsed_time.count());
  return 0;
}

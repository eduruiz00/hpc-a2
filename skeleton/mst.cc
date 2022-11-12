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

  // Create all initial spanning trees
  // Add indexes
  int add_idx = 0;
  vector<int> idxs_spanning_trees {}; 
  for (Node node : nodes) {
    spanning_trees.push_back(SpanningTree(vector<Node> {node}));
    idxs_spanning_trees.push_back(add_idx);
    add_idx += 1;
  }

  auto factorization_start_time = std::chrono::high_resolution_clock::now();

  /* Perform MST Boruvka’s algorithm here */
  fprintf(stderr, "Size before: %lu\n", spanning_trees.size());

  int num_iterations = 0;
  Node node_conn(1); // Create a default spanning tree which will be used
  Edge conn_edge(1,2,0.1);
  int idx;
  int size_spt = idxs_spanning_trees.size();
  int count = 0;
  int real_idx;

  int stopping_counter = 0;
  

  while (num_iterations <= 1000000) {
    //fprintf(stderr, "Iter: %i\n", num_iterations);
    if (idxs_spanning_trees.size() == 1 || stopping_counter == idxs_spanning_trees.size()) {
      fprintf(stderr, "BREAKING!\n");
      break;
    }
    count = idxs_spanning_trees[0];
    //fprintf(stderr, "Index: %i\n", count);
    auto spt = &spanning_trees[count];
    conn_edge = spt->find_minimum_edge(edges, node_conn);
    //fprintf(stderr, "Edge connecting %i-%i with connection: %f\n", conn_edge.node1.vertex, conn_edge.node2.vertex, conn_edge.connection);
    SpanningTree connectionSpanningTree = find_spanning_tree_of_node(node_conn, spanning_trees);
    spt->join_spanning_tree(connectionSpanningTree, conn_edge);
    idx = remove_spanning_tree_from_list(spanning_trees, connectionSpanningTree);
    auto it = find(idxs_spanning_trees.begin(), idxs_spanning_trees.end(), idx);
    real_idx = it - idxs_spanning_trees.begin();
    if (real_idx < size_spt) {
      //fprintf(stderr, "Index: %i, size: %i, count: %i\n", idx, size_spt, count);
      //spanning_trees.erase(spanning_trees.begin() + idx);
      idxs_spanning_trees.erase(idxs_spanning_trees.begin() + real_idx);
      idxs_spanning_trees.push_back(idxs_spanning_trees[0]);
      idxs_spanning_trees.erase(idxs_spanning_trees.begin());
      stopping_counter = 0;
    } else {
      stopping_counter += 1;
    }
    size_spt = idxs_spanning_trees.size();
    num_iterations += 1;
  }

  fprintf(stderr, "=======FINAL EDGES=======\n");
  int final_idx = idxs_spanning_trees[0];
  /*
  for (Edge final_edge : spanning_trees[final_idx].edges) {
    fprintf(stderr, "Edge connecting %i-%i with connection: %f\n", final_edge.node1.vertex, final_edge.node2.vertex, final_edge.connection);
  }
  */

  for (Node final_node : spanning_trees[final_idx].nodes) {
    fprintf(stderr, "Node: %i\n", final_node.vertex);
  }

  fprintf(stderr, "Size after: %lu\n", idxs_spanning_trees.size());
  
  // TODO: Check in the same iteration

  /*
  vector<Edge> edges_node2 = get_edges_from_node(nodes[0], edges);

  for (Node node : spt.nodes) {
    fprintf(stderr, "Vertex: %i\n", node.vertex);
  }

  for (Edge edge : edges_node0) {
    fprintf(stderr, "Node 1: %i, Node 2: %i, Connection: %f\n", edge.node1.vertex, edge.node2.vertex, edge.connection);
  }
  */

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

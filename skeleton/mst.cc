#include <chrono>
#include <vector>

#include <cstdio>

using namespace std;

#include "matrix.h"
#include "trees.h"

int
main(int argc, char **argv)
{

  // Create vectors that will contain the information
  vector<Node> nodes;
  vector<Edge> edges;
  vector<SpanningTree> spanning_trees;

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
  int idx, real_idx;
  int size_spt = idxs_spanning_trees.size();
  int count = 0;

  long unsigned stopping_counter = 0;
  SpanningTree* connectionSpanningTreePtr = nullptr;
  SpanningTree* spt = nullptr;

  

  while (num_iterations <= 1000000) {
    if (idxs_spanning_trees.size() == 1 || stopping_counter == idxs_spanning_trees.size()) {
      fprintf(stderr, "BREAKING!\n");
      break;
    }
    count = idxs_spanning_trees[0];
    spt = &spanning_trees[count];
    conn_edge = spt->find_minimum_edge(edges, node_conn);
    spt->add_connection_edge(conn_edge);

    connectionSpanningTreePtr = find_spanning_tree_of_node(node_conn, spanning_trees);
    spt->join_spanning_tree(*connectionSpanningTreePtr, conn_edge);
    idx = remove_spanning_tree_from_list(spanning_trees, *connectionSpanningTreePtr);
    
    auto it = find(idxs_spanning_trees.begin(), idxs_spanning_trees.end(), idx);
    real_idx = it - idxs_spanning_trees.begin();
    if (real_idx < size_spt) {
      //fprintf(stderr, "Index: %i, size: %i, count: %i\n", idx, size_spt, count);
      idxs_spanning_trees.erase(idxs_spanning_trees.begin() + real_idx);
      idxs_spanning_trees.push_back(idxs_spanning_trees[0]);
      idxs_spanning_trees.erase(idxs_spanning_trees.begin());
      stopping_counter = 0;
    } else {
      stopping_counter += 1;
    }
    size_spt = idxs_spanning_trees.size();
    num_iterations += 1;
    fprintf(stderr, "%i, %i\n", num_iterations, size_spt);
  }

  fprintf(stderr, "=======FINAL EDGES=======\n");
  int final_idx = idxs_spanning_trees[0];
  
  for (Edge final_edge : spanning_trees[final_idx].get_connection_edges()) {
    fprintf(stderr, "Edge connecting %i-%i with connection: %f\n", final_edge.node1.vertex, final_edge.node2.vertex, final_edge.connection);
  }
  
  /*
  for (Node final_node : spanning_trees[final_idx].nodes) {
    fprintf(stderr, "Node: %i\n", final_node.vertex);
  }
  */

  fprintf(stderr, "Size after: %lu\n", idxs_spanning_trees.size());
  
  // TODO: Check in the same iteration


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

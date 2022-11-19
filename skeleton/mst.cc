#include <chrono>
#include <vector>

#include <cstdio>

using namespace std;

#include "matrix.h"
#include "trees.h"

// Create vectors that will contain the information
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


  // Create all initial spanning trees
  // Add indexes
  vector<SpanningTree*> alloc_spanning_trees {};
  for (int i = 0; i < nodes.size(); i++) {
    spanning_trees.push_back(SpanningTree(vector<Node> {nodes[i]}, get_edges_from_node(nodes[i], edges)));
  }

  for (int i = 0; i < spanning_trees.size(); i++) {
    alloc_spanning_trees.push_back(&(spanning_trees[i]));
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  /* Perform MST Boruvka’s algorithm here */
  fprintf(stderr, "Size before: %lu\n", spanning_trees.size());

  int num_iterations = 0;
  Node node_conn(1); // Create a default spanning tree which will be used
  Edge* conn_edge;
  int idx, real_idx;
  int size_spt = alloc_spanning_trees.size();

  long unsigned stopping_counter = 0;
  SpanningTree* connectionSpanningTreePtr = nullptr;
  SpanningTree* spt = nullptr;


  while (num_iterations <= 1000000) {
    auto it_start_time = std::chrono::high_resolution_clock::now();
    if (alloc_spanning_trees.size() == 1 || stopping_counter == alloc_spanning_trees.size()) {
      fprintf(stderr, "BREAKING!\n");
      break;
    }

    spt = alloc_spanning_trees[0];

    if (spt->edges.size() == 0) { //Check to avoid all the process
      stopping_counter += 1;
      continue;
    }

    conn_edge = spt->find_minimum_edge(node_conn);
    spt->add_connection_edge(conn_edge);

    connectionSpanningTreePtr = find_spanning_tree_of_node(node_conn, alloc_spanning_trees);
    int idx = find_index_spanning_tree(alloc_spanning_trees, connectionSpanningTreePtr);

    spt->join_spanning_tree(*connectionSpanningTreePtr);
    //idx = remove_spanning_tree_from_list(spanning_trees, *connectionSpanningTreePtr);
    
    if (idx < size_spt) {
      //fprintf(stderr, "Index: %i, size: %i, count: %i\n", idx, size_spt, count);
      alloc_spanning_trees.erase(alloc_spanning_trees.begin() + idx);
      alloc_spanning_trees.push_back(alloc_spanning_trees[0]);
      alloc_spanning_trees.erase(alloc_spanning_trees.begin());
      stopping_counter = 0;
    } else {
      stopping_counter += 1;
    }
    size_spt = alloc_spanning_trees.size();
    num_iterations += 1;
    fprintf(stderr, "%i, %i\n", num_iterations, size_spt);
    auto it_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> it_total_time = it_end_time - it_start_time;
    if (it_total_time.count() > 5.) {
      auto variable_inveted = 0;
    }
  }

  fprintf(stderr, "========FINAL EDGES======\n");
  SpanningTree* final_spanning_tree = alloc_spanning_trees[0];
  
  for (Edge* final_edge : final_spanning_tree->get_connection_edges()) {
    //fprintf(stderr, "Edge connecting %i-%i with connection: %f\n", final_edge->node1.vertex, final_edge->node2.vertex, final_edge->connection);
  }
  
  /*
  for (Node final_node : spanning_trees[final_idx].nodes) {
    fprintf(stderr, "Node: %i\n", final_node.vertex);
  }
  */

  fprintf(stderr, "Size after: %lu\n", alloc_spanning_trees.size());
  
  // TODO: Check in the same iteration


  auto end_time = std::chrono::high_resolution_clock::now();
  
  /* Compute relative errors here */
  
  std::chrono::duration<double> elapsed_time = end_time - start_time;
  
  freopen("results.txt", "w", stdout);
  /* Print results */
  fprintf(stdout, "%.20f\n", elapsed_time.count());

  for (SpanningTree* spanning_tree : alloc_spanning_trees) {
    fprintf(stdout, "%d", spanning_tree->edges.size());
    double total_tree_weight = 0.0;
    for (Edge* edge : spanning_tree->get_connection_edges()) {
      fprintf(stdout, "%d %d %.20f\n", edge->node1.vertex, edge->node2.vertex, edge->connection);
      total_tree_weight += edge->connection;
    }
    fprintf(stdout, "%.20f\n\n", total_tree_weight);
  }
  return 0;
}

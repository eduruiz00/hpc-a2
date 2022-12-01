#include <chrono>
#include <vector>
#include <cstring>
#include <array>
#include <mpi.h>

#include <cstdio>

using namespace std;

#include "matrix.h"
#include "trees.h"

// Create MPI Datatypes
MPI_Datatype MPI_SpanningTreeType;
// Node1 conn others | Node 2 conn others | Conn others value | Nodes | Node1 conns | Node2 conns | Conn values
MPI_Datatype st_types[9] = { MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT };
int blocklengths[9] = {1, 1, 1, 1, 1, SZ, SZ, SZ, SZ};

int received_info = 0;

int
main(int argc, char **argv)
{

  if (argc != 2)
    {
      fprintf(stderr, "usage: %s <filename>\n", argv[0]);
      return -1;
    }


  MPI_Init(&argc, &argv);

  // Get the number of processes in MPI_COMM_WORLD
  int n_proc;
  MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
  fprintf(stderr, "Size : %i\n", n_proc);

  // Get the rank of this process in MPI_COMM_WORLD
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  fprintf(stderr, "My rank: %i\n", my_rank);
  struct SendData ex_sd;
  MPI_Aint offsets[9];
  /*= {
    offsetof(SendData, node1_conn),
    offsetof(SendData, node2_conn),
    offsetof(SendData, conn_val),
    offsetof(SendData, nodes_sz),
    offsetof(SendData, edges_sz),
    offsetof(SendData, nodes),
    offsetof(SendData, nodes1),
    offsetof(SendData, nodes2),
    offsetof(SendData, connections)
  };
  */

  
  MPI_Aint addresses[10];

  MPI_Get_address(&ex_sd, &addresses[0]);
  MPI_Get_address(&ex_sd.node1_conn, &addresses[1]);
  MPI_Get_address(&ex_sd.node2_conn, &addresses[2]);
  MPI_Get_address(&ex_sd.conn_val, &addresses[3]);
  MPI_Get_address(&ex_sd.nodes_sz, &addresses[4]);
  MPI_Get_address(&ex_sd.edges_sz, &addresses[5]);
  MPI_Get_address(&ex_sd.nodes, &addresses[6]);
  MPI_Get_address(&ex_sd.nodes1, &addresses[7]);
  MPI_Get_address(&ex_sd.nodes2, &addresses[8]);
  MPI_Get_address(&ex_sd.connections, &addresses[9]);
  
  offsets[0] = MPI_Aint_diff(addresses[1], addresses[0]);
  offsets[1] = MPI_Aint_diff(addresses[2], addresses[0]);
  offsets[2] = MPI_Aint_diff(addresses[3], addresses[0]);
  offsets[3] = MPI_Aint_diff(addresses[4], addresses[0]);
  offsets[4] = MPI_Aint_diff(addresses[5], addresses[0]);
  offsets[5] = MPI_Aint_diff(addresses[6], addresses[0]);
  offsets[6] = MPI_Aint_diff(addresses[7], addresses[0]);
  offsets[7] = MPI_Aint_diff(addresses[8], addresses[0]);
  offsets[8] = MPI_Aint_diff(addresses[9], addresses[0]);

  MPI_Type_create_struct(9, blocklengths, offsets, st_types, &MPI_SpanningTreeType);
  MPI_Type_commit(&MPI_SpanningTreeType);

  SendData sd;
  MPI_Request request;
  if (my_rank == 0) {

    //MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //fprintf(stderr, "World Size: %i, Rank: %i\n", n_proc, number);
    vector<SpanningTree> final_spanning_trees;
    vector<Edge> final_edges;
    SendData rec_data;
    int number;
    while (received_info < n_proc-1){
      MPI_Recv(&rec_data, 1, MPI_SpanningTreeType, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      fprintf(stderr, "Recv data: %i\n", rec_data.connections[0]);
      fill_final_spanning_trees(final_spanning_trees, rec_data, final_edges);
      received_info += 1;
    }
    fprintf(stderr, "Received information from all processors\n");
    
    // TODO: Add the same Boruvka's algorithm as above, generate a function to do it.
    
    /* Print results */
    //freopen("results.txt", "w", stdout);
    //fprintf(stdout, "%.20f\n", elapsed_time.count());

    for (SpanningTree spanning_tree : final_spanning_trees) {
      fprintf(stdout, "%d\n", spanning_tree.edges.size());
      double total_tree_weight = 0.0;
      for (Edge* edge : spanning_tree.get_connection_edges()) {
        fprintf(stdout, "%d %d %.20f\n", edge->node1.vertex, edge->node2.vertex, edge->connection);
        total_tree_weight += edge->connection;
      }
      fprintf(stdout, "%.20f\n\n", total_tree_weight);
    }


  } else {
    
    vector<Node> nodes;
    vector<Edge> edges;
    vector<SpanningTree> spanning_trees;

    int n_rows, n_cols;
    bool ok(false);

    ok = load_matrix_market(argv[1], n_rows, n_cols, nodes, edges, my_rank, n_proc, spanning_trees);
    if (!ok)
      {
        fprintf(stderr, "failed to load matrix.\n");
        return -1;
      }

    // Create all initial spanning trees
    // Add indexes
    vector<SpanningTree*> alloc_spanning_trees {};
    /*
    for (int i = 0; i < nodes.size(); i++) {
      spanning_trees.push_back(SpanningTree(vector<Node> {nodes[i]}, get_edges_from_node(nodes[i], edges)));
    }
    */

    for (int i = 0; i < spanning_trees.size(); i++) {
      alloc_spanning_trees.push_back(&(spanning_trees[i]));
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    /* Perform MST Boruvka’s algorithm here */
    fprintf(stderr, "Size before: %lu\n", alloc_spanning_trees.size());

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
      if (idx < size_spt) {
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
    }
    fprintf(stderr, "Size after: %lu\n", alloc_spanning_trees.size());
    for (int i = 0; i < alloc_spanning_trees.size(); i++)
    {
      SendData sd;
      if (alloc_spanning_trees[i]->conn_other_proc) {
        sd.node1_conn = alloc_spanning_trees[i]->conn_other_proc->node1.vertex;
        fprintf(stderr, "Data 1: %i\n", sd.node1_conn);
        sd.node2_conn = alloc_spanning_trees[i]->conn_other_proc->node2.vertex;
        fprintf(stderr, "Data 2: %i\n", sd.node2_conn);
        sd.conn_val = alloc_spanning_trees[i]->conn_other_proc->connection;
        fprintf(stderr, "Data 3: %f\n", sd.conn_val);
      } else {
        sd.node1_conn = -1;
        sd.node2_conn = -1;
        sd.conn_val = 0;
      }
      sd.nodes_sz = alloc_spanning_trees[i]->nodes.size();
      sd.edges_sz = alloc_spanning_trees[i]->get_connection_edges().size();
      alloc_spanning_trees[i]->return_list_nodes(sd);
      alloc_spanning_trees[i]->return_list_edges_nodes1(sd);
      alloc_spanning_trees[i]->return_list_edges_nodes2(sd);
      alloc_spanning_trees[i]->return_list_edges_connections(sd);
      fprintf(stderr, "Send data: %i\n", sd.connections[0]);
      MPI_Send(&sd, 1, MPI_SpanningTreeType, 0, 0, MPI_COMM_WORLD);
      //MPI_Send(&my_rank, 1, MPI_SpanningTreeType, 0, 1, MPI_COMM_WORLD);
    }

  }

  
  MPI_Finalize();  
  
  return 0;
}

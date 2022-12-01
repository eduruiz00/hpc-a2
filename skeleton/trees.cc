#include <vector>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <mpi.h>

using namespace std;

#include "trees.h"

extern "C" {
#include "mmio.h"
}

bool node_is_not_inside(vector<Node> &nodes, Node ev_node)
{
  if (std::find(nodes.begin(), nodes.end(), ev_node) != nodes.end()) {
    return false;
  }
  return true;
}

bool comparePtrToEdge(Edge* a, Edge* b) { return (*a < *b); }

vector<Edge*> get_edges_from_node(Node node, vector<Edge> &edges)
{
  vector<Edge*> edges_node;
  for (int ptr_edge : node.ptr_edges) {
    edges_node.push_back(&edges[ptr_edge]);
  }
  return edges_node;
}

void SpanningTree::join_spanning_tree(SpanningTree &st)
{
  // Copy nodes
  for (int i = 0; i < st.nodes.size(); i++) {
    nodes.push_back(st.nodes[i]);
  }

  // Remove actual edges that connect between them
  
  for (int i = 0; i < edges.size(); i++) {
    if (!(node_is_not_inside(nodes, edges[i]->node1)) && !(node_is_not_inside(nodes, edges[i]->node2))) {
        edges.erase(edges.begin() + i);
        i--; //Avoid skipping one element
    }
  }

  // Copy edges that don't connect edges from the same spanning tree
  for (int i = 0; i < st.edges.size(); i++) {
    if (node_is_not_inside(nodes, st.edges[i]->node1) || node_is_not_inside(nodes, st.edges[i]->node2)) {
      edges.push_back(st.edges[i]);
    }
  }

  Edge* conn_edge = nullptr;
  // Add edge
  for (int i = 0; i < st.get_connection_edges().size(); i++){
    conn_edge = st.get_connection_edges()[i];
    add_connection_edge(conn_edge);
  }

  // Take the lower connection to others
  if (st.conn_other_proc != nullptr) {
    if (conn_other_proc != nullptr) {
      if (conn_other_proc->connection > st.conn_other_proc->connection) {
        conn_other_proc = st.conn_other_proc;
      }
    } else {
      conn_other_proc = st.conn_other_proc;
    }
  }
  
}

bool edge_is_not_inside(vector<Edge> &edges, Edge ev_edge)
{
  if (std::find(edges.begin(), edges.end(), ev_edge) != edges.end()) {
    return false;
  }
  return true;
}

Edge* SpanningTree::find_minimum_edge(Node &node_connection)
{
  std::sort(edges.begin(), edges.end(), comparePtrToEdge);
  if (node_is_not_inside(nodes, edges[0]->node1)) {
    node_connection = edges[0]->node1;
  } else {
    node_connection = edges[0]->node2;
  }
  return edges[0];
}

int remove_spanning_tree_from_list(vector<SpanningTree> &spanning_trees, SpanningTree &remove_st)
{
  auto it = find(spanning_trees.begin(), spanning_trees.end(), remove_st);
  int idx = it - spanning_trees.begin();
  
  //fprintf(stderr, "Index: %i", idx);
  if (idx == spanning_trees.size()) {
    fprintf(stderr, "NOT FOUND!!!\n");
  }
  return idx;
}

SpanningTree *find_spanning_tree_of_node(Node node, vector<SpanningTree*> &alloc_spanning_trees)
{
  
  for (int i = 0; i < alloc_spanning_trees.size(); i++) {
    if (node_is_not_inside(alloc_spanning_trees[i]->nodes, node) == false) {
      SpanningTree* ptr = (SpanningTree*) malloc(sizeof(*(alloc_spanning_trees[i])));
      ptr = alloc_spanning_trees[i];
      return ptr;
    }
  }
}


void SpanningTree::add_connection_edge(Edge* add_edge)
{
  connection_edges_.push_back(add_edge);
}

vector<Edge*> SpanningTree::get_connection_edges()
{
  return connection_edges_;
}

int find_index_spanning_tree(vector<SpanningTree*> alloc_spanning_trees, SpanningTree* connectionSpanningTreePtr) {
  for (int i = 0; i < alloc_spanning_trees.size(); i++) {
    if (alloc_spanning_trees[i] == connectionSpanningTreePtr) {
      return i;
    }
  }
  return alloc_spanning_trees.size();
}

bool edge_inside_proc_spanning_trees(vector<SpanningTree> &spanning_trees, Edge* edge)
{
  bool node1_inside = false;
  bool node2_inside = false;
  for (int st = 0; st < spanning_trees.size(); st++) {
    if (!node_is_not_inside(spanning_trees[st].nodes, edge->node1)) {
      node1_inside = true;
    }
    if (!node_is_not_inside(spanning_trees[st].nodes, edge->node2)) {
      node2_inside = true;
    }
    if (node1_inside && node2_inside) {
      return true;
    }
  }
  return false;
}

void SpanningTree::lowest_connection_st_other_processors(vector<SpanningTree> &spanning_trees)
{
  vector<Edge*> edges_conn_other_proc;
  for (int i = 0; i < edges.size(); i++) {
    if (!edge_inside_proc_spanning_trees(spanning_trees, edges[i])) {
      fprintf(stderr, "Entering\n");
      edges_conn_other_proc.push_back(edges[i]);
      edges.erase(edges.begin() + i);
      i--; //Avoid skipping one element
    }
  }

  
  // Sort and take the first one
  if (edges_conn_other_proc.size() > 0) {
    std::sort(edges_conn_other_proc.begin(), edges_conn_other_proc.end(), comparePtrToEdge);
    conn_other_proc = edges_conn_other_proc[0];
  }
  
}

void SpanningTree::return_list_nodes(SendData &sd) {
  for (int i = 0; i < nodes.size(); i++) {
    sd.nodes[i] = nodes[i].vertex;
  }
}

void SpanningTree::return_list_edges_nodes1(SendData &sd) {
  for (int i = 0; i < edges.size(); i++) {
    sd.nodes1[i] = edges[i]->node1.vertex;
  }
}

void SpanningTree::return_list_edges_nodes2(SendData &sd) {
  for (int i = 0; i < edges.size(); i++) {
    sd.nodes2[i] = edges[i]->node2.vertex;
  }
}

void SpanningTree::return_list_edges_connections(SendData &sd) {
  for (int i = 0; i < edges.size(); i++) {
    sd.connections[i] = edges[i]->connection;
  }
}

void fill_final_spanning_trees(vector<SpanningTree> &final_spanning_trees, SendData &rec_data, vector<Edge> &final_edges) {
  vector<Node> vec_nodes;
  for (int i = 0; i < rec_data.nodes_sz; i++) {
    vec_nodes.push_back(Node(rec_data.nodes[i]));
  }
  SpanningTree add_st(vec_nodes, {});
  for (int i = 0; i < rec_data.edges_sz; i++) {
    final_edges.push_back(Edge(rec_data.nodes1[i], rec_data.nodes2[i], rec_data.connections[i]));
    add_st.add_connection_edge(&(final_edges.back()));
  }
  if (rec_data.node1_conn != -1) {
    final_edges.push_back(Edge(rec_data.node1_conn, rec_data.node2_conn, rec_data.conn_val));
    add_st.edges.push_back(&(final_edges.back()));
  }
  final_spanning_trees.push_back(add_st);
}
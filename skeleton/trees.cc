#include <vector>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cstdlib>

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
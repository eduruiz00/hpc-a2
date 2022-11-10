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

vector<Edge> get_edges_from_node(Node node, vector<Edge> &edges)
{
  vector<Edge> edges_node;
  for (int ptr_edge : node.ptr_edges) {
    edges_node.push_back(edges[ptr_edge]);
  }
  return edges_node;
}

void SpanningTree::join_spanning_tree(SpanningTree st, Edge connection_edge)
{
  // Copy nodes
  for (Node node : st.nodes) {
    nodes.push_back(node);
  }

  // Copy edges
  for (Edge edge : st.edges) {
    edges.push_back(edge);
  }

  // Add edge
  edges.push_back(connection_edge);
}

bool node_is_not_inside(vector<Node> &nodes, Node ev_node)
{
  if (std::find(nodes.begin(), nodes.end(), ev_node) != nodes.end()) {
    return false;
  }
  return true;
}

bool edge_is_not_inside(vector<Edge> &edges, Edge ev_edge)
{
  if (std::find(edges.begin(), edges.end(), ev_edge) != edges.end()) {
    return false;
  }
  return true;
}

Edge SpanningTree::find_minimum_edge(vector<Edge> &edges, Node &node_connection)
{
  vector<Edge> edges_evaluated;
  for (Node node : nodes) {
    for (int ptr_edge : node.ptr_edges) {
      Edge ev_edge = edges[ptr_edge];
      if ((node_is_not_inside(nodes, ev_edge.node1) || node_is_not_inside(nodes, ev_edge.node2)) && edge_is_not_inside(edges_evaluated, ev_edge)) {
        edges_evaluated.push_back(ev_edge);
        
      }
    }
  }
  std::sort(edges_evaluated.begin(), edges_evaluated.end());
  if (node_is_not_inside(nodes, edges_evaluated[0].node1)) {
    node_connection = edges_evaluated[0].node1;
  } else {
    node_connection = edges_evaluated[0].node2;
  }
  return edges_evaluated[0];
}

void remove_spanning_tree_from_list(vector<SpanningTree> &spanning_trees, SpanningTree &remove_st)
{
  auto it = find(spanning_trees.begin(), spanning_trees.end(), remove_st);
  int idx = it - spanning_trees.begin();
  spanning_trees.erase(spanning_trees.begin() + idx);
}

SpanningTree find_spanning_tree_of_node(Node node, vector<SpanningTree> &spanning_trees)
{
  for (SpanningTree spanning_tree : spanning_trees) {
    if (node_is_not_inside(spanning_tree.nodes, node) == false) {
      return spanning_tree;
    }
  }
}

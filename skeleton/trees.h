#ifndef __TREES_H__
#define __TREES_H__

class Node {
    public:
        int vertex;
        vector<int> ptr_edges;
        Node(int vertex) : vertex(vertex) {};
        bool operator== (const Node &other) const {
            return vertex == other.vertex;
        };
};

class Edge {
    public:
        Node node1;
        Node node2;
        float connection;
        Edge(Node n1, Node n2, float conn) : node1(n1), node2(n2), connection(conn) {};
        bool operator< (const Edge &other) const {
            return connection < other.connection;
        };
        bool operator== (const Edge &other) const {
            return (node1 == other.node1) && (node2 == other.node2);
        }
};

class SpanningTree {
    public:
        vector<Node> nodes;
        vector<Edge> edges;
        SpanningTree(vector<Node> nodes) : nodes(nodes) {};
        void join_spanning_tree(SpanningTree st, Edge connection_edge);
        Edge find_minimum_edge(vector<Edge> &edges, Node &node_connection);
        bool operator== (const SpanningTree &other) const {
            return (nodes[0] == other.nodes[0]) && (nodes.size() == other.nodes.size());
        }
};

vector<Edge> get_edges_from_node(Node node, vector<Edge> &edges);
void remove_spanning_tree_from_list(vector<SpanningTree> &spanning_trees, SpanningTree &remove_st);
SpanningTree find_spanning_tree_of_node(Node node, vector<SpanningTree> &spanning_trees);

#endif /* __TREES_H__ */
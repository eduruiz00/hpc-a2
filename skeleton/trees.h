#ifndef __TREES_H__
#define __TREES_H__

#define SZ 200000

typedef struct SendData {
  int node1_conn = 0;
  int node2_conn = 0;
  float conn_val = 0;
  int nodes_sz = 0;
  int edges_sz = 0;
  int nodes[SZ] = {0};
  int nodes1[SZ] = {0};
  int nodes2[SZ] = {0};
  float connections[SZ] = {0};
} send;

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
        vector<Edge*> edges;
        Edge* conn_other_proc;
        void add_connection_edge(Edge* add_edge);
        SpanningTree(vector<Node> nodes, vector<Edge*> edges) : nodes(nodes), edges(edges) {};
        void join_spanning_tree(SpanningTree &st);
        Edge* find_minimum_edge(Node &node_connection);
        void lowest_connection_st_other_processors(vector<SpanningTree> &spanning_trees);
        vector<Edge*> get_connection_edges();
        void return_list_nodes(SendData &sd);
        void return_list_edges_nodes1(SendData &sd);
        void return_list_edges_nodes2(SendData &sd);
        void return_list_edges_connections(SendData &sd);
        bool operator== (const SpanningTree &other) const {
            return (nodes[0] == other.nodes[0]) && (nodes.size() == other.nodes.size()) && nodes.back() == other.nodes.back();
        }
    private:
        vector<Edge*> connection_edges_;

};

bool comparePtrToEdge(Edge* a, Edge* b);
vector<Edge*> get_edges_from_node(Node node, vector<Edge> &edges);
int remove_spanning_tree_from_list(vector<SpanningTree> &spanning_trees, SpanningTree &remove_st);
SpanningTree *find_spanning_tree_of_node(Node node, vector<SpanningTree*> &alloc_spanning_trees);
int find_index_spanning_tree(vector<SpanningTree*> alloc_spanning_trees, SpanningTree* connectionSpanningTreePtr);
void fill_final_spanning_trees(vector<SpanningTree> &final_spanning_trees, SendData &rec_data, vector<Edge> &final_edges);

#endif /* __TREES_H__ */
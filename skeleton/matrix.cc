#include <vector>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

#include "trees.h"
#include "matrix.h"

extern "C" {
#include "mmio.h"
}

/*
 * Load matrix market file
 */

struct Element
{
  int row, col;
  double val;

  Element(int row, int col, double val) : row(row), col(col), val(val) { }
};

inline bool operator==(const struct Element &a, const struct Element &b)
{
  return a.row == b.row && a.col == b.col && a.val == b.val;
}

inline bool operator!=(const struct Element &a, const struct Element &b)
{
  return !operator==(a, b);
}

inline bool operator<(const struct Element &a, const struct Element &b)
{
  if (a.row < b.row)
    return true;

  if (a.row == b.row && a.col < b.col)
    return true;

  return false;
}



static bool
read_matrix_market(const char *filename,
                   int &n_rows, int &n_cols,
                   vector<Node> &nodes,
                   vector<Edge> &edges,
                   int &my_rank,
                   int &n_proc,
                   vector<SpanningTree> &spanning_trees)
{
  FILE *fh = fopen(filename, "r");
  if (!fh)
    {
      perror("fopen");
      return false;
    }

  fprintf(stderr, "Reading matrix '%s'...\n", filename);

  MM_typecode matcode;
  if (mm_read_banner(fh, &matcode) != 0)
    {
      fprintf(stderr, "mm_read_banner failed\n");
      return false;
    }

  int M, N, nz;
  int ret_code;
  ret_code = mm_read_mtx_crd_size(fh, &M, &N, &nz);
  if (ret_code != 0)
    {
      fprintf(stderr, "mm_read_mtx_crd_size failed\n");
      return false;
    }

  n_rows = M;
  n_cols = N;

  int n_elem_proc = n_rows/n_proc;
  int first_element = n_elem_proc*(my_rank-1);
  int last_element = n_elem_proc*my_rank - 1;
  if (my_rank == n_proc-1) {
    last_element = n_rows - 1;
  }


  for (int i = 0; i < n_rows; i++) { // Starting with index 0
    nodes.push_back(Node(i));
  }

  for (int i = 0; i < nz; i++)
    {
      int row, col;
      double val;

      if (mm_is_pattern(matcode))
        {
          fscanf(fh, "%d %d\n", &row, &col);
          val = 1.0;
        }
      else
        fscanf(fh, "%d %d %lg\n", &row, &col, &val);

      row--; /* adjust from 1-based to 0-based */
      col--;

      bool cond1 = (row >= first_element) && (row <= last_element);
      bool cond2 = (col >= first_element) && (col <= last_element) && (cond1 == false);
      bool verif = (cond1 && !cond2) || (cond2 && !cond1);
      if (col != row && verif) {
        Node &node1 = nodes[col];
        Node &node2 = nodes[row];
        int size_edge = edges.size();
        edges.push_back(Edge(node1, node2, val));
        node1.ptr_edges.push_back(size_edge);
        node2.ptr_edges.push_back(size_edge);
      }
      //if (mm_is_symmetric(matcode) && row != col)
      //  elements.push_back(Element(col, row, val));
    }

  // Fill spanning trees with data
  for (int i = first_element; i <= last_element; i++) {
    spanning_trees.push_back(SpanningTree(vector<Node> {nodes[i]}, get_edges_from_node(nodes[i], edges)));
  }
  
  for (int i = 0; i < spanning_trees.size(); i++) {
    spanning_trees[i].lowest_connection_st_other_processors(spanning_trees);
  }
  

  //std::sort(elements.begin(), elements.end());
  fclose(fh);

  

  return true;
}


bool
load_matrix_market(const char *filename,
                   int &n_rows, int &n_cols,
                   std::vector<Node> &nodes,
                   std::vector<Edge> &edges,
                   int &my_rank,
                   int &n_proc,
                   std::vector<SpanningTree> &spanning_trees)
{

  if (!read_matrix_market(filename, n_rows, n_cols, nodes, edges, my_rank, n_proc, spanning_trees))
    return false;

  fprintf(stderr, "import ok: %d x %d matrix\n",
         n_rows, n_cols);

  return true;
}

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "trees.h"

void dump_nonzeros(const int     n_rows,
                   const double  values[],
                   const int     col_ind[],
                   const int     row_ptr_begin[],
                   const int     row_ptr_end[]);

bool load_matrix_market(const char *filename,
                        int        &n_rows,
                        int        &n_cols,
                        std::vector<Node> &nodes,
                        std::vector<Edge> &edges);

#endif /* __MATRIX_H__ */

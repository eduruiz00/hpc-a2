#include <chrono>

#include <cstdio>

#include "matrix.h"


/* Global variables holding the matrix data. To complete this assignment
 * you are requested to only use arrays and access these arrays with
 * subscripts. Do not use pointers.
 */

const int max_n_elements = 131072;
const int max_n_rows = 16384;
const int test_vector_count = 5;

static double values[max_n_elements];

static int col_ind[max_n_elements];
static int row_ptr_begin[max_n_rows];
static int row_ptr_end[max_n_rows];




int
main(int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "usage: %s <filename>\n", argv[0]);
      return -1;
    }

  int nnz, n_rows, n_cols;
  bool ok(false);

  ok = load_matrix_market(argv[1], max_n_elements, max_n_rows,
                          nnz, n_rows, n_cols,
                          values, col_ind, row_ptr_begin, row_ptr_end);
  if (!ok)
    {
      fprintf(stderr, "failed to load matrix.\n");
      return -1;
    }

  /* For debugging, can be removed when implementation is finished. */
  dump_nonzeros(n_rows, values, col_ind, row_ptr_begin, row_ptr_end);


  auto factorization_start_time = std::chrono::high_resolution_clock::now();

  /* Perform LU factorization here */

  auto factorization_end_time = std::chrono::high_resolution_clock::now();
  
  
  auto solve_start_time = std::chrono::high_resolution_clock::now();
  
  /* Compute all 5 solution vectors here */

  auto solve_end_time = std::chrono::high_resolution_clock::now();
  
  
  double relative_errors[test_vector_count] = {0};
  
  /* Compute relative errors here */
  
  
  std::chrono::duration<double> factorization_elapsed_time = factorization_end_time - factorization_start_time;
  std::chrono::duration<double> solve_elapsed_time = solve_end_time - solve_start_time;
  
  
  /* Print results */
  fprintf(stdout, "%.20f\n", factorization_elapsed_time.count());
  fprintf(stdout, "%.20f\n", solve_elapsed_time.count());
  for (size_t vector_idx = 0; vector_idx < test_vector_count; ++vector_idx)
    {
      fprintf(stdout, "%.20f\n", relative_errors[vector_idx]);
    }

  return 0;
}

#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
  // Initialize MPI
  // This must always be called before any other MPI functions
  MPI_Init(&argc, &argv);

  // Get the number of processes in MPI_COMM_WORLD
  int world_size;
  
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of this process in MPI_COMM_WORLD
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  char r[10];
  if (my_rank == 0) {
    for (int i = 0; i < world_size; i++) {
        MPI_Recv(r, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "World Size: " << world_size << "   Rank: " << r << std::endl;
    }
  } else {
    MPI_Send(r, 10, MPI_CHAR, 0, my_rank, MPI_COMM_WORLD);
  }

  // Print out information about MPI_COMM_WORLD
  

  // Finalize MPI
  // This must always be called after all other MPI functions
  MPI_Finalize();

  return 0;
}
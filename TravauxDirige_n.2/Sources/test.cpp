#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <cmath>
#include <vector>
#include <fstream>
#include <mpi.h>

int main(int nargs, char *argv[])
{

    MPI_Init(&nargs, &argv);
    // Pour des raison préfère toujours cloner le communicateur global
    // MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    // On interroge le communicateur global pour connaître le nombre de processus
    // qui ont été lancés par l'utilisateur :
    int nbp;
    MPI_Comm_size(globComm, &nbp);
    // On interroge le communicateur global pour connaître l'identifiant qui
    // m'a été attribué ( en tant que processus ). Cet identifiant est compris
    // entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
    // l'utilisateur )
    int rank;
    MPI_Comm_rank(globComm, &rank);

    int sent_token[] = {1, 2};
    int root = 0;

    if (rank != root)
        MPI_Gather(&sent_token, 2, MPI_INT, NULL, 0, MPI_INT, root, globComm);

    if (rank == root)
    {
        int buffer[nbp * 2];
        MPI_Gather(&sent_token, 2, MPI_INT, buffer, 2, MPI_INT, root, globComm);
        std::cout << buffer[0] << " " << buffer[3] << std::endl;
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
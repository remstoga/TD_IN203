#include <cstdlib>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <mpi.h>

int main(int nargs, char *argv[])
{
	int jeton = 42;
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...
	MPI_Init(&nargs, &argv);
	// Pour des raisons de portabilité qui débordent largement du cadre
	// de ce cours, on préfère toujours cloner le communicateur global
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

	int d = log2(nbp);

	bool did_receive = false;

	MPI_Status status;

	int jeton_recu = 0;
	if (rank == 0)
	{
		for (int i = 0; i < d; i++)
		{
			MPI_Send(&jeton, 1, MPI_INT, rank + pow(2, i), 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		for (int i = 0; i < d; i++)
		{

			if (rank < pow(2, i) && !did_receive)
			{
				MPI_Recv(&jeton_recu, 1, MPI_INT, rank - pow(2, i - 1), 0, MPI_COMM_WORLD, &status);
				std::cout << "rank :" << rank << " jeton reçu :" << jeton_recu << " de " << rank - pow(2, i - 1) << std::endl;
				did_receive = true;
				std::cout << "rank : " << rank << "envoie " << jeton_recu << " a " << rank + pow(2, i) << std::endl;
				MPI_Send(&jeton_recu, 1, MPI_INT, rank + pow(2, i), 0, MPI_COMM_WORLD);
			}
			else if (rank < pow(2, i))
			{
				std::cout << "rank : " << rank << "envoie " << jeton_recu << " a " << rank + pow(2, i) << std::endl;
				MPI_Send(&jeton_recu, 1, MPI_INT, rank + pow(2, i), 0, MPI_COMM_WORLD);
			}
		}
		if (rank > pow(2, d - 1))
		{
			MPI_Recv(&jeton_recu, 1, MPI_INT, rank - pow(2, d - 1), 0, MPI_COMM_WORLD, &status);
			std::cout << "rank :" << rank << " jeton reçu :" << jeton_recu << " de " << rank - pow(2, d - 1) << std::endl;
		}
	}
	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

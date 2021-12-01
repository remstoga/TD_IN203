#include <iostream>
#include <cstdlib>
#include <mpi.h>

int main(int nargs, char *argv[])
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...

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

	int jeton_send;
	int jeton_recv;
	MPI_Status status;
	if (rank == 0)
	{
		jeton_send = 0;
		MPI_Send(&jeton_send, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&jeton_recv, 1, MPI_INT, nbp - 1, 0, MPI_COMM_WORLD, &status);
		std::cout << "Jeton reçu par le processus de rang " << rank<< ": "<< jeton_recv << std::endl;
	}
	else if (rank == nbp - 1)
	{
		MPI_Recv(&jeton_recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		jeton_send = jeton_recv + 1;
		std::cout << "Jeton reçu par le processus de rang " << rank<< ": "<<jeton_send << std::endl;
		MPI_Send(&jeton_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(&jeton_recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		jeton_send = jeton_recv + 1;
		std::cout << "Jeton reçu par le processus de rang " << rank<< ": "<< jeton_send << std::endl;
		MPI_Send(&jeton_send, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	}

	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

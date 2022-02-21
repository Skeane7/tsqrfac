/** 
 * @file tsqr.c 
 * @brief Program to implement communication avoiding
 * 	  factorisation of a tall narrow matrix
 * @author Shane Keane 17329836
 * @version 1.0
 * @date 17/02/2022
 **/ 

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <mpi.h>

unsigned long get_seed_from_random_device(void);
void populate_random_matrix(double *const rngs, const int m, const int n, const long seed);
int decomp1d(int n, int p, int myid, int *s, int *e);

/* Function to get a random long from /dev/random */
unsigned long get_seed_from_random_device(void){
        unsigned long randomseed;
        FILE *fp;
        /*Opening /dev/random and error checking*/
        if((fp = fopen("/dev/random", "r")) == NULL){
                perror("Error opening /dev/random");
                exit(EXIT_FAILURE);
        }
        /*Reading in seed from /dev/random and error checking*/
        if(fread(&randomseed, sizeof(unsigned long), 1,fp) != 1){
                perror("Error reading from /dev/random");
                exit(EXIT_FAILURE);
        }
        fclose(fp);
        return abs(randomseed);
}


/* Function to populate matrix (m x n) with random numbers */
void populate_random_matrix(double *const rngs, const int m, const int n, const long seed){
        /*Setting seed for PRNG*/
        /*Intialising PRNG*/
        gsl_rng *r = gsl_rng_alloc(gsl_rng_cmrg);
        gsl_rng_set(r, seed);
        /*Setting vector elements equal to Gaussian random numbers*/
        for(int i=0;i<m*n;i++){
                	rngs[i] = gsl_rng_uniform(r);
	}
        gsl_rng_free(r);
}

/*Function to decompose an array of doubles of size n across p processes*/
int decomp1d(int n, int p, int myid, int *s, int *e){
        *s = (myid*n)/p + 1;
        *e = ((myid+1)*n)/p;
        if(*e<*s){
                return 1;
        }
        return 0;
}

/* Main function putting it all together */
int main(int argc, char **argv){
	int m=10, n=2; /*Size of matrix*/
	/* Initilising ranks and world size MPI */
	int rank, numproc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  	MPI_Comm_size(MPI_COMM_WORLD, &numproc);
	/* Initilising matrix */
	double *rvs = malloc(sizeof *rvs * (m*n));
	/* Getting seed from dev on process 1 and broadcasting to other processes */
	long seed; 
	if(rank == 0){
		seed = get_seed_from_random_device();
	}
	MPI_Bcast(&seed, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	/* Populate matrix on each process with identical random numbers */
	populate_random_matrix(rvs, m, n, seed);
	printf(" %f, %ld \n ", rvs[0], seed);
	/* Free memory and finalize processes */
	free(rvs);
	MPI_Finalize();
	return 0;
}

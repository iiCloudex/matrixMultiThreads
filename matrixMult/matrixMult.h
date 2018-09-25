/*
* Name: Marco Antonio Bustamante
* Lab/Task: Lab 4 Task 1
* Date: 9/20/18
*/

#ifndef __MATRIX_MULT_H_
#define __MATRIX_MULT_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define oops(errmsg, err) { perror(errmsg); exit(err); }
#define MAX_PAIRS 5

// struct for passing indices to the threads
struct matrixCell
{
   int i;
   int j;
   int k;
   int **a;
   int **b;
   int **c;
};

void *matrixThread(void *);
void allocateAndLoadMatrices(int ** *, int ** *, int ** *, int *, int *, int *);
void loadMatrix(int ** *, int, int);
pthread_t **alloc_tids(int, int);
void free_tids(pthread_t **, int);
pthread_t **multiply(int **, int **, int **, int, int, int);
void join(pthread_t **, int, int);
void displayMatrix(int **, int, int);
void freeMatrixCells(int **a, int m);

#endif
/*
* Name: Marco Antonio Bustamante
* Lab/Task: Lab 4 Task 1
* Date: 9/20/18
*/

#include "matrixMult.h"

int main(int argc, char *argv[])
{


    if ( (freopen(argv[1], "r", stdin)) == 0)
        oops("Cannot open the input file.\n", -1);

    struct matrixCell matrixPairs[MAX_PAIRS];

    int c;
    int index = 0;
    pthread_t **tids[MAX_PAIRS];

    while( ((c = getc(stdin)) != EOF) && index < MAX_PAIRS)
    {
        if(isdigit(c))
        {
            ungetc(c, stdin);

            //Pass the struct through instead
            allocateAndLoadMatrices(&(matrixPairs[index].a), &(matrixPairs[index].b), &(matrixPairs[index].c), &(matrixPairs[index].i), &(matrixPairs[index].k), &(matrixPairs[index].j));
            index++;
        }

    }

    //Do each step for the whole array before moving onto the next step

    for(int i = 0; i < index; i++)
        tids[i] = multiply(matrixPairs[i].a, matrixPairs[i].b, matrixPairs[i].c,
                                   matrixPairs[i].i, matrixPairs[i].k, matrixPairs[i].j);

    for(int i = 0; i < index; i++)
        join(tids[i], matrixPairs[i].i, matrixPairs[i].j);

    for(int i = 0; i < index; i++)
        free_tids(tids[i], matrixPairs[i].i);

    for(int i = 0; i < index; i++)
    {
        printf("\nMATRIX A%d\n", i+1);
        displayMatrix(matrixPairs[i].a, matrixPairs[i].i, matrixPairs[i].k);
        printf("\nMATRIX B%d\n", i+1);
        displayMatrix(matrixPairs[i].b, matrixPairs[i].k, matrixPairs[i].j);
        printf("\nMATRIX A%d x B%d\n", i+1, i+1);
        displayMatrix(matrixPairs[i].c, matrixPairs[i].i, matrixPairs[i].j);
    }

    for(int i = 0; i < index; i++)
    {
       freeMatrixCells(matrixPairs[i].a, matrixPairs[i].i);
       freeMatrixCells(matrixPairs[i].b, matrixPairs[i].k);
       freeMatrixCells(matrixPairs[i].c, matrixPairs[i].i);
    }

    return 0;
}

void *matrixThread(void *param)
{
    // map the parameter onto the structure
    struct matrixCell cell = *(struct matrixCell *) param;

    cell.c[cell.i][cell.j] = 0;

    for (int k = 0; k < cell.k; k++)
        cell.c[cell.i][cell.j] += ((cell.a[cell.i][k]) * (cell.b[k][cell.j]));

    free(param);
    pthread_exit(0);

    return NULL;
}

// takes pointers to two-dimensional matrices, so they can be allocated in here
// and used by the caller
void allocateAndLoadMatrices(int ***matrixA, int ***matrixB, int ***matrixC, int *m, int *k, int *n)
{
    if (scanf("%d %d %d", m, k, n) == 0)
        oops("Cannot read matrix sizes.\n", -2);

    *matrixA = (int **) malloc(sizeof(int *) * (*m));
    *matrixB = (int **) malloc(sizeof(int *) * (*k));
    *matrixC = (int **) malloc(sizeof(int *) * (*m));

    for (int rows = 0; rows < (*m); rows++)
        (*matrixA)[rows] = (int *) malloc(sizeof(int) * (*k));

    for (int rows = 0; rows < (*k); rows++)
        (*matrixB)[rows] = (int *) malloc(sizeof(int) * (*n));

    for (int rows = 0; rows < (*m); rows++)
        (*matrixC)[rows] = (int *) malloc(sizeof(int) * (*n));

    loadMatrix(matrixA, *m, *k);
    loadMatrix(matrixB, *k, *n);

}

void loadMatrix(int ***matrix, int m, int n)
{
    for (int rows = 0; rows < m; rows++)
    {
        for (int columns = 0; columns < n; columns++)
            scanf("%d", &((*matrix)[rows][columns]));
    }
}

pthread_t **multiply(int **matrixA, int **matrixB, int **matrixC, int m, int k, int n)
{
    pthread_t **tids = alloc_tids(m, n);

    for (int rows = 0; rows < m; rows++)
    {
        for (int columns = 0; columns < n; columns++)
        {
            //Must be in here because each thread gets its own
            struct matrixCell *cell = malloc(sizeof(struct matrixCell));
            cell->j = columns;
            cell->a = matrixA;
            cell->b = matrixB;
            cell->c = matrixC;
            cell->k = k;
            cell->i = rows;
            if (pthread_create(&tids[rows][columns], NULL, matrixThread, (void *) cell) != 0)
            oops("Failed to create thread\n", 1);
        }
    }

    return tids;
}

pthread_t **alloc_tids(int m, int n)
{
    pthread_t **tids;

    //Allocate memory for 2D array of pthreads
    tids = (pthread_t **) malloc(sizeof(pthread_t *) * m);

    for (int rows = 0; rows < m; rows++)
        tids[rows] = (pthread_t *) malloc(sizeof(pthread_t) * n);

    return tids;
}

void free_tids(pthread_t **tids, int m)
{
    //Only need to free row by row
    for (int rows = 0; rows < m; rows++)
        free(tids[rows]);
}

void join(pthread_t **tids, int m, int n)
{
    //Join each thread in the 2D array

    for (int rows = 0; rows < m; rows++)
    {
        for (int columns = 0; columns < n; columns++)
            pthread_join(tids[rows][columns], NULL);
    }
}

void displayMatrix(int **matrix, int m, int n)
{
    for (int rows = 0; rows < m; rows++)
    {
        for (int columns = 0; columns < n; columns++)
        {
            printf("%d ", matrix[rows][columns]);
        }
        printf("\n");
    }
}

void freeMatrixCells(int **a, int m)
{
   for (int rows = 0; rows < m; rows++)
       free(a[rows]);

   free(a);
}
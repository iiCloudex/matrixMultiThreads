/*
* Name: Marco Antonio Bustamante
* Lab/Task: Lab 4 Task 1
* Date: 9/20/18
*/

#include "matrixMult.h"
#define MAX_PAIRS 5

int main(int argc, char *argv[])
{
    if (freopen(argv[1], "r", stdin) == 0)
      oops("Cannot open the input file.\n", -1);

    int **a1, **b1, **c1, **a2, **b2, **c2; // matrices
    int m1, k1, n1, m2, k2, n2; // dimensions of the matices m x k and k x m/

    allocateAndLoadMatrices(&a1, &b1, &c1, &m1, &k1, &n1);
    loadMatrix(&a1, m1, k1);
    loadMatrix(&b1, k1, n1);

    allocateAndLoadMatrices(&a2, &b2, &c2, &m2, &k2, &n2);
    loadMatrix(&a2, m2, k2);
    loadMatrix(&b2, k2, n2);

    pthread_t **tid1s = multiply(a1, b1, c1, m1, k1, n1);
    pthread_t **tid2s = multiply(a2, b2, c2, m2, k2, n2);

    join(tid1s, m1, n1);
    join(tid2s, m2, n2);
    
    free_tids(tid1s, m1);
    free_tids(tid2s, m2);


    //Dispaly results of matrix multiplication

    printf("\nMATRIX A1\n");
    displayMatrix(a1, m1, k1);
    printf("\nMATRIX B1\n");
    displayMatrix(b1, k1, n1);
    printf("\nMATRIX A1 x B1\n");
    displayMatrix(c1, m1, n1);

    printf("\nMATRIX A2\n");
    displayMatrix(a2, m2, k2);
    printf("\nMATRIX B2\n");
    displayMatrix(b2, k2, n2);
    printf("\nMATRIX A2 x B2\n");
    displayMatrix(c2, m2, n2);

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
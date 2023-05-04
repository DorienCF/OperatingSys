// Dorien C.F @02978765 | GTG
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define MAX 4 

int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

void fillMatrix(int matrix[MAX][MAX])
{
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX])
{
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args)
{
    int* x = (int*) args;
    int row = (*x) / MAX;
    int col = (*x) % MAX;
    matSumResult[row][col] = matA[row][col] + matB[row][col];
    free(x);
    pthread_exit(NULL);
}

void* computeDiff(void* args)
{
    int* x = (int*) args;
    int row = (*x) / MAX;
    int col = (*x) % MAX;
    matDiffResult[row][col] = matA[row][col] - matB[row][col];
    free(x);
    pthread_exit(NULL);
}

void* computeProduct(void* args) 
{
    int* x = (int*) args;
    int row = (*x) / MAX;
    int col = (*x) % MAX;
    matProductResult[row][col] = matA[row][col] * matB[row][col];
    free(x);
    pthread_exit(NULL);
}

int main() 
{
    typedef void*(*Operations)(void*);

    srand(time(0));
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread_t objects for our threads.
    pthread_t threads[MAX*MAX*3];

    // 4. Create a thread for each cell of each matrix operation.
    Operations MatrixOperations[3] = {computeSum, computeDiff, computeProduct};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < MAX*MAX; j++) {
            int* index = (int*) malloc(sizeof(int));
            *index = j;
            pthread_create(&threads[i*MAX*MAX + j], NULL, MatrixOperations[i], (void*) index);
        }
    }

    // 5. Wait for all threads to finish.
    for (int i = 0; i < MAX*MAX*3; i++) {
        pthread_join(threads[i], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}

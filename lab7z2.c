#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define DEFAULT_ITERATIONS 64
#define GRID_WIDTH 256
#define DIM 16

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main(int argc, char **argv)
{

    int global_grid[256] =
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // MPI Standard variable
    int proces;
    int ID, j;
    int iters = 0;
    int irer;

    // Setup number of iterations
    if (argc == 1)
    {
        irer = DEFAULT_ITERATIONS;
    }
    else if (argc == 2)
    {
        irer = atoi(argv[1]);
    }
    else
    {
        exit(1);
    }

    MPI_Status stat;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        printf("error");
    }

    MPI_Comm_size(MPI_COMM_WORLD, &proces);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);

    assert(DIM % proces == 0);

    int *arr = (int *)malloc(DIM * ((DIM / proces) + 2) * sizeof(int));
    for (iters = 0; iters < irer; iters++)
    {
        j = DIM;
        for (int i = ID * (GRID_WIDTH / proces); i < (ID + 1) * (GRID_WIDTH / proces); i++)
        {
            arr[j] = global_grid[i];
            j++;
        }

        if (proces != 1)
        {
            int incoming_1[DIM];
            int incoming_2[DIM];
            int send_1[DIM];
            int send_2[DIM];
            if (ID % 2 == 0)
            {
                for (int i = 0; i < DIM; i++)
                {
                    send_1[i] = arr[i + DIM];
                }
                MPI_Ssend(&send_1, DIM, MPI_INT, mod(ID - 1, proces), 1, MPI_COMM_WORLD);

                for (int i = 0; i < DIM; i++)
                {
                    send_2[i] = arr[(DIM * (DIM / proces)) + i];
                }
                MPI_Ssend(&send_2, DIM, MPI_INT, mod(ID + 1, proces), 1, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(&incoming_2, DIM, MPI_INT, mod(ID + 1, proces), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&incoming_1, DIM, MPI_INT, mod(ID - 1, proces), 1, MPI_COMM_WORLD, &stat);
            }
            if (ID % 2 == 0)
            {
                MPI_Recv(&incoming_2, DIM, MPI_INT, mod(ID + 1, proces), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&incoming_1, DIM, MPI_INT, mod(ID - 1, proces), 1, MPI_COMM_WORLD, &stat);
            }
            else
            {
                for (int i = 0; i < DIM; i++)
                {
                    send_1[i] = arr[i + DIM];
                }
                MPI_Ssend(&send_1, DIM, MPI_INT, mod(ID - 1, proces), 1, MPI_COMM_WORLD);

                for (int i = 0; i < DIM; i++)
                {
                    send_2[i] = arr[(DIM * (DIM / proces)) + i];
                }
                MPI_Ssend(&send_2, DIM, MPI_INT, mod(ID + 1, proces), 1, MPI_COMM_WORLD);
            }
            for (int i = 0; i < DIM; i++)
            {
                arr[i] = incoming_1[i];
                arr[(DIM * ((DIM / proces) + 1)) + i] = incoming_2[i];
            }
        }
        else
        {
            for (int i = 0; i < DIM; i++)
            {
                arr[i + GRID_WIDTH + DIM] = global_grid[i];
            }
            for (int i = GRID_WIDTH; i < GRID_WIDTH + DIM; i++)
            {
                arr[i - GRID_WIDTH] = global_grid[i - DIM];
            }
        }
        int * final = (int *)malloc(DIM * ((DIM / proces)) * sizeof(int));

        for (int k = DIM; k < DIM * ((DIM / proces) + 1); k++)
        {
            int total_rows = DIM * (DIM / proces) + 2;
            int r = k / DIM;
            int c = k % DIM;
            int prev_r = mod(r - 1, total_rows);
            int prev_c = mod(c - 1, DIM);
            int next_r = mod(r + 1, total_rows);
            int next_c = mod(c + 1, DIM);

            int count = arr[prev_r * DIM + prev_c] + arr[prev_r * DIM + c] + arr[prev_r * DIM + next_c] + arr[r * DIM + prev_c] + arr[r * DIM + next_c] + arr[next_r * DIM + prev_c] + arr[next_r * DIM + c] + arr[next_r * DIM + next_c];
            if (arr[k] == 1)
            {
                if (count < 2)
                    final[k - DIM] = 0;
                else if (count > 3)
                    final[k - DIM] = 0;
                else
                    final[k - DIM] = 1;
            }
            else
            {
                if (count == 3)
                    final[k - DIM] = 1;
                else
                    final[k - DIM] = 0;
            }
        }

        j = 0;
        for (int i = ID * (GRID_WIDTH / proces); i < (ID + 1) * (GRID_WIDTH / proces); i++)
        {
            global_grid[i] = final[j];
            j++;
        }
        MPI_Gather(final, DIM * (DIM / proces), MPI_INT, &global_grid, DIM * (DIM / proces), MPI_INT, 0, MPI_COMM_WORLD);

        if (ID == 0)
        {
            printf("Iter %d: siatka:\n", iters);
            for (j = 0; j < GRID_WIDTH; j++)
            {
                if (j % DIM == 0)
                {
                    printf("\n");
                }
                printf("%d  ", global_grid[j]);
            }
            printf("\n");
        }
    }

    free(arr);
    MPI_Finalize();
}
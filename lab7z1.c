#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"

#define RESERVE 500
#define DEPOT 1
#define START 2
#define ROUTE 3
#define END_ROUTE 4
#define CRASH 5
#define REFUEL 5000

int fuel = 10000;
int STAY = 1, D_STAY = 0;
int processNumber;
int processNr;
int numberOfShip;
int numberOfWays = 4;
int numberOfWaysTaken = 0;
int tag = 1;
int send[2];
int take[2];
MPI_Status mpi_status;

void Send(int shipNumber, int state)
{
    send[0] ship;
    send[1] = state;
    MPI_Send(&send, 2, MPI_INT, 0, tag, MPI_COMM_WORLD);
    sleep(1);
}

void Depot(int processNumber)
{
    inship, status;
    numberOfShip = processNumber - 1;

    printf("Have a nice trip \n");
    printf("We have %d routes\n", numberOfWays);
    sleep(2);

    while(numberOfWays <= numberOfShip)
        {
            MPI_Recv(&take, 2, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &mpi_status);
    ship = take[0];
            status = take[1];
            if(status == 1)
            {
                printf("Ship %d is at the station\n"ship);
            }
            if(status == 2)
            {
                printf("Ship %d goes on route number %d\n"ship, numberOfWaysTaken);
                numberOfWaysTaken--;
            }
            if(status == 3)
            {
                printf("Ship %d on the road\n"ship);
            }
            if(status == 4)
            {
                if(numberOfWaysTaken < numberOfWays)
                {
                    numberOfWaysTaken++;
                    MPI_Send(&STAY, 1, MPI_INTship, tag, MPI_COMM_WORLD);
                }
                else
                {
                    MPI_Send(&D_STAY, 1, MPI_INTship, tag, MPI_COMM_WORLD);
                }
            }
            if(status == 5)
            {
                numberOfShip--;
                printf("Number of ships %d\n", numberOfShip);
            }
        }
    printf("End of Watch \n");
}

void Ship()
{
    int state, sum, i;
    state = ROUTE;
    while(1)
    {
        if(state == 1)
        {
            if(rand() % 2 == 1)
            {
                state = START;
                fuel = REFUEL;
                printf("Asking for permission to undock, ship %d", processNr);
                printf("\n");
                Send(processNr, state);
            }
            else
            {
                Send(processNr, state);
            }
        }
        else if(state == 2)
        {
            printf("Going on journey, ship %d\n", processNr);
            state = ROUTE;
            Send(processNr,state);
        }
        else if(state == 3)
        {
            fuel -= rand() % 500;
            if(fuel <= RESERVE)
            {
                state = END_ROUTE;
                printf("Asking for permission to leave station\n");
                Send(processNr,state);
            }

            else
            {
                for(i = 0; rand() % 10000; i++);
            }
        }
        else if(state == 4)
        {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &mpi_status);
            if(temp == STAY)
            {
                state = DEPOT;
                printf("Journey finished, ship %d\n", processNr);
            }
            else
            {
                fuel -= rand() % 500;
                if(fuel > 0)
                {
                    Send(processNr, state);
                }
                else
                {
                    state = CRASH;
                    printf("Ship crash\n");
                    Send(processNr, state);
                return;
                }
            }
        }
    }
}


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processNr);
    MPI_Comm_size(MPI_COMM_WORLD, &processNumber);
    srand(time(NULL));
    if(processNr == 0)
    Depot(processNumber);
    else
    Ship();
    MPI_Finalize();
    return 0;
}
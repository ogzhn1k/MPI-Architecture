/*
* @Author Oguzhan BIRK, Musa Ogural
*/

/*
*  Run the MPI program
*  mpiexec -n 4 MPItest.exe 
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define IMAGE_WIDTH  15
#define IMAGE_HEIGHT 15
#define NUM_THREADS  8
#define PATTERN_SIZE 5
#define NUMBER_OF_MAX_MATCHES ((IMAGE_HEIGHT - PATTERN_SIZE + 1) * (IMAGE_WIDTH - PATTERN_SIZE + 1))

typedef struct MatchedCoor
{
    int x;
    int y;
} MatchCoordinates;

// Define your 5x5 patterns A and B (replace with actual patterns)
unsigned short patternA[PATTERN_SIZE][PATTERN_SIZE] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

unsigned short patternB[PATTERN_SIZE][PATTERN_SIZE] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}
};

unsigned short sampleImage[IMAGE_HEIGHT][IMAGE_WIDTH] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void displayImage(unsigned short image[][IMAGE_WIDTH])
{
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            printf("%d\t", image[i][j]);
        }
        printf("\n");
    }
}

// Function to perform the pattern matching
void searchPatterns(unsigned short image[][IMAGE_WIDTH], int start_row, int end_row, int rank) {
    int matchA, matchB;
    MatchCoordinates mA;
    MatchCoordinates mB;
    MatchCoordinates matchArrA[NUMBER_OF_MAX_MATCHES];
    MatchCoordinates matchArrB[NUMBER_OF_MAX_MATCHES];
    int aMatchIndexCounter = 0;
    int bMatchIndexCounter = 0;

    printf("************************************************\n");
    printf("Parallel\n");
    printf("************************************************\n");
    // Loop over the image to find matches within the assigned rows
    for (int i = start_row; i < end_row; i++) {
        for (int j = 2; j < IMAGE_WIDTH - 2; j++) {
            // Check for a match with pattern A
            matchA = 1;
            for (int m = 0; m < 5; m++) {
                for (int n = 0; n < 5; n++) {
                    if (image[i + m - 2][j + n - 2] != patternA[m][n]) {
                        matchA = 0;
                        break;
                    }
                }
                if (!matchA) break;
            }

            // Print the result for a match with pattern A
            if (matchA) {
                printf("Process %d found a match with pattern A at (%d, %d)\n", rank, (i - 2), (j - 2));
                if ((j - 2) >= (i - 2))
                {
                    matchArrA[aMatchIndexCounter].x = (j - 2);
                    matchArrA[aMatchIndexCounter].y = (i - 2);
                    aMatchIndexCounter++;
                }
            }

            // Check for a match with pattern B
            matchB = 1;
            for (int m = 0; m < 5; m++) {
                for (int n = 0; n < 5; n++) {
                    if (image[i + m - 2][j + n - 2] != patternB[m][n]) {
                        matchB = 0;
                        break;
                    }
                }
                if (!matchB) break;
            }

            // Print the result for a match with pattern B
            if (matchB) {
                printf("Process %d found b match with pattern B at (%d, %d)\n", rank, (i - 2), (j - 2));
                if ((j - 2) <= (i - 2))
                {
                    matchArrB[bMatchIndexCounter].x = (j - 2);
                    matchArrB[bMatchIndexCounter].y = (i - 2);
                    bMatchIndexCounter++;
                }
            }
        }
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        printf("Pattern A Match Coordinates : (%d,%d)\n", matchArrA[i].x, matchArrA[i].y);
    }

    for (size_t i = 0; i < bMatchIndexCounter; i++)
    {
        printf("Pattern B Match Coordinates : (%d,%d)\n", matchArrB[i].x, matchArrB[i].y);
    }


    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        for (size_t j = 0; j < bMatchIndexCounter; j++)
        {
            if ((matchArrA[i].x == matchArrB[j].y) && (matchArrA[i].y == matchArrB[j].x))
            {
                printf("Patterns are matched ---> (%d,%d) - (%d,%d)\n", matchArrA[i].x, matchArrA[j].y, matchArrB[j].x, matchArrB[i].y);
            }
        }
    }

    printf("************************************************\n\n\n");

    // Implement logic for handling results from different processes
}

// Function to perform the pattern matching
void searchPatternsSerial(unsigned short image[][IMAGE_WIDTH]) {
    int matchA, matchB;
    MatchCoordinates mA;
    MatchCoordinates mB;
    MatchCoordinates matchArrA[NUMBER_OF_MAX_MATCHES];
    MatchCoordinates matchArrB[NUMBER_OF_MAX_MATCHES];
    int aMatchIndexCounter = 0;
    int bMatchIndexCounter = 0;

    printf("************************************************\n");
    printf("Parallel\n");
    printf("************************************************\n");
    // Loop over the image to find matches within the assigned rows
    for (int i = 2; i < IMAGE_HEIGHT - 2; i++) {
        for (int j = 2; j < IMAGE_WIDTH - 2; j++) {
            // Check for a match with pattern A
            matchA = 1;
            for (int m = 0; m < 5; m++) {
                for (int n = 0; n < 5; n++) {
                    if (image[i + m - 2][j + n - 2] != patternA[m][n]) {
                        matchA = 0;
                        break;
                    }
                }
                if (!matchA) break;
            }

            // Print the result for a match with pattern A
            if (matchA) {
                printf("Found a match with pattern A at (%d, %d)\n", (i - 2), (j - 2));
                if ((j - 2) >= (i - 2))
                {
                    matchArrA[aMatchIndexCounter].x = (j - 2);
                    matchArrA[aMatchIndexCounter].y = (i - 2);
                    aMatchIndexCounter++;
                }
            }

            // Check for a match with pattern B
            matchB = 1;
            for (int m = 0; m < 5; m++) {
                for (int n = 0; n < 5; n++) {
                    if (image[i + m - 2][j + n - 2] != patternB[m][n]) {
                        matchB = 0;
                        break;
                    }
                }
                if (!matchB) break;
            }

            // Print the result for a match with pattern B
            if (matchB) {
                printf("Found b match with pattern B at (%d, %d)\n",(i - 2), (j - 2));
                if ((j - 2) <= (i - 2))
                {
                    matchArrB[bMatchIndexCounter].x = (j - 2);
                    matchArrB[bMatchIndexCounter].y = (i - 2);
                    bMatchIndexCounter++;
                }
            }
        }
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        printf("Pattern A Match Coordinates : (%d,%d)\n", matchArrA[i].x, matchArrA[i].y);
    }

    for (size_t i = 0; i < bMatchIndexCounter; i++)
    {
        printf("Pattern B Match Coordinates : (%d,%d)\n", matchArrB[i].x, matchArrB[i].y);
    }

    for (size_t i = 0; i < aMatchIndexCounter; i++)
    {
        for (size_t j = 0; j < bMatchIndexCounter; j++)
        {
            if ((matchArrA[i].x == matchArrB[j].y) && (matchArrA[i].y == matchArrB[j].x))
            {
                printf("Patterns are matched ---> (%d,%d) - (%d,%d)\n", matchArrA[i].x, matchArrA[j].y, matchArrB[j].x, matchArrB[i].y);
            }
        }
    }

    printf("************************************************\n\n\n");
    // Implement logic for handling results from different processes
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the total number of processes and the rank of the current process
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double parallel_start_time, parallel_end_time, parallel_elapsed_time;

    if (world_rank == 0) {
        // Master process loads the image
        // Replace this with your image loading logic
        /*loadImage(image);*/
        displayImage(sampleImage);
    }

    // Broadcast the image to all processes
    MPI_Bcast(sampleImage, IMAGE_HEIGHT * IMAGE_WIDTH, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);

    // Divide the work among processes
    int rows_per_process = IMAGE_HEIGHT / world_size;
    int start_row = world_rank * rows_per_process + 2;
    int end_row = (world_rank + 1) * rows_per_process + 2;

    MPI_Barrier(MPI_COMM_WORLD);

    parallel_start_time = MPI_Wtime();

    // Perform pattern matching
    searchPatterns(sampleImage, start_row, end_row, world_rank);

    parallel_end_time = MPI_Wtime();
    parallel_elapsed_time = parallel_end_time - parallel_start_time;

    // Implement logic for gathering results (if needed)
    double serial_start_time, serial_end_time, serial_elapsed_time;
    double speedup;
    double efficiency;

    serial_start_time = MPI_Wtime();
    searchPatternsSerial(sampleImage);
    serial_end_time = MPI_Wtime();

    // Finalize MPI
    MPI_Finalize();

    serial_elapsed_time = serial_end_time - serial_start_time;

    speedup = serial_elapsed_time / parallel_elapsed_time;
    efficiency = speedup / world_size;

    printf("w:%d\n",world_size);

    printf("Serial Elapsed Time: %f seconds\n", serial_elapsed_time);
    printf("Parallel Elapsed Time: %f seconds\n", parallel_elapsed_time);
    printf("Speedup: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}

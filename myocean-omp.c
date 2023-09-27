/*
myocean-onp.c
Vincent Broda
cs462
Assignment 2
This program is very similar to myocean.c, however it uses omp to help speed up the efficency of the average calculations.
It takes an additional threads command line argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

enum {BLACK, RED};

typedef struct Node
{
    struct Node* north;
    struct Node* south;
    struct Node* east;
    struct Node* west;
    double temp;
    int node_class;
} Node;


int main(int argc, char **argv)
{
    int yMax, xMax, steps, numThreads;
    int i, j, step;
    Node ***grid;   // Basically a 2d array that holds pointers to nodes
    int tmp_temp, tmp_class;
    Node *tmp_node;
    double avg;
    double start, end; 
    double convergenceEpsilon = 0.001;  // picked kinda arbitrarily, I feel like this is a small number but doesn't take to long. I also changed this often 
    double maxChange = 0;

    if(argc != 5) exit(1);

    xMax = atoi(argv[1]);
    yMax = atoi(argv[2]);
    steps = atoi(argv[3]);
    numThreads = atoi(argv[4]);

    // setting number of threads
    omp_set_num_threads(numThreads);

    // allocating memory, done in three parts
    grid = (Node***)malloc(yMax * sizeof(Node**));
    for(i = 0; i < yMax; i++) grid[i] = (Node**)malloc(xMax * sizeof(Node*));
    for(i = 0; i < yMax; i++)
    {
        for(j = 0; j < xMax; j++)
        {
            // allocate individual node
            grid[i][j] = (Node*)malloc(sizeof(Node));
        }
    }

    // making initializing cells and grid
    for(i = 0; i < yMax; i++)
    {
        for(j = 0; j < xMax; j++)
        {
            // get its starting temp, error check here to
            if (scanf("%d", &tmp_temp) != 1) 
            {
                printf("Invalid input, see read me.\n");
                return 1;
            }

            tmp_node = grid[i][j];
            tmp_node->temp = (double)tmp_temp;

            // Initialize edges, neighbors are null if the node isn't in range
            tmp_node->north = (i > 0) ? grid[i - 1][j] : NULL;
            tmp_node->south = (i < yMax - 1) ? grid[i + 1][j] : NULL;
            tmp_node->east = (j < xMax - 1) ? grid[i][j + 1] : NULL;
            tmp_node->west = (j > 0) ? grid[i][j - 1] : NULL;

            // setting Node as read or black. making assumption of size being 2^n + 2
            if((i + j) % 2 == 0) tmp_node->node_class = BLACK;
            else tmp_node->node_class = RED;
        }
    }

    // starting time
    start = omp_get_wtime();

    // parallel part about to begin
    for(step = 0; step < steps; step++) // not doing this loop in parralell since each step will effect the following one, I belive it could cause a diffrent result easily, but it would most likely still converge if mutexes are used properly
    {
        // setting if we are doing red or black
        if(step % 2 == 0) tmp_class = RED;
        else tmp_class = BLACK;
        // looping through grid
        maxChange = 0;
        // Parallel part, we are doing all of the average calculations as threads and using the reduction feture to help use check the convergence
        #pragma omp parallel for private(tmp_node, j, avg) reduction(max:maxChange) // shared(grid, tmp_class)
        for(i = 1; i < yMax - 1; i++)
        {
            for(j = 1; j < xMax - 1; j++)
            {
                //doing caclulation only if right step
                tmp_node = grid[i][j];
                if(tmp_class == tmp_node->node_class)
                {
                    avg = (tmp_node->north->temp + tmp_node->south->temp + tmp_node->east->temp + tmp_node->west->temp + tmp_node->temp) / 5.0;
                    double change = fabs(avg - tmp_node->temp);
                    if (change > maxChange) maxChange = change;
                    tmp_node->temp = avg;
                }
            }
        }
        
        // printing every 30 steps
        if(step % 30 == 0)
        {
            printf("\nStep: %d\n", step);
            if(xMax < 20)       // so terminal isnt flooded on bigger inputs
            {
                for(i = 0; i < yMax; i++)
                {
                    for(j = 0; j < xMax; j++)
                    {
                        tmp_node = grid[i][j];
                        printf("%.3lf ", tmp_node->temp);
                    }
                    printf("\n");
                }
            }
            printf("MAX CHANGE: %lf\n", maxChange);
            printf("\n");
        }

        if(maxChange <= convergenceEpsilon)
        {
            printf("Convergence within %lf achived in %d steps\n", convergenceEpsilon, step);
            break;
        }
    }
    end = omp_get_wtime(); 

    printf("Final:\n");
    for(i = 0; i < yMax; i++)
    {   // temperature
        for(j = 0; j < xMax; j++)
        {
            tmp_node = grid[i][j];
            printf("%f ", tmp_node->temp);
        }
        printf("\n");
    }

    printf("TIME %.5f s\n", end - start);

    // freeing memory
    for (i = 0; i < yMax; i++) {
        for (j = 0; j < xMax; j++) free(grid[i][j]);
        free(grid[i]);
    }
    free(grid);
    return 0;
}

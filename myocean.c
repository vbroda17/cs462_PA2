#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


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
    int yMax, xMax, steps;
    int i, j, step;
    Node ***grid;   // Basically a 2d array that holds pointers to nodes
    int tmp_temp, tmp_class;
    Node *tmp_node;
    double avg;
    double start, end; 

    // doing this so the input files can be easilt piped in
    if(argc != 4)
    {
        scanf("%d %d %d", &xMax, &yMax, &steps);
    }
    else
    {
        xMax = atoi(argv[1]);
        yMax = atoi(argv[2]);
        steps = atoi(argv[3]);
    }
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

    // making individual cells
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

   // serial traversing, red on even and black on odd steps.
    start = omp_get_wtime(); // timing this. 
    for(step = 0; step < steps; step++)
    {
        // setting if we are doing red or black
        if(step % 2 == 0) tmp_class = RED;
        else tmp_class = BLACK;
        // looping through grid
        for(i = 1; i < yMax - 1; i++)
        {
            for(j = 1; j < xMax - 1; j++)
            {
                //doing caclulation only if right step
                tmp_node = grid[i][j];
                if(tmp_class == tmp_node->node_class)
                {

                    if(tmp_node->north != NULL, tmp_node->south != NULL, tmp_node->east != NULL, tmp_node->west != NULL)
                    {
                        avg = (tmp_node->north->temp + tmp_node->south->temp + tmp_node->east->temp + tmp_node->west->temp + tmp_node->temp) / 5.0;
                        tmp_node->temp = avg;
                    }
                }
            }
        }
        
        // printing every 30 steps
        if(step % 30 == 0)
        //if(0 == 0) // this is if i want to check every step
        {
            printf("\nStep: %d\n", step);
            for(i = 0; i < yMax; i++)
            {   // temp
                for(j = 0; j < xMax; j++)
                {
                    tmp_node = grid[i][j];
                    printf("%.3lf ", tmp_node->temp);
                }
                printf("\n");
            }
            printf("\n");
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

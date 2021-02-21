#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "queue.h"

#define MAX_LINE 1024
#define DEF_ARR_SIZE 10
#define VALID_ARG_AMOUNT 4
#define DELIMS " \t\r\n"

/**
 * @brief Struct representing a vertex:
 */
typedef struct Vertex
{
    /**
     * @brief number representing vertex in graph
     */
    int id;
    /**
     * @brief flag for whether or not the vertex was visited in DFS/BFS
     */
    int visited;
    /**
     * @brief array of pointers that are the vertex' parents
     */
    struct Vertex **parents;
    /**
     * @brief amount of vertices that are the vertex' parents
     */
    int parentsAmount;
    /**
     * @brief size of array storing pointers of parents
     */
    int parentsArraySize;
    /**
     * @brief array of pointers that are the vertex' children
     */
    struct Vertex **children;
    /**
     * @brief amount of vertices that are the vertex' children
     */
    int childrenAmount;
    /**
     * @brief size of array storing pointers of children
     */
    int childrenArraySize;
    /**
     * @brief flag for whether or not vertex is the graph's root
     */
    int isRoot;
    /**
     * @brief distance of vertex from root
     */
    int dist;
    /**
     * @brief vertex that came before in the path from the root
     */
    struct Vertex *prev;
} Vertex;

/**
 * @brief Struct representing a graph
 */
typedef struct Graph
{
    /**
     * @brief array of pointers to vertices in graph
     */
    Vertex **vertices;
    /**
     * @brief amount of vertices in graph
     */
    int verticesAmount;
    /**
     * @brief amount of edges in graph
     */
    int edgesAmount;
    /**
     * @brief id of root vertex
     */
    int rootID;
} Graph;

/**
 * Create new vertex with num as ID.
 * Initializes parent/children array with default array sizes
 * @param num will serve as ID of vertex
 * @return pointer to new vertex
 */
Vertex *newVertex(const int num)
{
    Vertex *pv = (Vertex *) calloc(1, sizeof( Vertex));
    pv -> id = num;
    pv -> parents = (Vertex **) calloc(DEF_ARR_SIZE , sizeof(Vertex *));
    pv -> parentsAmount = 0;
    pv -> parentsArraySize = DEF_ARR_SIZE;
    pv -> children = (Vertex **) calloc(DEF_ARR_SIZE , sizeof(Vertex *));
    pv -> childrenAmount = 0;
    pv -> childrenArraySize = DEF_ARR_SIZE;
    pv -> visited = 0;
    pv -> isRoot = 0;
    pv -> dist = 0;
    return pv;
}

/**
 * Create new graph
 * @return pointer to graph
 */
Graph *newGraph()
{
    Graph *pg = (Graph *) calloc(1, sizeof(Graph));
    pg -> verticesAmount = 0;
    pg -> edgesAmount = 0;
    pg -> rootID = -1;
    return pg;
}

/**
 * Frees all the memory that was allocated to vertex
 * @param v pointer to pointer of vertex
 */
void freeVertex(Vertex **v)
{
    free((*v) -> children);
    free((*v) -> parents);
    free(*v);
}

/**
 * Frees all the memory that was allocated to graph
 * @param g pointer to pointer of graph
 */
void freeGraph(Graph **g)
{
    for(int i = 0; i < (*g) -> verticesAmount; i++)
    {
        freeVertex(&((*g) -> vertices[i]));
    }

    free((*g) -> vertices);
    free(*g);
}

/**
 * Count amount of lines in file
 * @param fp pointer to file
 * @return amount of lines
 */
int lineAmount(FILE *fp)
{
    int numOfLines = 0;
    char lastChar = '\0'; //Keeps track of last char we inspected

    for(char c = (char) fgetc(fp); c != EOF; )
    {
        if(c == '\n')
        {
            numOfLines++;
        }

        lastChar = c;
        c = (char) fgetc(fp);
    }

    rewind(fp); //bring pointer inside the file back to start

    /* We need to count the last line even without a linebreak */
    if(lastChar != '\n')
    {
        return (numOfLines + 1);
    }

    return numOfLines;
}

/**
 * Parse a string to an int
 * @param num string representing a number
 * @param result int to parse the number into
 * @return EXIT_SUCCESS if parse successful, EXIT_FAILURE otherwise
 */
int parseToInt(char *num, int *result)
{
    *result = 0;

    /* Get rid of '\r' and '\n' */
    for(unsigned long i = 0; i < strlen(num); i++)
    {
        if(num[i] == '\r' || num[i] == '\n')
        {
            num[i] = '\0';
        }
    }

    int size = (short) strlen(num);
    for(int i = 0; i < size; i++)
    {
        /* Char is not a digit */
        if(num[i] < '0' || num[i] > '9')
        {
            return EXIT_FAILURE;
        }

        char *temp;
        *result = (int) strtol(num, &temp, 10);
    }

    return EXIT_SUCCESS;
}

/**
 * Checks if arguments in argv are valid and parses them to fp, v1 and v2
 * @param argv should be path to file and two vertices
 * @param size amount of arguments
 * @param fp will hold pointer to data file
 * @param v1 will hold id of first vertex
 * @param v2 will hold id of second vertex
 * @return EXIT_FAILIURE if an error is found, otherwise EXIT_SUCCESS
 */
int areValidArgs(char **argv, int size, FILE **fp, int *v1, int *v2)
{
    for(int i = 1; i < size; i++)
    {
        switch(i)
        {
            case 1: //Should store path to file
                *fp = fopen(argv[i], "r");
                if(*fp == NULL)
                {
                    return EXIT_FAILURE;
                }
                break;

            case 2: //Should store number representing first vertex
                if(parseToInt(argv[i], v1) == EXIT_FAILURE || *v1 > (lineAmount(*fp) - 2) || *v1 < 0)
                {
                    return EXIT_FAILURE;
                }
                break;

            case 3: //Should store number representing second vertex
                if(parseToInt(argv[i], v2) == EXIT_FAILURE || *v2 > (lineAmount(*fp) - 2))
                {
                    return EXIT_FAILURE;
                }
                break;

            default:
                return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Makes sure arguments supplied in cmd are valid
 * @param argc amount of arguments
 * @param argv arguments
 * @param fp will hold pointer to valid file
 * @param v1 will hold id of first vertex
 * @param v2 will hold id of second vertex
 * @return EXIT_FAILURE if error is found, otherwise EXIT_SUCCESS
 */
int isValidInput(int argc, char **argv, FILE **fp, int *v1, int *v2)
{
    /* Must be four arguments including program name */
    if(argc != (VALID_ARG_AMOUNT))
    {
        fprintf(stderr, "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n");
        return EXIT_FAILURE;
    }

    if(areValidArgs(argv, VALID_ARG_AMOUNT, fp, v1, v2) == EXIT_FAILURE)
    {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Checks if line represents valid vertex + edges (whole numbers seperated by spaces)
 * Performs seperate check for first line, which must be the number of vertices
 * @param line line in file
 * @param lineNum line number
 * @param fileLength amount of lines in file
 * @return EXIT_FAILURE if invalid, EXIT_SUCCESS otherwise
 */
int isValidLine(char *line, int lineNum, int fileLength)
{
    int tempNum = 0;

    /* First line has to be a single number */
    if(lineNum == 1)
    {
        if(parseToInt(line, &tempNum) == EXIT_FAILURE || tempNum != (fileLength - 1))
        {
            return EXIT_FAILURE;
        }
    }
    else //Second line and further
    {
        char *value = NULL;

        value = strtok(line, DELIMS);
        if(value == NULL)
        {
            return EXIT_FAILURE;
        }

        while(value != NULL)
        {
            if(strlen(value) == 1 && line[0] == '-')
            {
                return EXIT_SUCCESS;
            }

            if(parseToInt(value, &tempNum) == EXIT_FAILURE || tempNum > (fileLength - 1))
            {
                return EXIT_FAILURE;
            }

            value = strtok(NULL, DELIMS);
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Links parent and child vertices by adding parent to list of child and adding
 * child to list of parent
 * @param parentID id of parent vertex
 * @param childID id of child vertex
 */
void linkVertices(Graph *graph, int parentID, int childID)
{
    Vertex *parent = graph -> vertices[parentID];
    Vertex *child = graph -> vertices[childID];

    /* Don't link parent/child if they are already linked */
    for(int i = 0; i < parent -> childrenAmount; i++)
    {
        if(parent -> children[i] -> id == child -> id)
        {
            return;
        }
    }

    for(int i = 0; i < child -> parentsAmount; i++)
    {
        if(child -> parents[i] -> id == parent -> id)
        {
            return;
        }
    }

    /* Ratios of members to array sizes of child/parent arrays in parent/child vertices */
    float pRatio = (float)(parent -> childrenAmount) / (float)(parent -> childrenArraySize);
    float cRatio = (float)(child -> parentsAmount) / (float)(child -> parentsArraySize);

    if(pRatio > 0.5)
    {
        parent -> children = realloc(parent -> children, parent -> childrenArraySize * 2 * sizeof(Vertex));
        parent -> childrenArraySize *= 2;
    }

    parent -> children[parent -> childrenAmount] = child;
    parent -> childrenAmount++;

    if(cRatio > 0.5)
    {
        child -> parents = realloc(child -> parents, child -> parentsArraySize * 2 * sizeof(Vertex));
        child -> parentsArraySize *= 2;
    }

    child -> parents[child -> parentsAmount] = parent;
    child -> parentsAmount++;
}

/**
 * Add data (line from file) to graph
 * @param data line of data from file
 * @param lineNum number of line in file
 * @param pg pointer to graph
 */
void addToGraph(char *data, int lineNum, Graph *pg)
{
    /* First line: initialize vertices array (now that we know the exact size) */
    if(lineNum == 1)
    {
        int vAmount = 0;
        parseToInt(data, &vAmount);
        pg -> vertices = (Vertex **) calloc(vAmount , sizeof(Vertex *));
        pg -> verticesAmount = vAmount;

        /* We'll be checking the members of the array later before actually
         * putting a vertex from the graph there, so we need to initialize
         * the array with some junk value vertex*/
        for(int i = 0; i < vAmount; i++)
        {
            pg -> vertices[i] = newVertex(-1);
        }
    }

    /* Other lines: add all vertices in line to graph and link to vertex represented by
     * the line number */
    if(lineNum > 1)
    {
        int idx = lineNum - 2;
        if(pg -> vertices[idx] -> id == -1)
        {
            freeVertex(&(pg -> vertices[idx])); //Free previously allocated junk
            pg -> vertices[idx] = newVertex(idx);
        }

        int newID = -1;
        char *reader = strtok(data, DELIMS);
        while(reader != NULL)
        {
            if(strlen(reader) == 1 && reader[0] == '-')
            {
                return;
            }

            parseToInt(reader, &newID);

            if(pg -> vertices[newID] -> id == -1)
            {
                freeVertex(&(pg -> vertices[newID]));
                pg -> vertices[newID] = newVertex(newID);
            }

            linkVertices(pg, idx, newID);

            reader = strtok(NULL, DELIMS);
        }
    }
}

/**
 * Make graph according to data in file. Assumes data is valid.
 * @param pg ponter to graph
 * @param pf pointer to file containing data
 * @return EXIT_FAILURE if an error occurs, otherwise EXIT_SUCCESS
 */
int makeGraph(Graph *pg, FILE *pf)
{
    char line[MAX_LINE]; //Char array that will store the line
    int fileLength = lineAmount(pf);
    int lineNum = 1;

    if(fileLength == 0)
    {
        return EXIT_FAILURE;
    }

    /* Read every line and add its data to the graph */
    while(fgets(line, MAX_LINE, pf))
    {
        char *res = calloc(strlen(line) + 1, sizeof(char));
        strcpy(res, line);

        if(isValidLine(line, lineNum, fileLength) == EXIT_FAILURE)
        {
            free(res);
            return EXIT_FAILURE;
        }

        addToGraph(res, lineNum, pg);

        lineNum++;

        free(res);
    }


    return EXIT_SUCCESS;
}

/**
 * Explore function for DFS
 * @param graph
 * @param id
 */
void explore(Graph *graph, int id)
{
    graph -> vertices[id] -> visited = 1;

    for(int i = 0; i < graph -> vertices[id] -> childrenAmount; i++)
    {
        if(graph -> vertices[id] -> children[i] -> visited == 0)
        {
            explore(graph, graph -> vertices[id] -> children[i] -> id);
        }
    }
}

/**
 * Check if graph is tree or not
 * @param graph pointer to graph
 * @return EXIT_FAILURE if not a tree, EXIT_SUCCESS otherwise
 */
int isTree(Graph *graph)
{
    /* Make sure that |E| = |V| - 1 */
    int numOfEdges = 0;

    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        numOfEdges += graph -> vertices[i] -> childrenAmount;
    }

    graph -> edgesAmount = numOfEdges;

    if((graph -> verticesAmount) - 1 != numOfEdges)
    {
        return EXIT_FAILURE;
    }

    /* Count all vertices with no parents */
    int orphanVertices = 0; //Count vertices with no parents
    int newRoot = -1; //ID of root, only used if orphanVertices == 1

    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        if(graph -> vertices[i] -> parentsAmount == 0)
        {
            newRoot = graph -> vertices[i] -> id;
            graph -> rootID = newRoot;
            graph -> vertices[i] -> isRoot = 1;
            orphanVertices++;
        }
    }

    if(orphanVertices != 1)
    {
        return EXIT_FAILURE;
    }

    /* Mark all vertices in same component as root as visited = 1 */
    explore(graph, newRoot);

    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        if(graph -> vertices[i] -> visited == 0)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Convert directed graph to non-directed graph
 * @param graph pointer to graph
 */
void convertToNonDirected(Graph *graph)
{
    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        for(int j = 0; j < graph -> vertices[i] -> childrenAmount; j++)
        {
            int id1 = graph -> vertices[i] -> children[j] -> id;
            int id2 = graph -> vertices[i] -> id;
            linkVertices(graph, id1, id2);
        }
    }
}

/**
 * Breadth First Search algorithm
 * @param graph
 */
void BFS(Graph *graph, Vertex *v)
{
    /* Assign amount of vertices as distance of all vertices (instead of infinity) */
    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        graph -> vertices[i] -> dist = graph -> verticesAmount;
    }

    v -> dist = 0;
    Vertex *pJunkVertex = newVertex(-1);
    pJunkVertex -> dist = 0;
    v -> prev = pJunkVertex;
    Queue *myQueue = allocQueue();
    enqueue(myQueue, v -> id);
    while(!queueIsEmpty(myQueue))
    {
        int id = (short) dequeue(myQueue);
        Vertex *u = graph -> vertices[id];

        for(int j = 0; j < u -> childrenAmount; j++)
        {
            if(u -> children[j] -> dist == graph -> verticesAmount)
            {
                enqueue(myQueue, u -> children[j] -> id);
                u -> children[j] -> prev = u;
                u -> children[j] -> dist = u -> dist + 1;
            }
        }
    }

    /* Free all memory alllocated inside this scope */
    freeQueue(&myQueue);
    free(pJunkVertex -> children);
    free(pJunkVertex -> parents);
    free(pJunkVertex);
}

/**
 * Returns length of shortest path from root
 * @param graph pointer to graph
 * @return distance of longest path
 */
int minDist(Graph *graph)
{
    /* Edge case: graph with one vertex  */
    if(graph -> verticesAmount == 1)
    {
        return 0;
    }

    int minDist = graph -> verticesAmount;

    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        if(graph -> vertices[i] -> isRoot == 1)
        {
            continue;
        }

        if(graph -> vertices[i] -> childrenAmount == 1 && graph -> vertices[i] -> parentsAmount == 1)
        {
            if(minDist > graph -> vertices[i] -> dist)
            {
                minDist = graph -> vertices[i] -> dist;
            }
        }
    }

    return minDist;
}

/**
 * Returns length of longest path from root
 * @param graph pointer to graph
 * @return length of longest path
 */
int maxDist(Graph *graph)
{
    int maxDist = 0;

    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        if(graph -> vertices[i] -> childrenAmount == 1 && graph -> vertices[i] -> parentsAmount == 1)
        {
            if(maxDist < graph -> vertices[i] -> dist)
            {
                maxDist = graph -> vertices[i] -> dist;
            }
        }
    }

    return maxDist;
}

/**
 * Returns longest possible path in graph by running BFS using
 * every vertex and returning the maximum value
 * @param graph pointer to graph
 * @return length of longest path in graph
 */
int graphDiameter(Graph *graph)
{
    int diameter = 0;
    for(int i = 0; i < graph -> verticesAmount; i++)
    {
        BFS(graph, graph -> vertices[i]);
        int temp = maxDist(graph);
        if(temp > diameter)
        {
            diameter = temp;
        }
    }

    return diameter;
}

/**
 * Stores path in graph from root vertex to finish vertex
 * @param finish target vertex
 * @param result path from root to finish
 * @param len size of path
 */
void getPath(Vertex *finish, int *result, int len)
{
    Vertex *tempV = finish;

    for(int i = 1; i <= len; i++)
    {
        result[len - i] = tempV -> id;
        tempV = tempV -> prev;
    }
}

/**
 * Main function for running the program
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
    /* Initialize some variables */
    FILE *pf = NULL;
    int v1 = 0;
    int v2 = 0;
    Graph *graph = newGraph();

    /* Check validity of input */
    if(isValidInput(argc, argv, &pf, &v1, &v2) == EXIT_FAILURE)
    {
        freeGraph(&graph);
        fclose(pf);
        return EXIT_FAILURE;
    }

    /* Construct graph from data in file */
    if(makeGraph(graph, pf) == EXIT_FAILURE)
    {
        fprintf(stderr, "Invalid input\n");
        freeGraph(&graph);
        fclose(pf);
        return EXIT_FAILURE;
    }

    /* Make sure graph is a tree */
    if(isTree(graph) == EXIT_FAILURE)
    {
        fprintf(stderr, "The given graph is not a tree\n");
        freeGraph(&graph);
        fclose(pf);
        return EXIT_FAILURE;
    }

    /* Convert graph to non-directed graph and calculate all
     * relevant information for output */
    convertToNonDirected(graph);
    BFS(graph, graph -> vertices[graph -> rootID]);

    int minimumDist = minDist(graph);
    int maximumDist = maxDist(graph);

    int diameter = graphDiameter(graph);

    BFS(graph, graph -> vertices[v1]);

    int size = graph -> vertices[v2] -> dist + 1;
    int *path = (int *) calloc(size , sizeof(int));
    getPath(graph -> vertices[v2] , path, size);

    /* Print result */
    printf("Root Vertex: %d\n", graph -> rootID);
    printf("Vertices Count: %d\n", graph -> verticesAmount);
    printf("Edges Count: %d\n", graph -> edgesAmount);
    printf("Length of Minimal Branch: %d\n", minimumDist);
    printf("Length of Maximal Branch: %d\n", maximumDist);
    printf("Diameter Length: %d\n", diameter);
    printf("Shortest Path Between %d and %d: ", v1, v2);
    for(int i = 0; i < size; i++)
    {
        printf("%d ", path[i]);
    }

    free(path);
    freeGraph(&graph);
    fclose(pf);

    return EXIT_SUCCESS;
}
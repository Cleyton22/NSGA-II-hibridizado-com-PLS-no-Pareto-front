#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#define NUM_OBJECTIVES 2
#define POPULATION_SIZE 80
#define TOURNAMENT_SIZE 4
#define GENERATION_AMOUNT 100
#define MUTATION_RATE 10
#define CROSSOVER_RATE 80

typedef struct {
    int vertex1; // Vértice 1 da aresta
    int vertex2; // Vértice 2 da aresta
    int objectives[NUM_OBJECTIVES]; // Array dos valores dos objetivos da aresta
} Edge;




typedef struct {
    Edge *edges;  // Array de arestas da solução
    int numEdges;  // Número de arestas na solução
    int totalObjective1; // Total do objetivo 1 da solução
    int totalObjective2; // Total do objetivo 2 da solução
    double crowdingDistance;  // Distância de aglomeração da solução
    bool exploration; // Indicador de vizinhança explorada
} Solution;




typedef struct {
    Solution *solutions;  // Array de soluções da população
    int numSolutions;  // Número de soluções na população
} Population;




typedef struct Node {
    Solution *solution;  // Solução armazenada no nó da lista
    struct Node *next;  // Ponteiro para o próximo nó na lista
} Node;




typedef struct {
    Node *head;  // Ponteiro para o primeiro nó na lista
    int size;  // Número de nós na lista
} List;




typedef struct {
    List *solutions;  // Lista de soluções do front
} Front;




typedef struct NodeFront {
    Front *front;  // Front armazenado no nó da lista de fronts
    struct NodeFront *next;  // Ponteiro para o próximo nó na lista de fronts
} NodeFront;




typedef struct {
    NodeFront *head;  // Ponteiro para o primeiro nó na lista de fronts
} ListFront;




typedef struct NodeDoubly {
    Solution *solution;  // Solução armazenada no nó da lista dupla
    struct NodeDoubly *next;  // Ponteiro para o próximo nó na lista dupla
    struct NodeDoubly *prev;  // Ponteiro para o nó anterior na lista dupla
} NodeDoubly;




typedef struct {
    NodeDoubly *head;  // Ponteiro para o primeiro nó na lista dupla
    int size;  // Número de nós na lista dupla
} ListDoubly;




// Função para receber uma solução e retornar a cópia dela
Solution* copySolution(Solution *originalSolution){
    Solution *copiedSolution = malloc(sizeof(Solution));
    copiedSolution->crowdingDistance = originalSolution->crowdingDistance;
    copiedSolution->exploration = originalSolution->exploration;
    copiedSolution->totalObjective1 = originalSolution->totalObjective1;
    copiedSolution->totalObjective2 = originalSolution->totalObjective2;

    copiedSolution->numEdges = originalSolution->numEdges;
    copiedSolution->edges = malloc(copiedSolution->numEdges * sizeof(Edge));
    for (int i = 0; i < copiedSolution->numEdges; i++){
        copiedSolution->edges[i] = originalSolution->edges[i];
    }

    return copiedSolution;
}




// Função para receber uma população de soluções e retornar a cópia dela
Population* copyPopulation(Population *originalPopulation){
    Population *copiedPopulation = malloc(sizeof(Population));
    copiedPopulation->numSolutions = originalPopulation->numSolutions;
    copiedPopulation->solutions = malloc(copiedPopulation->numSolutions * sizeof(Solution));

    for (int i = 0; i < copiedPopulation->numSolutions; i++){
        copiedPopulation->solutions[i] = *copySolution(&originalPopulation->solutions[i]);
    }

    return copiedPopulation;
}




// Função para criar uma lista
List *createList() {
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->size = 0;
    return list;
}




// Função para verificar se uma lista está vazia
int isListEmpty(List *list) {
    return list->size == 0;
}




// Função para adicionar uma solução a uma lista
Node* addToList(List *list, Solution *solution, Node *lastNode) {
    Node *node = malloc(sizeof(Node));
    node->solution = solution;
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
    } else {
        lastNode->next = node;
    }

    list->size++;
    return node;
}




// Função adaptada para adicionar uma solução a uma lista
Node* addToList2(List *list, Solution *solution, Node *lastNode) {
    Node *node = malloc(sizeof(Node));
    node->solution = copySolution(solution);
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
    } else {
        lastNode->next = node;
    }

    list->size++;
    return node;
}




// Função para remover uma solução de uma lista
void removeFromList(List *list, Solution *solution) {
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL && current->solution != solution) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current->solution->edges);
    free(current->solution);
    free(current);
    list->size--;
}




// Função adaptada para remover uma solução de uma lista
void removeFromList2(List *list, Solution *solution) {
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL && (current->solution->totalObjective1 != solution->totalObjective1 || current->solution->totalObjective2 != solution->totalObjective2)) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current->solution->edges);
    free(current->solution);
    free(current);
    list->size--;
}




// Função para criar uma lista de fronts
ListFront *createListFront() {
    ListFront *listOfFronts = malloc(sizeof(ListFront));
    listOfFronts->head = NULL;
    return listOfFronts;
}




// Função para adicionar um front a uma lista de fronts
NodeFront* addToListFront(ListFront *listFront, Front *front, NodeFront *lastNodeFront) {
    NodeFront *nodeFront = malloc(sizeof(NodeFront));
    nodeFront->front = front;
    nodeFront->next = NULL;

    if (listFront->head == NULL) {
        listFront->head = nodeFront;
    } else {
        lastNodeFront->next = nodeFront;
    }
    return nodeFront;
}




// Função para remover um front de uma lista de fronts
void removeFromListFront(ListFront *listOfFronts, Front *front) {
    NodeFront *currentFront = listOfFronts->head;
    NodeFront *previousFront = NULL;

    while (currentFront != NULL && currentFront->front != front) {
        previousFront = currentFront;
        currentFront = currentFront->next;
    }

    if (currentFront == NULL) {
        return;
    }

    if (previousFront == NULL) {
        listOfFronts->head = currentFront->next;
    } else {
        previousFront->next = currentFront->next;
    }

    free(currentFront);
}




// Função para criar uma lista dupla
ListDoubly *createListDoubly() {
    ListDoubly *listDoubly = malloc(sizeof(ListDoubly));
    listDoubly->head = NULL;
    listDoubly->size = 0;
    return listDoubly;
}




// Função para verificar se uma lista dupla está vazia
int isListDoublyEmpty(ListDoubly *listDoubly) {
    return listDoubly->size == 0;
}




// Função para adicionar uma solução a uma lista dupla
NodeDoubly* addToListDoubly(ListDoubly *listDoubly, Solution *solution, NodeDoubly *lastNodeDoubly) {
    NodeDoubly *nodeDoubly = malloc(sizeof(NodeDoubly));
    nodeDoubly->solution = solution;
    nodeDoubly->next = NULL;
    nodeDoubly->prev = NULL;

    if (listDoubly->head == NULL) {
        listDoubly->head = nodeDoubly;
    } else {
        lastNodeDoubly->next = nodeDoubly;
        nodeDoubly->prev = lastNodeDoubly;
    }

    listDoubly->size++;
    return nodeDoubly;
}




// Função para remover uma solução de uma lista dupla
void removeFromListDoubly(ListDoubly *listDoubly, Solution *solution) {
    NodeDoubly *currentNodeDoubly = listDoubly->head;

    while (currentNodeDoubly != NULL) {
        if (currentNodeDoubly->solution == solution) {
            if (currentNodeDoubly->prev != NULL) {
                currentNodeDoubly->prev->next = currentNodeDoubly->next;
            } else {
                listDoubly->head = currentNodeDoubly->next;
            }
            if (currentNodeDoubly->next != NULL) {
                currentNodeDoubly->next->prev = currentNodeDoubly->prev;
            }
            free(currentNodeDoubly);
            listDoubly->size--;
            return;
        }
        currentNodeDoubly = currentNodeDoubly->next;
    }
}




// Função para copiar uma população para uma lista
List *copyPopulationToList(Population *population) {
    List *list = createList();
    Node *lastNode = NULL;
    for (int i = 0; i < population->numSolutions; i++) {
        lastNode = addToList2(list, &population->solutions[i], lastNode);
    }
    return list;
}




// Função para liberar a memória de uma população de soluções
void freePopulation(Population *population) {
    for (int i = 0; i < population->numSolutions; i++) {
        free(population->solutions[i].edges);
    }
    free(population->solutions);
    free(population);
}




// Função para liberar a memória de uma lista
void freeList(List *list) {
    Node *current = list->head;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current->solution->edges);
        free(current->solution);
        free(current);
        current = next;
    }
    free(list);
}



// Função para liberar a memória de uma lista dupla
void freeListDoubly(ListDoubly *list) {
    NodeDoubly *currentNodeDoubly = list->head;
    NodeDoubly *nextNodeDoubly;
    while (currentNodeDoubly != NULL) {
        nextNodeDoubly = currentNodeDoubly->next;
        free(currentNodeDoubly->solution->edges);
        free(currentNodeDoubly->solution);
        free(currentNodeDoubly);
        currentNodeDoubly = nextNodeDoubly;
    }
    free(list);
}




// Função para liberar a memória de uma lista de fronts
void freeListFront(ListFront *listFront) {
    NodeFront *currentFront = listFront->head;
    NodeFront *nextFront;
    while (currentFront != NULL) {
        nextFront = currentFront->next;
        Node *currentNode = currentFront->front->solutions->head;
        Node *nextNode;
        while (currentNode != NULL) {
            nextNode = currentNode->next;
            free(currentNode->solution->edges);
            free(currentNode->solution);
            free(currentNode);
            currentNode = nextNode;
        }
        free(currentFront->front);
        free(currentFront);
        currentFront = nextFront;
    }
    free(listFront);
}




// Função para imprimir os dados de uma solução
void printSolution(Solution *solution) {
    for (int i = 0; i < solution->numEdges; i++) {
        printf("Aresta %d: %d-%d [%d, %d]\n", i + 1, solution->edges[i].vertex1, solution->edges[i].vertex2,
               solution->edges[i].objectives[0], solution->edges[i].objectives[1]);
    }
    printf("\nObjetivo 1: %d | Objetivo 2: %d\n", solution->totalObjective1, solution->totalObjective2);
    printf("\n");
}




// Função para imprimir os dados de uma populção
void printPopulation(Population *population) {
    for (int p = 0; p < population->numSolutions; p++) {
        printf("Solucao %d:\n\n", p + 1);
        for (int i = 0; i < population->solutions[p].numEdges; i++) {
            printf("Aresta %d: %d-%d [%d, %d]\n", i + 1, population->solutions[p].edges[i].vertex1, population->solutions[p].edges[i].vertex2,
                   population->solutions[p].edges[i].objectives[0], population->solutions[p].edges[i].objectives[1]);
        }
        printf("\nObjetivo 1: %d | Objetivo 2: %d\n", population->solutions[p].totalObjective1, population->solutions[p].totalObjective2);
        printf("\n");
    }
}




// Função para imprime todas soluções de um front
void printFront(Front *front){
    int solutionNumber = 1;

    for (Node *node = front->solutions->head; node != NULL; node = node->next) {
        printf("Solucao %d:\n\n", solutionNumber);
        printSolution(node->solution);
        solutionNumber++;
    }
}




// Função para imprime todas soluções de todos os fronts
void printAllFronts(ListFront *allFronts){
    int frontNumber = 1;

    for (NodeFront *nodeFront = allFronts->head; nodeFront != NULL; nodeFront = nodeFront->next) {
        printf("Front %d:\n\n", frontNumber);
        printFront(nodeFront->front);
        frontNumber++;
    }
}




// Função para imprimir os dados de todas as aresta do grafo original
void printInstanceData(Edge *allEdges, int amountOfAllEdges) {
    printf("Grafo original:\n\n");
    for (int i = 0; i < amountOfAllEdges; i++) {
        printf("%d %d %d %d\n", allEdges[i].vertex1, allEdges[i].vertex2,
               allEdges[i].objectives[0], allEdges[i].objectives[1]);
    }
}




// Função para encontrar o conjunto de um elemento i (usando compressão de caminho)
int find(int parent[], int i) {
    if (parent[i] == -1)
        return i;
    return find(parent, parent[i]);
}



// Função para unir dois subconjuntos em um único conjunto
void unionEdge(int parent[], int x, int y) {
    int xset = find(parent, x);
    int yset = find(parent, y);
    parent[xset] = yset;
}



// Função para verificar se a adição de uma nova aresta cria um ciclo
int doesFormCycle(int parent[], int vertex1, int vertex2) {
    int x = find(parent, vertex1);
    int y = find(parent, vertex2);

    if (x == y)
        return 1;

    unionEdge(parent, x, y);
    return 0;
}




// Função para criar uma solução válida
Solution createInitialSolution(Edge *allEdges, int amountOfAllEdges, int numVertices) {
    Solution solution;
    solution.edges = malloc((numVertices - 1) * sizeof(Edge));
    solution.numEdges = 0;
    solution.crowdingDistance = 0;
    solution.totalObjective1 = 0;
    solution.totalObjective2 = 0;

    int *parent = malloc(numVertices * sizeof(int));

    for (int v = 0; v < numVertices; ++v){
        parent[v] = -1;
    }

    while (solution.numEdges < numVertices - 1) {
        int randomIndex = rand() % amountOfAllEdges;
        Edge randomEdge = allEdges[randomIndex];

        if (!doesFormCycle(parent, randomEdge.vertex1 - 1, randomEdge.vertex2 - 1)) {
            solution.edges[solution.numEdges] = randomEdge;
            solution.numEdges++;
            solution.totalObjective1 += randomEdge.objectives[0];
            solution.totalObjective2 += randomEdge.objectives[1];
        }
    }

    free(parent);
    return solution;
}




// Função para criar uma população de soluções válidas
Population* createInitialPopulation(Edge *allEdges, int amountOfAllEdges, int numVertices) {
    Population *population = malloc(sizeof(Population));
    population->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    population->numSolutions = POPULATION_SIZE;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        population->solutions[i] = createInitialSolution(allEdges, amountOfAllEdges, numVertices);
    }

    return population;
}




// Função para determinar se uma solução é dominada por outra solução em uma lista
int isDominatedInList(Solution *solution, List *list) {
    Node *current = list->head;

    while (current != NULL) {
        Solution *otherSolution = current->solution;
        if ((otherSolution->totalObjective1 <= solution->totalObjective1 && otherSolution->totalObjective2 < solution->totalObjective2) ||
            (otherSolution->totalObjective1 < solution->totalObjective1 && otherSolution->totalObjective2 <= solution->totalObjective2)) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}




// Função para verificar se uma solução 1 domina uma solução 2
int dominates(Solution *solution1, Solution *solution2) {
    return (solution1->totalObjective1 <= solution2->totalObjective1 && solution1->totalObjective2 < solution2->totalObjective2) ||
           (solution1->totalObjective1 < solution2->totalObjective1 && solution1->totalObjective2 <= solution2->totalObjective2);
}




// Função para realizar um torneio e retornar os dois vencedores
void tournament(Population *population, int populationSize, Solution **parent1, Solution **parent2) {
    Solution *best1 = NULL;
    Solution *best2 = NULL;
    int selected[4] = {-1, -1, -1, -1};
    int count = 0;

    while (count < 4) {
        int index = rand() % populationSize;

        bool alreadySelected = false;
        for (int i = 0; i < 4; i++) {
            if (selected[i] == index) {
                alreadySelected = true;
                break;
            }
        }

        if (!alreadySelected) {
            selected[count] = index;
            count++;

            Solution *competitor = &population->solutions[index];

            if (best1 == NULL || dominates(competitor, best1)) {
                best2 = best1;
                best1 = competitor;
            } else if (best2 == NULL || dominates(competitor, best2)) {
                best2 = competitor;
            }
        }
    }

    *parent1 = best1;
    *parent2 = best2;
}




// Função para checar se uma aresta já existe em uma solução
int checkDuplicateEdge(Solution *solution, int vertex1, int vertex2){
    for(int i = 0; i < solution->numEdges; i++){
        if(solution->edges[i].vertex1 == vertex1 && solution->edges[i].vertex2 == vertex2){
            return 1;
        }
    }
    return 0;
}




// Função para avisar se foi achado um ciclo em uma solução durante a função de crossover
void findCycleInCrossover(Solution *solutionCycleCheck, int startEdge, int edgeConnectedToNext, int *stopPoint){
    int edgeNextLoop;

    for(int i = 0; i < solutionCycleCheck->numEdges; i++){
        if(stopPoint[0] == 0){
            break;
        }
        if(solutionCycleCheck->edges[i].vertex1 == edgeConnectedToNext){
            if(solutionCycleCheck->edges[i].vertex2 == startEdge){
                stopPoint[0] = 0;
                break;
            } else{
                edgeNextLoop = solutionCycleCheck->edges[i].vertex2;
                solutionCycleCheck->edges[i].vertex1 = 0;
                solutionCycleCheck->edges[i].vertex2 = 0;
                findCycleInCrossover(solutionCycleCheck, startEdge, edgeNextLoop, stopPoint);
            }
        } else if(solutionCycleCheck->edges[i].vertex2 == edgeConnectedToNext){
            if(solutionCycleCheck->edges[i].vertex1 == startEdge){
                stopPoint[0] = 0;
                break;
            } else{
                edgeNextLoop = solutionCycleCheck->edges[i].vertex1;
                solutionCycleCheck->edges[i].vertex1 = 0;
                solutionCycleCheck->edges[i].vertex2 = 0;
                findCycleInCrossover(solutionCycleCheck, startEdge, edgeNextLoop, stopPoint);
            }
        }
    }
}




// Função para utilizar findCycleInCrossover para indicar se foi achado um ciclo em uma solução
int removeCycleCrossover(Solution *offspring, Edge newEdge){
    Solution *solutionCopy = copySolution(offspring);

    int startEdge = newEdge.vertex2;
    int nextToStartEdge = newEdge.vertex1;
    int edgeConnectedToNext;

    int *stopPoint = malloc(sizeof(int));
    stopPoint[0] = -1;

    for(int i = 0; i < solutionCopy->numEdges; i++){
        if(solutionCopy->edges[i].vertex1 == nextToStartEdge){
            edgeConnectedToNext = solutionCopy->edges[i].vertex2;
            solutionCopy->edges[i].vertex1 = 0;
            solutionCopy->edges[i].vertex2 = 0;
            findCycleInCrossover(solutionCopy, startEdge, edgeConnectedToNext, stopPoint);
            if(stopPoint[0] == 0){
                break;
            }
        } else if(solutionCopy->edges[i].vertex2 == nextToStartEdge){
            edgeConnectedToNext = solutionCopy->edges[i].vertex1;
            solutionCopy->edges[i].vertex1 = 0;
            solutionCopy->edges[i].vertex2 = 0;
            findCycleInCrossover(solutionCopy, startEdge, edgeConnectedToNext, stopPoint);
            if(stopPoint[0] == 0){
                break;
            }
        }
    }

    free(solutionCopy->edges);
    free(solutionCopy);

    if(stopPoint[0] == 0){
        return 0;
    } else{
        return 1;
    }
}




// Função de comparação para a função sortEdgeArray
int compareEdge(const void *a, const void *b) {
    Edge *edgeA = (Edge *)a;
    Edge *edgeB = (Edge *)b;
    int sumA = edgeA->objectives[0] + edgeA->objectives[1];
    int sumB = edgeB->objectives[0] + edgeB->objectives[1];
    return sumA - sumB;
}




// Função para organizar um vetor de arestas em ordem crescente da soma dos objetivos
void sortEdgeArray(Edge *array, size_t length) {
    qsort(array, length, sizeof(Edge), compareEdge);
}




// Função para efetuar o crossover entre duas soluções
Solution* crossover(Solution *parent1, Solution *parent2){
    int sizeNewEdges = 0;
    Edge *newEdges = malloc((parent1->numEdges * 2) * sizeof(Edge));

    for(int i = 0; i < parent1->numEdges; i++){
        newEdges[i] = parent1->edges[i];
        sizeNewEdges++;
    }

    for(int i = 0; i < parent2->numEdges; i++){
        int currentlyEdge = checkDuplicateEdge(parent1, parent2->edges[i].vertex1, parent2->edges[i].vertex2);

        if(currentlyEdge == 0){
            newEdges[sizeNewEdges] = parent2->edges[i];
            sizeNewEdges++;
        }
    }

    sortEdgeArray(newEdges, sizeNewEdges);

    Solution *offSpring = malloc(sizeof(Solution));
    offSpring->edges = calloc(parent1->numEdges, sizeof(Edge));
    offSpring->numEdges = 0;
    offSpring->crowdingDistance = 0;
    offSpring->totalObjective1 = 0;
    offSpring->totalObjective2 = 0;

    int *includedeVertices = calloc(parent1->numEdges + 2, sizeof(int));

    for(int i = 0; i < sizeNewEdges; i++){
        if(includedeVertices[newEdges[i].vertex1] == 0 && includedeVertices[newEdges[i].vertex2] == 0){
            if(offSpring->numEdges == parent1->numEdges){
                break;
            }

            includedeVertices[newEdges[i].vertex1] = newEdges[i].vertex1;
            includedeVertices[newEdges[i].vertex2] = newEdges[i].vertex2;

            offSpring->edges[offSpring->numEdges].vertex1 = newEdges[i].vertex1;
            offSpring->edges[offSpring->numEdges].vertex2 = newEdges[i].vertex2;
            offSpring->edges[offSpring->numEdges].objectives[0] = newEdges[i].objectives[0];
            offSpring->edges[offSpring->numEdges].objectives[1] = newEdges[i].objectives[1];
            offSpring->totalObjective1 += newEdges[i].objectives[0];
            offSpring->totalObjective2 += newEdges[i].objectives[1];

            offSpring->numEdges++;

        } else if(includedeVertices[newEdges[i].vertex1] == newEdges[i].vertex1 && includedeVertices[newEdges[i].vertex2] == 0){
            if(offSpring->numEdges == parent1->numEdges){
                break;
            }

            includedeVertices[newEdges[i].vertex2] = newEdges[i].vertex2;

            offSpring->edges[offSpring->numEdges].vertex1 = newEdges[i].vertex1;
            offSpring->edges[offSpring->numEdges].vertex2 = newEdges[i].vertex2;
            offSpring->edges[offSpring->numEdges].objectives[0] = newEdges[i].objectives[0];
            offSpring->edges[offSpring->numEdges].objectives[1] = newEdges[i].objectives[1];
            offSpring->totalObjective1 += newEdges[i].objectives[0];
            offSpring->totalObjective2 += newEdges[i].objectives[1];

            offSpring->numEdges++;

        } else if(includedeVertices[newEdges[i].vertex1] == 0 && includedeVertices[newEdges[i].vertex2] == newEdges[i].vertex2){
            if(offSpring->numEdges == parent1->numEdges){
                break;
            }

            includedeVertices[newEdges[i].vertex1] = newEdges[i].vertex1;

            offSpring->edges[offSpring->numEdges].vertex1 = newEdges[i].vertex1;
            offSpring->edges[offSpring->numEdges].vertex2 = newEdges[i].vertex2;
            offSpring->edges[offSpring->numEdges].objectives[0] = newEdges[i].objectives[0];
            offSpring->edges[offSpring->numEdges].objectives[1] = newEdges[i].objectives[1];
            offSpring->totalObjective1 += newEdges[i].objectives[0];
            offSpring->totalObjective2 += newEdges[i].objectives[1];

            offSpring->numEdges++;

        } else{
            if(offSpring->numEdges == parent1->numEdges){
                break;
            }

            int testCycle = -1;
            Edge edgeToTryCycle;

            edgeToTryCycle.vertex1 = newEdges[i].vertex1;
            edgeToTryCycle.vertex2 = newEdges[i].vertex2;
            testCycle = removeCycleCrossover(offSpring, edgeToTryCycle);

            if(testCycle != 0){
                offSpring->edges[offSpring->numEdges].vertex1 = newEdges[i].vertex1;
                offSpring->edges[offSpring->numEdges].vertex2 = newEdges[i].vertex2;
                offSpring->edges[offSpring->numEdges].objectives[0] = newEdges[i].objectives[0];
                offSpring->edges[offSpring->numEdges].objectives[1] = newEdges[i].objectives[1];
                offSpring->totalObjective1 += newEdges[i].objectives[0];
                offSpring->totalObjective2 += newEdges[i].objectives[1];

                offSpring->numEdges++;
            }
        }
    }

    free(newEdges);
    free(includedeVertices);
    return offSpring;
}




// Função para escolher um número aleatório para qual aresta mutar durante a mutação
int pickEdgeMutation(int y) {
    return rand() % (y + 1);
}




// Função para verificar se um vértice foi removido da solução durante a mutação ou busca de vizinhos
int findMissingVertice(int *array, int *size, int find){
    int found;
    for(int i = 0; i < size[0]; i++){
        if(array[i] == find){
            return 0;
        } else{
            found = -1;;
        }
    }
    return found;
}




// Função secundária para armazenar quais arestas estão conectadas entre sí em uma solução
void verticesConnecteds2(Solution *solutionCheckEdgeConnecteds, int *part, int *sizeOfPart, int search){
    for(int i = 0; i < solutionCheckEdgeConnecteds->numEdges; i++){
        if(solutionCheckEdgeConnecteds->edges[i].vertex1 == search){
            part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex2;
            sizeOfPart[0]++;
            int searchAgain = solutionCheckEdgeConnecteds->edges[i].vertex2;
            solutionCheckEdgeConnecteds->edges[i].vertex1 = 0;
            solutionCheckEdgeConnecteds->edges[i].vertex2 = 0;
            verticesConnecteds2(solutionCheckEdgeConnecteds, part, sizeOfPart, searchAgain);

        } else if(solutionCheckEdgeConnecteds->edges[i].vertex2 == search){
            part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex1;
            sizeOfPart[0]++;
            int searchAgain = solutionCheckEdgeConnecteds->edges[i].vertex1;
            solutionCheckEdgeConnecteds->edges[i].vertex1 = 0;
            solutionCheckEdgeConnecteds->edges[i].vertex2 = 0;
            verticesConnecteds2(solutionCheckEdgeConnecteds, part, sizeOfPart, searchAgain);

        }
    }
}




// Função primária para armazenar quais arestas estão conectadas entre sí em uma solução
void verticesConnecteds(Solution *solutionCheckEdgeConnecteds, int *part, int *sizeOfPart, int search, int *stop){
    for(int i = 0; i < solutionCheckEdgeConnecteds->numEdges; i++){
        if(solutionCheckEdgeConnecteds->edges[i].vertex1 == search){
            part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex2;
            sizeOfPart[0]++;
            if(stop[0] == 0){
                part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex1;
                sizeOfPart[0]++;
                stop[0] = 1;
            }
            int searchAgain = solutionCheckEdgeConnecteds->edges[i].vertex2;
            solutionCheckEdgeConnecteds->edges[i].vertex1 = 0;
            solutionCheckEdgeConnecteds->edges[i].vertex2 = 0;
            verticesConnecteds2(solutionCheckEdgeConnecteds, part, sizeOfPart, searchAgain);

        } else if(solutionCheckEdgeConnecteds->edges[i].vertex2 == search){
            part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex1;
            sizeOfPart[0]++;
            if(stop[0] == 0){
                part[sizeOfPart[0]] = solutionCheckEdgeConnecteds->edges[i].vertex2;
                sizeOfPart[0]++;
                stop[0] = 1;
            }
            int searchAgain = solutionCheckEdgeConnecteds->edges[i].vertex1;
            solutionCheckEdgeConnecteds->edges[i].vertex1 = 0;
            solutionCheckEdgeConnecteds->edges[i].vertex2 = 0;
            verticesConnecteds2(solutionCheckEdgeConnecteds, part, sizeOfPart, searchAgain);

        }
    }
}




// Função para calcular qual a posição de uma determinada aresta no grafo original
int calculateEdgePositionOnAllEdges(int vertice1, int vertice2, int numVertices){
    int edgePosition = numVertices * (vertice1 - 1) - (vertice1 * (vertice1 - 1)) / 2 + (vertice2 - vertice1);
    return edgePosition;
}




// Função para efetuar uma mutação em uma solução
Solution* mutation(Solution *solutionM, Edge *allEdges, int numVertices){
    int pickedEdge = pickEdgeMutation(solutionM->numEdges - 1);
    int *stop = calloc(1, sizeof(int));

    int verticesCopy[2];
    verticesCopy[0] = solutionM->edges[pickedEdge].vertex1;
    verticesCopy[1] = solutionM->edges[pickedEdge].vertex2;

    solutionM->edges[pickedEdge].vertex1 = 0;
    solutionM->edges[pickedEdge].vertex2 = 0;
    solutionM->totalObjective1 -= solutionM->edges[pickedEdge].objectives[0];
    solutionM->totalObjective2 -= solutionM->edges[pickedEdge].objectives[1];
    solutionM->edges[pickedEdge].objectives[0] = 0;
    solutionM->edges[pickedEdge].objectives[1] = 0;

    int startPosition;
    if (pickedEdge == 0){
        startPosition = 1;
    } else{
        startPosition = 0;
    }

    int search = solutionM->edges[startPosition].vertex1;

    int *firstPart = calloc(solutionM->numEdges, sizeof(int));
    int *sizeOfPart1 = calloc(1, sizeof(int));
    int *secondPart = calloc(solutionM->numEdges, sizeof(int));
    int *sizeOfPart2 = calloc(1, sizeof(int));

    Solution *newSolution = malloc(sizeof(Solution));
    newSolution->edges = malloc((solutionM->numEdges) * sizeof(Edge));

    for(int i = 0; i < solutionM->numEdges; i++){
        newSolution->edges[i] = solutionM->edges[i];
    }

    newSolution->numEdges = solutionM->numEdges;
    verticesConnecteds(newSolution, firstPart, sizeOfPart1, search, stop);
    stop[0] = 0;

    if(sizeOfPart1[0] == solutionM->numEdges){
        int test = findMissingVertice(firstPart, sizeOfPart1, verticesCopy[0]);
        if (test == -1){
            int verticeToConnect = pickEdgeMutation(sizeOfPart1[0] - 1);
            while(firstPart[verticeToConnect] == verticesCopy[1]){
                verticeToConnect = pickEdgeMutation(sizeOfPart1[0] - 1);
            }
            int verticeMissing = firstPart[verticeToConnect];
            if (verticeMissing < verticesCopy[0]){
                int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[0], numVertices);
                solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
            } else{
                int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[0], verticeMissing, numVertices);
                solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
            }
        } else{
            int verticeToConnect = pickEdgeMutation(sizeOfPart1[0] - 1);
            while(firstPart[verticeToConnect] == verticesCopy[0]){
                verticeToConnect = pickEdgeMutation(sizeOfPart1[0] - 1);
            }
            int verticeMissing = firstPart[verticeToConnect];
            if (verticeMissing < verticesCopy[1]){
                int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[1], numVertices);
                solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
            } else{
                int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[1], verticeMissing, numVertices);
                solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
            }
        }
    } else{
        for(int i = 0; i < newSolution->numEdges; i++){
            if (newSolution->edges[i].vertex1 != 0){
                int search2 = newSolution->edges[i].vertex1;
                verticesConnecteds(newSolution, secondPart, sizeOfPart2, search2, stop);
                int verticeToConnect1 = pickEdgeMutation(sizeOfPart1[0] - 1);
                int vertice1 = firstPart[verticeToConnect1];
                int verticeToConnect2 = pickEdgeMutation(sizeOfPart2[0] - 1);
                int vertice2 = secondPart[verticeToConnect2];
                while((vertice1 == verticesCopy[0] && vertice2 == verticesCopy[1]) || (vertice1 == verticesCopy[1] && vertice2 == verticesCopy[0])){
                    verticeToConnect1 = pickEdgeMutation(sizeOfPart1[0] - 1);
                    vertice1 = firstPart[verticeToConnect1];
                    verticeToConnect2 = pickEdgeMutation(sizeOfPart2[0] - 1);
                    vertice2 = secondPart[verticeToConnect2];
                }
                if (vertice1 < vertice2){
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice1, vertice2, numVertices);
                    solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                } else{
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice2, vertice1, numVertices);
                    solutionM->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    solutionM->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    solutionM->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                }
                break;
            }
        }
    }

    free(stop);
    free(firstPart);
    free(sizeOfPart1);
    free(secondPart);
    free(sizeOfPart2);
    free(newSolution->edges);
    free(newSolution);
    return solutionM;
}




// Função de comparação para a função sortSolutions2
int compareSolutionsObjective2(const void *a, const void *b) {
    Solution *solutionA = (Solution *)a;
    Solution *solutionB = (Solution *)b;
    if (solutionA->totalObjective2 < solutionB->totalObjective2) {
        return -1;
    } else if (solutionA->totalObjective2 > solutionB->totalObjective2) {
        return 1;
    } else {
        return 0;
    }
}




// Função para organizar uma população de soluções em ordem crescente para o valor do objetivo 2
void sortSolutionsObjective2(Population *population) {
    qsort(population->solutions, population->numSolutions, sizeof(Solution), compareSolutionsObjective2);
}




// Função de comparação para a função sortSolutions1
int compareSolutionsObjective1(const void *a, const void *b) {
    Solution *solutionA = (Solution *)a;
    Solution *solutionB = (Solution *)b;
    if (solutionA->totalObjective1 < solutionB->totalObjective1) {
        return -1;
    } else if (solutionA->totalObjective1 > solutionB->totalObjective1) {
        return 1;
    } else {
        return 0;
    }
}




// Função para organizar uma população de soluções em ordem crescente para o valor do objetivo 1
void sortSolutionsObjective1(Population *population) {
    qsort(population->solutions, population->numSolutions, sizeof(Solution), compareSolutionsObjective1);
}




// Função de comparação para a função sortSolutionsByCrowdingDistance
int compareSolutionsByCrowdingDistance(const void *a, const void *b) {
    Solution *solutionA = (Solution *)a;
    Solution *solutionB = (Solution *)b;
    if (solutionA->crowdingDistance > solutionB->crowdingDistance) {
        return -1;
    } else if (solutionA->crowdingDistance < solutionB->crowdingDistance) {
        return 1;
    } else {
        return 0;
    }
}




// Função para organizar uma população de soluções em ordem decrescente para o valor de distância de aglomeração
void sortSolutionsByCrowdingDistance(Population *population) {
    qsort(population->solutions, population->numSolutions, sizeof(Solution), compareSolutionsByCrowdingDistance);
}




// Função para calcular a distância de aglomeração para as soluções dentro de um front
void calculateCrowdingDistance(Front *front) {
    int size = front->solutions->size;
    int currentPosition = 0;
    Population *tempPopulation = malloc(sizeof(Population));
    tempPopulation->solutions = malloc(size * sizeof(Solution));
    tempPopulation->numSolutions = size;

    for (Node *node = front->solutions->head; node != NULL; node = node->next) {
        node->solution->crowdingDistance = 0;
        tempPopulation->solutions[currentPosition] = *copySolution(node->solution);
        currentPosition++;
    }

    for (int obj = 0; obj < NUM_OBJECTIVES; obj++) {
        if(obj == 0){
            sortSolutionsObjective1(tempPopulation);
        }else{
            sortSolutionsObjective2(tempPopulation);
        }

        tempPopulation->solutions[0].crowdingDistance = INT_MAX;
        tempPopulation->solutions[size - 1].crowdingDistance = INT_MAX;

        for (int i = 1; i < size - 1; i++) {
            double objectiveMinus1, objectivePlus1;
            if (obj == 0) {
                objectiveMinus1 = tempPopulation->solutions[i - 1].totalObjective1;
                objectivePlus1 = tempPopulation->solutions[i + 1].totalObjective1;
                tempPopulation->solutions[i].crowdingDistance = (objectivePlus1 - objectiveMinus1);
            } else {
                objectiveMinus1 = tempPopulation->solutions[i - 1].totalObjective2;
                objectivePlus1 = tempPopulation->solutions[i + 1].totalObjective2;
                tempPopulation->solutions[i].crowdingDistance += (objectivePlus1 - objectiveMinus1);
            }
        }
    }

    sortSolutionsByCrowdingDistance(tempPopulation);
    int i = 0;
    for (Node *node = front->solutions->head; node != NULL; node = node->next) {
        free(node->solution->edges);
        free(node->solution);
        node->solution = copySolution(&tempPopulation->solutions[i]);
        i++;
    }

    freePopulation(tempPopulation);
}




// Função para criar os fronts a partir de uma população de soluções
ListFront* createFronts(Population *populationForTheFronts) {
    Population *populationCopy = copyPopulation(populationForTheFronts);
    ListFront *allFronts = createListFront();
    NodeFront *lastNodeFront = NULL;

    List *unassigned = copyPopulationToList(populationCopy);

    while (!isListEmpty(unassigned)) {
        Front *newFront = malloc(sizeof(Front));
        newFront->solutions = createList();
        Node *lastNode = NULL;

        List *removeFromUnassigned = createList();
        Node *lastNode2 = NULL;

        Node *node = unassigned->head;
        while (node != NULL) {
            Solution *solution = copySolution(node->solution);
            Solution *copiedSolution = copySolution(node->solution);
            Node *nextNode = node->next;

            if (!isDominatedInList(solution, unassigned)) {
                lastNode = addToList(newFront->solutions, solution, lastNode);
                lastNode2 = addToList(removeFromUnassigned, copiedSolution, lastNode2);
            }

            node = nextNode;
        }

        for (Node *node2 = removeFromUnassigned->head; node2 != NULL; node2 = node2->next){
            removeFromList2(unassigned, node2->solution);
        }

        freeList(removeFromUnassigned);
        lastNodeFront = addToListFront(allFronts, newFront, lastNodeFront);
    }

    freePopulation(populationCopy);
    freeList(unassigned);
    return allFronts;
}




// Função para escolher um número aleatório para: qual pai duplicar, taxa de mutação e taxa de crossover
int pickRandomNumber(int y) {
    return (rand() % y) + 1;
}




// Função para criar a população de soluções filhas a partir da população dos pais
Population* createOffspringsForParentPopulation(Population *initialPopulation, Edge *allEdges, int numVertices){
    Population *initialOffsprings = malloc(sizeof(Population));
    initialOffsprings->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    initialOffsprings->numSolutions = 0;

    for(int i = 0; i < POPULATION_SIZE; i++){
        Solution *parent1 = malloc(sizeof(Solution));
        Solution *parent2 = malloc(sizeof(Solution));
        tournament(initialPopulation, initialPopulation->numSolutions, &parent1, &parent2);
        Solution *offSpring = malloc(sizeof(Solution));

        if(pickRandomNumber(100) <= CROSSOVER_RATE){
            offSpring = crossover(parent1, parent2);
        } else{
            if(pickRandomNumber(100) <= 50){
                offSpring = copySolution(parent1);
            } else{
                offSpring = copySolution(parent2);
            }
        }

        if(pickRandomNumber(100) <= MUTATION_RATE){
            offSpring = mutation(offSpring, allEdges, numVertices);
        }

        initialOffsprings->solutions[i] = *copySolution(offSpring);
        initialOffsprings->numSolutions++;
        free(offSpring->edges);
        free(offSpring);
    }

    return initialOffsprings;
}




// Função para verificar, adicionar e remover soluções da lista dupla de soluções não dominadas finais
void addNonDominatedSolutionsToParetoFront(ListDoubly *nonDominatedSolutions, Solution *tryNewSolution){
    if(isListDoublyEmpty(nonDominatedSolutions)){
        Solution *newSolution = copySolution(tryNewSolution);
        NodeDoubly *lastNodeDoubly = NULL;
        lastNodeDoubly = addToListDoubly(nonDominatedSolutions, newSolution, lastNodeDoubly);
    } else{
        int proceed = 0;

        for (NodeDoubly *current = nonDominatedSolutions->head; current != NULL; current = current->next) {
            if(dominates(current->solution, tryNewSolution) || (current->solution->totalObjective1 == tryNewSolution->totalObjective1 && current->solution->totalObjective2 == tryNewSolution->totalObjective2)){
                proceed = 1;
                break;
            }

            if(current->next == NULL){
                Solution *newSolution = copySolution(tryNewSolution);
                NodeDoubly *newNode = malloc(sizeof(NodeDoubly));
                newNode->solution = newSolution;
                newNode->next = NULL;
                newNode->prev = current;
                current->next = newNode;
                nonDominatedSolutions->size++;
                break;
            }
        }

        if(proceed == 0){
            NodeDoubly *current2 = nonDominatedSolutions->head;

            while (current2 != NULL) {
                NodeDoubly *nextNode = current2->next;
                if(dominates(tryNewSolution, current2->solution)){
                    if(current2->prev == NULL){
                        nonDominatedSolutions->head = current2->next;
                        current2->next->prev = current2->prev;
                    }else{
                        current2->prev->next = current2->next;
                        current2->next->prev = current2->prev;
                    }
                    free(current2->solution->edges);
                    free(current2->solution);
                    free(current2);
                    nonDominatedSolutions->size--;
                }
                current2 = nextNode;
            }
        }
    }
}




// Função para escolher um número aleatório para qual solução vai ser explorado a vizinhança
int pickRandomSolution(int y) {
    return rand() % (y + 1);
}




// Função para criar uma população de soluções não dominadas para usar como entrada para o PLS no pareto front
List* createNonDominatedPopulationList(ListDoubly *paretoFront) {
    List *nonDominatedSolutions = createList();
    Node *lastNode = NULL;

    for (NodeDoubly *nodeDoubly = paretoFront->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){
        Solution *copySolutionTemp = copySolution(nodeDoubly->solution);
        lastNode = addToList(nonDominatedSolutions, copySolutionTemp, lastNode);
    }

    freeListDoubly(paretoFront);
    return nonDominatedSolutions;
}




// Função para verificar, adicionar e remover soluções da lista de soluções não dominadas
void onlyAddNonDominatedSolutions(List *temporaryNeighborsSolutions, Solution *neighbor){
    if(isListEmpty(temporaryNeighborsSolutions)){
        Solution *newSolution = copySolution(neighbor);
        Node *lastNode = NULL;
        lastNode = addToList(temporaryNeighborsSolutions, newSolution, lastNode);
    } else{
        int proceed = 0;

        for (Node *current = temporaryNeighborsSolutions->head; current != NULL; current = current->next) {
            if(dominates(current->solution, neighbor) || (current->solution->totalObjective1 == neighbor->totalObjective1 && current->solution->totalObjective2 == neighbor->totalObjective2)){
                proceed = 1;
                break;
            }

            if(current->next == NULL){
                Solution *newSolution = copySolution(neighbor);
                Node *newNode = malloc(sizeof(Node));
                newNode->solution = newSolution;
                newNode->next = NULL;
                current->next = newNode;
                temporaryNeighborsSolutions->size++;
                break;
            }
        }

        if(proceed == 0){
            Node *current2 = temporaryNeighborsSolutions->head;
            Node *previous2 = NULL;

            while (current2 != NULL) {
                Node *nextNode = current2->next;
                if(dominates(neighbor, current2->solution)){
                    if(previous2 == NULL){
                        temporaryNeighborsSolutions->head = current2->next;
                    }else{
                        previous2->next = current2->next;
                    }
                    free(current2->solution->edges);
                    free(current2->solution);
                    free(current2);
                    temporaryNeighborsSolutions->size--;
                } else {
                    previous2 = current2;
                }
                current2 = nextNode;
            }
        }
    }
}




// Função para buscar os vizinhos de uma solução
void neighborSearch(Solution *solutionToFindNeighbor, Edge *allEdges, int pickedEdge, List *temporaryNeighborsSolutions, int numVertices){
    int *stop = calloc(1, sizeof(int));

    int verticesCopy[NUM_OBJECTIVES];
    verticesCopy[0] = solutionToFindNeighbor->edges[pickedEdge].vertex1;
    verticesCopy[1] = solutionToFindNeighbor->edges[pickedEdge].vertex2;

    int objectivesCopy[NUM_OBJECTIVES];
    objectivesCopy[0] = solutionToFindNeighbor->edges[pickedEdge].objectives[0];
    objectivesCopy[1] = solutionToFindNeighbor->edges[pickedEdge].objectives[1];

    solutionToFindNeighbor->edges[pickedEdge].vertex1 = 0;
    solutionToFindNeighbor->edges[pickedEdge].vertex2 = 0;
    solutionToFindNeighbor->totalObjective1 -= objectivesCopy[0];
    solutionToFindNeighbor->totalObjective2 -= objectivesCopy[1];
    solutionToFindNeighbor->edges[pickedEdge].objectives[0] = 0;
    solutionToFindNeighbor->edges[pickedEdge].objectives[1] = 0;

    int startPosition;
    if (pickedEdge == 0){
        startPosition = 1;
    } else{
        startPosition = 0;
    }

    int search = solutionToFindNeighbor->edges[startPosition].vertex1;

    int *firstPart = calloc(solutionToFindNeighbor->numEdges, sizeof(int));
    int *sizeOfPart1 = calloc(1, sizeof(int));
    int *secondPart = calloc(solutionToFindNeighbor->numEdges, sizeof(int));
    int *sizeOfPart2 = calloc(1, sizeof(int));

    Solution *temporaryCopy = copySolution(solutionToFindNeighbor);

    verticesConnecteds(temporaryCopy, firstPart, sizeOfPart1, search, stop);
    stop[0] = 0;

    if(sizeOfPart1[0] == solutionToFindNeighbor->numEdges){
        int test = findMissingVertice(firstPart, sizeOfPart1, verticesCopy[0]);
        if (test == -1){
            for (int i = 0; i < sizeOfPart1[0]; i++){
                int verticeMissing = firstPart[i];
                if (firstPart[i] == verticesCopy[1]){
                    if ((i + 1) == sizeOfPart1[0]){
                        break;
                    } else{
                        verticeMissing = firstPart[i + 1];
                        i++;
                    }
                }

                if (verticeMissing < verticesCopy[0]){
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[0], numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                } else{
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[0], verticeMissing, numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                }
            }
        } else{
            for (int i = 0; i < sizeOfPart1[0]; i++){
                int verticeMissing = firstPart[i];

                if (firstPart[i] == verticesCopy[0]){
                    if ((i + 1) == sizeOfPart1[0]){
                        break;
                    } else{
                        verticeMissing = firstPart[i + 1];
                        i++;
                    }
                }

                if (verticeMissing < verticesCopy[1]){
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticeMissing, verticesCopy[1], numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                } else{
                    Solution *neighbor = copySolution(solutionToFindNeighbor);
                    int positionInAllEdges = calculateEdgePositionOnAllEdges(verticesCopy[1], verticeMissing, numVertices);
                    neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                    neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                    neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                    onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                }
            }
        }
    } else{
        for(int i = 0; i < temporaryCopy->numEdges; i++){
            if (temporaryCopy->edges[i].vertex1 != 0){
                int search2 = temporaryCopy->edges[i].vertex1;
                verticesConnecteds(temporaryCopy, secondPart, sizeOfPart2, search2, stop);

                for (int i = 0; i < sizeOfPart1[0]; i++){
                    for (int h = 0; h < sizeOfPart2[0]; h++){
                        int vertice1 = firstPart[i];
                        int vertice2 = secondPart[h];

                        if ((vertice1 == verticesCopy[0] && vertice2 == verticesCopy[1]) || (vertice1 == verticesCopy[1] && vertice2 == verticesCopy[0])){
                            if ((h + 1) == sizeOfPart2[0]){
                                break;
                            } else{
                                vertice2 = secondPart[h + 1];
                                h++;
                            }
                        }

                        if (vertice1 < vertice2){
                            Solution *neighbor = copySolution(solutionToFindNeighbor);
                            int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice1, vertice2, numVertices);
                            neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                            neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                            neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                            onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                        } else{
                            Solution *neighbor = copySolution(solutionToFindNeighbor);
                            int positionInAllEdges = calculateEdgePositionOnAllEdges(vertice2, vertice1, numVertices);
                            neighbor->edges[pickedEdge] = allEdges[positionInAllEdges - 1];
                            neighbor->totalObjective1 += allEdges[positionInAllEdges - 1].objectives[0];
                            neighbor->totalObjective2 += allEdges[positionInAllEdges - 1].objectives[1];
                            onlyAddNonDominatedSolutions(temporaryNeighborsSolutions, neighbor);
                        }
                    }
                }
                break;
            }
        }
    }

    solutionToFindNeighbor->edges[pickedEdge].vertex1 = verticesCopy[0];
    solutionToFindNeighbor->edges[pickedEdge].vertex2 = verticesCopy[1];
    solutionToFindNeighbor->totalObjective1 += objectivesCopy[0];
    solutionToFindNeighbor->totalObjective2 += objectivesCopy[1];
    solutionToFindNeighbor->edges[pickedEdge].objectives[0] = objectivesCopy[0];
    solutionToFindNeighbor->edges[pickedEdge].objectives[1] = objectivesCopy[1];

    free(stop);
    free(firstPart);
    free(sizeOfPart1);
    free(secondPart);
    free(sizeOfPart2);
    free(temporaryCopy->edges);
    free(temporaryCopy);
}




// Função para executar os passos principais do PLS no pareto front
List* paretoLocalSearchAdaptedToNsga2(ListDoubly *paretoFront, Edge *allEdges, int numVertices){
    List *unexploredSolutions = createNonDominatedPopulationList(paretoFront);

    for(Node *node = unexploredSolutions->head; node != NULL; node = node->next){
        node->solution->exploration = false;
    }

    List *allNonDominatedSolutions = createList();
    Node *lastNode = NULL;
    for(Node *node = unexploredSolutions->head; node != NULL; node = node->next){
        lastNode = addToList(allNonDominatedSolutions, node->solution, lastNode);
    }

    do{
        int picked = pickRandomSolution(unexploredSolutions->size - 1);
        List *temporaryNeighborsSolutions = createList();

        Node *current = unexploredSolutions->head;
        for (int i = 0; i < picked; i++){
            current = current->next;
        }

        for (int j = 0; j < current->solution->numEdges; j++){
            neighborSearch(current->solution, allEdges, j, temporaryNeighborsSolutions, numVertices);
        }

        current->solution->exploration = true;

        for(Node *node = temporaryNeighborsSolutions->head; node != NULL; node = node->next){
            onlyAddNonDominatedSolutions(allNonDominatedSolutions, node->solution);
        }

        Node* currentEmptyer = unexploredSolutions->head;
        Node* nextEmptyer;
        while (currentEmptyer != NULL) {
            nextEmptyer = currentEmptyer->next;
            free(currentEmptyer);
            unexploredSolutions->size--;
            currentEmptyer = nextEmptyer;
        }
        unexploredSolutions->head = NULL;
        Node *lastNode2 = NULL;

        for (Node *node = allNonDominatedSolutions->head; node != NULL; node = node->next){
            if (node->solution->exploration == false){
                lastNode2 = addToList(unexploredSolutions, node->solution, lastNode2);
            }
        }

        freeList(temporaryNeighborsSolutions);

    } while(unexploredSolutions->size != 0);

    return allNonDominatedSolutions;
}




// Função secundária para executar os passos principais do NSGA-II com o PLS no pareto front até o critério de parada
Population* secondaryMainLoopToNsga2WithPls(Population *nextGeneration, ListDoubly *nonDominatedSolutions, Edge *allEdges, int numVertices){
    Population *initialOffsprings = createOffspringsForParentPopulation(nextGeneration, allEdges, numVertices);

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialOffsprings->solutions[i]);
    }

    Population *combinedPopulation = malloc(sizeof(Population));
    combinedPopulation->solutions = malloc((POPULATION_SIZE * 2) * sizeof(Solution));
    combinedPopulation->numSolutions = 0;

    for(int i = 0; i < POPULATION_SIZE; i++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&nextGeneration->solutions[i]);
        combinedPopulation->numSolutions++;
    }

    for(int j = 0; j < POPULATION_SIZE; j++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialOffsprings->solutions[j]);
        combinedPopulation->numSolutions++;
    }

    ListFront *allFronts = createFronts(combinedPopulation);

    freePopulation(nextGeneration);
    freePopulation(initialOffsprings);
    freePopulation(combinedPopulation);

    Population *nextGeneration2 = malloc(sizeof(Population));
    nextGeneration2->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    nextGeneration2->numSolutions = 0;

    int nextG = 0;
    for (NodeFront *nodeFront = allFronts->head; nodeFront != NULL; nodeFront = nodeFront->next){
        if(nextG == POPULATION_SIZE){
            break;
        }
        if(nextG + nodeFront->front->solutions->size > POPULATION_SIZE){
            calculateCrowdingDistance(nodeFront->front);
        }

        for(Node *node = nodeFront->front->solutions->head; node != NULL; node = node->next){
            if(nextG == POPULATION_SIZE){
                break;
            }

            nextGeneration2->solutions[nextG] = *copySolution(node->solution);
            nextGeneration2->numSolutions++;
            nextG++;
        }
    }

    freeListFront(allFronts);

    return nextGeneration2;
}




// Função primária para executar os passos principais do NSGA-II com o PLS no pareto front até o critério de parada
void nsga2WithPlsMainLoop(Population *initialPopulation, Edge *allEdges, int numVertices){
    clock_t start, end, pauseStart, pauseEnd;
    double cpuTimeUsed;

    start = clock();

    ListDoubly *nonDominatedSolutions = createListDoubly();

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialPopulation->solutions[i]);
    }

    pauseStart = clock();
    /*
    printf("Solucoes nao dominadas encontradas na primeira populacao (NSGA-II e PLS no pareto front):\n\n");
    for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){
        printSolution(nodeDoubly->solution);
    }*/

    FILE *outfile = fopen("ValoresDoObjetivo1DasSoluçõesIniciais.in", "w");
    if (outfile == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile2 = fopen("ValoresDoObjetivo2DasSoluçõesIniciais.in", "w");
    if (outfile2 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next) {
        fprintf(outfile, "%d\n", nodeDoubly->solution->totalObjective1);
        fprintf(outfile2, "%d\n", nodeDoubly->solution->totalObjective2);
    }

    fclose(outfile);
    fclose(outfile2);

    pauseEnd = clock();

    Population *initialOffsprings = createOffspringsForParentPopulation(initialPopulation, allEdges, numVertices);

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialOffsprings->solutions[i]);
    }

    Population *combinedPopulation = malloc(sizeof(Population));
    combinedPopulation->solutions = malloc((POPULATION_SIZE * 2) * sizeof(Solution));
    combinedPopulation->numSolutions = 0;

    for(int i = 0; i < POPULATION_SIZE; i++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialPopulation->solutions[i]);
        combinedPopulation->numSolutions++;
    }

    for(int j = 0; j < POPULATION_SIZE; j++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialOffsprings->solutions[j]);
        combinedPopulation->numSolutions++;
    }

    ListFront *allFronts = createFronts(combinedPopulation);

    freePopulation(initialPopulation);
    freePopulation(initialOffsprings);
    freePopulation(combinedPopulation);

    Population *nextGeneration = malloc(sizeof(Population));
    nextGeneration->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    nextGeneration->numSolutions = 0;

    int nextG = 0;
    for (NodeFront *nodeFront = allFronts->head; nodeFront != NULL; nodeFront = nodeFront->next){
        if(nextG == POPULATION_SIZE){
            break;
        }
        if(nextG + nodeFront->front->solutions->size > POPULATION_SIZE){
            calculateCrowdingDistance(nodeFront->front);
        }

        for(Node *node = nodeFront->front->solutions->head; node != NULL; node = node->next){
            if(nextG == POPULATION_SIZE){
                break;
            }

            nextGeneration->solutions[nextG] = *copySolution(node->solution);
            nextGeneration->numSolutions++;
            nextG++;
        }
    }

    freeListFront(allFronts);

    for (int i = 0; i < GENERATION_AMOUNT; i++){
        nextGeneration = secondaryMainLoopToNsga2WithPls(nextGeneration, nonDominatedSolutions, allEdges, numVertices);
    }

    freePopulation(nextGeneration);

    List *paretoFrontWithPls = paretoLocalSearchAdaptedToNsga2(nonDominatedSolutions, allEdges, numVertices);

    end = clock();
    /*
    printf("Todas solucoes nao dominadas encontradas apos a execucao completa (NSGA-II e PLS no pareto front):\n\n");
    for(Node *node = paretoFrontWithPls->head; node != NULL; node = node->next){
        printSolution(node->solution);
    }*/

    FILE *outfile3 = fopen("ValoresDoObjetivo1DasSoluçõesFinais.in", "w");
    if (outfile3 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile4 = fopen("ValoresDoObjetivo2DasSoluçõesFinais.in", "w");
    if (outfile4 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (Node *node = paretoFrontWithPls->head; node != NULL; node = node->next) {
        fprintf(outfile3, "%d\n", node->solution->totalObjective1);
        fprintf(outfile4, "%d\n", node->solution->totalObjective2);
    }

    fclose(outfile3);
    fclose(outfile4);

    printf("Quantidade de solucoes encontradas: %d\n\n", paretoFrontWithPls->size);

    cpuTimeUsed = ((double) (end - start - (pauseEnd - pauseStart))) / CLOCKS_PER_SEC;
    printf("NSGA-II hibridizado com PLS no pareto front levou %f segundos para executar \n", cpuTimeUsed);
}




// Função secundária para executar os passos principais do NSGA-II até o critério de parada
Population* secondaryMainLoopToNsga2Only(Population *nextGeneration, ListDoubly *nonDominatedSolutions, Edge *allEdges, int numVertices){
    Population *initialOffsprings = createOffspringsForParentPopulation(nextGeneration, allEdges, numVertices);

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialOffsprings->solutions[i]);
    }

    Population *combinedPopulation = malloc(sizeof(Population));
    combinedPopulation->solutions = malloc((POPULATION_SIZE * 2) * sizeof(Solution));
    combinedPopulation->numSolutions = 0;

    for(int i = 0; i < POPULATION_SIZE; i++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&nextGeneration->solutions[i]);
        combinedPopulation->numSolutions++;
    }

    for(int j = 0; j < POPULATION_SIZE; j++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialOffsprings->solutions[j]);
        combinedPopulation->numSolutions++;
    }

    ListFront *allFronts = createFronts(combinedPopulation);

    freePopulation(nextGeneration);
    freePopulation(initialOffsprings);
    freePopulation(combinedPopulation);

    Population *nextGeneration2 = malloc(sizeof(Population));
    nextGeneration2->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    nextGeneration2->numSolutions = 0;

    int nextG = 0;
    for (NodeFront *nodeFront = allFronts->head; nodeFront != NULL; nodeFront = nodeFront->next){
        if(nextG == POPULATION_SIZE){
            break;
        }
        if(nextG + nodeFront->front->solutions->size > POPULATION_SIZE){
            calculateCrowdingDistance(nodeFront->front);
        }

        for(Node *node = nodeFront->front->solutions->head; node != NULL; node = node->next){
            if(nextG == POPULATION_SIZE){
                break;
            }

            nextGeneration2->solutions[nextG] = *copySolution(node->solution);
            nextGeneration2->numSolutions++;
            nextG++;
        }
    }

    freeListFront(allFronts);

    return nextGeneration2;
}




// Função primária para executar os passos principais do NSGA-II até o critério de parada
void nsga2OnlyMainLoop(Population *initialPopulation, Edge *allEdges, int numVertices){
    clock_t start, end, pauseStart, pauseEnd;
    double cpuTimeUsed;

    start = clock();

    ListDoubly *nonDominatedSolutions = createListDoubly();

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialPopulation->solutions[i]);
    }

    pauseStart = clock();
    /*
    printf("Solucoes nao dominadas encontradas na primeira populacao (NSGA-II sozinho):\n\n");
    for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){
        printSolution(nodeDoubly->solution);
    }*/

    FILE *outfile = fopen("ValoresDoObjetivo1DasSoluçõesIniciaisSozinho.in", "w");
    if (outfile == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile2 = fopen("ValoresDoObjetivo2DasSoluçõesIniciaisSozinho.in", "w");
    if (outfile2 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next) {
        fprintf(outfile, "%d\n", nodeDoubly->solution->totalObjective1);
        fprintf(outfile2, "%d\n", nodeDoubly->solution->totalObjective2);
    }

    fclose(outfile);
    fclose(outfile2);

    pauseEnd = clock();

    Population *initialOffsprings = createOffspringsForParentPopulation(initialPopulation, allEdges, numVertices);

    for (int i = 0; i < POPULATION_SIZE; i++){
        addNonDominatedSolutionsToParetoFront(nonDominatedSolutions, &initialOffsprings->solutions[i]);
    }

    Population *combinedPopulation = malloc(sizeof(Population));
    combinedPopulation->solutions = malloc((POPULATION_SIZE * 2) * sizeof(Solution));
    combinedPopulation->numSolutions = 0;

    for(int i = 0; i < POPULATION_SIZE; i++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialPopulation->solutions[i]);
        combinedPopulation->numSolutions++;
    }

    for(int j = 0; j < POPULATION_SIZE; j++){
        combinedPopulation->solutions[combinedPopulation->numSolutions] = *copySolution(&initialOffsprings->solutions[j]);
        combinedPopulation->numSolutions++;
    }

    ListFront *allFronts = createFronts(combinedPopulation);

    freePopulation(initialPopulation);
    freePopulation(initialOffsprings);
    freePopulation(combinedPopulation);

    Population *nextGeneration = malloc(sizeof(Population));
    nextGeneration->solutions = malloc(POPULATION_SIZE * sizeof(Solution));
    nextGeneration->numSolutions = 0;

    int nextG = 0;
    for (NodeFront *nodeFront = allFronts->head; nodeFront != NULL; nodeFront = nodeFront->next){
        if(nextG == POPULATION_SIZE){
            break;
        }
        if(nextG + nodeFront->front->solutions->size > POPULATION_SIZE){
            calculateCrowdingDistance(nodeFront->front);
        }

        for(Node *node = nodeFront->front->solutions->head; node != NULL; node = node->next){
            if(nextG == POPULATION_SIZE){
                break;
            }

            nextGeneration->solutions[nextG] = *copySolution(node->solution);
            nextGeneration->numSolutions++;
            nextG++;
        }
    }

    freeListFront(allFronts);

    for (int i = 0; i < GENERATION_AMOUNT; i++){
        nextGeneration = secondaryMainLoopToNsga2Only(nextGeneration, nonDominatedSolutions, allEdges, numVertices);
    }

    end = clock();

    freePopulation(nextGeneration);

    FILE *outfile3 = fopen("ValoresDoObjetivo1DasSoluçõesFinaisSozinho.in", "w");
    if (outfile3 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    FILE *outfile4 = fopen("ValoresDoObjetivo2DasSoluçõesFinaisSozinho.in", "w");
    if (outfile4 == NULL) {
        printf("Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    // Escrevendo o objetivo 1 e 2 nos arquivos
    for (NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next) {
        fprintf(outfile3, "%d\n", nodeDoubly->solution->totalObjective1);
        fprintf(outfile4, "%d\n", nodeDoubly->solution->totalObjective2);
    }

    fclose(outfile3);
    fclose(outfile4);
    /*
    printf("Todas solucoes nao dominadas encontradas apos a execucao completa (NSGA-II sozinho):\n\n");
    for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){
        printSolution(nodeDoubly->solution);
    }*/

    printf("Quantidade de solucoes encontradas (NSGA-II sozinho): %d\n\n", nonDominatedSolutions->size);

    cpuTimeUsed = ((double) (end - start - (pauseEnd - pauseStart))) / CLOCKS_PER_SEC;
    printf("NSGA-II sozinho levou %f segundos para executar \n", cpuTimeUsed);
}




int main() {
    srand(time(0));

    FILE *file = fopen("20.grasp1.in", "r");
    if (file == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return 1;
    }

    int numVertices;
    fscanf(file, "%d", &numVertices);

    int amountOfAllEdges = (numVertices * (numVertices - 1)) / 2;

    Edge *allEdges = malloc(amountOfAllEdges * sizeof(Edge));
    for (int i = 0; i < amountOfAllEdges; i++) {
        fscanf(file, "%d %d %d %d", &allEdges[i].vertex1, &allEdges[i].vertex2, &allEdges[i].objectives[0], &allEdges[i].objectives[1]);
    }

    fclose(file);

    Population *initialPopulation = createInitialPopulation(allEdges, amountOfAllEdges, numVertices);
    Population *copyOfInitialPopulation = copyPopulation(initialPopulation);

    nsga2OnlyMainLoop(initialPopulation, allEdges, numVertices);

    printf("\n=========================================================================================\n\n");

    nsga2WithPlsMainLoop(copyOfInitialPopulation, allEdges, numVertices);

    return 0;
}

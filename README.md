# NSGA-II-hibridizado-com-PLS-no-Pareto-front
NSGA-II hibridizado com PLS no Pareto front para o problema da árvore geradora biobjetiva

O arquivo de entrada da instância da árvore geradora biobjetiva (AG-BI) deve seguir o mesmo padrão que os arquivos .grasp para o algoritmo funcinar corretamente. Para escolher qual instância o algoritmo usará, é preciso mudar o nome do aquivo "20.grasp1.in" na linha 208 "FILE *file = fopen("20.grasp1.in", "r");", na função main, para a instância desejada.

Este algoritmo executa NSGA-II sozinho com a mesma população inicial que a população inicial para o híbrido, por motivos de comparação. Para desativar a execução do NSGA-II sozinho é necessário comentar a linha 2103 "nsga2OnlyMainLoop(initialPopulation, allEdges, numVertices);"

Para modificar os parâmetros do algoritmo é preciso alterar os valores das constantes: POPULATION_SIZE, TOURNAMENT_SIZE, GENERATION_AMOUNT, MUTATION_RATE e CROSSOVER_RATE

Para visualizar as arestas e o valor total dos objetivos de cada solução no terminal é preciso remover "/" e "/" das seguintes seções:

/*

printf("Solucoes nao dominadas encontradas na primeira populacao (NSGA-II e PLS embutido):\n\n");

for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){

printSolution(nodeDoubly->solution);

}\*/

/*

printf("Todas solucoes nao dominadas encontradas apos a execucao completa (NSGA-II e PLS no pareto front):\n\n");

for(Node *node = paretoFrontWithPls->head; node != NULL; node = node->next){

printSolution(node->solution);

}\*/

/*

printf("Solucoes nao dominadas encontradas na primeira populacao (NSGA-II sozinho):\n\n");

for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){

printSolution(nodeDoubly->solution);

}\*/

/*

printf("Todas solucoes nao dominadas encontradas apos a execucao completa (NSGA-II sozinho):\n\n");

for(NodeDoubly *nodeDoubly = nonDominatedSolutions->head; nodeDoubly != NULL; nodeDoubly = nodeDoubly->next){

printSolution(nodeDoubly->solution);

}\*/

No momento os valores total de cada objetivo de cada solução não dominada na população inicial e no pareto front são escritos nos arquivos: ValoresDoObjetivo1DasSoluçõesIniciaisSozinho, ValoresDoObjetivo2DasSoluçõesIniciaisSozinho, ValoresDoObjetivo1DasSoluçõesFinaisSozinho, ValoresDoObjetivo2DasSoluçõesFinaisSozinho, ValoresDoObjetivo1DasSoluçõesIniciais, ValoresDoObjetivo2DasSoluçõesIniciais, ValoresDoObjetivo1DasSoluçõesFinais e ValoresDoObjetivo2DasSoluçõesFinais. Esse metódo foi escolhido para não transbordar o terminal, além de facilitar a armazenação dos resultados em algo como uma planilha.

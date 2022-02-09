#include "Game.h"
#include "Random.h" 
#include "Observer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Agent initializeAgent(int id, stratType strat, int maxMemory) {
	extern observer globalObserver;
	Agent agent = (Agent)malloc(sizeof(struct agent));
	if (agent == NULL) exit(1);
	agent->id = id;
	agent->strategy = strat; globalObserver.n[strat]++;
	agent->memory = initializeMemory(maxMemory);

	return agent;
}

Memory initializeMemory(int maxMemory) {
	Memory memory  = (Memory)malloc(sizeof(struct memory));
	if (memory == NULL) exit(1);
	memory->rounds = 0;
	memory->maxMemory = maxMemory;
	memory->games = (Game*)malloc(sizeof(Game)*maxMemory);
	if (memory->games == NULL) exit(1);

	return memory;
}

/*Creates a vector of agents with random strategies between 0 and 3*/
Agent * randomInitializeAgents(long *seed, int nAgents, int maxMemory, double * init) {
	Agent * agents = (Agent*)malloc(sizeof(Agent)*nAgents);
	if (agents == NULL) exit(1);

	double r;
	
	for (int i = 0; i < nAgents; i++) {
		r = randEBD(seed);
		if (r < init[0])
			agents[i] = initializeAgent(i, NDD, maxMemory);
		else if (r < init[0]+init[1])
			agents[i] = initializeAgent(i, NDC, maxMemory);
		else if (r < init[0] + init[1]+init[2])
			agents[i] = initializeAgent(i, NCD, maxMemory);
		else if (r < init[0] + init[1] + init[2] + init[3])
			agents[i] = initializeAgent(i, NCC, maxMemory);
		else if (r < init[0] + init[1] + init[2] + init[3] + init[4])
			agents[i] = initializeAgent(i, YDD, maxMemory);
		else if (r < init[0] + init[1] + init[2] + init[3] + init[4] + init[5])
			agents[i] = initializeAgent(i, YDC, maxMemory);
		else if (r < init[0] + init[1] + init[2] + init[3] + init[4] + init[5] + init[6])
			agents[i] = initializeAgent(i, YCD, maxMemory);
		else 
			agents[i] = initializeAgent(i, YCC, maxMemory);
	}
		
	return agents;
}

void printAgent(Agent agent) {
	printf("Player %d strategy:\t%d\n", agent->id, agent->strategy);
	printf("Games played:\t%d\n", agent->memory->rounds);
	for (int i = 0; i < min(agent->memory->maxMemory, agent->memory->rounds); i++)
		printGame(agent->memory->games[i]);

	return;
}

void printAgentShort(Agent agent) {
	printf("Player %d strategy:\t%d\n", agent->id, agent->strategy);
	printf("Games played:\t%d\n", agent->memory->rounds);
	return;
}

void printAgents(Agent* agents, int n) {
	for (int i = 0; i < n; i++) {
		printAgent(agents[i]); printf("\n");
	}
	return;
}

void printAgentsShort(Agent* agents, int n) {
	for (int i = 0; i < n; i++) {
		printAgentShort(agents[i]); printf("\n");
	}
	return;
}

int findAgent(Agent * agents, int size, int id) {
	if (agents[id]->id == id)
		return id;
	else {
		for (int i = 0; i < size; i++) {
			if (agents[i]->id == id)
				return i;
		}
	}

	return -1;
}

void freeAgent(Agent agent) {
	for (int i = 0; i < min(agent->memory->maxMemory, agent->memory->rounds); i++)
		forgetGame(agent->memory->games[i]);
	free(agent->memory->games);
	free(agent->memory);
	free(agent);
}
#pragma once

typedef struct agent * Agent;
typedef struct game * Game;
typedef struct memory * Memory;

typedef enum stratType_ {NDD, NDC, NCD, NCC, YDD, YDC, YCD, YCC} stratType;

#define mutIndex (0)
#define betaIndex (1)
#define cIndex (2+0)
#define FIndex (2+1)
#define ciIndex (2+2)
#define FiIndex (2+3)
#define qIndex (2+4)

#define PIDIndex (0)
#define gameIndex (1)
#define ZIndex (2)
#define NIndex (3)
#define maxMemIndex (4)
#define nGamesIndex (5)


struct game {
	stratType * strategies;
	int groupSize;
	double * outcomes;
	int inMemoryOf;
	Agent * players;
};


Game initializeGame(int groupSize, Agent * players);
void playGameA(Game game, int * intParams, double * doubParams);
void playGameB(Game game, int * intParams, double * doubParams);
void playGameD(Game game, int * intParams, double * doubParams);
void playGameC(Game game, int * intParams, double * doubParams);
void rememberGame(Game game, Agent * agents);
void forgetGame(Game game);
void printGame(Game game);




//Agent.h

struct memory {
	int maxMemory;
	int rounds;
	Game * games;
};

struct agent {
	int id;
	stratType strategy;
	Memory memory;
};


Agent initializeAgent(int id, stratType strat, int maxMemory);
Memory initializeMemory(int maxMemory);
Agent * randomInitializeAgents(long* seed, int nAgents, int maxMemory, double * init);
void printAgent(Agent agent);
void printAgentShort(Agent agent);
void printAgents(Agent* agents, int n);
void printAgentsShort(Agent * agents, int n);
int findAgent(Agent * agents, int size, int id);

void freeAgent(Agent agent);

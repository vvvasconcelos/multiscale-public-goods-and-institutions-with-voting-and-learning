#include <stdio.h>
#include <stdlib.h>
#include "Random.h"
#include "StrategyRevision.h"
#include <time.h>
#include "Game.h"
#include "Observer.h"

#define _DEBUG_ 0
#define _GRAD_ 0

#if _DEBUG_
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/
#endif

int main(int argc, char * argv[]);

void getparameters(char * argv[], int * intParams, double * doubParams);
void setparameters(int * intParams, double * doubParams, int * PID, int *game, int * Z, int * N, int * maxMem, int * nGames);
void setseeds(int PID, long * seed, long * seedStrat, long * seedSelect, long * seedMut, long * seedMutSel, long * seedVillage);

#define POPSIZE 100
#define GROUPSIZE 2
#define MAXMEM 10
#define NGAMES 10000


observer globalObserver;


int main(int argc, char *argv[]) {

#if _DEBUG_
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(95);
#endif

	//Parameters and seeds
	int intParams[] = { 1,/*Game*/ 1, POPSIZE, GROUPSIZE, MAXMEM, NGAMES };
	double doubParams[] = { /*mu*/0.001, /*beta*/1.0, /*c*/1.0, /*F*/1.8, /*ci*/0.1, /*Fi*/2.0 , /*Quorum*/0.5, /*x0init*/ 0.125,/*x1init*/ 0.125,/*x2init*/ 0.125,0.125,0.125,0.125,0.125};
	int PID, Z, N, maxMem, nGames, c;
	double* init = &doubParams[7];
	if (argc == 21) {
		getparameters(argv, intParams, doubParams);
	}
	else {
		printf("Using default parameters\n");
	}

	setparameters(intParams, doubParams, &PID, &c, &Z, &N, &maxMem, &nGames);
	initializeObserver(&globalObserver, (Z + 1)*(Z + 1), nGames);

	long seed, seedStrat, seedSelect, seedMut, seedMutSel, seedVillage;
	setseeds(PID, &seed, &seedStrat, &seedSelect, &seedMut, &seedMutSel, &seedVillage);

	//Run the game
	for (int runs = 0; runs < 1; runs++) {
		//Randomly initialize agents
		for(int i=NDD; i<=YCC; i++)
			globalObserver.n[i] = 0;
		Agent * agents = randomInitializeAgents(&seed, Z, maxMem, init);

		//Set vectors of sampling from population and both villages
		int * randIdY = RangeVec(Z - 1);
		int * randIdN = RangeVec(Z - 1);
		int * randIdAll = RangeVec(Z - 1);

		for (int i = 0, iY = 0, iN = 0; i < Z; i++) {
			if (agents[i]->strategy <= NCC)
				randIdN[iN++] = agents[i]->id;
			else
				randIdY[iY++] = agents[i]->id;
		}

		
		//Select the rules of the game to be played
		void(*playGame)(Game, int*, double*);
		switch (c - 1 + 'A') {
		case 'A': playGame = playGameA; break;
		case 'B': playGame = playGameB; break;
		case 'C': playGame = playGameC; break;
		case 'D': playGame = playGameD; break;
		default: printf("Game rules not found.\n"); system("pause"); exit(1);
		}

		//printAgentsShort(agents, Z);
		int ZY, ZN, ZVil;

		for (int g = 0; g < nGames; g++) {
			globalObserver.time = g;
			ZN = ZY = 0;
			for(int i=NDD; i<=NCC; i++)
				ZN += globalObserver.n[i];
			for (int i = YDD; i <= YCC; i++)
				ZY += globalObserver.n[i];  

			//printf("ZY neq Z-ZN. %d neq %d - %d\n", ZY, Z, ZN); printOberver(&globalObserver);
			if (ZY != Z - ZN) {
				printf("ZY neq Z-ZN. %d neq %d - %d\n", ZY, Z, ZN); system("pause"); exit(1);
			}


			//Select a village to play the game
			double rand = randEBD(&seedVillage)*(Z);
			int vId, *randId;
			if (rand < ZN) {
				vId = 0;
				randId = randIdN;
				ZVil = ZN;
			}
			else {
				vId = 1;
				randId = randIdY;
				ZVil = ZY;
			}

			int groupSize;
			if (c - 1 + 'A' == 'A')
				groupSize = min(N, ZVil);
			else {
				groupSize = N;
				ZVil = Z;
				randId = randIdAll;
			}

			Agent * players = (Agent*)malloc(sizeof(Agent)*groupSize);
			if (players == NULL) exit(1);

			randomFirstN(&seed, randId, ZVil - 1, groupSize);

			for (int i = 0; i < groupSize; i++) {
				players[i] = agents[randId[i]];
			}

			Game game = initializeGame(groupSize, players);
			playGame(game, intParams, doubParams);
			rememberGame(game, agents);


			if(g%Z==0) printObserver(&globalObserver); //system("Pause");

			/*i0 += globalObserver.n[0];
			i1 += globalObserver.n[1];
			i2 += globalObserver.n[2];
			i3 += globalObserver.n[3];

			if(g%1==0) printf("%g\t%g\t%g\t%g\n", 100*i0/(g+1)/Z, 100*i1 / (g + 1) / Z, 100*i2 / (g + 1) / Z, 100*i3 / (g + 1) / Z);
			*/
			for (int i = 0; i < groupSize; i++) {
				stratType stratFrom = players[i]->strategy;
				if (reviseStrategy(&seedStrat, &seedSelect, &seedMut, &seedMutSel, players[i], doubParams, intParams)) {
					stratType stratTo = players[i]->strategy;

					if (stratFrom <= NCC && stratTo >= YDD) { //change from N to Y
						if (ZY == Z) printf("my seg fault0\n");
						randIdY[ZY] = randId[i];
						for (int j = 0; j < ZN; j++)
							if (randIdN[j] == randId[i]) {
								randIdN[j] = randIdN[ZN - 1];
								j = ZN;
							}
						ZN--; ZY++;

					}
					else if (stratFrom >= YDD && stratTo <= NCC) {//change from Y to N
						if (ZN == Z) printf("my seg fault0\n");
						randIdN[ZN] = randId[i];
						for (int j = 0; j < ZY; j++)
							if (randIdY[j] == randId[i]) {
								randIdY[j] = randIdY[ZY - 1];
								j = ZY;
							}
						ZY--; ZN++;

					}
				}
			}

			//printAgents(agents, Z);


		}

		

		for (int i = 0; i < Z; i++) {
			freeAgent(agents[i]);
		}
		free(agents);

		free(randIdAll);
		free(randIdN);
		free(randIdY);
	}

	//printOberverGrad(&globalObserver, Z, nGames);

	deleteObserver(&globalObserver, (Z + 1)*(Z + 1), nGames);

#if _DEBUG_
	_CrtDumpMemoryLeaks();
	system("pause");
#endif
	return 0;
}



void getparameters(char * argv[], int * intParams, double * doubParams) {
	int nints = 6;

	for (int i = 0; i < nints; i++)
		intParams[i] = atoi(argv[1 + i]);

	for (int i = 0; i < 14; i++)
		doubParams[i] = atof(argv[1 + nints + i]);
}

void setparameters(int * intParams, double * doubParams, int* PID, int *game, int* Z, int* N, int* maxMem, int* nGames) {
	*PID = intParams[0];
	*game = intParams[1];
	*Z = intParams[2];
	*N = intParams[3];
	*maxMem = intParams[4];
	*nGames = intParams[5]; 
}

void setseeds(int PID, long * seed, long * seedStrat, long * seedSelect, long * seedMut, long * seedMutSel, long * seedVillage) {
	* seed = (long) -time(NULL)*PID; randEBD(seed);
	* seedStrat = (long) -2 * time(NULL)*PID; randEBD(seedStrat);
	* seedSelect = (long) -3 * time(NULL)*PID; randEBD(seedSelect);
	* seedMut = (long) -4 * time(NULL)*PID; randEBD(seedMut);
	* seedMutSel = (long) -5 * time(NULL)*PID; randEBD(seedMutSel);
	* seedVillage = (long) -6 * time(NULL)*PID; randEBD(seedVillage);
}
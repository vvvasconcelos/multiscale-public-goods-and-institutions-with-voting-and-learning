#include "Game.h"
#include "Observer.h"
#include <stdlib.h>
#include <stdio.h>

#define GLOBALINFO (0)


Game initializeGame(int groupSize, Agent * players) {
	if (groupSize < 1) printf("something wrong: group size is %d.\n", groupSize);
	Game game = (Game)malloc(sizeof(struct game));
	if (game == NULL) exit(1);
	game->groupSize = groupSize;
	game->players = players;
	game->strategies = (stratType *)malloc(sizeof(stratType)*groupSize);
	if (game->strategies == NULL) exit(1);
	game->outcomes = (double *)malloc(sizeof(double)*groupSize);
	if (game->outcomes == NULL) exit(1);

	for (int i = 0; i < groupSize; i++) {
		game->strategies[i] = players[i]->strategy;
		game->outcomes[i] = 0;
	}
	game->inMemoryOf = 0;

	return game;
}

void playGameA(Game game, int * intParams, double * doubParams) {
	double c = doubParams[2 + 0];
	double F = doubParams[2 + 1];
	double ci = doubParams[2 + 2];
	double Fi = doubParams[2 + 3];
	int n = game->groupSize;
	int nNDD, nNDC, nNCD, nNCC;
	int nYDD, nYDC, nYCD, nYCC;
	nNDD = nNDC = nNCD = nNCC = nYDD = nYDC = nYCD = nYCC = 0;
	int nY, nN;
	int nYC, nYD, nNC, nND;

	//double contrib = 0;
	//double contribInst = 0;
	/*0 - ND, 1-NC, 2-YD, 3-YC*/

	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
			case NDD: nNDD++; break;
			case NDC: nNDC++; break;
			case NCD: nNCD++; break;
			case NCC: nNCC++; break;
			case YDD: nYDD++; break;
			case YDC: nYDC++; break;
			case YCD: nYCD++; break;
			case YCC: nYCC++; break;
			default: exit(1); break;
		}
	}
	nYC = nYDC + nYCC;
	nYD = nYDD + nYCD;
	nNC = nNCD + nNCC;
	nND = nNDD + nNDC;
	nY = nYD + nYC;
	nN = nND + nNC;

	if (nY*nN != 0) { printf("Error: there is N(Y) in Y(N) village."); exit(1); } //guarantees members of only one village

	//if (nY > 0) printf("nY: %d, nYC: %d, nYD: %d, nYDC: %d, nYCC: %d, nYDD: %d, nYCD: %d\n", nY, nYC, nYD, nYDC, nYCC, nYDD, nYCD);

	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD:
			game->outcomes[i] = (nNC)*c*F / n;
			break;

		case NDC:
			game->outcomes[i] = (nNC)*c*F / n;
			break;

		case NCD:
			game->outcomes[i] = (nNC)*c*F / n - c;
			break;

		case NCC:
			game->outcomes[i] = (nNC)*c*F / n - c;
			break;

		case YDD:
			game->outcomes[i] = (nYC)*c*F / n - ci - nY*ci*Fi/(nYD);
			break;

		case YDC:
			game->outcomes[i] = (nYC)*c*F / n - c - ci;  
			break;

		case YCD:
			game->outcomes[i] = (nYC)*c*F / n - ci - nY*ci*Fi / (nYD);
			break;

		case YCC:
			game->outcomes[i] = (nYC)*c*F / n - c - ci;
			break;

		default:
			exit(1);
			break;
		}

	}
	return;
}

void playGameB(Game game, int * intParams, double * doubParams) {
	double c = doubParams[2 + 0];
	double F = doubParams[2 + 1];
	double ci = doubParams[2 + 2];
	double Fi = doubParams[2 + 3];
	int Z = intParams[2];
	int quorum = (int)(Z * doubParams[6] + 0.5);
	int n = game->groupSize;
	int nNDD, nNDC, nNCD, nNCC;
	int nYDD, nYDC, nYCD, nYCC;
	nNDD = nNDC = nNCD = nNCC = nYDD = nYDC = nYCD = nYCC = 0;
	int iY;
	int nC, nD;
	//int nY, nN;
	//int nYC, nYD, nNC, nND;


	extern observer globalObserver;
	iY = globalObserver.n[YDD] + globalObserver.n[YDC] + globalObserver.n[YCD] + globalObserver.n[YCC];

	//double contrib = 0;
	//double contribInst = 0;
	/*0 - ND, 1-NC, 2-YD, 3-YC*/

	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD: nNDD++; break;
		case NDC: nNDC++; break;
		case NCD: nNCD++; break;
		case NCC: nNCC++; break;
		case YDD: nYDD++; break;
		case YDC: nYDC++; break;
		case YCD: nYCD++; break;
		case YCC: nYCC++; break;
		default: exit(1); break;
		}
	}
	//nYC = nYDC + nYCC;
	//nYD = nYDD + nYCD;
	//nNC = nNCD + nNCC;
	//nND = nNDD + nNDC;
	//nY = nYD + nYC;
	//nN = nND + nNC;
	nC = (iY > quorum ? nNDC + nNCC + nYDC + nYCC : nNCD + nNCC + nYCD + nYCC );
	nD = (iY > quorum ? nNDD + nNCD + nYDD + nYCD : nNDD + nNDC + nYDD + nYDC); if (nD != n - nC) printf("SOMETHING WRONG!!!\n");


	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + n*ci*Fi / (nD) : 0);
			break;

		case NDC:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + c : 0);
			break;

		case NCD:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + n*ci*Fi / (nD) : c);
			break;

		case NCC:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + c : c);
			break;

		case YDD:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + n*ci*Fi / (nD) : 0);
			break;

		case YDC:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + c : 0);
			break;

		case YCD:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + n*ci*Fi / (n - nC) : c);
			break;

		case YCC:
			game->outcomes[i] = (nC)*c*F / n - (iY > quorum ? ci + c : c);
			break;

		default:
			exit(1);
			break;
		}

	}

	return;
}

void playGameC(Game game, int * intParams, double * doubParams) {
	double c = doubParams[2 + 0];
	double F = doubParams[2 + 1];
	double ci = doubParams[2 + 2];
	double Fi = doubParams[2 + 3];
	int Z = intParams[2]; 
	int n = game->groupSize;/*N=Z*/if (n != Z) printf("Group size is not population size.\n");
	int nNDD, nNDC, nNCD, nNCC;
	int nYDD, nYDC, nYCD, nYCC;
	nNDD = nNDC = nNCD = nNCC = nYDD = nYDC = nYCD = nYCC = 0;
	int iY;
	int iYC, iYD, iNC, iND, iC;

	extern observer globalObserver;
	
	//printf("c %g, F %g, ci %g, Fi %g, n %d, Z %d\n", c, F, ci, Fi, n, Z);

	//double contrib = 0;
	//double contribInst = 0;
	/*0 - ND, 1-NC, 2-YD, 3-YC*/

	/*for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD: nNDD++; break;
		case NDC: nNDC++; break;
		case NCD: nNCD++; break;
		case NCC: nNCC++; break;
		case YDD: nYDD++; break;
		case YDC: nYDC++; break;
		case YCD: nYCD++; break;
		case YCC: nYCC++; break;
		default: exit(1); break;
		}
	}*/

	iYC = globalObserver.n[YDC] + globalObserver.n[YCC];
	iYD = globalObserver.n[YDD] + globalObserver.n[YCD];
	iNC = globalObserver.n[NCD] + globalObserver.n[NCC];
	iND = globalObserver.n[NDD] + globalObserver.n[NDC];
	iY = iYD + iYC;
	iC = iNC + iYC;
	//nC = (nY > Z / 2 ? nNDC + nNCC + nYDC + nYCC : nNCD + nNCC + nYCD + nYCC);
	//nD = (nY > Z / 2 ? nNDD + nNCD + nYDD + nYCD : nNDD + nNDC + nYDD + nYDC); if (nD != n - nC) printf("SOMETHING WRONG!!!\n");
	//if (iC != nC) printf("Something wrong!\n");

	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD:
			game->outcomes[i] = (iC)*c*F / Z;
			break;

		case NDC:
			game->outcomes[i] = (iC)*c*F / Z;
			break;

		case NCD:
			game->outcomes[i] = (iC)*c*F / Z - c;
			break;

		case NCC:
			game->outcomes[i] = (iC)*c*F / Z - c;
			break;

		case YDD:
			game->outcomes[i] = (iC)*c*F / Z - (ci + iY*ci*Fi / (iYD) );
			break;

		case YDC:
			game->outcomes[i] = (iC)*c*F / Z - c - ci;
			break;

		case YCD:
			game->outcomes[i] = (iC)*c*F / Z - (ci + iY*ci*Fi / (iYD));
			break;

		case YCC:
			game->outcomes[i] = (iC)*c*F / Z - c - ci;
			break;

		default:
			exit(1);
			break;
		}

	}
	return;
}

void playGameD(Game game, int * intParams, double * doubParams) {
	double c = doubParams[2 + 0];
	double F = doubParams[2 + 1];
	double ci = doubParams[2 + 2]; 
	double Fi = doubParams[2 + 3]; 
	int Z = intParams[2];
	int n = game->groupSize;
	int quorum = (int)(n * doubParams[6] + 0.5);
	int nNDD, nNDC, nNCD, nNCC;
	int nYDD, nYDC, nYCD, nYCC;
	nNDD = nNDC = nNCD = nNCC = nYDD = nYDC = nYCD = nYCC = 0;
	int iY, iC, iD;
	int iYC, iYD, iNC, iND;

	extern observer globalObserver;

	iYC = globalObserver.n[YDC] + globalObserver.n[YCC];
	iYD = globalObserver.n[YDD] + globalObserver.n[YCD];
	iNC = globalObserver.n[NCD] + globalObserver.n[NCC];
	iND = globalObserver.n[NDD] + globalObserver.n[NDC];


	iY = globalObserver.n[YDC] + globalObserver.n[YCC] + globalObserver.n[YDD] + globalObserver.n[YCD];

	if (iY > quorum) {
		iC = globalObserver.n[YDC] + globalObserver.n[YCC] + globalObserver.n[NCC] + globalObserver.n[NDC];
	}
	else {
		iC = globalObserver.n[YCC] + globalObserver.n[YCD] + globalObserver.n[NCD] + globalObserver.n[NCC];
	
}

	iD = Z - iC;

	if (n != Z) {
		printf("Something is wrong. Z neq N\n"); exit(1);
	}
	/*
	iC = iNC + iYC;
	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD: nNDD++; break;
		case NDC: nNDC++; break;
		case NCD: nNCD++; break;
		case NCC: nNCC++; break;
		case YDD: nYDD++; break;
		case YDC: nYDC++; break;
		case YCD: nYCD++; break;
		case YCC: nYCC++; break;
		default: exit(1); break;
		}
	}
	
	nYC = nYDC + nYCC;
	nYD = nYDD + nYCD;
	nNC = nNCD + nNCC;
	nND = nNDD + nNDC;
	nY = nYD + nYC;
	nN = nND + nNC;
	nC = (nY > Z / 2 ? nNDC + nNCC + nYDC + nYCC : nNCD + nNCC + nYCD + nYCC);
	nD = (nY > Z / 2 ? nNDD + nNCD + nYDD + nYCD : nNDD + nNDC + nYDD + nYDC); if (nD != n - nC) printf("SOMETHING WRONG!!!\n");
	
	if (nNDD + nNDC + nNCD + nNCC + nYDD + nYDC + nYCD + nYCC != n) printf("Something wrong with counting players'strategies.\n");
	*/

	for (int i = 0; i < n; i++) {
		switch (game->strategies[i])
		{
		case NDD: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? (iC)*ci*Fi / (iD) + ci : 0);	break;
		case NDC: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? c + ci : 0);					break;
		case NCD: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? (iC)*ci*Fi / (iD) + ci: c);		break;
		case NCC: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? c + ci : c);					break;
		case YDD: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? (iC)*ci*Fi / (iD) + ci : 0);	break;
		case YDC: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? c + ci : 0);					break;
		case YCD: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? (iC)*ci*Fi / (iD) + ci : c);	break;
		case YCC: game->outcomes[i] = (iC)*c*F / n - (iY > quorum ? c + ci : c);					break;
		default: exit(1); break;
		}

	}
	return;
}

#if GLOBALINFO

void rememberGame(Game game, Agent * agents) {
	extern observer globalObserver;
	int Z = 0; for (int i = NDD; i <= YCC; i++) Z+= globalObserver.n[i];
	int n = Z;

	for (int i = 0; i < n; i++) {
		int r = (agents[i]->memory->rounds)++;
		int max = agents[i]->memory->maxMemory;
		if (r >= max) {
			forgetGame(agents[i]->memory->games[r%max]);
		}
		agents[i]->memory->games[r%max] = game;
		game->inMemoryOf++;
	}

	return;
}

#else

void rememberGame(Game game, Agent * agents) {
	int n = game->groupSize;

	for (int i = 0; i < n; i++) {
		int r = (game->players[i]->memory->rounds)++;
		int max = game->players[i]->memory->maxMemory;
		if (r >= max) {
			forgetGame(game->players[i]->memory->games[r%max]);
		}
		game->players[i]->memory->games[r%max] = game;
		game->inMemoryOf++;
	}

	return;
}

#endif

void forgetGame(Game game) {
	if (game->inMemoryOf <= 0) printf("something went really wrong\n");
	game->inMemoryOf--;
	if (game->inMemoryOf == 0) {
		//printf("forgetting game.\n");
		if (game->outcomes == NULL)printf("LELEL\n");
		free(game->outcomes);
		//printf("outcomes forgotten");
		if (game->outcomes == NULL) printf("LOLOLOL\n");
		free(game->players);
		//printf("players forgotten");
		if (game->strategies == NULL) printf("VERY BAD\n");
		free(game->strategies);
		//printf("strategies forgotten");


		free(game); //printf("game forgotten");
	}
	return;
}

void printGame(Game game) {
	
	printf("Group size:\t%d\n", game->groupSize);
	for (int i = 0; i < game->groupSize; i++) {
		printf("s%d\t", game->strategies[i]);
	}printf("\noutomes: ");
	for (int i = 0; i < game->groupSize; i++) {
		printf("%g\t", game->outcomes[i]);
	}printf("\nplayers: ");
	for (int i = 0; i < game->groupSize; i++) {
		printf("%d\t", (game->players)[i]->id);
	}printf("\n");

	return;
}
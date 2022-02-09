#include "Random.h"
#include <stdlib.h>

/*Creates an ordered vector of integers between 0 and maxId (inclusive)*/
int * RangeVec(int maxId) {
	int * randId = (int*)malloc(sizeof(int)*(maxId + 1));
	if (randId == NULL) exit(1);
	for (int i = 0; i <= maxId; i++) {
		randId[i] = i;
	}
	return randId;
}

void randomLastN(long * seed, int * randId, int maxId, int n) {
	int r, temp;
	double rand;

	for (int i = 0; i < n; i++) {
		rand = randEBD(seed);
		r = maxId - (i + (int)((maxId + 1. - i)*rand));
		temp = randId[maxId-i];
		randId[maxId-i] = randId[r];
		randId[r] = temp;
	}
}

void randomFirstN(long * seed, int * randId, int maxId, int n) {
	if (n > maxId / 2) {
		randomLastN(seed, randId, maxId, maxId - n + 1);
	}
	else {
		int r, temp;
		double rand;

		for (int i = 0; i < n; i++) {
			rand = randEBD(seed);
			r = i + (int)((maxId + 1. - i)*rand);
			temp = randId[i];
			randId[i] = randId[r];
			randId[r] = temp;
		}
	}
}

/*****************************************************************
* Long period (> 2 x 10^18) random number generator of           *
* L'Ecuyer with Bays-Durham shuffle and added safeguards.        *
* Returns a uniform random deviate between 0.0 and 1.0           *
* (exclusive of the endpoint values). Call with idum a negative  *
* integer to initialize; thereafter, do not alter idum between   *
* successive deviates in a sequence. RNMX should approximate     *
* the largest floating value that is less than 1.                *
* From Numerical Recepies                                        *
*****************************************************************/
#define IM1_1 2147483563
#define IM2_1 2147483399
#define AM_1 (1.0/IM1_1)
#define IMM1_1 (IM1_1-1)
#define IA1_1 40014
#define IA2_1 40692
#define IQ1_1 53668
#define IQ2_1 52774
#define IR1_1 12211
#define IR2_1 3791
#define NTAB_1 32
#define NDIV_1 (1+IMM1_1/NTAB_1)
#define EPS_1 1.2e-7
#define RNMX_1 (1.0-EPS_1)


double randEBD(long *idum) {
	int j;
	long k;
	static long idum2 = 123456789, iy = 0, iv[NTAB_1];
	double temp;

	if (*idum <= 0) {
		if (-(*idum) < 1) *idum = 1;
		else *idum = -(*idum);
		idum2 = (*idum);
		for (j = NTAB_1 + 7; j >= 0; j--) {
			k = (*idum) / IQ1_1;
			*idum = IA1_1*(*idum - k*IQ1_1) - k*IR1_1;
			if (*idum < 0) *idum += IM1_1;
			if (j < NTAB_1) iv[j] = *idum;
		}
		iy = iv[0];
	}
	k = (*idum) / IQ1_1;
	*idum = IA1_1*(*idum - k*IQ1_1) - k*IR1_1;
	if (*idum < 0) *idum += IM1_1;
	k = idum2 / IQ2_1;
	idum2 = IA2_1*(idum2 - k*IQ2_1) - k*IR2_1;
	if (idum2 < 0) idum2 += IM2_1;
	j = iy / NDIV_1;
	iy = iv[j] - idum2;
	iv[j] = *idum;
	if (iy < 1) iy += IMM1_1;
	if ((temp = AM_1*iy) > RNMX_1) return RNMX_1;
	else return temp;
}
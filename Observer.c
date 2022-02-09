#include "Game.h"
#include "Observer.h"
#include <stdio.h>
#include <stdlib.h>

void initializeObserver(observer * observer, int nConfigs, int tMax) {
	for (int i = NDD; i <= YCC; i++) {
		observer->n[i] = 0;
	}

#if _GRAD_

	const int neigh = 4;
	observer->gradient = (double ***)malloc(sizeof(double **)*tMax); if (observer->gradient == NULL) exit(1);
	observer->gradientCount = (int ***)malloc(sizeof(int**)*tMax); if (observer->gradientCount == NULL) exit(1);

	for (int i = 0; i < tMax; i++) {

		observer->gradient[i] = (double **)malloc(sizeof(double*)*nConfigs); if (observer->gradient[i] == NULL) exit(1);
		observer->gradientCount[i] = (int**)malloc(sizeof(int*)*nConfigs); if (observer->gradientCount[i] == NULL) exit(1);

		for (int j = 0; j < nConfigs; j++) {

			int * pi = (int*)malloc(sizeof(*pi) * neigh); if (pi == NULL) exit(1);
			observer->gradientCount[i][j] = pi;

			double * x = (double*)malloc(sizeof(double) * neigh); if (x == NULL) exit(1);
			observer->gradient[i][j] = x;

			for (int k = 0; k < neigh; k++) {
				observer->gradient[i][j][k] = 0.0;
				observer->gradientCount[i][j][k] = 0;
			}
		}
	}
#endif // _GRAD_

}

void initializeObserverSkipGrad(observer * observer, int nConfigs, int tMax) {
	for (int i = NDD; i <= YCC; i++) {
		observer->n[i] = 0;
	}
}

void printObserver(observer * observer) {
	for (int i = NDD; i <= YCC; i++) {
		printf("%d\t", observer->n[i]);
	} printf("\n");
}

void printOberverGrad(observer * observer, int Z, int tMax) {
	double grad;
	for (int t = 0; t < tMax; t++) {
		for (int ic = 0; ic <= Z; ic++) {
			for (int iy = 0; iy <= Z; iy++) {
				int conf=(iy)*(Z + 1) + (ic);
				grad = -200.;
				if (observer->gradientCount[t][conf][0] != 0 && observer->gradientCount[t][conf][2] != 0)
					 grad = observer->gradient[t][conf][0]/ observer->gradientCount[t][conf][0] + observer->gradient[t][conf][2]/ observer->gradientCount[t][conf][2];
				printf("%d\t%d\t%d\t%g\t", t, ic, iy, grad);

				grad = -200;
				if (observer->gradientCount[t][conf][1] != 0 && observer->gradientCount[t][conf][3] != 0)
					grad = observer->gradient[t][conf][1] / observer->gradientCount[t][conf][1] + observer->gradient[t][conf][3] / observer->gradientCount[t][conf][3];
				printf("%g\n", grad);
			}
		}
	}
}

void deleteObserver(observer *observer, int nConfigs, int tMax) {
#if _GRAD_

	for (int i = 0; i < tMax; i++) {
		for (int j = 0; j < nConfigs; j++) {
			free(observer->gradient[i][j]);
			free(observer->gradientCount[i][j]);
		}
		free(observer->gradient[i]);
		free(observer->gradientCount[i]);
	}
	free(observer->gradient);
	free(observer->gradientCount);
#endif
}
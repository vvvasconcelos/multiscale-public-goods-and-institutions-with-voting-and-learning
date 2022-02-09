#pragma once

typedef struct {
	int *** gradientCount; 
	double *** gradient; 
	int n[YCC+1];
	int time;
} observer;

void initializeObserver(observer * observer, int nConfigs, int tMax);

void printObserver(observer * observer);

void printOberverGrad(observer * observer, int Z, int tMax);

void deleteObserver(observer * observer, int nConfigs, int tMax);

#include "NoiseReduced.h"
#include "Arduino.h"
#include <stdlib.h>
// Constructors

double gaussian(double x){
	return exp(-0.5*pow(x,2.));
}
double noWeight(){
  return 1;
}

NoiseReduced::NoiseReduced() {
  len = 10;
  filled = 0;
  data = (double *)malloc(sizeof(double)*len);
  weights = (double *)malloc(sizeof(double)*len);
  avg = 0;
  head = 0;
}

NoiseReduced::NoiseReduced(int sampleSize) {
  if (sampleSize < 1) {
    len = 1;
  } else if (sampleSize > 512) {
    len = 512;
  } else {
    len = sampleSize;
  }
  filled = 0;
  
  data = (double *)malloc(sizeof(double)*sampleSize);
  weights = (double *)malloc(sizeof(double)*sampleSize);
  
  double totalWeights = 0;
  for (int i = 0; i < sampleSize; i++)
  {
      data[i] = 0;
	  totalWeights += noWeight();//gaussian( (STD_DEV / sampleSize) * i);
	  weights[i] = noWeight();//gaussian( (STD_DEV / sampleSize) * i);
  }
  // make weights add to 1
  for (int i = 0; i < sampleSize; i++) {
	  weights[i] = weights[i] / totalWeights;
  }
  avg = 0;
  head = 0;
}

int NoiseReduced::addData(double input) {
  if (data == NULL) {
    return -1;
  }
  avg = avg + ((input - data[head])/len);
  data[head] = input;
  head++;
  if (head == len) {
    head = 0;
  }
  return 0;
}

double NoiseReduced::getData() {
  return avg;
}

double NoiseReduced::getGauss() {
  double gauss = 0;
  int head_local = head;
  for (int i = 0; i < len; i++){
	gauss += weights[i]*data[head_local];
	head_local++;
	if(head_local == len)
	  head_local = 0;
  }
  return gauss;
}

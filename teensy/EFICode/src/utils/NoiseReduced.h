// Averages the last x values fed to NoiseReduced, default x=10, to
// reduce momentary noise in data streams. Optional length parameter.
// If sample size is set to <0, class defaults to sample size 1.
// If sample size is set to >512, class defaults to sample size 512.


// int addData(double input);
// Takes parameter input and calculates new internal average of past
// len input values.
// Error values:
// -1 - memory allocation error
// 0 - no error

// double getData();
// Takes no parameters and returns average of past len input values.

#ifndef NOISEREDUCE_H
#define NOISEREDUCE_H

#define STD_DEV 6.0 //standard deviations

struct NoiseReduced {
  public:
    NoiseReduced();
    NoiseReduced(int sampleSize);
    int addData(double input);
    double getData();
	double getGauss();
    
  private:
    int len;
    int filled;
    int head;
    double *data;
	double *weights;
    double avg;
};

#endif

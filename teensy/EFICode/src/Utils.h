#ifndef UTILS_H
#define UTILS_H

namespace Utils {

static double doubleMap(double val, double minIn, double maxIn, double minOut, double maxOut){
  return ((val - minIn) / (maxIn - minIn)) * (maxOut - minOut) + minOut;
}

}

#endif
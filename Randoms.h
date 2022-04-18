#ifndef RANDOMS_CLASS_H
#define RANDOMS_CLASS_H

#include <algorithm>

class RandomGenerator {
public:
	long long seed;
	RandomGenerator();
	RandomGenerator(long long _seed);
	long long gen();
	long long range(long long left, long long right);
};

#endif
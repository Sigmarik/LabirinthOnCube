#include "Randoms.h"

RandomGenerator::RandomGenerator() {
	seed = 0;
}

RandomGenerator::RandomGenerator(long long _seed) {
	seed = _seed;
}

long long RandomGenerator::gen() {
	seed ^= 123456789;
	seed *= 9182736455;
	seed %= ((long long)1e16 + 7);
	return seed;
}

long long RandomGenerator::range(long long left, long long right) {
	if (right - left <= 0) return left;
	return abs(gen() % (right - left)) + left;
}
#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <string>

#define PI  int hindsight, int foresight, long length, char *pair, char *interval, double *opening_price, double *closing_price, double *max_price, double *min_price, double *volume
#define PIARGS hindsight, foresight, length, pair, interval, opening_price, closing_price, max_price, min_price, volume

struct pair_info {
	int
		hindsight,
		foresight;
	long length;
	double
		*opening_price,
		*closing_price,
		*max_price,
		*min_price,
		*volume;
	std::string
		pair,
		interval;
	pair_info(void);
	pair_info(PI);
};

extern "C" int __declspec(dllexport) reset_fanna(PI);
extern "C" int __declspec(dllexport) train_fanna(PI);
extern "C" int __declspec(dllexport) build_fanna_database(PI, int samples);
extern "C" double __declspec(dllexport) pulse_fanna(PI);

#endif

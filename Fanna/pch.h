#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <string>

#define PI  int console, double offset, int hindsight, long length, char *pair, char *interval, double *opening_price, double *closing_price, double *max_price, double *min_price, double *volume
#define PIARGS console, offset, hindsight, length, pair, interval, opening_price, closing_price, max_price, min_price, volume

struct pair_info {
	int
		console,
		hindsight;
	long length;
	double
		* opening_price,
		* closing_price,
		* max_price,
		* min_price,
		* volume,
		offset;
	std::string
		pair,
		interval;
	pair_info(void);
	pair_info(PI);
};

extern "C" int __declspec(dllexport) reset_fanna(PI);
extern "C" int __declspec(dllexport) train_fanna(PI);
extern "C" int __declspec(dllexport) build_fanna_database(PI);
extern "C" double __declspec(dllexport) pulse_fanna(PI);

#endif

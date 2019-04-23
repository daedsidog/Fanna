#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <string>

#define PI std::string pair, std::string interval, float *opening_price, float *closing_price, float *max_price, float *min_price, float *volume

struct pair_info {
	float
		*opening_price,
		*closing_price,
		*max_price,
		*min_price,
		*volume;
	std::string
		pair,
		interval;
	pair_info(PI);
};

extern "C" int __declspec(dllexport) reset_fanna(PI);
extern "C" int __declspec(dllexport) train_fanna(PI);
extern "C" int __declspec(dllexport) build_fanna_database(PI, int samples);
extern "C" float __declspec(dllexport) pulse_fanna(PI);

#endif

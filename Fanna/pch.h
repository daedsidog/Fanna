#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <string>

#define PI std::string pair, std::string interval, double* opening_price, double* closing_price, double* volume

struct pair_info {
	double
		*opening_price,
		*closing_price,
		*volume;
	std::string
		pair,
		interval;
	pair_info(PI);
};

__declspec(dllexport) bool reset_fanna(PI);
__declspec(dllexport) bool train_fanna(PI);
__declspec(dllexport) bool build_fanna_database(PI, int samples);
__declspec(dllexport) float pulse_fanna(PI);

#endif

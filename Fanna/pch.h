#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <string>

__declspec(dllexport) struct pair_info {
	double
		*volume,
		*opening_price,
		*closing_price;
	std::string
		pair,
		interval;
	pair_info(std::string pair, std::string interval, double *volume, double *opening_price, double *closing_price) {
		this->pair = pair;
		this->interval = interval;
		this->volume = volume;
		this->opening_price = opening_price;
		this->closing_price = closing_price;
	}
};

__declspec(dllexport) void reset_fanna(pair_info pi);
__declspec(dllexport) void build_fanna_database(pair_info pi, int samples);
__declspec(dllexport) void train_fanna(pair_info pi);
__declspec(dllexport) float pulse_fanna(pair_info pi);

#endif

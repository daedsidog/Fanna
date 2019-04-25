#include <iostream>
#include <string>
#include <Windows.h>
#include <libloaderapi.h>
#include <vector>

#define PI int length, std::string pair, std::string interval, double *opening_price, double *closing_price, double *max_price, double *min_price, double *volume

typedef int(__cdecl* INTPROC)(PI);
typedef int(__cdecl* INTPROCSEC)(PI, int samples);
typedef double(__cdecl* DOUBLEPROC)(PI);

int main(void) {
	HINSTANCE lib = LoadLibrary("Fanna.dll");
	INTPROC reset_fanna = (INTPROC)GetProcAddress(lib, "reset_fanna");
	INTPROC train_fanna = (INTPROC)GetProcAddress(lib, "train_fanna");
	INTPROCSEC build_fanna_database = (INTPROCSEC)GetProcAddress(lib, "build_fanna_database");
	DOUBLEPROC pulse_fanna = (DOUBLEPROC)GetProcAddress(lib, "pulse_fanna");

	//	Test declarations
	int length = 100;
	std::vector<double> vec;
	for (int i = 0; i < 100; ++i)
		vec.push_back(100);
	double
		* opening_price = vec.data(),
		* closing_price = vec.data(),
		* max_price = vec.data(),
		* min_price = vec.data(),
		* volume = vec.data();
	std::string
		pair = "EURUSD",
		interval = "4H";

	opening_price[0] = 123.0f;

	build_fanna_database(length, pair, interval, opening_price, closing_price, max_price, min_price, volume, 30);
}

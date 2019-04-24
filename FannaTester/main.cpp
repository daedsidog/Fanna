#include <iostream>
#include <string>
#include <Windows.h>
#include <libloaderapi.h>

#define PI std::string pair, std::string interval, double *opening_price, double *closing_price, double *max_price, double *min_price, double *volume


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
	double
		* opening_price = new double[1],
		* closing_price = new double[1],
		* max_price = new double[1],
		* min_price = new double[1],
		* volume = new double[1];
	std::string
		pair = "EURUSD",
		interval = "4H";

	opening_price[0] = 123.0f;

	FreeConsole();
	pulse_fanna(pair, interval, opening_price, closing_price, max_price, min_price, volume);

	system("pause");
}

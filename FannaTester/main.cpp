#include <iostream>
#include <string>
#include <Windows.h>
#include <libloaderapi.h>

#define PI std::string pair, std::string interval, float *opening_price, float *closing_price, float *max_price, float *min_price, float *volume


typedef int(__cdecl* INTPROC)(PI);
typedef int(__cdecl* INTPROCSEC)(PI, int samples);
typedef float(__cdecl* FLOATPROC)(PI);

int main(void) {
	HINSTANCE lib = LoadLibrary("Fanna.dll");
	INTPROC reset_fanna = (INTPROC)GetProcAddress(lib, "reset_fanna");
	INTPROC train_fanna = (INTPROC)GetProcAddress(lib, "train_fanna");
	INTPROCSEC build_fanna_database = (INTPROCSEC)GetProcAddress(lib, "build_fanna_database");
	FLOATPROC pulse_fanna = (FLOATPROC)GetProcAddress(lib, "pulse_fanna");

	//	Test declarations
	float
		* opening_price = new float[1],
		* closing_price = new float[1],
		* max_price = new float[1],
		* min_price = new float[1],
		* volume = new float[1];
	std::string
		pair = "EURUSD",
		interval = "4H";

	opening_price[0] = 123.0f;

	FreeConsole();
	pulse_fanna(pair, interval, opening_price, closing_price, max_price, min_price, volume);

	system("pause");
}

#include <iostream>
#include <string>
#include <Windows.h>
#include <libloaderapi.h>

#define PI std::string pair, std::string interval, double* opening_price, double* closing_price, double *max_price, double *min_price, double* volume


typedef void(__cdecl* VOIDPROC)(PI);
typedef void(__cdecl* VOIDPROCSEC)(PI, int samples);
typedef float(__cdecl* FLOATPROC)(PI);

int main(void) {
	HINSTANCE lib = LoadLibrary("Fanna.dll");
	VOIDPROC reset_fanna = (VOIDPROC)GetProcAddress(lib, "reset_fanna");
	VOIDPROC train_fanna = (VOIDPROC)GetProcAddress(lib, "train_fanna");
	VOIDPROCSEC build_fanna_database = (VOIDPROCSEC)GetProcAddress(lib, "build_fanna_database");
	FLOATPROC pulse_fanna = (FLOATPROC)GetProcAddress(lib, "pulse_fanna");
	system("pause");
}

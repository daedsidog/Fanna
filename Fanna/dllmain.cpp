#include "pch.h"

#include <iostream>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
pair_info::pair_info(void){}
pair_info::pair_info(PI){
	this->pair = pair;
	this->interval = interval;
	this->volume = volume;
	this->opening_price = opening_price;
	this->closing_price = closing_price;
	this->max_price = max_price;
	this->min_price = min_price;
}

extern "C" int __declspec(dllexport) reset_fanna(PI) {
	return 1;
}
extern "C" int __declspec(dllexport) train_fanna(PI) {
	return 1;
}
extern "C" int __declspec(dllexport) build_fanna_database(PI, int samples) {
	return 1;
}
extern "C" double __declspec(dllexport) pulse_fanna(PI) {
	double sentiment = 0.0f;
	return sentiment;
}
#ifdef _DEBUG
extern "C" int __declspec(dllexport) test(void) {
	AllocConsole();
	freopen_s((FILE * *)stdout, "CONOUT$", "w", stdout);
	FreeConsole();
	return 1;
}
#endif

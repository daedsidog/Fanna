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

pair_info::pair_info(PI){
	this->pair = pair;
	this->interval = interval;
	this->volume = volume;
	this->opening_price = opening_price;
	this->closing_price = closing_price;
	this->max_price = max_price;
	this->min_price = min_price;
}

extern "C" void __declspec(dllexport) reset_fanna(PI) {

}
extern "C" void __declspec(dllexport) train_fanna(PI) {

}
extern "C" void __declspec(dllexport) build_fanna_database(PI, int samples) {

}
extern "C" float __declspec(dllexport) pulse_fanna(PI) {
	return 0.0f;
}

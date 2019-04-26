#include "pch.h"

#include <iostream>

#include "net.hpp"

#define free_console() FreeConsole()
#define init_console() AllocConsole(); freopen_s((FILE * *)stdout, "CONOUT$", "w", stdout); pair_info pi = pair_info(PIARGS);

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
pair_info::pair_info(void){
	this->hindsight = 0;
	this->foresight = 0;
	this->length = 0;
	this->pair = "";
	this->interval = "";
	this->volume = NULL;
	this->opening_price = NULL;
	this->closing_price = NULL;
	this->max_price = NULL;
	this->min_price = NULL;
}
pair_info::pair_info(PI){
	this->hindsight = hindsight;
	this->foresight = foresight;
	this->length = length;
	this->pair = pair;
	this->interval = interval;
	this->volume = volume;
	this->opening_price = opening_price;
	this->closing_price = closing_price;
	this->max_price = max_price;
	this->min_price = min_price;
}

extern "C" int __declspec(dllexport) reset_fanna(PI) {
	init_console();
	net ann(&pi);
	ann.reset();
	free_console();
	return 1;
}
extern "C" int __declspec(dllexport) train_fanna(PI) {
	init_console();
	net ann(&pi);
	ann.train();
	free_console();
	return 1;
}
extern "C" int __declspec(dllexport) build_fanna_database(PI, int samples) {
	init_console();
	net ann(&pi);
	ann.rebuild_database(samples);
	free_console();
	return 1;
}
extern "C" double __declspec(dllexport) pulse_fanna(PI) {
	pair_info pi = pair_info(PIARGS);
	net ann(&pi);
	double sentiment = ann.pulse();
	return sentiment;
}
#ifdef _DEBUG
extern "C" int __declspec(dllexport) test(void) {
	return 1;
}
#endif

#include "pch.h"

#include <iostream>

#include "net.hpp"

#define free_console() FreeConsole()
#define init_console() AllocConsole(); freopen_s((FILE * *)stdout, "CONOUT$", "w", stdout);

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
	this->console = false;
	this->hindsight = 0;
	this->length = 0;
	this->pair = "";
	this->interval = "";
	this->volume = NULL;
	this->opening_price = NULL;
	this->closing_price = NULL;
	this->max_price = NULL;
	this->min_price = NULL;
	this->offset = 0;
}
pair_info::pair_info(PI){
	this->console = console;
	this->hindsight = hindsight;
	this->length = length;
	this->pair = pair;
	this->interval = interval;
	this->volume = volume;
	this->opening_price = opening_price;
	this->closing_price = closing_price;
	this->max_price = max_price;
	this->min_price = min_price;
	this->offset = offset;
}

extern "C" int __declspec(dllexport) reset_fanna(PI) {
	if (console)
		init_console();
	pair_info pi = pair_info(PIARGS);
	net ann(&pi);
	int response = ann.reset();
	if (console)
		free_console();
	return response;
}
extern "C" int __declspec(dllexport) train_fanna(PI) {
	if (console)
		init_console();
	pair_info pi = pair_info(PIARGS);
	net ann(&pi);
	int response = ann.train();
	if (console)
		free_console();
	return response;
}
extern "C" int __declspec(dllexport) build_fanna_database(PI) {
	if(console)
		init_console();
	pair_info pi = pair_info(PIARGS);
	net ann(&pi);
	int response = ann.rebuild_database();
	if (console)
		free_console();
	return response;
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

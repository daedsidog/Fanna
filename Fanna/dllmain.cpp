#include "pch.h"

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
}

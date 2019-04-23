#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>

typedef int(__cdecl* MYPROC)();

int main(void) {
	HINSTANCE lib = LoadLibrary("Fanna.dll");
	MYPROC ProcAdd = (MYPROC)GetProcAddress(lib, "test");
	(ProcAdd)();
	system("pause");
}

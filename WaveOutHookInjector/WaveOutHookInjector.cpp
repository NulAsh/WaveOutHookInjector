#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <easyhook.h>

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD processId;
	std::wcout << "Enter the target process Id: ";
	std::cin >> processId;

	std::string wavdir;
	std::cout << "Enter a directory to save wav files: ";
	std::cin >> wavdir;
	char *tmpnm = _tempnam(wavdir.c_str(), NULL);
	if (!tmpnm) {
		std::cout << "Error creating tempnam " << errno << "\n";
		exit(1);
	}
	std::ofstream fout(tmpnm);
	if (!fout.is_open()) {
		std::cout << "Error: dir " << wavdir << " is not writable\n";
		free(tmpnm);
		exit(2);
	}
	fout.close();
	remove(tmpnm);
	free(tmpnm);

	WCHAR* dllToInject = L"WaveOutHook.dll";
	wprintf(L"Attempting to inject: %s\n\n", dllToInject);

	// Inject dllToInject into the target process Id, passing 
	// freqOffset as the pass through data.
	NTSTATUS nt = RhInjectLibrary(
		processId,   // The process to inject into
		0,           // ThreadId to wake up upon injection
		EASYHOOK_INJECT_DEFAULT,
		dllToInject, // 32-bit
		NULL,		 // 64-bit not provided
		(PVOID)wavdir.c_str(), // data to send to injected DLL entry point
		(wavdir.size() + 1)// size of data to send
	);

	if (nt != 0)
	{
		printf("RhInjectLibrary failed with error code = %d\n", nt);
		PWCHAR err = RtlGetLastErrorString();
		std::wcout << err << "\n";
	}
	else
	{
		std::wcout << L"Library injected successfully.\n";
	}

	std::wcout << "Press Enter to exit";
	std::wstring input;
	std::getline(std::wcin, input);
	std::getline(std::wcin, input);
	return 0;
}
#include "pch.h"
#include "Launcher.h"


int ExitWithErrorMsg(const char* eMSG, DWORD eCODE)
{
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, eCODE, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	std::string errorMsg(messageBuffer, size);
	LocalFree(messageBuffer);

	char buf[0x512];
	sprintf_s(buf, "Palworld Launcher encountered an error and will now exit.\n\nMSG: %s\nSRC: %s", errorMsg.c_str(), eMSG);
	MessageBoxA(nullptr, buf, "Palworld Launcher Fatal Error", MB_ICONWARNING);
	free(buf);
	return EXIT_FAILURE;
}

std::string GetCurrentPath()
{
	char buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

bool IsGameRunning(const wchar_t* procName, DWORD* dwPID)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	if (!Process32First(hSnap, &pe32))
	{
		CloseHandle(hSnap);
		return false;
	}

	do
	{
		if (_wcsicmp(pe32.szExeFile, procName))
			continue;

		*dwPID = pe32.th32ProcessID;
		CloseHandle(hSnap);
		return true;

	} while (Process32Next(hSnap, &pe32));
	
	CloseHandle(hSnap);
	return false;
}

bool Inject(HANDLE hProc)
{
	void* addr = VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!addr)
		return false;

	std::string path = GetCurrentPath() + "\\" + DLL_NAME;
	if (!WriteProcessMemory(hProc, addr, path.c_str(), strlen(path.c_str()) + 1, NULL))
	{
		VirtualFreeEx(hProc, addr, 0, MEM_RELEASE);
		CloseHandle(hProc);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), addr, 0, 0);
	if (!hThread)
	{
		VirtualFreeEx(hProc, addr, 0, MEM_RELEASE);
		CloseHandle(hProc);
		return false;
	}
}


//	main function
int exec()
{
	__int8 rTick = 0;
	DWORD procID;
	while (!IsGameRunning(PROCESS_NAME, &procID))
	{
		Sleep(1000);
		rTick++;

		if (rTick >= 10)
			return ExitWithErrorMsg("", 3);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, procID);
	if (!hProc)
		return ExitWithErrorMsg("", GetLastError());

	if (!Inject(hProc))
		return ExitWithErrorMsg("", GetLastError());

	return 0;
}
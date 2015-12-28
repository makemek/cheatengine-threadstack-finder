#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdint>
#include <vector>
#include <tlhelp32.h>
#include <Psapi.h>

#include "threadInfo.h"
#include "ntinfo.h"

// TODO: FIND the way to obtain threadstack0 address
// change this everytim when I run cheat engine browse it to mem region -> go to address -> threadstack0
#define THREADSTACK0 0xF9FDF0
#define PLAYBACK_BASE_ADDRESS  THREADSTACK0 - 0x318
#define UPDATE_MILLS 100

//typedef LONG    NTSTATUS;
//typedef NTSTATUS(WINAPI *pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
//#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)
//#define ThreadQuerySetWin32StartAddress 9
//
//DWORD WINAPI _GetThreadStartAddress(HANDLE hThread)
//{
//	NTSTATUS ntStatus;
//	HANDLE hDupHandle;
//	DWORD dwStartAddress;
//
//	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread");
//	
//	if (NtQueryInformationThread == NULL)
//		return 0;
//
//	HANDLE hCurrentProcess = GetCurrentProcess();
//	if (!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0)) {
//		SetLastError(ERROR_ACCESS_DENIED);
//
//		return 0;
//	}
//
//	
//	ntStatus = NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);
//	CloseHandle(hDupHandle);
//	if (ntStatus != STATUS_SUCCESS)
//		return 0;
//
//	return dwStartAddress;
//
//}

DWORD getVirtualAddress(int pointerLevel, HANDLE hProcHandle, DWORD offset[], DWORD baseAddress) {
	DWORD pointer = baseAddress;
	DWORD pTemp;

	DWORD pointerAddr;

	ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
	for (int c = 0; c < pointerLevel; ++c) {
		pointerAddr = pTemp + offset[c];
		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
	}
	
	return pointerAddr;
}

std::vector<DWORD> threadList(DWORD pid) {
	std::vector<DWORD> vect = std::vector<DWORD>();
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h == INVALID_HANDLE_VALUE)
		return vect;

	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	if (Thread32First(h, &te)) {
		do {
			if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
				sizeof(te.th32OwnerProcessID)) {

				
				if (te.th32OwnerProcessID == pid) {
					printf("PID: %04d Thread ID: 0x%04x\n", te.th32OwnerProcessID, te.th32ThreadID);
				vect.push_back(te.th32ThreadID);
				}
					
			}
			te.dwSize = sizeof(te);
		} while (Thread32Next(h, &te));
	}

	return vect;
}

DWORD GetModuleBase(LPSTR lpModuleName, DWORD dwProcessId)
{
	MODULEENTRY32 lpModuleEntry = { 0 };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	if (!hSnapShot)
		return NULL;
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	BOOL bModule = Module32First(hSnapShot, &lpModuleEntry);
	while (bModule)
	{
		if (!strcmp(lpModuleEntry.szModule, lpModuleName))
		{
			CloseHandle(hSnapShot);
			return (DWORD)lpModuleEntry.modBaseAddr;
		}
		bModule = Module32Next(hSnapShot, &lpModuleEntry);
	}
	CloseHandle(hSnapShot);
	return NULL;
}

DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread) {
	
	DWORD used = 0, ret = 0;
	DWORD stacktop = 0, result = 0;

	MODULEINFO mi;
		
	GetModuleInformation(processHandle, LoadLibrary("kernel32.dll"), &mi, sizeof(mi));
	stacktop = (DWORD)GetThreadStackTopAddress_x86(processHandle, hThread);

	// same as GetThreadStackTopAddress
	// LPCVOID tebBaseAddress = GetThreadTebBaseAddress(processHandle, hThread);
	//if (tebBaseAddress)
	//	ReadProcessMemory(processHandle, (LPCVOID)((DWORD)tebBaseAddress + 4), &stacktop, 4, NULL);

	

	// 32 bit - not work at GetThreadContext()
	/*HANDLE hDupHandle;
	DuplicateHandle(processHandle, hThread, processHandle, &hDupHandle, THREAD_QUERY_INFORMATION, TRUE, 0);
	
	GetModuleInformation(processHandle, LoadLibrary("kernel32.dll"), &mi, sizeof(mi));

	std::cout << GetThreadContext(processHandle, &context) << std::endl;

	if (GetThreadContext(hDupHandle, &context)) {
		
		if (GetThreadSelectorEntry(hThread, context.SegFs, &ldtentry)) {
			ReadProcessMemory(processHandle,
				(LPCVOID)( (DWORD*)(ldtentry.BaseLow + ldtentry.HighWord.Bytes.BaseMid << ldtentry.HighWord.Bytes.BaseHi << 24) + 4),
				&stacktop,
				4,
				NULL);
		}
	}*/

	CloseHandle(hThread);

	if (stacktop) {
		//find the stack entry pointing to the function that calls "ExitXXXXXThread"
		//Fun thing to note: It's the first entry that points to a address in kernel32

		DWORD* buf32 = new DWORD[4096/4];
		
		if (ReadProcessMemory(processHandle, (LPCVOID)(stacktop - 4096), buf32, 4096, NULL)) {
			for (int i = 4096 / 4 - 1; i >= 0; --i) {
				if (buf32[i] >= (DWORD)mi.lpBaseOfDll && buf32[i] <= (DWORD)mi.lpBaseOfDll + mi.SizeOfImage) {
					result = stacktop - 4096 + i * 4;
					break;
				}
					
			}
		}

		delete buf32;
	}

	return result;
}

bool updateOnNextRun;
bool isGameAvail;

int main(int argc, char** argv) {
	std::string gameName = "osu!";
	LPCSTR LGameName = "osu!";
	DWORD playbackOffsets[] = { 0x194, 0x228, 0x154, 0x134, 0x2EC };

	HWND hGameWindow = NULL;
	DWORD dwProcID = NULL;
	HANDLE hProcHandle = NULL;
	isGameAvail = false;
	std::string gameStatus;

	std::cout << "Looking for " << gameName << std::endl;
	while (!isGameAvail) {
		hGameWindow = FindWindow(NULL, LGameName);
		GetWindowThreadProcessId(hGameWindow, &dwProcID);
		if (!dwProcID)
			continue;

		std::cout << "Found it PID " << dwProcID << std::endl;
		std::cout << "Grabbing handle" << std::endl;
		hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);

		if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL) {
			gameStatus = "Failed to open process -- invalid handle";
		}
		else {
			std::cout << "Success" << std::endl;
			gameStatus = gameName + " is ready to hack";
			isGameAvail = true;
		}
	}
	
	//std::cout << std::hex << GetModuleBase("osu!.exe", dwProcID) << std::endl;

	std::vector<DWORD> threadId = threadList(dwProcID);
	int stackNum = 0;
	for (auto it = threadId.begin(); it != threadId.end(); ++it) {
		HANDLE threadHandle = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, *it);
		DWORD threadStartAddress = GetThreadStartAddress(hProcHandle, threadHandle);
		std::cout << "TID: " << std::hex << *it << " = " << "THREADSTACK" << std::dec << stackNum << " : " << std::hex << threadStartAddress << std::endl;
		stackNum++;
	}

	return EXIT_SUCCESS;
}
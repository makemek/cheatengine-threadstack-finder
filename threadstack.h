#ifndef THREADSTACK_H
#define THREADSTACK_H

#include <windows.h>
#include <vector>

// list all PIDs and TIDs
#include <tlhelp32.h>
#include <Psapi.h>

std::vector<DWORD> threadList(DWORD pid);
DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread);

#endif

#ifndef THREADSTACK_H
#define THREADSTACK_H

#include <windows.h>
#include <vector>

std::vector<DWORD> threadList(DWORD pid);
DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread);
DWORD baseThreadstackAddress(DWORD processID, DWORD threadstackNumber);

#endif

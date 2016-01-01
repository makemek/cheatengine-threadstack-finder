#include "ntinfo.h"
#include "threadstack.h"
#include "InvalidHandleException.hpp"
#include "BadAddressException.hpp"

#include <sstream>

// list all PIDs and TIDs
#include <tlhelp32.h>
#include <Psapi.h>

std::vector<DWORD> threadstack::threadList(DWORD pid) {
	/* solution from http://stackoverflow.com/questions/1206878/enumerating-threads-in-windows */
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
					vect.push_back(te.th32ThreadID);
				}

			}
			te.dwSize = sizeof(te);
		} while (Thread32Next(h, &te));
	}

	return vect;
}

DWORD threadstack::GetThreadStartAddress(HANDLE processHandle, HANDLE hThread) {
	/* rewritten from https://github.com/cheat-engine/cheat-engine/blob/master/Cheat%20Engine/CEFuncProc.pas#L3080 */
	DWORD used = 0, ret = 0;
	DWORD stacktop = 0, result = 0;

	MODULEINFO mi;

	GetModuleInformation(processHandle, GetModuleHandle("kernel32.dll"), &mi, sizeof(mi));
	stacktop = (DWORD)GetThreadStackTopAddress_x86(processHandle, hThread);

	CloseHandle(hThread);

	if (stacktop) {
		//find the stack entry pointing to the function that calls "ExitXXXXXThread"
		//Fun thing to note: It's the first entry that points to a address in kernel32

		DWORD* buf32 = new DWORD[4096];

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

DWORD threadstack::baseThreadstackAddress(DWORD pid, DWORD stackNum) {
	
	auto tids = threadList(pid);
	if (tids.empty()) {
		throw std::invalid_argument("Process ID not found");
	}
	if (stackNum > tids.size()) {
		std::ostringstream stringstream;
		stringstream << "THREADSTACK" << stackNum << " doesn't exist!"
			<< " There are " << tids.size() << " threads available";
		throw std::out_of_range(stringstream.str());
	}
		
	HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, tids[stackNum]);
	if (!threadHandle) {
		throw InvalidHandleException("Invalid thread handle", GetLastError());
	}

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!procHandle) {
		throw InvalidHandleException("Invalid process handle", GetLastError());
	}

	DWORD addr = GetThreadStartAddress(procHandle, threadHandle);
	CloseHandle(procHandle);

	if (!addr)
		throw BadAddressException("Invalid address. 64-bit process?", addr);

	CloseHandle(threadHandle);

	return addr;
}
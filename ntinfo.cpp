#include "ntinfo.h"

typedef struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS                ExitStatus;
	PVOID                   TebBaseAddress;
	CLIENT_ID               ClientId;
	KAFFINITY               AffinityMask;
	KPRIORITY               Priority;
	KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

enum THREADINFOCLASS
{
	ThreadBasicInformation,
};

void* GetThreadStackTopAddress_x86(HANDLE hProcess, HANDLE hThread) {
	
	LPSTR moduleName = "ntdll.dll";

	bool loadedManually = false;
	HMODULE module = GetModuleHandle(moduleName);

	if (!module)
	{
		module = LoadLibrary(moduleName);
		loadedManually = true;
	}

	NTSTATUS(__stdcall *NtQueryInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
	NtQueryInformationThread = reinterpret_cast<decltype(NtQueryInformationThread)>(GetProcAddress(module, "NtQueryInformationThread"));

	if (NtQueryInformationThread)
	{
		NT_TIB tib = { 0 };
		THREAD_BASIC_INFORMATION tbi = { 0 };
		
		NTSTATUS status = NtQueryInformationThread(hThread, ThreadBasicInformation, &tbi, sizeof(tbi), nullptr);
		if (status >= 0)
		{
			ReadProcessMemory(hProcess, tbi.TebBaseAddress, &tib, sizeof(tbi), nullptr);

			if (loadedManually)
			{
				FreeLibrary(module);
			}
			return tib.StackBase;
		}
	}


	if (loadedManually)
	{
		FreeLibrary(module);
	}

	return nullptr;
}


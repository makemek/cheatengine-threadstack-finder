/*
http://stackoverflow.com/questions/32297431/getting-the-tib-teb-of-a-thread-by-its-thread-handle-2015
*/

//#include <iostream>
//#include <windows.h>
//
//typedef LONG NTSTATUS;
//typedef DWORD KPRIORITY;
//typedef WORD UWORD;
//
//typedef struct _CLIENT_ID
//{
//	PVOID UniqueProcess;
//	PVOID UniqueThread;
//} CLIENT_ID, *PCLIENT_ID;
//
//typedef struct _THREAD_BASIC_INFORMATION
//{
//	NTSTATUS                ExitStatus;
//	PVOID                   TebBaseAddress;
//	CLIENT_ID               ClientId;
//	KAFFINITY               AffinityMask;
//	KPRIORITY               Priority;
//	KPRIORITY               BasePriority;
//} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
//
//enum THREADINFOCLASS
//{
//	ThreadBasicInformation,
//};
//
//void* GetThreadStackTopAddress(HANDLE hProcess, HANDLE hThread)
//{
//	bool loadedManually = false;
//	HMODULE module = GetModuleHandle("ntdll.dll");
//
//	if (!module)
//	{
//		module = LoadLibrary("ntdll.dll");
//		loadedManually = true;
//	}
//
//	NTSTATUS(__stdcall *NtQueryInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
//	NtQueryInformationThread = reinterpret_cast<decltype(NtQueryInformationThread)>(GetProcAddress(module, "NtQueryInformationThread"));
//
//	if (NtQueryInformationThread)
//	{
//		NT_TIB tib = { 0 };
//		THREAD_BASIC_INFORMATION tbi = { 0 };
//
//		NTSTATUS status = NtQueryInformationThread(hThread, ThreadBasicInformation, &tbi, sizeof(tbi), nullptr);
//		if (status >= 0)
//		{
//			ReadProcessMemory(hProcess, tbi.TebBaseAddress, &tib, sizeof(tbi), nullptr);
//
//			if (loadedManually)
//			{
//				FreeLibrary(module);
//			}
//			return tib.StackBase;
//		}
//	}
//
//
//	if (loadedManually)
//	{
//		FreeLibrary(module);
//	}
//
//	return nullptr;
//}
//
//void __stdcall Test()
//{
//	for (int i = 0; i < 10; ++i)
//	{
//		printf("Hi. ");
//		Sleep(500);
//	}
//}
//
//
//int main()
//{
//	std::cout << GetThreadStackTopAddress(GetCurrentProcess(), GetCurrentThread()) << "\n";
//
//	DWORD threadID = 0;
//	HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Test), nullptr, 0, &threadID);
//	std::cout << GetThreadStackTopAddress(GetCurrentProcess(), hThread) << "\n\n";
//	CloseHandle(hThread);
//	Sleep(7000);
//
//	return 0;
//}
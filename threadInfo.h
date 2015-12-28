#ifndef THREADINFO_H
#define THREADINFO_H
#include <Windows.h>

typedef long(__stdcall *NtQueryInformationThread_PROC)(HANDLE thread,
	int infoclass, void *buf, long size, long *used);
typedef struct _THREAD_BASIC_INFORMATION {
	unsigned long ExitStatus;
	void *TebBaseAddress;
	ULONG UniqueProcessId;
	ULONG UniqueThreadId;
	unsigned long AffinityMask;
	unsigned long BasePriority;
	unsigned long DiffProcessPriority;

} THREAD_BASIC_INFORMATION;

#endif

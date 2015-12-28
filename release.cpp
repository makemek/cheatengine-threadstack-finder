//#include <Windows.h>
//#include <iostream>
//#include <string>
//#include <ctime>
//#include <winternl.h>
//#include <cstdint>
//#include <vector>
//#include <tlhelp32.h>
//#include <sstream>
//
//// TODO: FIND the way to obtain threadstack0 address
//// change this everytim when I run cheat engine browse it to mem region -> go to address -> threadstack0
//#define THREADSTACK0 0xF9FDF0
//#define PLAYBACK_BASE_ADDRESS  THREADSTACK0 - 0x318
//
//typedef LONG    NTSTATUS;
//typedef NTSTATUS(WINAPI *pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
//#define UPDATE_MILLS 100
//#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)
//#define ThreadQuerySetWin32StartAddress 9
//
//DWORD getVirtualAddress(int pointerLevel, HANDLE hProcHandle, DWORD offset[], DWORD baseAddress) {
//	DWORD pointer = baseAddress;
//	DWORD pTemp;
//
//	DWORD pointerAddr;
//
//	ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
//	for (int c = 0; c < pointerLevel; ++c) {
//		pointerAddr = pTemp + offset[c];
//		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
//	}
//
//	return pointerAddr;
//}
//
//bool updateOnNextRun;
//bool isGameAvail;
//
//int main(int argc, char** argv) {
//	std::string gameName = "osu!";
//	LPCSTR LGameName = "osu!";
//	DWORD playbackOffsets[] = { 0x194, 0x228, 0x154, 0x134, 0x2EC };
//
//	HWND hGameWindow = NULL;
//	DWORD dwProcID = NULL;
//	HANDLE hProcHandle = NULL;
//
//	std::stringstream ss;
//	if (argc < 1)
//		return 0;
//	DWORD threadStackPtr;
//	ss << std::hex << argv[1];
//	ss >> threadStackPtr;
//
//	isGameAvail = false;
//	std::string gameStatus;
//
//	std::cout << "Looking for " << gameName << std::endl;
//	while (!isGameAvail) {
//		hGameWindow = FindWindow(NULL, LGameName);
//		GetWindowThreadProcessId(hGameWindow, &dwProcID);
//		if (!dwProcID)
//			continue;
//
//		std::cout << "Found it PID " << dwProcID << std::endl;
//		std::cout << "Grabbing handle" << std::endl;
//		hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
//
//		if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL) {
//			gameStatus = "Failed to open process -- invalid handle";
//		}
//		else {
//			std::cout << "Success" << std::endl;
//			gameStatus = gameName + " is ready to hack";
//			isGameAvail = true;
//		}
//	}
//
//	DWORD millPtr = getVirtualAddress(5, hProcHandle, playbackOffsets, threadStackPtr);
//
//	while (true) {
//		DWORD pTemp;
//		ReadProcessMemory(hProcHandle, (LPCVOID)millPtr, &pTemp, sizeof(pTemp), NULL);
//		int currentPlaybackTime = pTemp;
//		system("cls");
//		std::cout << std::dec << currentPlaybackTime << std::endl;
//		Sleep(10);
//	}
//
//	return EXIT_SUCCESS;
//}
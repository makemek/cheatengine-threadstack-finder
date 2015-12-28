# cheatengine-threadstack-finder
A simple program that list all thread's base address based on process's name.
ONLY WORK FOR 
	- 32 bit process
	- Launched by you
Depending on a process's access rights.
IF YOU SEE BASE ADDRESS AS 0x00000000, it propably is a 64-bit process.

Pass command process id (PID) in decimal as a command line argument.
The program will output all thread's base addresses.

Make sure that a target process is opened before passing an argument.

# Example
threadstack.exe 6540

Output
PID 6540 (0x198c)
Grabbing handle
Success
PID: 6540 Thread ID: 0x1990
PID: 6540 Thread ID: 0x1b1c
PID: 6540 Thread ID: 0x1bbc
TID: 0x1990 = THREADSTACK 0 BASE ADDRESS: 0xbcff8c
TID: 0x1b1c = THREADSTACK 1 BASE ADDRESS: 0x4d8ff8c
TID: 0x1bbc = THREADSTACK 2 BASE ADDRESS: 0x518ff8c

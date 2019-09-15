#pragma comment(lib,"winmm.lib")
#ifndef audio_header_h    
#define audio_header_h
#define MY_MAX_PATH MAX_PATH*3

#include <Windows.h>
#include <mmsystem.h>
#include <fstream>
#include <iostream>

DWORD WINAPI audio_main(LPVOID lpParameter);

#endif 









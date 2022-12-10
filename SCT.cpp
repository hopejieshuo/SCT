#include <windows.h>
#include <math.h>
#include <time.h>
#include <shlwapi.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"msimg32.lib")
#define PI acos( -1.0 )

DWORD xs;
void SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}
DWORD Xorshift32( VOID ) {
	xs ^= xs << 13;
	xs ^= xs >> 17;
	xs ^= xs << 5;
	return xs;
}
void WINAPI InitDPI( VOID ) {
	HMODULE hModUser32 = LoadLibrary((char *)"user32.dll");
	BOOL(WINAPI * SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModUser32, "SetProcessDPIAware");
	if (SetProcessDPIAware){
		SetProcessDPIAware();
	}
	FreeLibrary(hModUser32);
	
    SeedXorshift32((DWORD)(time(NULL)));
}

void audio(){
    int nSamplesPerSec = 8000, nSampleCount = nSamplesPerSec * 120;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, NULL, 0, NULL, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, NULL, 0, NULL);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((1 + (t >> 10) % 7) * (t * (1 + (t >> 13) % 4) % (24 + 9 * (t >> 14) % 8) & 16) * 10);
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}

void payload1(){
	HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
	HDC hcdc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
	
	int bl = 1;
	for(int t = 0; ; t++){
	    if(t % w == 0){
	        bl = -bl;
	    }
	    hdc = GetDC(NULL);
	    int y = bl * (sin((t % w) * (PI / (w / 5))) * (h / 10));
	    BitBlt(hdc, t % w, 0, 1, h, hcdc, t % w, 0, SRCCOPY);
	    BitBlt(hdc, t % w, y, 1, h, hcdc, t % w, 0, SRCCOPY);
	    Sleep(20);
	}
	
	ReleaseDC(NULL, hdc);
	ReleaseDC(NULL, hcdc);
	DeleteObject(hdc);
	DeleteObject(hcdc);
	DeleteObject(hBitmap);
}

void payload2(){
	HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    
    for(int t = 0; ; t++){
        hdc = GetDC(NULL);
        SelectObject(hdc, CreateSolidBrush(Xorshift32() % 0xffffff));
        PatBlt(hdc, 0, 0, w, h, PATINVERT);
        Sleep(1000);
    }
    
	ReleaseDC(NULL, hdc);
	DeleteObject(hdc);
}

void payload3(){
	HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    
    for(int t = 0; ; t++){
        hdc = GetDC(NULL);
        int x = Xorshift32() % w, y = Xorshift32() % h, i = (Xorshift32() % 18) * 20;
        for(int a = i; a <= i + 180; a += 10){
            double IcoX = x + (cos(a * (PI / 180)) * 50), IcoY = y + (sin(a * (PI / 180)) * 50);
            DrawIcon(hdc, IcoX, IcoY, LoadIcon(NULL, IDI_ERROR));
            Sleep(100);
        }
    }
    
	ReleaseDC(NULL, hdc);
	DeleteObject(hdc);
}

void payload4(){
	HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    
    for(int t = 0; ; t++){
        hdc = GetDC(NULL);
        int x = Xorshift32() % w, y = Xorshift32() % h, bl = Xorshift32() % 3 - 1;
        for(int a = 0; a <= 10; a++){
            BitBlt(hdc, x + (a + bl) * 5, y + (a + bl) * 5, w / 10, h / 10, hdc, x + a * 5, y + a * 5, SRCCOPY);
            Sleep(100);
        }
    }
    
	ReleaseDC(NULL, hdc);
	DeleteObject(hdc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow){
    InitDPI();
    if(MessageBox(NULL, "THIS MALWARE WILL DISTURB YOU FOR SOME TIME,\nARE YOU SURE TO RUN IT?", "SCT.exe - Warning", MB_YESNO+MB_ICONQUESTION) != IDYES) {
        ExitProcess(0);
    }
    if(MessageBox(NULL, "THIS IS LAST WARNING,\nTHE MALWARE AUTHOR COULDN'T ASSUME LEGAL LIABILITY,\nSO ARE YOU SURE TO RUN IT?", "SCT.exe - Warning", MB_YESNO+MB_ICONWARNING) != IDYES) {
        ExitProcess(0);
    }
    HANDLE handle0 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)audio, NULL, 0, NULL);
    HANDLE handle1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payload1, NULL, 0, NULL);
    Sleep(30000);
    HANDLE handle2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payload2, NULL, 0, NULL);
    Sleep(30000);
    HANDLE handle3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payload3, NULL, 0, NULL);
    Sleep(30000);
    HANDLE handle4 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payload4, NULL, 0, NULL);
    Sleep(30000);
    TerminateThread(handle1, 0);
    CloseHandle(handle1);
    TerminateThread(handle2, 0);
    CloseHandle(handle2);
    TerminateThread(handle3, 0);
    CloseHandle(handle3);
    TerminateThread(handle4, 0);
    CloseHandle(handle4);
    TerminateThread(handle0, 0);
    CloseHandle(handle0);
    Sleep(2500);
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(2500);
    return 0;
}
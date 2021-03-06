#include "stdafx.h"
#include <windows.h>
#include <winioctl.h>

DWORD WINAPI RegHackerEventHandler(LPVOID args)
{
        DWORD ret;
        HANDLE *phEvent = (PHANDLE)args;

        while (1) {
                ret = WaitForSingleObject(*phEvent, INFINITE);
                // if (ret == WAIT_OBJECT_0) {
                        printf("received kernel event\n");
                // }
        }

        return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
        DWORD dwBytes = 0;
        HANDLE hDevice = CreateFile(L"\\\\.\\RegHacker", GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                    0, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
                printf("fail to open device\n");
                return 1;
        }

        HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, L"RegNotify");
        if (hEvent == NULL) {
                printf("fail to create event %d\n", GetLastError());
                return 1;
        }

        printf("hEvent: %x\n", hEvent);
	HANDLE hThread = (HANDLE)CreateThread(NULL, 0, RegHackerEventHandler, &hEvent, 0, NULL);

        // char out[12] = { 0 };
        // DWORD dwBytes = 0;
        // BOOL b = DeviceIoControl(hDevice, CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS),
        //                 "hello", 5, out, 11, &dwBytes, NULL);
        // printf("ret: %d\n", b);

        DeviceIoControl(hDevice, CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS),
                        &hEvent, sizeof(hEvent), NULL, 0, &dwBytes, NULL);
        WaitForSingleObject(hThread, INFINITE);

        CloseHandle(hDevice);
	return 0;
}


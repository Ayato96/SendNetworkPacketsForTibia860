#include "pch.h"

#include <Windows.h>
#include <iostream>
using namespace std;

void SendPacket(int packetTypeID, int cid, int number, int one);



DWORD WINAPI InjectThread(HMODULE hModule)
{
    FILE* f;





    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    cout << "We are Injected";

    cout << "following";

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            cout << "Follow Attempt" <<endl;
            SendPacket(0x0a2, 0x800000A7, 10, 1);
            cout << "Did it work ? :X";

        }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {

        }
        Sleep(10);
    }
    cout << "Ejecting DLL!!!";
    Sleep(2000);

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;

}


void SendPacket(int packetTypeID, int cid, int number, int one)
{
    uint32_t callpackettypeFunc = 0x4F8290;
    uint32_t callPacketInformation = 0x4F88A0;
    uint32_t callpacketcool = 0x4F8E40;
    __asm
    {
        PUSH packetTypeID
        CALL callpackettypeFunc
        PUSH cid
        CALL callPacketInformation
        PUSH number
        CALL callPacketInformation
        PUSH one
        CALL callpacketcool

        ADD ESP, 10
    }

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
       CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InjectThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



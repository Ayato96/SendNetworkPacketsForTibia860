#include "pch.h"

#include <Windows.h>
#include <iostream>
#include <string>
const uint32_t baseadr = (uint32_t)GetModuleHandle(NULL);

uint32_t packetlisteneradr = baseadr + 0xF82A1;
uint32_t retadr = baseadr + 0xF82B1;

int packetID = 0;

void SendPacket(int packetTypeID, int cid, int number, int one);


BOOL Hook(void* pTarget, void* ourFunct, int len);
volatile void __declspec() StartListen();

void __declspec() ListenForPacket();

struct PacketTypes {
public:
    static const int Follow = 0xa2;
    static const int Attack = 0xa1;

};


DWORD WINAPI InjectThread(HMODULE hModule)
{
    FILE* f;
   

    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "We are Injected" << std::endl;

    std::cout << "[3] Follow" << std::endl;
    std::cout << "[4] For Reading Outgoing Packets" << std::endl;
    std::cout << "[END] Eject DLL" << std::endl;


    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            std::cout << "Follow Attempt" << std::endl;
            SendPacket(PacketTypes::Follow, 0x800000A7, 10, 1);

        }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {

            std::cout << std::endl << "Trying to hook ->" << std::endl;
            Hook((void*)packetlisteneradr, &StartListen, 5);
            std::cout << "Hook worked no crashes o/";
        }
        Sleep(10);
    }
    std::cout << std::endl<< std::endl<< "Ejecting DLL";
    Sleep(2000);

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;

}

void Printer()
{
    std::string packetType = "";
    if(packetID == 30)
    {
        packetType = "Hey Im Still Here Message"; 
    }
    else if (packetID == 111)
    {
        packetType = "Look direction UP";
    }
    else if(packetID == 112)
    {
        packetType = "Look direction Right";
    }
    else if (packetID == 113)
    {
        packetType = "Look direction Down";
    }
    else if (packetID == 114)
    {
        packetType = "Look direction Left";
    }
    else if (packetID == 101)
    {
        packetType = "Walk UP";
    }
    else if (packetID == 102)
    {
        packetType = "Walk Right";
    }
    else if (packetID == 103)
    {
        packetType = "Walk Down";
    }
    else if (packetID == 104)
    {
        packetType = "Walk Left";
    }
    else
    {
        packetType = "Unknown";
    }
    std::cout << std::endl << "PacketID ::  " + std::to_string(packetID) + " :: PacketType:: " + packetType;
}

void SendPacket(int packetTypeID, int cid, int number, int one)
{
    // dynamic addresses, just for testing purposes.
    uint32_t callpackettypeFunc = baseadr + 0xF8290;
    uint32_t callPacketInformation = baseadr + 0xF88A0;
    uint32_t callpacketcool = baseadr + 0xF8E40;
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

        ADD ESP, 16
    }

}


volatile void __declspec(naked) StartListen()
{
    __asm
    {
      PUSHFD
      PUSHAD
    }
   ListenForPacket();
   Printer();
    __asm // replacement code, 5 bytes of length ..
    {
        POPAD
        POPFD
        MOV DWORD PTR FS : [0] , ESP
        SUB ESP, 250
        PUSH EBX
        PUSH ESI
        PUSH EDI
        JMP retadr
    }


}

void __declspec(naked) ListenForPacket()
{

  __asm
   {
      PUSHFD
      PUSHAD
      MOV EAX,[ESP + 0x64]
      MOV packetID, EAX
  }

    __asm
    {
        POPFD
        POPAD
        RET
    }
}  

//2147483816 creature id of the npc we follow..
//2147483816 we can see its a perfect match. so the parameter is cid 
BOOL Hook(void* pTarget, void* ourFunct, int len)
{

    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    VirtualProtect(pTarget, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(pTarget, 0x90, len);

    DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)pTarget) - 5;
    *(BYTE*)pTarget = 0xE8; // E8 == call E9 == JMP
    *(DWORD*)((DWORD)pTarget + 1) = relativeAddress;

    DWORD temp;
    VirtualProtect(pTarget, len, curProtection, &temp);
    return true;

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
       CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InjectThread, hModule, 0, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



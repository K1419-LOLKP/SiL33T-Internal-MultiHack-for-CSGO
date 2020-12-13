#pragma once
#include "windows.h"

bool Detour32(char* src, char* dst, const intptr_t length)
{
    if (length < 5) return false;

    DWORD  curProc;
    VirtualProtect(src, length, PAGE_EXECUTE_READWRITE, &curProc);

    intptr_t  relativeAddress = (intptr_t)(dst - (intptr_t)src) - 5;

    *src = (char)'\xE9';
    *(intptr_t*)((intptr_t)src + 1) = relativeAddress;

    VirtualProtect(src, length, curProc, &curProc);
    return true;
}

char* TrampHook32(char* src, char* dst, const intptr_t len)
{
    // Make sure the length is greater than 5
    if (len < 5) return 0;

    // Create the gateway (len + 5 for the overwritten bytes + the jmp)
    void* gateway = VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    //Write the stolen bytes into the gateway
    memcpy(gateway, src, len);

    // Get the gateway to destination addy
    intptr_t  gatewayRelativeAddr = ((intptr_t)src - (intptr_t)gateway) - 5;

    // Add the jmp opcode to the end of the gateway
    *(char*)((intptr_t)gateway + len) = 0xE9;

    // Add the address to the jmp
    *(intptr_t*)((intptr_t)gateway + len + 1) = gatewayRelativeAddr;

    // Perform the detour
    Detour32(src, dst, len);

    return (char*)gateway;
}


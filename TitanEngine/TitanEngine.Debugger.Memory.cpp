#include "stdafx.h"
#include "definitions.h"
#include "Global.Debugger.h"
#include <stdlib.h>

__declspec(dllexport) bool TITCALL MatchPatternEx(HANDLE hProcess, void* MemoryToCheck, int SizeOfMemoryToCheck, void* PatternToMatch, int SizeOfPatternToMatch, PBYTE WildCard)
{
    if(!MemoryToCheck || !PatternToMatch)
        return false;
    int i = NULL;
    BYTE intWildCard = NULL;
    LPVOID ueReadBuffer = NULL;
    ULONG_PTR ueNumberOfBytesRead = NULL;
    MEMORY_BASIC_INFORMATION memoryInformation = {};
    PMEMORY_COMPARE_HANDLER memCmp = (PMEMORY_COMPARE_HANDLER)MemoryToCheck;
    PMEMORY_COMPARE_HANDLER memPattern = (PMEMORY_COMPARE_HANDLER)PatternToMatch;

    if(WildCard == NULL)
    {
        WildCard = &intWildCard;
    }
    if(SizeOfMemoryToCheck >= SizeOfPatternToMatch)
    {
        if(hProcess != GetCurrentProcess())
        {
            ueReadBuffer = VirtualAlloc(NULL, SizeOfMemoryToCheck, MEM_COMMIT, PAGE_READWRITE);
            if(!ReadProcessMemory(hProcess, MemoryToCheck, ueReadBuffer, SizeOfMemoryToCheck, &ueNumberOfBytesRead))
            {
                if(ueNumberOfBytesRead == NULL)
                {
                    if(VirtualQueryEx(hProcess, MemoryToCheck, &memoryInformation, sizeof memoryInformation) != NULL)
                    {
                        SizeOfMemoryToCheck = (int)((ULONG_PTR)memoryInformation.BaseAddress + memoryInformation.RegionSize - (ULONG_PTR)MemoryToCheck);
                        if(!ReadProcessMemory(hProcess, MemoryToCheck, ueReadBuffer, SizeOfMemoryToCheck, &ueNumberOfBytesRead))
                        {
                            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
                            return(NULL);
                        }
                        else
                        {
                            memCmp = (PMEMORY_COMPARE_HANDLER)ueReadBuffer;
                        }
                    }
                    else
                    {
                        VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
                        return(NULL);
                    }
                }
                else
                {
                    memCmp = (PMEMORY_COMPARE_HANDLER)ueReadBuffer;
                }
            }
            else
            {
                memCmp = (PMEMORY_COMPARE_HANDLER)ueReadBuffer;
            }
        }
        __try
        {
            while(SizeOfPatternToMatch > NULL)
            {
                if(memCmp->Array.bArrayEntry[i] != memPattern->Array.bArrayEntry[i] && memPattern->Array.bArrayEntry[i] != *WildCard)
                {
                    return(false);
                }
                SizeOfPatternToMatch--;
                i++;
            }
            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
            return(true);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
            return(false);
        }
    }
    else
    {
        return(false);
    }
}
__declspec(dllexport) bool TITCALL MatchPattern(void* MemoryToCheck, int SizeOfMemoryToCheck, void* PatternToMatch, int SizeOfPatternToMatch, PBYTE WildCard)
{

    if(dbgProcessInformation.hProcess != NULL)
    {
        return(MatchPatternEx(dbgProcessInformation.hProcess, MemoryToCheck, SizeOfMemoryToCheck, PatternToMatch, SizeOfPatternToMatch, WildCard));
    }
    else
    {
        return(MatchPatternEx(GetCurrentProcess(), MemoryToCheck, SizeOfMemoryToCheck, PatternToMatch, SizeOfPatternToMatch, WildCard));
    }
}
__declspec(dllexport) long long TITCALL FindEx(HANDLE hProcess, LPVOID MemoryStart, DWORD MemorySize, LPVOID SearchPattern, DWORD PatternSize, LPBYTE WildCard)
{

    int i = NULL;
    int j = NULL;
    ULONG_PTR Return = NULL;
    LPVOID ueReadBuffer = NULL;
    PUCHAR SearchBuffer = NULL;
    PUCHAR CompareBuffer = NULL;
    MEMORY_BASIC_INFORMATION memoryInformation = {};
    ULONG_PTR ueNumberOfBytesRead = NULL;
    LPVOID currentSearchPosition = NULL;
    DWORD currentSizeOfSearch = NULL;
    BYTE nWildCard = NULL;

    if(WildCard == NULL)
    {
        WildCard = &nWildCard;
    }
    if(hProcess != NULL && MemoryStart != NULL && MemorySize != NULL)
    {
        if(hProcess != GetCurrentProcess())
        {
            ueReadBuffer = VirtualAlloc(NULL, MemorySize, MEM_COMMIT, PAGE_READWRITE);
            if(!ReadProcessMemory(hProcess, MemoryStart, ueReadBuffer, MemorySize, &ueNumberOfBytesRead))
            {
                if(ueNumberOfBytesRead == NULL)
                {
                    if(VirtualQueryEx(hProcess, MemoryStart, &memoryInformation, sizeof memoryInformation) != NULL)
                    {
                        MemorySize = (DWORD)((ULONG_PTR)memoryInformation.BaseAddress + memoryInformation.RegionSize - (ULONG_PTR)MemoryStart);
                        if(!ReadProcessMemory(hProcess, MemoryStart, ueReadBuffer, MemorySize, &ueNumberOfBytesRead))
                        {
                            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
                            return(NULL);
                        }
                        else
                        {
                            SearchBuffer = (PUCHAR)ueReadBuffer;
                        }
                    }
                    else
                    {
                        VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
                        return(NULL);
                    }
                }
                else
                {
                    SearchBuffer = (PUCHAR)ueReadBuffer;
                }
            }
            else
            {
                SearchBuffer = (PUCHAR)ueReadBuffer;
            }
        }
        else
        {
            SearchBuffer = (PUCHAR)MemoryStart;
        }
        __try
        {
            CompareBuffer = (PUCHAR)SearchPattern;
            for(i = 0; i < (int)MemorySize && Return == NULL; i++)
            {
                for(j = 0; j < (int)PatternSize; j++)
                {
                    if(CompareBuffer[j] != *(PUCHAR)WildCard && SearchBuffer[i + j] != CompareBuffer[j])
                    {
                        break;
                    }
                }
                if(j == (int)PatternSize)
                {
                    Return = (ULONG_PTR)MemoryStart + i;
                }
            }
            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
            return(Return);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            VirtualFree(ueReadBuffer, NULL, MEM_RELEASE);
            return(NULL);
        }
    }
    else
    {
        return(NULL);
    }
}

extern "C" __declspec(dllexport) long long TITCALL Find(LPVOID MemoryStart, DWORD MemorySize, LPVOID SearchPattern, DWORD PatternSize, LPBYTE WildCard)
{

    if(dbgProcessInformation.hProcess != NULL)
    {
        return(FindEx(dbgProcessInformation.hProcess, MemoryStart, MemorySize, SearchPattern, PatternSize, WildCard));
    }
    else
    {
        return(FindEx(GetCurrentProcess(), MemoryStart, MemorySize, SearchPattern, PatternSize, WildCard));
    }
}

__declspec(dllexport) bool TITCALL FillEx(HANDLE hProcess, LPVOID MemoryStart, DWORD MemorySize, PBYTE FillByte)
{

    unsigned int i;
    MEMORY_BASIC_INFORMATION MemInfo;
    ULONG_PTR ueNumberOfBytesRead;
    BYTE defFillByte = 0x90;
    DWORD OldProtect;

    if(hProcess != NULL)
    {
        if(FillByte == NULL)
        {
            FillByte = &defFillByte;
        }
        VirtualQueryEx(hProcess, MemoryStart, &MemInfo, sizeof MEMORY_BASIC_INFORMATION);
        OldProtect = MemInfo.Protect;
        VirtualProtectEx(hProcess, MemoryStart, MemorySize, PAGE_EXECUTE_READWRITE, &OldProtect);
        for(i = 0; i < MemorySize; i++)
        {
            WriteProcessMemory(hProcess, MemoryStart, FillByte, 1, &ueNumberOfBytesRead);
            MemoryStart = (LPVOID)((ULONG_PTR)MemoryStart + 1);
        }
        VirtualProtectEx(hProcess, MemoryStart, MemorySize, OldProtect, &OldProtect);
        return(true);
    }
    return(false);
}
__declspec(dllexport) bool TITCALL Fill(LPVOID MemoryStart, DWORD MemorySize, PBYTE FillByte)
{

    if(dbgProcessInformation.hProcess != NULL)
    {
        return(FillEx(dbgProcessInformation.hProcess, MemoryStart, MemorySize, FillByte));
    }
    else
    {
        return(FillEx(GetCurrentProcess(), MemoryStart, MemorySize, FillByte));
    }
}
__declspec(dllexport) bool TITCALL PatchEx(HANDLE hProcess, LPVOID MemoryStart, DWORD MemorySize, LPVOID ReplacePattern, DWORD ReplaceSize, bool AppendNOP, bool PrependNOP)
{

    unsigned int i,recalcSize;
    LPVOID lpMemoryStart = MemoryStart;
    MEMORY_BASIC_INFORMATION MemInfo;
    ULONG_PTR ueNumberOfBytesRead;
    BYTE FillByte = 0x90;
    DWORD OldProtect;

    if(hProcess != NULL)
    {
        VirtualQueryEx(hProcess, MemoryStart, &MemInfo, sizeof MEMORY_BASIC_INFORMATION);
        OldProtect = MemInfo.Protect;
        VirtualProtectEx(hProcess, MemoryStart, MemorySize, PAGE_EXECUTE_READWRITE, &OldProtect);

        if(MemorySize - ReplaceSize != NULL)
        {
            recalcSize = abs((long)(MemorySize - ReplaceSize));
            if(AppendNOP)
            {
                WriteProcessMemory(hProcess, MemoryStart, ReplacePattern, ReplaceSize, &ueNumberOfBytesRead);
                lpMemoryStart = (LPVOID)((ULONG_PTR)MemoryStart + ReplaceSize);
                for(i = 0; i < recalcSize; i++)
                {
                    WriteProcessMemory(hProcess, lpMemoryStart, &FillByte, 1, &ueNumberOfBytesRead);
                    lpMemoryStart = (LPVOID)((ULONG_PTR)lpMemoryStart + 1);
                }
            }
            else if(PrependNOP)
            {
                lpMemoryStart = MemoryStart;
                for(i = 0; i < recalcSize; i++)
                {
                    WriteProcessMemory(hProcess, lpMemoryStart, &FillByte, 1, &ueNumberOfBytesRead);
                    lpMemoryStart = (LPVOID)((ULONG_PTR)lpMemoryStart + 1);
                }
                WriteProcessMemory(hProcess, lpMemoryStart, ReplacePattern, ReplaceSize, &ueNumberOfBytesRead);
            }
            else
            {
                WriteProcessMemory(hProcess, MemoryStart, ReplacePattern, ReplaceSize, &ueNumberOfBytesRead);
            }
        }
        else
        {
            WriteProcessMemory(hProcess, MemoryStart, ReplacePattern, ReplaceSize, &ueNumberOfBytesRead);
        }
        VirtualProtectEx(hProcess, MemoryStart, MemorySize, OldProtect, &OldProtect);
        return(true);
    }
    return(false);
}
__declspec(dllexport) bool TITCALL Patch(LPVOID MemoryStart, DWORD MemorySize, LPVOID ReplacePattern, DWORD ReplaceSize, bool AppendNOP, bool PrependNOP)
{

    if(dbgProcessInformation.hProcess != NULL)
    {
        return(PatchEx(dbgProcessInformation.hProcess, MemoryStart, MemorySize, ReplacePattern, ReplaceSize, AppendNOP, PrependNOP));
    }
    else
    {
        return(PatchEx(GetCurrentProcess(), MemoryStart, MemorySize, ReplacePattern, ReplaceSize, AppendNOP, PrependNOP));
    }
}
__declspec(dllexport) bool TITCALL ReplaceEx(HANDLE hProcess, LPVOID MemoryStart, DWORD MemorySize, LPVOID SearchPattern, DWORD PatternSize, DWORD NumberOfRepetitions, LPVOID ReplacePattern, DWORD ReplaceSize, PBYTE WildCard)
{

    unsigned int i;
    ULONG_PTR ueNumberOfBytesRead;
    ULONG_PTR CurrentFoundPattern;
    LPVOID cMemoryStart = MemoryStart;
    DWORD cMemorySize = MemorySize;
    LPVOID lpReadMemory = VirtualAlloc(NULL, PatternSize, MEM_COMMIT, PAGE_READWRITE);

    CurrentFoundPattern = (ULONG_PTR)FindEx(hProcess, cMemoryStart, cMemorySize, SearchPattern, PatternSize, WildCard);
    NumberOfRepetitions--;
    while(CurrentFoundPattern != NULL && NumberOfRepetitions != NULL)
    {
        if(ReadProcessMemory(hProcess, (LPVOID)CurrentFoundPattern, lpReadMemory, PatternSize, &ueNumberOfBytesRead))
        {
            for(i = 0; i < ReplaceSize; i++)
            {
                if(memcmp((LPVOID)((ULONG_PTR)ReplacePattern + i), WildCard, 1) != NULL)
                {
                    RtlMoveMemory((LPVOID)((ULONG_PTR)lpReadMemory + i), (LPVOID)((ULONG_PTR)ReplacePattern + i), 1);
                }
            }
            PatchEx(hProcess, (LPVOID)CurrentFoundPattern, PatternSize, lpReadMemory, ReplaceSize, true, false);
        }
        cMemoryStart = (LPVOID)(CurrentFoundPattern + PatternSize);
        cMemorySize = (DWORD)((ULONG_PTR)MemoryStart + MemorySize - CurrentFoundPattern);
        CurrentFoundPattern = (ULONG_PTR)FindEx(hProcess, cMemoryStart, cMemorySize, SearchPattern, PatternSize, WildCard);
        NumberOfRepetitions--;
    }
    VirtualFree(lpReadMemory, NULL, MEM_RELEASE);
    if(NumberOfRepetitions != NULL)
    {
        return(false);
    }
    else
    {
        return(true);
    }
}
__declspec(dllexport) bool TITCALL Replace(LPVOID MemoryStart, DWORD MemorySize, LPVOID SearchPattern, DWORD PatternSize, DWORD NumberOfRepetitions, LPVOID ReplacePattern, DWORD ReplaceSize, PBYTE WildCard)
{

    if(dbgProcessInformation.hProcess != NULL)
    {
        return(ReplaceEx(dbgProcessInformation.hProcess, MemoryStart, MemorySize, SearchPattern, PatternSize, NumberOfRepetitions, ReplacePattern, ReplaceSize, WildCard));
    }
    else
    {
        return(ReplaceEx(GetCurrentProcess(), MemoryStart, MemorySize, SearchPattern, PatternSize, NumberOfRepetitions, ReplacePattern, ReplaceSize, WildCard));
    }
}
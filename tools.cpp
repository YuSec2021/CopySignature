#include "tools.h"
#include <string.h>

tools::tools() {}

PCHAR tools::loadFile(IN LPCWSTR lpFileName, OUT PDWORD fileSize) {
    HANDLE hFile = CreateFile(
        lpFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    PCHAR retBuffer = NULL;
    if (hFile != INVALID_HANDLE_VALUE) {
        *fileSize = GetFileSize(hFile, NULL);
        retBuffer = (PCHAR)malloc(*fileSize);
        memset(retBuffer, 0, *fileSize);
        DWORD p = 0;
        if (ReadFile(hFile, retBuffer, *fileSize, &p, NULL)) {
            CloseHandle(hFile);
            return retBuffer;
        }
    }
    return NULL;
}

ADDRESS tools::RVAtoRAW(IN PIMAGE_SECTION_HEADER pSection, IN DWORD VirtualAddress, IN DWORD NumberOfSections) {
    DWORD index = tools::CheckSection(pSection, VirtualAddress, NumberOfSections);
    return VirtualAddress - pSection[index].VirtualAddress + pSection[index].PointerToRawData;
}

ADDRESS tools::RAWtoRVA(IN PIMAGE_SECTION_HEADER pSection, IN DWORD RAW, IN DWORD NumberOfSections) {
    return 0;
}

ADDRESS tools::CheckSection(IN PIMAGE_SECTION_HEADER pSection, IN DWORD VirtualAddress, IN DWORD NumberOfSections) {
    for (int i = 0; i < NumberOfSections; i++) {
        PIMAGE_SECTION_HEADER pTmp = &pSection[i];
        if (VirtualAddress >= pTmp->VirtualAddress
            && VirtualAddress < pTmp->VirtualAddress + pTmp->Misc.VirtualSize) {
            return i;
        }
    }
    return -1;
}

BOOLEAN tools::CheckFileType(PCHAR imageBuffer) {
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)imageBuffer;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)&imageBuffer[pDos->e_lfanew];
    if (pDos->e_magic == IMAGE_DOS_SIGNATURE && pNt->Signature == IMAGE_NT_SIGNATURE) {
        return TRUE;
    }
    return FALSE;
}

PIMAGE_SECTION_HEADER tools::GetLastSection(IN PIMAGE_NT_HEADERS pNt) {
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
    return pSection + pNt->FileHeader.NumberOfSections - 1;
}


PIMAGE_SECTION_HEADER tools::GetSection(IN PIMAGE_NT_HEADERS pNt, IN LPCSTR sectionName) {
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
    DWORD index = 0;
    while(index < pNt->FileHeader.NumberOfSections) {
        char temp[9] = {0};
        memcpy(temp, (pSection + index)->Name, 8);
        if (!strncmp(temp, sectionName, 8)) {
            return (pSection + index);
        }
        index++;
    }
    return NULL;
}

PIMAGE_NT_HEADERS tools::GetNtHeader(IN PCHAR buffer) {
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)buffer;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(buffer + pDos->e_lfanew);
    return pNt;
}

PIMAGE_DATA_DIRECTORY tools::GetSigDirectory(IN PIMAGE_NT_HEADERS pNt) {
    PIMAGE_DATA_DIRECTORY pSecurity = NULL;
    switch(pNt->FileHeader.Machine) {
        case IMAGE_FILE_MACHINE_I386:
        {
            PIMAGE_OPTIONAL_HEADER32 pOpt = (PIMAGE_OPTIONAL_HEADER32)&pNt->OptionalHeader;
            pSecurity = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
            break;
        }
        case IMAGE_FILE_MACHINE_AMD64:
        {
            PIMAGE_OPTIONAL_HEADER64 pOpt = (PIMAGE_OPTIONAL_HEADER64)&pNt->OptionalHeader;
            pSecurity = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
            break;
        }
    }

    return pSecurity;
}

PIMAGE_DATA_DIRECTORY tools::GetRelocDirectory(IN PIMAGE_NT_HEADERS pNt) {
    PIMAGE_DATA_DIRECTORY pReloc = NULL;
    switch(pNt->FileHeader.Machine) {
        case IMAGE_FILE_MACHINE_I386:
        {
            PIMAGE_OPTIONAL_HEADER32 pOpt = (PIMAGE_OPTIONAL_HEADER32)&pNt->OptionalHeader;
            pReloc = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;
        }
        case IMAGE_FILE_MACHINE_AMD64:
        {
            PIMAGE_OPTIONAL_HEADER64 pOpt = (PIMAGE_OPTIONAL_HEADER64)&pNt->OptionalHeader;
            pReloc = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;
        }
    }

    return pReloc;
}

PIMAGE_DATA_DIRECTORY tools::GetResourceDirectory(IN PIMAGE_NT_HEADERS pNt) {
    PIMAGE_DATA_DIRECTORY pReloc = NULL;
    switch(pNt->FileHeader.Machine) {
    case IMAGE_FILE_MACHINE_I386:
    {
        PIMAGE_OPTIONAL_HEADER32 pOpt = (PIMAGE_OPTIONAL_HEADER32)&pNt->OptionalHeader;
        pReloc = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
        break;
    }
    case IMAGE_FILE_MACHINE_AMD64:
    {
        PIMAGE_OPTIONAL_HEADER64 pOpt = (PIMAGE_OPTIONAL_HEADER64)&pNt->OptionalHeader;
        pReloc = &pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
        break;
    }
    }

    return pReloc;
}


BOOLEAN tools::SaveFile(IN LPCWSTR lpFileName, IN PCHAR buffer, IN DWORD bufferSize) {
    LPCWSTR extendName = ExtendString(lpFileName, L".copysig");
    HANDLE hFile = CreateFile(
        extendName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD p;
    BOOLEAN res = WriteFile(hFile, buffer, bufferSize, &p, NULL);
    CloseHandle(hFile);
    return res;
}

LPCWSTR tools::ExtendString(LPCWSTR original, LPCWSTR toAppend) {
    size_t originalLength = wcslen(original);
    size_t appendLength = wcslen(toAppend);

    wchar_t* extendedString = (wchar_t*)malloc((originalLength + appendLength + 1) * sizeof(wchar_t));
    if (extendedString == NULL) {
        return NULL;
    }

    wcscpy(extendedString, original);
    wcscat(extendedString, toAppend);

    return extendedString;
}

VOID tools::FreeMem(IN PVOID buffer) {
    free(buffer);
}

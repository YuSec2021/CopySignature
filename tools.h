#ifndef TOOLS_H
#define TOOLS_H

#include <windows.h>
#include <QDialog>
#include <QFileInfo>

typedef DWORD ADDRESS;

typedef struct _PEBUFFER{
    PCHAR pBuffer;
    DWORD bufferSize;

    PIMAGE_NT_HEADERS pNt;
    PIMAGE_SECTION_HEADER pFirstSection;

    PIMAGE_DATA_DIRECTORY pSecDir;
} PEBUFFER, *PPEBUFFER;

class tools
{
public:
    tools();
    static PCHAR loadFile(IN LPCWSTR lpFileName, OUT PDWORD fileSize);
    static BOOLEAN SaveFile(IN LPCWSTR lpFileName, IN PCHAR buffer, IN DWORD size);
    static BOOLEAN SaveFile2(IN PCHAR buffer, IN DWORD bufferSize);

    static DWORD RVAtoRAW(IN PEBUFFER pBuffer, IN DWORD VirtualAddress);
    static DWORD RAWtoRVA(IN PEBUFFER pBuffer, IN DWORD VirtualAddress);
    static DWORD CheckSection(IN PIMAGE_SECTION_HEADER pSection, IN DWORD VirtualAddress, IN DWORD NumberOfSections);

    static BOOLEAN CheckFileType(PCHAR imageBuffer);
    static PIMAGE_NT_HEADERS GetNtHeader(IN PCHAR buffer);
    static PIMAGE_SECTION_HEADER GetLastSection(IN PIMAGE_NT_HEADERS pNt);
    static PIMAGE_SECTION_HEADER GetSection(IN PIMAGE_NT_HEADERS pNt, IN LPCSTR sectionName);

    static PIMAGE_DATA_DIRECTORY GetSigDirectory(IN PIMAGE_NT_HEADERS pNt);
    static PIMAGE_DATA_DIRECTORY GetRelocDirectory(IN PIMAGE_NT_HEADERS pNt);
    static PIMAGE_DATA_DIRECTORY GetResourceDirectory(IN PIMAGE_NT_HEADERS pNt);
    static LPCWSTR ExtendString(LPCWSTR original, LPCWSTR toAppend);

    static VOID FreeMem(IN PVOID buffer);
};

#endif // TOOLS_H

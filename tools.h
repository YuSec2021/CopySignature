#ifndef TOOLS_H
#define TOOLS_H

#include <windows.h>
#include <QDialog>
#include <QFileInfo>

typedef DWORD ADDRESS;

class tools
{
public:
    tools();
    static PCHAR loadFile(IN LPCWSTR lpFileName, OUT PDWORD fileSize);
    static BOOLEAN SaveFile(IN LPCWSTR lpFileName, IN PCHAR buffer, IN DWORD size);

    static DWORD RVAtoRAW(IN PIMAGE_SECTION_HEADER pSection, IN DWORD VirtualAddress, IN DWORD NumberOfSections);
    static DWORD RAWtoRVA(IN PIMAGE_SECTION_HEADER pSection, IN DWORD VirtualAddress, IN DWORD NumberOfSections);
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

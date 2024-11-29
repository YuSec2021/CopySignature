#ifndef COPYSIGNATURE_H
#define COPYSIGNATURE_H
#include <Windows.h>

typedef struct _PEBUFFER{
    PCHAR pBuffer;
    DWORD bufferSize;

    PIMAGE_NT_HEADERS pNt;
    PIMAGE_SECTION_HEADER pFirstSection;

    PIMAGE_DATA_DIRECTORY pSecDir;
    PIMAGE_DATA_DIRECTORY pRelocDir;
} PEBUFFER, *PPEBUFFER;

class CopySignature
{
public:
    PEBUFFER SigBuffer = {0};
    PEBUFFER NoSigBuffer = {0};
    PEBUFFER NewBuffer = {0};

    CopySignature(PCHAR NoSigbuffer, PCHAR SigBuffer);
    void initBuffer(IN PCHAR buffer, OUT PPEBUFFER retbuffer);
    SIZE_T Copy();

private:
    VOID GetSignedResource(IN PCHAR buffer, OUT PIMAGE_DATA_DIRECTORY signDirectory);
};

#endif // COPYSIGNATURE_H

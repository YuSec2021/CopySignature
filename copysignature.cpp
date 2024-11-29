#include "copysignature.h"
#include "tools.h"
#include <string.h>

CopySignature::CopySignature(PCHAR NoSigBuffer, PCHAR SigBuffer) {
    this->initBuffer(NoSigBuffer, &this->NoSigBuffer);
    this->initBuffer(SigBuffer, &this->SigBuffer);
}

void CopySignature::initBuffer(IN PCHAR buffer, OUT PPEBUFFER retbuffer) {
    retbuffer->pBuffer = buffer;
    retbuffer->pNt = tools::GetNtHeader(buffer);
    retbuffer->pFirstSection = IMAGE_FIRST_SECTION(retbuffer->pNt);
    retbuffer->pSecDir = tools::GetSigDirectory(retbuffer->pNt);
    retbuffer->pRelocDir = tools::GetRelocDirectory(retbuffer->pNt);
}

SIZE_T CopySignature::Copy() {
    PIMAGE_SECTION_HEADER pResourceSection = tools::GetSection(this->NoSigBuffer.pNt, ".rsrc");

    // 加入签名资源后，资源表未对齐内存大小
    SIZE_T SigSize = this->SigBuffer.pSecDir->Size;
    DWORD unAlignSize = pResourceSection->SizeOfRawData + SigSize;

    // Check New Memory And Offset
    DWORD fileAlignment = this->NoSigBuffer.pNt->OptionalHeader.FileAlignment;
    DWORD sectionAlignment = this->NoSigBuffer.pNt->OptionalHeader.SectionAlignment;
    DWORD newFileSize = unAlignSize - unAlignSize % fileAlignment + fileAlignment;
    DWORD MemSize = unAlignSize - unAlignSize % sectionAlignment + sectionAlignment;

    DWORD addMem = newFileSize - pResourceSection->SizeOfRawData; // 增加的资源区大小
    this->NewBuffer.bufferSize = this->NoSigBuffer.bufferSize + addMem; // 计算需要申请的总大小
    this->NewBuffer.pBuffer = (PCHAR)malloc(this->NewBuffer.bufferSize);
    if (!this->NewBuffer.pBuffer) {
        return 0;
    }

    DWORD NoSigRAWOffset = pResourceSection->PointerToRawData + pResourceSection->SizeOfRawData;
    memset(this->NewBuffer.pBuffer, 0, this->NewBuffer.bufferSize);
    memcpy(this->NewBuffer.pBuffer, this->NoSigBuffer.pBuffer, NoSigRAWOffset);

    // 签名内容复制
    ADDRESS SigVirtualAddress = this->SigBuffer.pSecDir->VirtualAddress;
    ADDRESS SigRAW = tools::RVAtoRAW(
        this->SigBuffer.pFirstSection,
        SigVirtualAddress,
        this->SigBuffer.pNt->FileHeader.NumberOfSections
    );
    PCHAR resourceBuffer = this->NewBuffer.pBuffer + NoSigRAWOffset;
    memcpy(resourceBuffer, this->SigBuffer.pBuffer + SigRAW, SigSize);

    // Set NewBuffer
    this->NewBuffer.pNt = tools::GetNtHeader(this->NewBuffer.pBuffer);
    this->NewBuffer.pSecDir = tools::GetSigDirectory(this->NewBuffer.pNt);
    this->NewBuffer.pRelocDir = tools::GetRelocDirectory(this->NewBuffer.pNt);

    // Set Security Dir
    this->NewBuffer.pSecDir->VirtualAddress = pResourceSection->VirtualAddress + pResourceSection->SizeOfRawData;
    this->NewBuffer.pSecDir->Size = SigSize;

    // Fix Resource
    PIMAGE_SECTION_HEADER pNewResourceSection = tools::GetSection(this->NewBuffer.pNt, ".rsrc");
    PIMAGE_DATA_DIRECTORY pNewResourceDir = tools::GetResourceDirectory(this->NewBuffer.pNt);

    // Fix Dir
    pNewResourceDir->Size = unAlignSize;

    // Fix Section
    pNewResourceSection->Misc.VirtualSize = unAlignSize;
    pNewResourceSection->SizeOfRawData = newFileSize;

    // Fix Reloc

    // copy Reloc Text
    PIMAGE_SECTION_HEADER pRelocSection = tools::GetSection(this->NoSigBuffer.pNt, ".reloc");
    PCHAR relocBuffer = this->NewBuffer.pBuffer + pResourceSection->PointerToRawData + pNewResourceSection->SizeOfRawData;
    memcpy(relocBuffer, this->NoSigBuffer.pBuffer + pRelocSection->PointerToRawData, pRelocSection->SizeOfRawData);

    // Fix Reloc Section
    PIMAGE_SECTION_HEADER pNewRelocSection = tools::GetSection(this->NewBuffer.pNt, ".reloc");
    pNewRelocSection->PointerToRawData = pResourceSection->PointerToRawData + pNewResourceSection->SizeOfRawData;
    pNewRelocSection->VirtualAddress = pResourceSection->VirtualAddress + MemSize;

    // Fix Reloc Directory
    this->NewBuffer.pRelocDir->VirtualAddress = pNewRelocSection->VirtualAddress;

    return 1;
}





#include "copysignature.h"
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
}

SIZE_T CopySignature::Copy() {

    // 获取签名数据size
    SIZE_T SigSize = this->SigBuffer.pSecDir->Size;

    this->NewBuffer.bufferSize = this->NoSigBuffer.bufferSize + SigSize; // 计算需要申请的总大小
    this->NewBuffer.pBuffer = (PCHAR)malloc(this->NewBuffer.bufferSize);
    if (!this->NewBuffer.pBuffer) {
        return 0;
    }

    memset(this->NewBuffer.pBuffer, 0, this->NewBuffer.bufferSize);
    memcpy(this->NewBuffer.pBuffer, this->NoSigBuffer.pBuffer, this->NoSigBuffer.bufferSize);

    this->NewBuffer.pNt = tools::GetNtHeader(this->NewBuffer.pBuffer);
    this->NewBuffer.pFirstSection = IMAGE_FIRST_SECTION(this->NewBuffer.pNt);
    this->NewBuffer.pSecDir = tools::GetSigDirectory(this->NewBuffer.pNt);

    // 签名内容复制
    ADDRESS SigVirtualAddress = this->SigBuffer.pSecDir->VirtualAddress;
    memcpy(this->NewBuffer.pBuffer + this->NoSigBuffer.bufferSize, this->SigBuffer.pBuffer + SigVirtualAddress, SigSize);

    // Set Security Dir

    this->NewBuffer.pSecDir->VirtualAddress = this->NoSigBuffer.bufferSize;
    this->NewBuffer.pSecDir->Size = SigSize;

    return 1;
}





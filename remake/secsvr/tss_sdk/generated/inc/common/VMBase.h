// Auto-generated header stub from DWARF info
// Original path: inc/common/VMBase.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_COMMON_VMBASE_H_H_
#define SECSVR_INC_COMMON_VMBASE_H_H_

#include "inc/common/Types.h"

// sizeof = 8
struct CVMBase { // line 19
    enum ._1 {
        NoError = 0,
        MallocMemoryErr = 1,
        InvalidInstruction = 2,
        NullPointer = 3,
        InvalidHeader = 4,
        RunTimeError = 5,
        KeyLinkError = 6,
        KeyMatrixError = 7,
        KeyTreeError = 8,
        PackageCheckErr = 9,
        NoByteCode = 10,
        BinCodeError = 11,
    };
public:
int (***_vptr.CVMBase)(...);
CVMBase::._1 VMError;
CVMBase();
CVMBase(const CVMBase &arg0);
virtual BOOL Init(); // line 22
virtual void Release(); // line 23
virtual BOOL GetDecodeCode(BYTE *arg0, PBYTE &arg1, WORD &arg2); // line 28
virtual BOOL GetEncodeCode(BYTE *arg0, PBYTE &arg1, WORD &arg2); // line 29
virtual void GetHeader(PBYTE arg0, PWORD arg1, PWORD arg2, PWORD arg3); // line 30
virtual void SetHeader(PBYTE arg0, PWORD arg1, PWORD arg2, PWORD arg3); // line 31
virtual void GetDataBuffer(PBYTE &arg0, UINT32 &arg1); // line 32
virtual BOOL SetKeyTree(PVOID arg0, UINT32 arg1); // line 33
virtual BOOL SetKeyMatrix(PVOID arg0, UINT32 arg1, UINT32 arg2); // line 34
virtual BOOL SetKeyLink(PVOID arg0, UINT32 arg1); // line 35
virtual BOOL Run(); // line 36
virtual BOOL Load(PVOID arg0, SIZE_T arg1, PVOID arg2, SIZE_T arg3); // line 43
virtual int GetLastError(); // line 45
protected:
virtual void SetError(int arg0); // line 65
};
namespace CVMBase {
    enum ._1 {
        NoError = 0,
        MallocMemoryErr = 1,
        InvalidInstruction = 2,
        NullPointer = 3,
        InvalidHeader = 4,
        RunTimeError = 5,
        KeyLinkError = 6,
        KeyMatrixError = 7,
        KeyTreeError = 8,
        PackageCheckErr = 9,
        NoByteCode = 10,
        BinCodeError = 11,
    };
} // namespace CVMBase

#endif // SECSVR_INC_COMMON_VMBASE_H_H_

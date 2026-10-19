// Auto-generated header stub from DWARF info
// Original path: inc/VMCore/VM.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_VMCORE_VM_H_H_
#define SECSVR_INC_VMCORE_VM_H_H_

#include "inc/common/Types.h"
#include "inc/common/VMBase.h"
#include "inc/common/VMDefines.h"

// sizeof = 124
struct CVM : public CVMBase { // line 23
public:
int m_iErrCode;
protected:
CServerCryptCodeMgr *m_pCodeMgr;
UINT32 m_dwTreeLength;
UINT32 m_dwLinkLength;
PVOID m_pTree;
PVOID m_pMatrix;
PVOID m_pLink;
UINT32 m_MatrixX;
UINT32 m_MatrixY;
UINT32 m_Reg[];
RESERVEREGSTERS m_ReserveReg;
PVOID m_lpMem;
public:
CVM(const CVM &arg0);
virtual BOOL Init(); // line 1559
virtual void Release(); // line 1554
virtual BOOL GetDecodeCode(BYTE *pbyCodeBuff, PBYTE &pbyDecodeCode, WORD &wCodeLen); // line 1250
virtual BOOL GetEncodeCode(BYTE *pbyCodeBuff, PBYTE &pbyEncodeCode, WORD &wCodeLen); // line 1235
virtual void GetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen, PWORD pwDataLen); // line 1204
virtual void SetHeader(PBYTE pbyHeader, PWORD pwLen, PWORD pwCodeLen, PWORD pwDataLen); // line 1172
virtual void GetDataBuffer(PBYTE &pData, UINT32 &dwLen); // line 1165
virtual BOOL SetKeyTree(PVOID pTree, UINT32 dwSize); // line 1149
virtual BOOL SetKeyMatrix(PVOID pMatrix, UINT32 X, UINT32 Y); // line 1132
virtual BOOL SetKeyLink(PVOID pLink, UINT32 dwSize); // line 1116
virtual BOOL Run(); // line 311
virtual BOOL Load(PVOID pCodeBuffer, SIZE_T CodeBufferSize, PVOID pData, SIZE_T DataSize); // line 252
virtual int GetLastError(); // line 114
CVM(); // line 55
virtual ~CVM(); // line 96
protected:
virtual void SetError(int nErrorCode); // line 143
};


extern CVMBase * GetVM(); // line 46 (inferred)

#endif // SECSVR_INC_VMCORE_VM_H_H_

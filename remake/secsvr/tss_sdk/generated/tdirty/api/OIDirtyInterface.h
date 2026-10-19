// Auto-generated header stub from DWARF info
// Original path: tdirty/api/OIDirtyInterface.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_TDIRTY_API_OIDIRTYINTERFACE_H_H_
#define SECSVR_TDIRTY_API_OIDIRTYINTERFACE_H_H_

#include "tdirty/api/OIDirty.h"

// sizeof = 24
struct tagOIDirtyInst { // line 7
public:
CDirtyCheck objText;
CDirtyCheck objPiny;
};
typedef tagOIDirtyInst OIDIRTYINST;


extern OIDIRTYINST * OIDirtyinit(char *pszBuff, int iBuff); // line 680 (inferred)
extern int InitializeDirtyCheck(const char *pszDirtyFile); // line 268 (inferred)
extern int InitializeDirtyCheckEx(char *pszBuff, int iBuff); // line 607 (inferred)
extern int OIDirtyCheck(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 441 (inferred)
extern int OIDirtyCheckChn(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 454 (inferred)
extern int OIDirtyCheckChnEx(int iDoorLevel, char *pszContent, int &const iDirtyLevel, int &const iBeginIdx, int &const iEndIdx); // line 551 (inferred)
extern int OIDirtyCheckEng(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 474 (inferred)
extern int OIDirtyCheckEngEx(int iDoorLevel, char *pszContent, int &const iDirtyLevel, int &const iBeginIdx, int &const iEndIdx); // line 569 (inferred)
extern int OIDirtyCheckEx(int iDoorLevel, char *pszContent, int &const iDirtyLevel, int &const iBeginIdx, int &const iEndIdx); // line 541 (inferred)
extern int OIDirtyCheckPinyin(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 512 (inferred)
extern int OIDirtyCheckReg(int iDoorLevel, char *pszContent, int &const iDirtyLevel, int &const iBeginIdx, int &const iEndIdx); // line 494 (inferred)
extern int OIDirtyFini(); // line 420 (inferred)
extern int OIDirtyReplace(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 587 (inferred)
extern int OIDirtyReplace2(int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 597 (inferred)
extern int OIDirtycheck(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 745 (inferred)
extern int OIDirtycheckchn(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 768 (inferred)
extern int OIDirtycheckeng(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 788 (inferred)
extern int OIDirtycheckpiny(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 828 (inferred)
extern int OIDirtycheckreg(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 808 (inferred)
extern int OIDirtyreplace(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent, int &const iDirtyLevel); // line 758 (inferred)
extern void OIDirtyfini(OIDIRTYINST *pstInst); // line 857 (inferred)

#endif // SECSVR_TDIRTY_API_OIDIRTYINTERFACE_H_H_

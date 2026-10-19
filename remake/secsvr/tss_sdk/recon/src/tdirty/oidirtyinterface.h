// Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/OIDirtyInterface.h).
#ifndef RECON_TDIRTY_OIDIRTYINTERFACE_H_
#define RECON_TDIRTY_OIDIRTYINTERFACE_H_

#include "src/tdirty/oidirty.h"

/* ORIG OIDirtyInterface.h:7 的 tagOIDirtyInst（sizeof 24；DWARF 里
   OIDIRTYINST 是指向它的 typedef 名）。 */
struct tagOIDirtyInst {
    CDirtyCheck objText;   /* CDirtyCheck 12 字节 */
    CDirtyCheck objPiny;
};
typedef tagOIDirtyInst OIDIRTYINST;

extern int g_iCondition;

extern "C" {
void OIDirtyfini(OIDIRTYINST *pstInst);
int OIDirtycheckpiny(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                     int &iDirtyLevel);
int OIDirtycheckreg(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                    int &iDirtyLevel);
int OIDirtyCheckPinyin(int iDoorLevel, char *pszContent, int &iDirtyLevel);
int OIDirtyCheckReg(int iDoorLevel, char *pszContent, int &iDirtyLevel,
                    int &iBeginIdx, int &iEndIdx);
int OIDirtycheckeng(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                    int &iDirtyLevel);
int OIDirtyCheckEngEx(int iDoorLevel, char *pszContent, int &iDirtyLevel,
                      int &iBeginIdx, int &iEndIdx);
int OIDirtyCheckEng(int iDoorLevel, char *pszContent, int &iDirtyLevel);
int OIDirtycheckchn(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                    int &iDirtyLevel);
int OIDirtyCheckChnEx(int iDoorLevel, char *pszContent, int &iDirtyLevel,
                      int &iBeginIdx, int &iEndIdx);
int OIDirtyCheckChn(int iDoorLevel, char *pszContent, int &iDirtyLevel);
int OIDirtyreplace(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                   int &iDirtyLevel);
int OIDirtyReplace2(int iDoorLevel, char *pszContent, int &iDirtyLevel);
int OIDirtyReplace(int iDoorLevel, char *pszContent, int &iDirtyLevel);
int OIDirtycheck(OIDIRTYINST *pstInst, int iDoorLevel, char *pszContent,
                 int &iDirtyLevel);
int OIDirtyCheckEx(int iDoorLevel, char *pszContent, int &iDirtyLevel,
                   int &iBeginIdx, int &iEndIdx);
int OIDirtyCheck(int iDoorLevel, char *pszContent, int &iDirtyLevel);
OIDIRTYINST *OIDirtyinit(char *pszBuff, int iBuff);
int OIDirtyFini();
int InitializeDirtyCheckEx(char *pszBuff, int iBuff);
int InitializeDirtyCheck(char *pszFileName);
}

#endif // RECON_TDIRTY_OIDIRTYINTERFACE_H_

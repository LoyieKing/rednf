// Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/OIDirtyInterface.cpp).
#include "src/tdirty/oidirtyinterface.h"
#include "src/tdirty/charactermap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {
const int kErrNoCore     = -3329; // 0xfffff2ff
const int kErrNoInst     = -3344; // 0xfffff2f0
const int kErrMapCode    = -3345; // 0xfffff2ef
const int kErrOpen       = -3338; // 0xfffff2f6
const int kErrAlloc      = -3330; // 0xfffff2fe
const int kErrRead       = -3347; // 0xfffff2ed

CDirtyCheck *g_pstOIDirtyInstText = 0;
CDirtyCheck *g_pstOIDirtyInstPiny = 0;
}

extern "C" void OIDirtyfini(OIDIRTYINST *pstInst) {
    if (pstInst != 0)
        free(pstInst);
}

extern "C" int OIDirtycheckpiny(OIDIRTYINST *pstInst, int iDoorLevel,
                                char *pszContent, int &iDirtyLevel) {
    char sSpellCode[1024];
    char sPinyinCode[1024];
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (pstInst == 0)
        return kErrNoInst;

    memset(sSpellCode, 0, sizeof(sSpellCode));
    memset(sPinyinCode, 0, sizeof(sPinyinCode));

    if (MakeMapCode(pszContent, sSpellCode, sPinyinCode,
                    (int)sizeof(sPinyinCode)) < 0)
        return kErrMapCode;

    return pstInst->objPiny.OI_Dirty_Reg_Check(
               iDoorLevel, (unsigned char *)sPinyinCode, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtycheckreg(OIDIRTYINST *pstInst, int iDoorLevel,
                               char *pszContent, int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (pstInst == 0)
        return kErrNoInst;

    return pstInst->objText.OI_Dirty_Reg_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckPinyin(int iDoorLevel, char *pszContent,
                                  int &iDirtyLevel) {
    char sSpellCode[1024];
    char sPinyinCode[1024];
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (g_pstOIDirtyInstPiny == 0)
        return kErrNoInst;

    memset(sSpellCode, 0, sizeof(sSpellCode));
    memset(sPinyinCode, 0, sizeof(sPinyinCode));

    if (MakeMapCode(pszContent, sSpellCode, sPinyinCode,
                    (int)sizeof(sPinyinCode)) < 0)
        return kErrMapCode;

    return g_pstOIDirtyInstPiny->OI_Dirty_Reg_Check(
               iDoorLevel, (unsigned char *)sPinyinCode, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckReg(int iDoorLevel, char *pszContent,
                               int &iDirtyLevel, int &iBeginIdx,
                               int &iEndIdx) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Reg_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtycheckeng(OIDIRTYINST *pstInst, int iDoorLevel,
                               char *pszContent, int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (pstInst == 0)
        return kErrNoInst;

    return pstInst->objText.OI_Dirty_Eng_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckEngEx(int iDoorLevel, char *pszContent,
                                 int &iDirtyLevel, int &iBeginIdx,
                                 int &iEndIdx) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Eng_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckEng(int iDoorLevel, char *pszContent,
                               int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Eng_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtycheckchn(OIDIRTYINST *pstInst, int iDoorLevel,
                               char *pszContent, int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (pstInst == 0)
        return kErrNoInst;

    return pstInst->objText.OI_Dirty_Chn_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckChnEx(int iDoorLevel, char *pszContent,
                                 int &iDirtyLevel, int &iBeginIdx,
                                 int &iEndIdx) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Chn_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyCheckChn(int iDoorLevel, char *pszContent,
                               int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Chn_Check(
               iDoorLevel, (unsigned char *)pszContent, iDirtyLevel,
               iBeginIdx, iEndIdx, g_iCondition) > 0;
}

extern "C" int OIDirtyreplace(OIDIRTYINST *pstInst, int iDoorLevel,
                              char *pszContent, int &iDirtyLevel) {
    if (pstInst == 0)
        return kErrNoInst;

    return pstInst->objText.OI_Dirty_Rreplace(iDoorLevel, pszContent,
                                              iDirtyLevel, g_iCondition);
}

extern "C" int OIDirtyReplace2(int iDoorLevel, char *pszContent,
                               int &iDirtyLevel) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Rreplace(iDoorLevel, pszContent,
                                                   iDirtyLevel, 0);
}

extern "C" int OIDirtyReplace(int iDoorLevel, char *pszContent,
                              int &iDirtyLevel) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Rreplace(iDoorLevel, pszContent,
                                                   iDirtyLevel, g_iCondition);
}

extern "C" int OIDirtycheck(OIDIRTYINST *pstInst, int iDoorLevel,
                            char *pszContent, int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (pstInst == 0)
        return kErrNoInst;

    return pstInst->objText.OI_Dirty_Check(iDoorLevel, pszContent,
                                           iDirtyLevel, iBeginIdx, iEndIdx,
                                           g_iCondition);
}

extern "C" int OIDirtyCheckEx(int iDoorLevel, char *pszContent,
                              int &iDirtyLevel, int &iBeginIdx,
                              int &iEndIdx) {
    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Check(
               iDoorLevel, pszContent, iDirtyLevel, iBeginIdx, iEndIdx,
               g_iCondition);
}

extern "C" int OIDirtyCheck(int iDoorLevel, char *pszContent,
                            int &iDirtyLevel) {
    int iBeginIdx = 0;
    int iEndIdx = 0;

    if (g_pstOIDirtyInstText == 0)
        return kErrNoInst;

    return g_pstOIDirtyInstText->OI_Dirty_Check(
               iDoorLevel, pszContent, iDirtyLevel, iBeginIdx, iEndIdx,
               g_iCondition);
}

extern "C" OIDIRTYINST *OIDirtyinit(char *pszBuff, int iBuff) {
    OIDIRTYINST *pstInst = (OIDIRTYINST *)malloc(sizeof(OIDIRTYINST));
    if (pstInst == 0)
        return 0;

    if (pstInst->objText.OI_Dirty_Malloc() != 0)
        return 0;
    if (pstInst->objPiny.OI_Dirty_Malloc() != 0)
        return 0;

    pstInst->objText.OI_Dirty_Init();
    pstInst->objPiny.OI_Dirty_Init();

    pszBuff[iBuff] = 0;

    char *pch = strtok(pszBuff, "\r\n");
    while (pch != 0) {
        int iLevel = -1;
        int iType = -1;
        char szLine[1024];

        szLine[0] = 0;
        sscanf(pch, "%d %s %d", &iLevel, szLine, &iType);

        if (iType == 1)
            pstInst->objText.OI_Dirty_Chn_GetFromLineBuff(pch);
        else if (iType == 2)
            pstInst->objText.OI_Dirty_Eng_GetFromLineBuff(pch);
        else if (iType == 3)
            pstInst->objText.OI_Dirty_Reg_GetFromLineBuff(pch);
        else if (iType == 4)
            pstInst->objPiny.OI_Dirty_Reg_GetFromLineBuff(pch);

        pch = strtok(0, "\r\n");
    }

    return pstInst;
}

extern "C" int OIDirtyFini() {
    if (g_pstOIDirtyInstText != 0) {
        g_pstOIDirtyInstText->OI_Dirty_Fini();
        delete g_pstOIDirtyInstText;
        g_pstOIDirtyInstText = 0;
    }
    if (g_pstOIDirtyInstPiny != 0) {
        g_pstOIDirtyInstPiny->OI_Dirty_Fini();
        delete g_pstOIDirtyInstPiny;
        g_pstOIDirtyInstPiny = 0;
    }
    return 0;
}

extern "C" int InitializeDirtyCheckEx(char *pszBuff, int iBuff) {
    CDirtyCheck *pstText = new CDirtyCheck;
    CDirtyCheck *pstPiny = new CDirtyCheck;
    if (pstText == 0 || pstPiny == 0)
        return kErrNoInst;

    if (pstText->OI_Dirty_Malloc() != 0)
        return kErrNoCore;
    if (pstPiny->OI_Dirty_Malloc() != 0)
        return kErrNoCore;

    pstText->OI_Dirty_Init();
    pstPiny->OI_Dirty_Init();

    pszBuff[iBuff] = 0;

    char *pch = strtok(pszBuff, "\r\n");
    while (pch != 0) {
        int iLevel = -1;
        int iType = -1;
        char szLine[1024];

        szLine[0] = 0;
        sscanf(pch, "%d %s %d", &iLevel, szLine, &iType);
        if (iType == 1)
            pstText->OI_Dirty_Chn_GetFromLineBuff(pch);
        else if (iType == 2)
            pstText->OI_Dirty_Eng_GetFromLineBuff(pch);
        else if (iType == 3)
            pstText->OI_Dirty_Reg_GetFromLineBuff(pch);
        else if (iType == 4)
            pstPiny->OI_Dirty_Reg_GetFromLineBuff(pch);

        pch = strtok(0, "\r\n");
    }

    OIDirtyFini();
    g_pstOIDirtyInstText = pstText;
    g_pstOIDirtyInstPiny = pstPiny;
    return 0;
}

extern "C" int InitializeDirtyCheck(char *pszFileName) {
    FILE *fp = fopen(pszFileName, "r");
    if (fp == 0)
        return kErrOpen;

    fseek(fp, 0, SEEK_END);
    long lFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *pszBuff = (char *)malloc(lFileSize + 1);
    if (pszBuff == 0) {
        fclose(fp);
        return kErrAlloc;
    }

    size_t uiRead = fread(pszBuff, 1, lFileSize, fp);
    if ((long)uiRead != lFileSize) {
        free(pszBuff);
        fclose(fp);
        return kErrRead;
    }
    pszBuff[lFileSize] = 0;

    int iRet = InitializeDirtyCheckEx(pszBuff, (int)lFileSize);
    free(pszBuff);
    fclose(fp);
    return iRet;
}

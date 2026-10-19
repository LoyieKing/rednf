extern "C" int copyFile(char *pszFrom, char *pszDest);
extern "C" int tDirtyUpdateCheck();
extern "C" int utf8DirtyReplaceEx(int iDoorLevel, char *pszContent,
                                  int *piDirtyLevel, int iBuffLen);

// Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/tdirtyapi.cpp).
#include "src/tdirty/charactermap.h"
#include "src/tdirty/oidirtyinterface.h"

#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* ORIG tagentapi.h:76/95 的两个交换区结构（tdirtyapi.cpp CU 的 DWARF）：
   EXCHANGEHEAD 64 字节、EXCHANGEMNG 12 字节；ORIG 的 tagTdirtyEnv.pstExc
   类型是 EXCHANGEMNG*。这里按 tagentapi.h 的原文给出 typedef 形式。 */
typedef struct tagExchnageHead {
    int iExchangeSize;   /* +0  */
    int iBlockSize;      /* +4  */
    int iCreateTime;     /* +8  */
    int iMagic;          /* +12 */
    int iUsed;           /* +16 */
    int iVersion;        /* +20 */
    char szDesc[40];     /* +24 */
} EXCHANGEHEAD;

typedef struct tagExchangeMng {
    char *pShm;              /* +0 */
    void *aSem;              /* +4 */
    EXCHANGEHEAD *pstExcHead;/* +8 */
} EXCHANGEMNG;

typedef char assert_tagExchnageHead_size[sizeof(EXCHANGEHEAD) == 64 ? 1 : -1];
typedef char assert_tagExchangeMng_size[sizeof(EXCHANGEMNG) == 12 ? 1 : -1];

struct tagTdirtyEnv {
    char szFile[1024];
    char szMd5Val[33];
    int iAutoUpdate;
    int iEngineType;
    char *pShm;
    sem_t *aSem;
    tagExchangeMng *pstExc;
    unsigned char bUsed;
    int iIdx;
    pthread_t tid;
    OIDIRTYINST *pstInst;
};

struct tagSpecialChar {
    char cLen;
    char spec[7];
};

struct tagSCFHandle {
    unsigned char bUsed;
    unsigned char bCount;
    tagSpecialChar astSpecChar[255];
};

tagTdirtyEnv g_stEnv;
tagTdirtyEnv g_astEnv[255];
tagSCFHandle g_astSCF[16];
int g_iCondition = 1;

extern "C" {
void *semcreate(int iKey, int iFlag);
int semacquire(void *pSem);
int semrelease(void *pSem);
int semdestroy(void *pSem);
int agent_api_init(void **ppAgent);
int agent_api_register(void *pAgent, int iType, unsigned int uiBusId);
void *agent_api_get_freebufptr(void *pAgent, int iType, unsigned int uiBusId);
int agent_api_destroy(void *pAgent);
void Md5File(char *pszMd5, const char *pszFile);
int UTF8ToGB2312(char *pszOut, int iOutLen, const char *pszIn, int iInLen);
int GB2312ToUTF8(char *pszOut, int iOutLen, const char *pszIn, int iInLen);
}

namespace {
const int kErrInvalidFile = -300;
const int kErrOpen        = -3338; // 0xfffff2f6
const int kErrAlloc       = -3330; // 0xfffff2ee
const int kErrRead        = -3347; // 0xfffff2ed
const int kErrNoSlot      = -3350; // 0xfffff2ea
const int kErrNoHandle    = -3348; // 0xfffff2ec
const int kErrNoMem       = -2;
}

int updateExchange(tagTdirtyEnv *pstEnv, char *pszMd5) {
    if (pstEnv->pShm == 0)
        return -1;
    memcpy(pstEnv->pShm, pszMd5, 16);
    return 0;
}

int UpdateExchange_r(tagTdirtyEnv *pstEnv, char *pszMd5) {
    if (pstEnv->pShm == 0)
        return -1;
    memcpy(pstEnv->pShm, pszMd5, 16);
    return 0;
}

extern "C" int getMd5String(void *pShm, char *pszMd5) {
    if (pShm == 0)
        return -1;
    memcpy(pszMd5, pShm, 16);
    return 0;
}

extern "C" int setMd5String(void *pShm, const char *pszMd5) {
    if (pShm == 0)
        return -1;
    memcpy(pShm, pszMd5, 16);
    return 0;
}

extern "C" int getMd5FilePath(void *pShm, char *pszFile) {
    if (pShm == 0)
        return -1;
    unsigned short usLen = *(unsigned short *)((char *)pShm + 0x10);
    memcpy(pszFile, (char *)pShm + 0x12, usLen);
    pszFile[usLen] = 0;
    return 0;
}

extern "C" int setMd5FilePath(void *pShm, const char *pszFile) {
    if (pShm == 0)
        return -1;
    size_t uiLen = strlen(pszFile);
    *(unsigned short *)((char *)pShm + 0x10) = (unsigned short)uiLen;
    memcpy((char *)pShm + 0x12, pszFile, uiLen);
    return 0;
}

int validFile(const char *pszFileName) {
    struct stat stFileInfo;
    memset(&stFileInfo, 0, sizeof(stFileInfo));
    return stat(pszFileName, &stFileInfo) == -1 ? -1 : 0;
}

int initExchange(tagTdirtyEnv *pstEnv, unsigned int uiBusId) {
    if (agent_api_init((void **)&pstEnv->pstExc) < 0)
        return -1;
    if (agent_api_register(pstEnv->pstExc, 1, uiBusId) == -1)
        return -1;
    pstEnv->pShm = (char *)agent_api_get_freebufptr(pstEnv->pstExc, 1, uiBusId);
    return pstEnv->pShm != 0 ? 0 : -1;
}

int ReloadDirtyCheck_r(tagTdirtyEnv *pstEnv, const char *pszFile) {
    FILE *fp = fopen(pszFile, "r");
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

    if (fread(pszBuff, 1, lFileSize, fp) != (size_t)lFileSize) {
        free(pszBuff);
        fclose(fp);
        return kErrRead;
    }

    fclose(fp);
    pszBuff[lFileSize] = 0;
    pstEnv->pstInst = OIDirtyinit(pszBuff, (int)lFileSize);
    if (pstEnv->pstInst == 0) {
        free(pszBuff);
        return kErrAlloc;
    }

    Md5File(pstEnv->szMd5Val, pszFile);
    free(pszBuff);
    return 0;
}

int reloadDirtyCheck(tagTdirtyEnv *pstEnv, const char *pszFile) {
    if (pszFile == 0 || validFile(pszFile) == -1)
        return kErrInvalidFile;

    int iRet = InitializeDirtyCheck((char *)pszFile);
    if (iRet != 0)
        return iRet;

    Md5File(pstEnv->szMd5Val, pszFile);
    return 0;
}

int CheckExchange_r(tagTdirtyEnv *pstEnv) {
    char szMd5File[1024];
    char szMd5Val[16];
    int iRet = -1;

    memset(szMd5Val, 0, sizeof(szMd5Val));
    memset(szMd5File, 0, sizeof(szMd5File));

    if (pstEnv->pShm == 0)
        return iRet;

    getMd5String(pstEnv->pShm, szMd5Val);
    getMd5FilePath(pstEnv->pShm, szMd5File);

    if (memcmp(szMd5Val, pstEnv->szMd5Val, 16) == 0)
        return 0;

    iRet = kErrInvalidFile;
    if (validFile(szMd5File) != 0)
        return iRet;

    iRet = ReloadDirtyCheck_r(pstEnv, szMd5File);
    if (iRet != 0) {
        if (iRet == kErrInvalidFile)
            iRet = ReloadDirtyCheck_r(pstEnv, pstEnv->szFile);
        return iRet;
    }

    if (pstEnv->iIdx != 0)
        return 0;

    if (copyFile(szMd5File, (char *)pstEnv) != 0)
        return -3;
    return 0;
}

int checkExchange(tagTdirtyEnv *pstEnv) {
    char szMd5File[1024];
    char szMd5Val[16];
    int iRet = -1;

    memset(szMd5Val, 0, sizeof(szMd5Val));
    memset(szMd5File, 0, sizeof(szMd5File));

    if (pstEnv->pShm == 0)
        return iRet;

    getMd5String(pstEnv->pShm, szMd5Val);
    getMd5FilePath(pstEnv->pShm, szMd5File);

    if (memcmp(szMd5Val, pstEnv->szMd5Val, 16) == 0)
        return 0;

    iRet = reloadDirtyCheck(pstEnv, szMd5File);
    if (iRet != 0) {
        if (iRet == kErrInvalidFile)
            iRet = reloadDirtyCheck(pstEnv, pstEnv->szFile);
        return iRet;
    }

    if (copyFile(szMd5File, (char *)pstEnv) != 0)
        return -3;
    return 0;
}

int dos2unix(char *pszFileName) {
    char szTempFile[12];
    int iRet = 0;

    *(unsigned int *)(szTempFile + 0) = 0x6c632f2e; // "./cl"
    *(unsigned int *)(szTempFile + 4) = 0x706d746e; // "ntmp"
    szTempFile[8] = 0;
    strcpy(szTempFile + 4, "XXXXXX");

    mkstemp(szTempFile);

    FILE *fpIn = fopen(pszFileName, "rb");
    if (fpIn == 0)
        return 1;

    FILE *fpOut = fopen(szTempFile, "wb");
    if (fpOut == 0) {
        fclose(fpIn);
        return 1;
    }

    int c;
    while ((c = getc(fpIn)) != EOF) {
        if (c == '\r')
            continue;
        if (c == 0x1A)
            continue;
        if (putc(c, fpOut) == EOF)
            iRet = 1;
    }

    if (fclose(fpIn) == EOF)
        iRet = 1;
    if (fclose(fpOut) == EOF)
        iRet = 1;

    if (unlink(pszFileName) == -1 || iRet != 0) {
        unlink(szTempFile);
        return 1;
    }

    if (rename(szTempFile, pszFileName) == -1) {
        fprintf(stderr, "Dos2Unix: Problems renaming '%s' to '%s'\n",
                szTempFile, pszFileName);
        fprintf(stderr, "          However, file '%s' remains\n",
                szTempFile);
        return 0;
    }

    unlink(szTempFile);
    return 0;
}

extern "C" int copyFile(char *pszFrom, char *pszDest) {
    char szData[4096];
    struct stat stFileInfo;

    memset(szData, 0, sizeof(szData));
    memset(&stFileInfo, 0, sizeof(stFileInfo));

    if (stat(pszFrom, &stFileInfo) == -1)
        return -1;
    if (strcmp(pszFrom, pszDest) == 0)
        return 0;

    FILE *fpFrom = fopen(pszFrom, "r");
    FILE *fpDest = fopen(pszDest, "w");

    if (fpFrom == 0) {
        if (fpDest != 0)
            fclose(fpDest);
        return -1;
    }
    if (fpDest == 0) {
        fclose(fpFrom);
        return -1;
    }

    size_t uiRead;
    while ((uiRead = fread(szData, 1, sizeof(szData), fpFrom)) != 0)
        fwrite(szData, uiRead, 1, fpDest);

    fclose(fpFrom);
    fclose(fpDest);
    return 0;
}

// ---------------------------------------------------------------------------
// UTF-8 helpers
// ---------------------------------------------------------------------------

extern "C" char *utf8_strstr(const char *pszString, const char *pszSub,
                             int iSubLen) {
    int iLen = (int)strlen(pszString);
    if (iLen == 0)
        return 0;

    int i = 0;
    for (;;) {
        const char *p = pszString + i;
        unsigned char c = (unsigned char)*p;
        int iCharLen;
        if (c < 0x80)
            iCharLen = 1;
        else if (c <= 0xDF)
            iCharLen = 2;
        else if (c <= 0xEF)
            iCharLen = 3;
        else if (c <= 0xF7)
            iCharLen = 4;
        else if (c <= 0xFB)
            iCharLen = 5;
        else if (c == 0xFF)
            iCharLen = 0;
        else
            iCharLen = 6;

        if (iCharLen == iSubLen) {
            if (memcmp(p, pszSub, iSubLen) == 0)
                return (char *)p;
        }
        int iNext = i + iCharLen;
        if (iNext >= iLen)
            return 0;
        i = iNext;
    }
}

extern "C" int utf8_strlen(const char *pszString) {
    int iLen = (int)strlen(pszString);
    int iCount = 0;
    int i = 0;

    while (i < iLen) {
        unsigned char c = (unsigned char)pszString[i];
        if (c < 0x80)
            i += 1;
        else if (c <= 0xDF)
            i += 2;
        else if (c <= 0xEF)
            i += 3;
        else if (c <= 0xF7)
            i += 4;
        else if (c <= 0xFB)
            i += 5;
        else if (c != 0xFF)
            i += 6;
        ++iCount;
    }
    return iCount;
}

extern "C" int utf8SpecialCharFilterFini(int *piHandle) {
    if ((unsigned int)*piHandle <= 0xF) {
        tagSCFHandle *pstHandle = &g_astSCF[*piHandle];
        pstHandle->bUsed = 0;
        memset(pstHandle, 0, 0x7FA);
    }
    return 0;
}

extern "C" int utf8SpecialCharFilterInit(int *piHandle,
                                         const char *pszFileName) {
    int i = 0;
    tagSCFHandle *pstHandle = g_astSCF;

    while (pstHandle->bUsed != 0) {
        ++i;
        ++pstHandle;
        if (i == 16) {
            *piHandle = -1;
            return -1;
        }
    }

    memset(pstHandle, 0, 0x7FA);

    FILE *fp = fopen(pszFileName, "r");
    if (fp == 0) {
        *piHandle = -1;
        return -1;
    }

    *piHandle = i;
    pstHandle->bUsed = 1;

    char szLine[4096];
    char szUtf8[4096];
    int iCount = 0;

    while (!feof(fp)) {
        memset(szLine, 0, sizeof(szLine));
        if (fgets(szLine, 4096, fp) == 0)
            continue;

        char *p = strchr(szLine, '\r');
        if (p == 0)
            p = strchr(szLine, '\n');
        if (p != 0)
            *p = 0;

        int iLen = (int)strlen(szLine);
        int iRet = GB2312ToUTF8(szUtf8, 4096, szLine, iLen);
        if (iRet == -1)
            continue;

        int iUtf8Len = (int)strlen(szUtf8);
        pstHandle->astSpecChar[iCount].cLen = (char)iUtf8Len;
        if (iUtf8Len == 0) {
            pstHandle->astSpecChar[iCount].cLen = 1;
            pstHandle->astSpecChar[iCount].spec[0] = '\r';
            ++iCount;

            pstHandle->astSpecChar[iCount].cLen = 1;
            pstHandle->astSpecChar[iCount].spec[0] = '\n';
            ++iCount;
        } else {
            strncpy(pstHandle->astSpecChar[iCount].spec, szUtf8, 7);
            ++iCount;
        }
    }

    fclose(fp);
    pstHandle->bCount = (unsigned char)iCount;
    return 0;
}

extern "C" int utf8SpecialCharFilterScan(int iHandle, const char *pszString) {
    if ((unsigned int)iHandle > 0xF)
        return -1;

    tagSCFHandle *pstHandle = &g_astSCF[iHandle];
    if (pstHandle->bCount == 0)
        return 0;

    for (int i = 0; i < (int)pstHandle->bCount; ++i) {
        if (utf8_strstr(pszString, pstHandle->astSpecChar[i].spec,
                        (int)pstHandle->astSpecChar[i].cLen) != 0)
            return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Public C API
// ---------------------------------------------------------------------------

extern "C" char *tGetDirtywordVersion() {
    return g_stEnv.szMd5Val;
}

extern "C" void tDirtySetAutoUpdate(int iAutoUpdate) {
    g_stEnv.iAutoUpdate = iAutoUpdate;
}

extern "C" void tDirtySetCompCondition(int iCond) {
    g_iCondition = iCond;
}

extern "C" int tDirtyCheckPiny_r(int iHandle, int iDoorLevel,
                                 char *pszContent, int *piDirtyLevel) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;
    return OIDirtycheckpiny(pstEnv->pstInst, iDoorLevel, pszContent,
                            *piDirtyLevel);
}

extern "C" int tDirtyCheckReg_r(int iHandle, int iDoorLevel, char *pszContent,
                                int *piDirtyLevel) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;
    return OIDirtycheckreg(pstEnv->pstInst, iDoorLevel, pszContent,
                           *piDirtyLevel);
}

extern "C" int tDirtyReplace_r(int iHandle, int iDoorLevel, char *pszContent,
                               int *piDirtyLevel) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;
    return OIDirtyreplace(pstEnv->pstInst, iDoorLevel, pszContent,
                          *piDirtyLevel);
}

extern "C" int tDirtyCheck_r(int iHandle, int iDoorLevel, char *pszContent,
                             int *piDirtyLevel) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;
    return OIDirtycheck(pstEnv->pstInst, iDoorLevel, pszContent,
                        *piDirtyLevel);
}

extern "C" int tDirtyFini_r(int iHandle) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;

    sem_t *pSem = pstEnv->aSem;
    if (pSem == 0)
        return kErrNoMem;

    semacquire(pSem);
    pstEnv->bUsed = 0;
    pstEnv->aSem = 0;

    if (strstr(pstEnv->szFile, "vocabulary") != 0)
        unlink(pstEnv->szFile);

    OIDirtyfini(pstEnv->pstInst);
    pstEnv->pstInst = 0;

    semrelease(pSem);
    semdestroy(pSem);
    return 0;
}

extern "C" int tInitializeDirtyCheck_r(char *pszFile, unsigned int uiBusId) {
    int iRet = kErrNoMem;
    void *pSem = semcreate(0xccddeeff, 1);
    if (pSem == 0)
        return iRet;

    semacquire(pSem);

    int i = 0;
    tagTdirtyEnv *pstEnv = g_astEnv;
    while (pstEnv->bUsed != 0) {
        ++i;
        ++pstEnv;
        if (i == 255) {
            semrelease(pSem);
            return kErrNoSlot;
        }
    }

    semrelease(pSem);
    if (i == -1)
        return kErrNoSlot;

    memset(pstEnv, 0, sizeof(tagTdirtyEnv));
    pstEnv->aSem = (sem_t *)pSem;
    pstEnv->bUsed = 1;
    pstEnv->iIdx = i;
    pstEnv->iAutoUpdate = 0;
    pstEnv->iEngineType = 0;
    pstEnv->tid = pthread_self();

    if (pszFile != 0) {
        if (validFile(pszFile) == -1)
            return kErrInvalidFile;
        snprintf(pstEnv->szFile, 1024, "%s", pszFile);
    } else {
        pstEnv->iAutoUpdate = 1;
        sprintf(pstEnv->szFile, "/tmp/vocabulary.%d.inst[%d].%ld.txt",
                uiBusId, i, (long)pthread_self());
        FILE *fp = fopen(pstEnv->szFile, "a");
        if (fp == 0)
            return kErrInvalidFile;
        fclose(fp);
    }

    semacquire(pSem);
    if (initExchange(pstEnv, uiBusId) != 0) {
        semrelease(pstEnv->aSem);
        return -1;
    }

    iRet = ReloadDirtyCheck_r(pstEnv, g_astEnv[i].szFile);
    if (iRet < 0) {
        semrelease(pSem);
        return iRet;
    }

    semrelease(pSem);
    return i;
}

extern "C" int tDirtyUpdateCheck_r(int iHandle) {
    if ((unsigned int)iHandle > 0xFF)
        return kErrNoHandle;
    tagTdirtyEnv *pstEnv = &g_astEnv[iHandle];
    if (pstEnv->bUsed == 0)
        return kErrNoHandle;
    return CheckExchange_r(pstEnv);
}

extern "C" int tDirtyFini() {
    if (g_stEnv.pstExc != 0) {
        agent_api_destroy(g_stEnv.pstExc);
        g_stEnv.pstExc = 0;
        g_stEnv.pShm = 0;
    }
    if (g_stEnv.aSem != 0) {
        semdestroy(g_stEnv.aSem);
        g_stEnv.aSem = 0;
    }
    memset(&g_stEnv, 0, 1024);
    memset(g_stEnv.szMd5Val, 0, 33);
    OIDirtyFini();
    return 0;
}

extern "C" int tDirtyUpdateCheck() {
    return checkExchange(&g_stEnv);
}

extern "C" int tDirtyCheck(int iDoorLevel, char *pszContent,
                           int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheck(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int tDirtyCheckEx(int iDoorLevel, char *pszContent,
                             int *piDirtyLevel, int *piBeginIdx,
                             int *piEndIdx) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckEx(iDoorLevel, pszContent, *piDirtyLevel,
                          *piBeginIdx, *piEndIdx);
}

extern "C" int tDirtyCheckPinyin(int iDoorLevel, char *pszContent,
                                 int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckPinyin(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int tDirtyCheckReg(int iDoorLevel, char *pszContent,
                              int *piDirtyLevel, int *piBeginIdx,
                              int *piEndIdx) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckReg(iDoorLevel, pszContent, *piDirtyLevel,
                           *piBeginIdx, *piEndIdx);
}

extern "C" int tDirtyCheckEng(int iDoorLevel, char *pszContent,
                              int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckEng(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int tDirtyCheckEngEx(int iDoorLevel, char *pszContent,
                                int *piDirtyLevel, int *piBeginIdx,
                                int *piEndIdx) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckEngEx(iDoorLevel, pszContent, *piDirtyLevel,
                             *piBeginIdx, *piEndIdx);
}

extern "C" int tDirtyCheckChn(int iDoorLevel, char *pszContent,
                              int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckChn(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int tDirtyCheckChnEx(int iDoorLevel, char *pszContent,
                                int *piDirtyLevel, int *piBeginIdx,
                                int *piEndIdx) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyCheckChnEx(iDoorLevel, pszContent, *piDirtyLevel,
                             *piBeginIdx, *piEndIdx);
}

extern "C" int tDirtyReplace(int iDoorLevel, char *pszContent,
                             int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyReplace(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int tDirtyReplace2(int iDoorLevel, char *pszContent,
                              int *piDirtyLevel) {
    if (g_stEnv.iAutoUpdate != 0)
        tDirtyUpdateCheck();
    return OIDirtyReplace2(iDoorLevel, pszContent, *piDirtyLevel);
}

extern "C" int utf8DirtyCheck(int iDoorLevel, char *pszContent,
                              int *piDirtyLevel) {
    char szGbk[4096];

    if (g_stEnv.iEngineType != 0)
        return 0;

    memset(szGbk, 0, sizeof(szGbk));
    if (UTF8ToGB2312(szGbk, 4096, pszContent,
                     (int)strlen(pszContent)) == -1)
        return -200;
    return tDirtyCheck(iDoorLevel, szGbk, piDirtyLevel);
}

extern "C" int utf8DirtyReplace(int iDoorLevel, char *pszContent,
                                int *piDirtyLevel) {
    return utf8DirtyReplaceEx(iDoorLevel, pszContent, piDirtyLevel, 0);
}

extern "C" int utf8DirtyReplaceEx(int iDoorLevel, char *pszContent,
                                  int *piDirtyLevel, int iBuffLen) {
    char szGbk[4096];

    memset(szGbk, 0, sizeof(szGbk));

    if (iBuffLen == 0) {
        iBuffLen = (int)strlen(pszContent) + 1;
    } else {
        if (iBuffLen <= (int)strlen(pszContent))
            return -4;
    }

    if (g_stEnv.iEngineType != 0)
        return 0;

    int iConvLen = UTF8ToGB2312(szGbk, 4096, pszContent,
                                (int)strlen(pszContent));
    if (iConvLen == -1)
        return -200;

    int iRet = tDirtyReplace(iDoorLevel, szGbk, piDirtyLevel);
    if (iRet < 0)
        return iRet;

    if (GB2312ToUTF8(pszContent, iBuffLen, szGbk, iConvLen) == -1)
        return -200;

    pszContent[iBuffLen - 1] = 0;
    return 0;
}

extern "C" int tInitializeDirtyCheckEx(char *pszBuff, int iBuff) {
    return InitializeDirtyCheckEx(pszBuff, iBuff);
}

extern "C" int tMakeMapCode(char *pszCharacter, char *pszSpellCode,
                            char *pszPinyinCode, int iMaxCodeLenth) {
    return MakeMapCode(pszCharacter, pszSpellCode, pszPinyinCode,
                       iMaxCodeLenth);
}

extern "C" int tInitializeDirtyCheck(char *pszFile, unsigned int uiBusId) {
    if (g_stEnv.pShm != 0 || g_stEnv.aSem != 0)
        return 0;

    if (pszFile == 0) {
        g_stEnv.iAutoUpdate = 1;
        sprintf(g_stEnv.szFile, "/tmp/vocabulary.%d.txt", uiBusId);
        FILE *fp = fopen(g_stEnv.szFile, "a");
        if (fp == 0)
            return kErrInvalidFile;
        fclose(fp);
    } else {
        if (validFile(pszFile) == -1)
            return kErrInvalidFile;
        snprintf(g_stEnv.szFile, 1024, "%s", pszFile);
    }

    void *pSem = semcreate(0xccddeeff, 1);
    g_stEnv.aSem = (sem_t *)pSem;
    if (pSem == 0)
        return kErrNoMem;

    semacquire(pSem);
    if (initExchange(&g_stEnv, uiBusId) != 0) {
        semrelease(g_stEnv.aSem);
        return -1;
    }

    int iRet;
    if (g_stEnv.iAutoUpdate == 1) {
        Md5File(g_stEnv.szMd5Val, g_stEnv.szFile);
        iRet = checkExchange(&g_stEnv);
        if (iRet == kErrInvalidFile || iRet == 0) {
            iRet = reloadDirtyCheck(&g_stEnv, g_stEnv.szFile);
            if (iRet == 0)
                updateExchange(&g_stEnv, g_stEnv.szMd5Val);
        }
    } else {
        iRet = reloadDirtyCheck(&g_stEnv, g_stEnv.szFile);
    }

    semrelease(g_stEnv.aSem);
    return iRet;
}

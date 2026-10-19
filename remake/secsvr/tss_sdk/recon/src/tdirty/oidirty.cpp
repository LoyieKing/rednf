// Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/OIDirty.cpp).
// Control flow and constants were derived instruction by instruction from the
// original ELF; every error code is the literal observed in that image.
#include "src/tdirty/oidirty.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" long __strtol_internal(const char *nptr, char **endptr, int base,
                                 int group);

namespace {
const int kErrNoCore        = -3329; // 0xfffff2ff
const int kErrNotEnabled    = -3330; // 0xfffff2fe
const int kErrNullString    = -3331; // 0xfffff2fd
const int kErrEngFull       = -3332; // 0xfffff2fc
const int kErrChnFull       = -3333; // 0xfffff2fb
const int kErrRegFull       = -3334; // 0xfffff2fa
const int kErrRegCompile    = -3336; // 0xfffff2f8
const int kErrRegParse      = -3337; // 0xfffff2f7
const int kErrOpen          = -3338; // 0xfffff2f6
const int kErrKeyMissing    = -3340; // 0xfffff2f4
const int kErrBadCode       = -3341; // 0xfffff2f3
const int kErrDuplicate     = -3342; // 0xfffff2f2
const int kErrNoIndexTab    = -3339; // 0xfffff2f5
}

CDirtyCheck::CDirtyCheck() {
}

CDirtyCheck::~CDirtyCheck() {
    OI_Dirty_Fini();
}

void CDirtyCheck::OI_Dirty_Attach(void *ptrAddr) {
    m_pstDirtyCore = (OI_DIRTY_CORE *)ptrAddr;
    m_pstChnIndexTab = (OI_DIRTY_CHN_INDEX *)
        ((char *)m_pstDirtyCore + 16);
    m_pstDirtyEngRec = (OI_DIRTY_ENG_RECORD *)
        ((char *)m_pstDirtyCore + 0x550220);
}

int CDirtyCheck::OI_Dirty_Malloc() {
    m_pstDirtyCore = (OI_DIRTY_CORE *)malloc(sizeof(OI_DIRTY_CORE));
    if (m_pstDirtyCore == NULL)
        return kErrNoCore;
    return 0;
}

int CDirtyCheck::OI_Dirty_Init() {
    if (m_pstDirtyCore == NULL)
        return kErrNoCore;

    memset(m_pstDirtyCore, 0, sizeof(OI_DIRTY_CORE));
    m_pstChnIndexTab = (OI_DIRTY_CHN_INDEX *)
        ((char *)m_pstDirtyCore + 16);
    m_pstDirtyEngRec = (OI_DIRTY_ENG_RECORD *)
        ((char *)m_pstDirtyCore + 0x550220);

    m_pstDirtyCore->aiRegWordCount = 0;
    m_pstDirtyCore->aiEngWordCount = 0;
    m_pstDirtyCore->aiChnWordCount = 0;

    m_pstDirtyCore->iEnable = 0;
    memset(m_pstDirtyCore->astEngDirtyRec, 0,
           sizeof(m_pstDirtyCore->astEngDirtyRec));
    memset(m_pstDirtyCore->astChnDirtyRec, 0,
           sizeof(m_pstDirtyCore->astChnDirtyRec));
    memset(m_pstDirtyCore->astRegDirtyRec, 0,
           sizeof(m_pstDirtyCore->astRegDirtyRec));

    for (int i = 0; i < OI_DIRTY_CHN_INDEX_SIZE; ++i)
        m_pstChnIndexTab->iDirtyIndexTable[i] = -1;
    return 0;
}

int CDirtyCheck::OI_Dirty_Fini() {
    if (m_pstDirtyCore == NULL)
        return 0;

    for (int i = 0; i < m_pstDirtyCore->aiRegWordCount; ++i)
        regfree(&m_pstDirtyCore->astRegDirtyRec[i].stRegular);

    if (m_pstDirtyCore != NULL) {
        free(m_pstDirtyCore);
        m_pstDirtyCore = NULL;
    }
    return 0;
}

void CDirtyCheck::OI_Dirty_Report() {
    printf("Eng Words Count:%d\n", m_pstDirtyCore->aiEngWordCount);
    for (int i = 0; i < m_pstDirtyCore->aiEngWordCount; ++i)
        printf("Word:%s\tLevel:%d\n",
               m_pstDirtyCore->astEngDirtyRec[i].sDirtyStr,
               m_pstDirtyCore->astEngDirtyRec[i].iLevel);

    printf("Chn Words Count:%d\n", m_pstDirtyCore->aiChnWordCount);
    for (int i = 0; i < m_pstDirtyCore->aiChnWordCount; ++i)
        printf("Word:%s|%s\tLevel:%d\n",
               m_pstDirtyCore->astChnDirtyRec[i].sDirtyStr,
               m_pstDirtyCore->astChnDirtyRec[i].sKeyWord,
               m_pstDirtyCore->astChnDirtyRec[i].iLevel);

    printf("Reg Words Count:%d\n", m_pstDirtyCore->aiRegWordCount);
    for (int i = 0; i < m_pstDirtyCore->aiRegWordCount; ++i)
        printf("Word:%s\tLevel:%d\n",
               m_pstDirtyCore->astRegDirtyRec[i].sDirtyStr,
               m_pstDirtyCore->astRegDirtyRec[i].iLevel);
}

// ---------------------------------------------------------------------------
// Korean/Chinese line parsers
// ---------------------------------------------------------------------------

int CDirtyCheck::OI_Dirty_Eng_GetFromLineBuff(const char *pszBuff) {
    int iRet = kErrNullString;
    if (pszBuff == NULL)
        return iRet;
    if (m_pstDirtyCore->aiEngWordCount > OI_DIRTY_ENG_RECORD_SIZE - 1)
        return kErrEngFull;

    char szLineBuf[1024];
    memset(szLineBuf, 0, sizeof(szLineBuf));
    strncpy(szLineBuf, pszBuff, sizeof(szLineBuf));
    TrimStr(szLineBuf);
    if (szLineBuf[0] == '#')
        return 0;

    OI_DIRTY_ENG_RECORD *pstDirtyRecord =
        &m_pstDirtyCore->astEngDirtyRec[m_pstDirtyCore->aiEngWordCount];
    sscanf(szLineBuf, "%d %s", &pstDirtyRecord->iLevel,
           pstDirtyRecord->sDirtyStr);
    pstDirtyRecord->sDirtyStr[256] = 0;
    m_pstDirtyCore->aiEngWordCount += 1;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

int CDirtyCheck::OI_Dirty_Chn_GetFromLineBuff(const char *pszBuff) {
    unsigned char szDirtyStr[257];
    unsigned char szKeyWord[4];

    memset(szDirtyStr, 0, sizeof(szDirtyStr));
    szKeyWord[0] = 0;
    szKeyWord[1] = 0;
    szKeyWord[2] = 0;
    szKeyWord[3] = 0;

    if (pszBuff == NULL)
        return kErrNullString;
    if (m_pstDirtyCore->aiChnWordCount > OI_DIRTY_CHN_RECORD_SIZE - 1)
        return kErrChnFull;

    char szLineBuf[1024];
    memset(szLineBuf, 0, sizeof(szLineBuf));
    strncpy(szLineBuf, pszBuff, sizeof(szLineBuf));
    TrimStr(szLineBuf);
    if (szLineBuf[0] == '#')
        return 0;

    int iLevel = 0;
    sscanf(szLineBuf, "%d %s", &iLevel, szDirtyStr);
    szDirtyStr[256] = 0;

    char *pToken = strchr((char *)szDirtyStr, '|');
    if (pToken != NULL) {
        *pToken = 0;
        strncpy((char *)szKeyWord, pToken + 1, 3);
        szKeyWord[3] = 0;
    } else {
        szKeyWord[0] = 0;
    }

    int iRet = OI_Drity_Chn_Add_Item(szDirtyStr, szKeyWord,
                                     m_pstDirtyCore->aiChnWordCount, iLevel);
    if (iRet != 0)
        return iRet;

    m_pstDirtyCore->aiChnWordCount += 1;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

int CDirtyCheck::OI_Dirty_Reg_GetFromLineBuff(const char *pszBuff) {
    int iRet = kErrNullString;
    if (pszBuff == NULL)
        return iRet;
    if (m_pstDirtyCore->aiRegWordCount > OI_DIRTY_REG_RECORD_SIZE - 1)
        return kErrRegFull;

    char szLineBuf[1024];
    memset(szLineBuf, 0, sizeof(szLineBuf));
    strncpy(szLineBuf, pszBuff, sizeof(szLineBuf));
    TrimStr(szLineBuf);
    if (szLineBuf[0] == '#')
        return 0;

    int iOffset = m_pstDirtyCore->aiRegWordCount;
    OI_DIRTY_REG_RECORD *pstRegRecord = &m_pstDirtyCore->astRegDirtyRec[iOffset];

    int iReads = sscanf(szLineBuf, "%d %s", &pstRegRecord->iLevel,
                        pstRegRecord->sDirtyStr);
    if (iReads <= 1)
        return kErrRegParse;

    pstRegRecord->sDirtyStr[256] = 0;

    int err = regcomp(&pstRegRecord->stRegular,
                      (char *)pstRegRecord->sDirtyStr, REG_EXTENDED);
    if (err != 0) {
        char errbuf[1024];
        regerror(err, &pstRegRecord->stRegular, errbuf, sizeof(errbuf));
        return kErrRegCompile;
    }

    m_pstDirtyCore->aiRegWordCount += 1;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

// ---------------------------------------------------------------------------
// File parsers
// ---------------------------------------------------------------------------

int CDirtyCheck::OI_Dirty_Regular_GetFromFile(const char *pszRegFile) {
    if (pszRegFile == NULL)
        return kErrNullString;

    FILE *fpEngFile = fopen(pszRegFile, "r");
    if (fpEngFile == NULL)
        return kErrOpen;

    m_pstDirtyCore->iEnable = 0;
    m_pstDirtyCore->aiRegWordCount = 0;
    memset(m_pstDirtyCore->astRegDirtyRec, 0,
           sizeof(m_pstDirtyCore->astRegDirtyRec));

    int iCount = 0;
    int iOffset = 0;
    char szLineBuf[1024];

    while (!feof(fpEngFile)) {
        memset(szLineBuf, 0, sizeof(szLineBuf));
        fgets(szLineBuf, 1023, fpEngFile);
        TrimStr(szLineBuf);
        if (szLineBuf[0] == '#')
            continue;

        OI_DIRTY_REG_RECORD *pstRegRecord =
            &m_pstDirtyCore->astRegDirtyRec[iOffset];
        int reads = sscanf(szLineBuf, "%d %s", &pstRegRecord->iLevel,
                           pstRegRecord->sDirtyStr);
        if (reads <= 1)
            continue;

        pstRegRecord->sDirtyStr[256] = 0;
        int err = regcomp(&pstRegRecord->stRegular,
                          (char *)pstRegRecord->sDirtyStr, REG_EXTENDED);
        if (err != 0) {
            char errbuf[1024];
            regerror(err, &pstRegRecord->stRegular, errbuf, sizeof(errbuf));
            continue;
        }
        iCount += 1;
        iOffset += 1;
        if (iCount > OI_DIRTY_REG_RECORD_SIZE - 1)
            break;
    }

    fclose(fpEngFile);
    m_pstDirtyCore->aiRegWordCount = iCount;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

int CDirtyCheck::OI_Dirty_Eng_GetFromFile(const char *pszEngFile) {
    if (pszEngFile == NULL)
        return -1;

    FILE *fpEngFile = fopen(pszEngFile, "r");
    if (fpEngFile == NULL)
        return kErrOpen;

    m_pstDirtyCore->iEnable = 0;
    m_pstDirtyCore->aiEngWordCount = 0;
    memset(m_pstDirtyCore->astEngDirtyRec, 0,
           sizeof(m_pstDirtyCore->astEngDirtyRec));

    int iCount = 0;
    int iOffset = 0;
    char szLineBuf[1024];

    while (!feof(fpEngFile)) {
        memset(szLineBuf, 0, sizeof(szLineBuf));
        fgets(szLineBuf, 1023, fpEngFile);
        TrimStr(szLineBuf);
        if (szLineBuf[0] == 0)
            continue;
        if (szLineBuf[0] == '#')
            continue;

        OI_DIRTY_ENG_RECORD *pstDirtyRecord =
            &m_pstDirtyCore->astEngDirtyRec[iOffset];
        sscanf(szLineBuf, "%d %s", &pstDirtyRecord->iLevel,
               pstDirtyRecord->sDirtyStr);
        pstDirtyRecord->sDirtyStr[256] = 0;
        iCount += 1;
        iOffset += 1;
        if (iCount > OI_DIRTY_ENG_RECORD_SIZE - 1)
            break;
    }

    fclose(fpEngFile);
    m_pstDirtyCore->aiEngWordCount = iCount;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

int CDirtyCheck::OI_Dirty_Chn_GetFromFile(const char *pszChnFile) {
    if (pszChnFile == NULL)
        return kErrNullString;

    FILE *fpChnFile = fopen(pszChnFile, "r");
    if (fpChnFile == NULL)
        return kErrOpen;

    m_pstDirtyCore->iEnable = 0;
    m_pstDirtyCore->aiChnWordCount = 0;
    memset(m_pstDirtyCore->astChnDirtyRec, 0,
           sizeof(m_pstDirtyCore->astChnDirtyRec));

    for (int i = 0; i < OI_DIRTY_CHN_INDEX_SIZE; ++i)
        m_pstChnIndexTab->iDirtyIndexTable[i] = -1;

    int iCount = 0;
    char szLineBuf[1024];
    unsigned char szDirtyStr[257];
    unsigned char szKeyWord[4];

    while (!feof(fpChnFile)) {
        memset(szLineBuf, 0, sizeof(szLineBuf));
        fgets(szLineBuf, 1023, fpChnFile);
        TrimStr(szLineBuf);
        if (szLineBuf[0] == '#')
            continue;

        char *pNextToken = strtok(szLineBuf, " \t\r\n");
        if (pNextToken == NULL)
            continue;

        int iLevel = (int)__strtol_internal(pNextToken, NULL, 10, 0);

        pNextToken = strtok(NULL, "|\r\n");
        if (pNextToken == NULL)
            continue;

        strncpy((char *)szDirtyStr, pNextToken, 256);
        szDirtyStr[256] = 0;

        char *pToken = strtok(NULL, "\r\n");
        szKeyWord[0] = 0;
        if (pToken != NULL) {
            strncpy((char *)szKeyWord, pToken, 3);
            szKeyWord[3] = 0;
        }

        int iRet = OI_Drity_Chn_Add_Item(szDirtyStr, szKeyWord, iCount, iLevel);
        if (iRet < 1)
            iCount += 1;
        if (iCount > OI_DIRTY_CHN_RECORD_SIZE - 1)
            break;
    }

    fclose(fpChnFile);
    m_pstDirtyCore->aiChnWordCount = iCount;
    m_pstDirtyCore->iEnable = 1;
    return 0;
}

// ---------------------------------------------------------------------------
// Input analysis primitives
// ---------------------------------------------------------------------------

int CDirtyCheck::OI_Dirty_Mixed(unsigned char *sCheckStr) {
    while (*sCheckStr != 0) {
        unsigned char cLoChar;
        unsigned char cHiChar;

        if ((signed char)*sCheckStr >= 0)
            return 1;

        cLoChar = *sCheckStr;
        cHiChar = *(sCheckStr + 1);
        if (cHiChar == 0)
            return 0;

        if (cLoChar == 0xA3) {
            if ((unsigned char)(cHiChar + 0x50) <= 9)
                return 1;
            if ((unsigned char)(cHiChar + 0x3F) <= 0x19)
                return 1;
            if ((unsigned char)(cHiChar + 0x1F) <= 0x19)
                return 1;
        }

        if ((unsigned char)(cLoChar + 0x7F) > 0x7D)
            return 1;
        if ((unsigned char)(cHiChar - 0x40) <= 0x3E) {
            sCheckStr += 2;
            continue;
        }
        if ((unsigned char)(cHiChar - 0x80) <= 0x7E) {
            sCheckStr += 2;
            continue;
        }
        return 1;
    }
    return 0;
}

int CDirtyCheck::OI_Drity_Chn_Add_Item(unsigned char *sDirtyWord,
                                       unsigned char *sKeyWord, int iCount,
                                       int iLevel) {
    if (iCount > OI_DIRTY_CHN_RECORD_SIZE - 1)
        return kErrChnFull;
    if (m_pstChnIndexTab == NULL)
        return kErrNoIndexTab;
    if (*sKeyWord == 0)
        return -1;

    OI_DIRTY_CHN_RECORD *pstDirtyRecord =
        &m_pstDirtyCore->astChnDirtyRec[iCount];

    strncpy((char *)pstDirtyRecord->sDirtyStr, (char *)sDirtyWord, 256);
    pstDirtyRecord->sDirtyStr[256] = 0;
    strncpy((char *)pstDirtyRecord->sKeyWord, (char *)sKeyWord, 3);
    pstDirtyRecord->sKeyWord[3] = 0;
    pstDirtyRecord->iLevel = iLevel;
    pstDirtyRecord->bMix = (unsigned char)OI_Dirty_Mixed(sDirtyWord);

    if (strstr((char *)sDirtyWord, (char *)sKeyWord) == NULL) {
        memset(pstDirtyRecord, 0, sizeof(OI_DIRTY_CHN_RECORD));
        return kErrKeyMissing;
    }

    pstDirtyRecord->iKeyOffset = (short)(strstr((char *)sDirtyWord,
                                                (char *)sKeyWord) -
                                         (char *)sDirtyWord);
    pstDirtyRecord->iNextKey = -1;

    int iIndex = ((int)sKeyWord[0] << 8) + sKeyWord[1] - 0x8000;
    if ((unsigned int)iIndex > 0xFFFF) {
        return kErrBadCode;
    }

    short iKey = m_pstChnIndexTab->iDirtyIndexTable[iIndex];
    if (iKey != -1) {
        for (;;) {
            OI_DIRTY_CHN_RECORD *pstRecord =
                &m_pstDirtyCore->astChnDirtyRec[iKey];
            if (strcmp((char *)pstRecord->sDirtyStr,
                       (char *)sDirtyWord) == 0) {
                memset(pstDirtyRecord, 0, sizeof(OI_DIRTY_CHN_RECORD));
                return kErrDuplicate;
            }
            if (pstRecord->iNextKey == -1) {
                pstRecord->iNextKey = (short)iCount;
                return 0;
            }
            iKey = pstRecord->iNextKey;
        }
    }

    m_pstChnIndexTab->iDirtyIndexTable[iIndex] = (short)iCount;
    return 0;
}

int CDirtyCheck::OI_Dirty_Chn_Check_Word(int iKeyOff,
                                         unsigned char *sCheckStr,
                                         unsigned char *sReservBuff,
                                         unsigned char *sDirtyStr,
                                         int &iRealCount) {
    int iCount = 0;
    if (iKeyOff > 0) {
        if (sReservBuff[0] != sDirtyStr[0])
            return 1;
        for (iCount = 1; iCount != iKeyOff; ++iCount) {
            if (sReservBuff[iCount % 256] != sDirtyStr[iCount])
                return 1;
        }
    }

    int iCheckCount = 0;
    unsigned char *pDirty = sDirtyStr + iKeyOff;
    unsigned char cHiChar = *pDirty;

    for (;;) {
        unsigned char cLoChar;
        unsigned char cCompChar;

        if (cHiChar == 0)
            break;

        cLoChar = sCheckStr[iCheckCount];
        if (cLoChar == 0)
            return 2;
        ++iCheckCount;
        cCompChar = sCheckStr[iCheckCount];

        if ((unsigned char)(cLoChar + 0x7F) > 0x7D)
            continue;

        if (!((unsigned char)(cCompChar - 0x40) <= 0x3E ||
              (unsigned char)(cCompChar - 0x80) <= 0x7E))
            continue;

        if ((unsigned char)(cLoChar + 0x5F) <= 8 &&
            (unsigned char)(cCompChar - 0x40) <= 0xBE) {
            ++iCheckCount;
            continue;
        }

        if (cHiChar != cLoChar)
            return 3;
        if (cCompChar != pDirty[1])
            return 3;
        pDirty += 2;
        ++iCheckCount;
        cHiChar = *pDirty;
    }

    iRealCount = iCheckCount;
    return 0;
}

int CDirtyCheck::OI_Dirty_Eng_Check_Word(unsigned char *sCheckStr,
                                         unsigned char *sDirtyStr,
                                         int &iRealCount) {
    int iCount = 0;
    int iCheckCount = 0;
    unsigned char cHiChar = sDirtyStr[0];

    while (cHiChar != 0) {
        unsigned char cLoChar = sCheckStr[iCheckCount];
        unsigned char cCompChar;

        if (cLoChar == 0)
            return 1;
        ++iCheckCount;

        if ((unsigned char)(cLoChar + 0x60) <= 0x5E) {
            cCompChar = sCheckStr[iCheckCount];
            if (cCompChar == 0)
                return 1;
            ++iCheckCount;

            if (cLoChar == 0xA3) {
                if ((unsigned char)(cCompChar + 0x3F) <= 0x19) {
                    cCompChar = (unsigned char)(cCompChar - 0x80);
                } else if ((unsigned char)(cCompChar + 0x1F) <= 0x19) {
                    cCompChar = (unsigned char)(cCompChar + 0x60);
                } else if ((unsigned char)(cCompChar + 0x50) <= 9) {
                    cCompChar = (unsigned char)(cCompChar - 0x80);
                } else {
                    continue;
                }
            } else {
                continue;
            }
        } else {
            if ((unsigned char)(cLoChar - 0x61) <= 0x19)
                cCompChar = (unsigned char)(cLoChar - 0x20);
            else if ((unsigned char)(cLoChar - 0x41) <= 0x19)
                cCompChar = cLoChar;
            else if ((unsigned char)(cLoChar - 0x30) <= 9)
                cCompChar = cLoChar;
            else
                continue;
        }

        if (cCompChar != cHiChar) {
            if (!islower(cHiChar))
                return 2;
            if (cCompChar != (unsigned char)(cHiChar - 0x20))
                return 2;
        }

        ++iCount;
        cHiChar = sDirtyStr[iCount];
    }

    iRealCount = iCheckCount;
    return 0;
}

int CDirtyCheck::OI_Dirty_Eng_Check(int iLevel, unsigned char *sCheckStr,
                                    int &iDirtyLevel, int &iBeginIdx,
                                    int &iEndIdx, int iCond) {
    int iRet = 0;
    unsigned char cLoChar;
    unsigned char cHiChar;
    int iIsDirty = 0;
    int iCount = 0;
    int iIndex = 0;
    int iTempBeginIdx = 0;
    int iTempEndIdx = 0;

    iBeginIdx = 0;
    iEndIdx = 0;
    if (sCheckStr == NULL)
        return kErrNullString;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    for (;;) {
        unsigned char cCompChar;

        cLoChar = sCheckStr[iCount];
        if (cLoChar == 0)
            return 0;
        ++iCount;

        if ((unsigned char)(cLoChar + 0x60) <= 0x5E) {
            cHiChar = sCheckStr[iCount];
            if (cHiChar == 0)
                return 0;
            ++iCount;

            if (cLoChar != 0xA3)
                continue;

            if ((unsigned char)(cHiChar + 0x3F) <= 0x19) {
                cCompChar = (unsigned char)(cHiChar - 0x80);
            } else if ((unsigned char)(cHiChar + 0x1F) <= 0x19) {
                cCompChar = (unsigned char)(cHiChar + 0x60);
            } else if ((unsigned char)(cHiChar + 0x50) <= 9) {
                cCompChar = (unsigned char)(cHiChar - 0x80);
            } else {
                continue;
            }
            iTempBeginIdx = iCount - 2;
        } else {
            if ((unsigned char)(cLoChar - 0x61) <= 0x19) {
                cCompChar = (unsigned char)(cLoChar - 0x20);
            } else if ((unsigned char)(cLoChar - 0x41) <= 0x19) {
                cCompChar = cLoChar;
            } else if ((unsigned char)(cLoChar - 0x30) <= 9) {
                cCompChar = cLoChar;
            } else {
                continue;
            }
            iTempBeginIdx = iCount - 1;
        }

        for (iIndex = 0; iIndex < OI_DIRTY_ENG_RECORD_SIZE; ++iIndex) {
            OI_DIRTY_ENG_RECORD *pstRecord =
                &m_pstDirtyEngRec[iIndex];
            if (pstRecord->sDirtyStr[0] == 0)
                break;
            if (iCond < 0) {
                if (!(iLevel >= pstRecord->iLevel))
                    continue;
            } else if (iCond == 0) {
                if (iLevel != pstRecord->iLevel)
                    continue;
            } else {
                if (!(pstRecord->iLevel >= iLevel))
                    continue;
            }

            unsigned char cDirty = pstRecord->sDirtyStr[0];
            if (cCompChar != cDirty) {
                if (!islower(cDirty))
                    continue;
                if (cCompChar != (unsigned char)(cDirty - 0x20))
                    continue;
            }

            iRet = OI_Dirty_Eng_Check_Word(sCheckStr + iCount,
                                           pstRecord->sDirtyStr + 1,
                                           iIsDirty);
            if (iRet != 0)
                continue;

            iDirtyLevel = pstRecord->iLevel;
            iBeginIdx = iTempBeginIdx;
            iCount += iIsDirty;
            iEndIdx = iCount;
            return 1;
        }
    }
}

int CDirtyCheck::OI_Dirty_Chn_Check(int iLevel, unsigned char *sCheckStr,
                                    int &iDirtyLevel, int &iBeginIdx,
                                    int &iEndIdx, int iCond) {
    unsigned char sReservBuff[256];
    int iCount = 0;
    int iBufCount = 0;
    int iRet;
    int iReadCount = 0;
    unsigned char cHiChar;

    iBeginIdx = 0;
    iEndIdx = 0;
    if (sCheckStr == NULL)
        return kErrNullString;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    cHiChar = sCheckStr[0];
    for (;;) {
        unsigned char cLoChar;
        int iIndex;

        if (cHiChar == 0)
            return 0;
        ++iCount;
        cLoChar = sCheckStr[iCount];

        if ((unsigned char)(cHiChar + 0x7F) <= 0x7D) {
            if (!((unsigned char)(cLoChar - 0x40) <= 0x3E ||
                  (unsigned char)(cLoChar - 0x80) <= 0x7E)) {
                cHiChar = cLoChar;
                continue;
            }
            if ((unsigned char)(cHiChar + 0x5F) <= 8 &&
                (unsigned char)(cLoChar - 0x40) <= 0xBE) {
                ++iCount;
                cHiChar = sCheckStr[iCount];
                continue;
            }
        } else {
            cHiChar = cLoChar;
            continue;
        }

        ++iCount;
        sReservBuff[iBufCount % 256] = cHiChar;
        sReservBuff[(iBufCount + 1) % 256] = cLoChar;
        iBufCount += 2;

        iIndex = ((int)cHiChar << 8) + cLoChar - 0x8000;
        if ((unsigned int)iIndex > 0xFFFF)
            continue;

        int iKey = m_pstChnIndexTab->iDirtyIndexTable[iIndex];
        OI_DIRTY_CHN_RECORD *pstDirtyRecord;

        for (;;) {
            if ((unsigned int)iKey > OI_DIRTY_CHN_RECORD_SIZE - 1)
                goto next_char;

            pstDirtyRecord = &m_pstDirtyCore->astChnDirtyRec[iKey];
            if (pstDirtyRecord->sKeyWord[0] != cHiChar)
                goto next_char;
            if (pstDirtyRecord->sKeyWord[1] != cLoChar)
                goto next_char;

            {
                int iKeyOff = pstDirtyRecord->iKeyOffset;
                int bLevelOk;
                if (iCond < 0)
                    bLevelOk = (iLevel >= pstDirtyRecord->iLevel);
                else if (iCond == 0)
                    bLevelOk = (iLevel == pstDirtyRecord->iLevel);
                else
                    bLevelOk = (pstDirtyRecord->iLevel >= iLevel);
                if (!bLevelOk)
                    goto next_record;
            }

            iRet = OI_Dirty_Chn_Check_Word(
                pstDirtyRecord->iKeyOffset + 2, sCheckStr + iCount,
                &sReservBuff[(iBufCount - pstDirtyRecord->iKeyOffset - 2) % 256],
                pstDirtyRecord->sDirtyStr, iReadCount);
            if (iRet != 0)
                goto next_record;

            iDirtyLevel = pstDirtyRecord->iLevel;
            iBeginIdx = iCount - 2;
            iEndIdx = iCount - 2;

            if (pstDirtyRecord->iKeyOffset > 1 &&
                iBeginIdx > 1) {
                int iOffset = pstDirtyRecord->iKeyOffset;
                for (;;) {
                    unsigned char cLo;
                    unsigned char cHi;
                    unsigned char cIdxLo;
                    int bResult;

                    cLo = sCheckStr[iBeginIdx - 1];
                    cHi = sCheckStr[iBeginIdx - 2];
                    cIdxLo = pstDirtyRecord->sDirtyStr[iOffset - 1];
                    bResult = (cLo == cIdxLo);
                    iOffset -= 2;

                    if (cHi == pstDirtyRecord->sDirtyStr[iOffset]) {
                        if (bResult) {
                            iBeginIdx -= 2;
                        } else {
                            goto trim_case2;
                        }
                    } else if (bResult) {
                        iBeginIdx -= 1;
                    } else {
trim_case2:
                        if (cHi == cIdxLo) {
                            if (cLo == cIdxLo)
                                iBeginIdx -= 2;
                            else
                                iBeginIdx -= 1;
                        } else {
                            iBeginIdx -= 2;
                        }
                    }

                    if (iOffset <= 1)
                        break;
                    if (iBeginIdx <= 1)
                        break;
                }
            }

            iCount += iReadCount;
            iEndIdx = iCount;
            return 1;

next_record:
            iKey = pstDirtyRecord->iNextKey;
        }

next_char:
        cHiChar = sCheckStr[iCount];
    }
}

int CDirtyCheck::OI_Dirty_Mix_Check(int iLevel, unsigned char *sCheckStr,
                                    int &iDirtyLevel, int iReplace,
                                    int iCond) {
    unsigned char abyRsvBuf[4096];
    int iStrLen;
    int iOffset;
    int iBufCount;
    int iCount;

    if (sCheckStr == NULL || *sCheckStr == 0)
        return kErrNullString;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    iStrLen = (int)strlen((char *)sCheckStr);
    if (iStrLen + 10 > 0xFFF)
        return kErrNullString;

    iBufCount = 0;
    iCount = 0;
    for (;;) {
        unsigned char cLoChar;
        unsigned char cHiChar;

        cLoChar = sCheckStr[iCount];
        if (cLoChar == 0)
            break;
        ++iCount;

        if ((unsigned char)(cLoChar + 0x60) <= 0x5E) {
            cHiChar = sCheckStr[iCount];
            ++iCount;
            if (cHiChar == 0)
                break;

            if (cLoChar == 0xA3) {
                if ((unsigned char)(cHiChar + 0x50) <= 9) {
                    abyRsvBuf[iBufCount++] = (unsigned char)(cHiChar - 0x80);
                    continue;
                }
                if ((unsigned char)(cHiChar + 0x3F) <= 0x19) {
                    abyRsvBuf[iBufCount++] = (unsigned char)(cHiChar - 0x80);
                    continue;
                }
                if ((unsigned char)(cHiChar + 0x1F) <= 0x19) {
                    abyRsvBuf[iBufCount++] = (unsigned char)(cHiChar + 0x60);
                    continue;
                }
            }
            abyRsvBuf[iBufCount++] = cLoChar;
            abyRsvBuf[iBufCount++] = cHiChar;
        } else {
            abyRsvBuf[iBufCount++] = cLoChar;
        }
    }
    abyRsvBuf[iBufCount] = 0;

    iOffset = 0;
    int iIsDirty = 0;
    for (;;) {
        unsigned char cLoChar = abyRsvBuf[iOffset];

        if (cLoChar == 0)
            return 0;
        ++iOffset;

        if ((unsigned char)(cLoChar - 0x61) <= 0x19 ||
            (unsigned char)(cLoChar - 0x41) <= 0x19 ||
            (unsigned char)(cLoChar - 0x30) <= 9) {
            int iIndex;
            for (iIndex = 0; iIndex < OI_DIRTY_ENG_RECORD_SIZE; ++iIndex) {
                OI_DIRTY_ENG_RECORD *pstRecord =
                    &m_pstDirtyEngRec[iIndex];

                if (pstRecord->sDirtyStr[0] == 0)
                    break;
                if (iCond < 0) {
                    if (!(iLevel >= pstRecord->iLevel))
                        continue;
                } else if (iCond == 0) {
                    if (iLevel != pstRecord->iLevel)
                        continue;
                } else {
                    if (!(pstRecord->iLevel >= iLevel))
                        continue;
                }

                unsigned char cDirty = pstRecord->sDirtyStr[0];
                if (cLoChar != cDirty) {
                    if (!islower(cDirty))
                        continue;
                    if (cLoChar != (unsigned char)(cDirty - 0x20))
                        continue;
                }

                int iWordLen = (int)strlen((char *)pstRecord->sDirtyStr);
                if (strncasecmp((char *)abyRsvBuf + iOffset - 1,
                                (char *)pstRecord->sDirtyStr,
                                iWordLen) != 0)
                    continue;

                iDirtyLevel = pstRecord->iLevel;
                iIsDirty = 4;
                if (iReplace == 0)
                    return iIsDirty;

                memset(abyRsvBuf + iOffset - 1, '*', iWordLen);
                goto copy_back;
            }
            continue;
        }

        if ((unsigned char)(cLoChar + 0x60) <= 0x5E) {
            unsigned char cHiChar = abyRsvBuf[iOffset];
            int iIndex;
            if (cHiChar == 0)
                return 0;
            ++iOffset;

            if (!((unsigned char)(cLoChar + 0x7F) <= 0x7D))
                continue;
            if (!((unsigned char)(cHiChar - 0x40) <= 0x3E ||
                  (unsigned char)(cHiChar - 0x80) <= 0x7E))
                continue;

            iIndex = ((int)cLoChar << 8) + cHiChar - 0x8000;
            if ((unsigned int)iIndex > 0xFFFF)
                continue;

            {
                int iKey = m_pstChnIndexTab->iDirtyIndexTable[iIndex];
                for (;;) {
                    OI_DIRTY_CHN_RECORD *pstRecord;

                    if ((unsigned int)iKey > OI_DIRTY_CHN_RECORD_SIZE - 1)
                        break;
                    pstRecord = &m_pstDirtyCore->astChnDirtyRec[iKey];

                    if (pstRecord->sDirtyStr[0] == 0 &&
                        pstRecord->iNextKey == 0)
                        break;

                    if (pstRecord->bMix == 0) {
                        iKey = pstRecord->iNextKey;
                        continue;
                    }

                    if (pstRecord->sKeyWord[0] != cLoChar)
                        break;
                    if (pstRecord->sKeyWord[1] != cHiChar)
                        break;

                    {
                        int bLevelOk;
                        if (iCond < 0)
                            bLevelOk = (iLevel >= pstRecord->iLevel);
                        else if (iCond == 0)
                            bLevelOk = (iLevel == pstRecord->iLevel);
                        else
                            bLevelOk = (iLevel <= pstRecord->iLevel);
                        if (!bLevelOk) {
                            iKey = pstRecord->iNextKey;
                            continue;
                        }
                    }

                    {
                        int iKeyOff = pstRecord->iKeyOffset;
                        if (iKeyOff > iOffset - 2) {
                            iKey = pstRecord->iNextKey;
                            continue;
                        }

                        int iWordLen =
                            (int)strlen((char *)pstRecord->sDirtyStr);
                        if (strncasecmp((char *)abyRsvBuf + iOffset -
                                            iKeyOff - 2,
                                        (char *)pstRecord->sDirtyStr,
                                        iWordLen) != 0) {
                            iKey = pstRecord->iNextKey;
                            continue;
                        }

                        iDirtyLevel = pstRecord->iLevel;
                        iIsDirty = 4;
                        if (iReplace == 0)
                            return iIsDirty;

                        memset(abyRsvBuf + iOffset - iKeyOff - 2, '*',
                               iWordLen);
                        goto copy_back;
                    }
                }
            }
            continue;
        }
    }

copy_back:
    if (iReplace != 0 && iIsDirty != 0)
        memcpy(sCheckStr, abyRsvBuf, iBufCount + 1);
    return iIsDirty;
}

int CDirtyCheck::OI_Dirty_Check(int iLevel, char *pszContent,
                                int &iDirtyLevel, int &iBeginIdx,
                                int &iEndIdx, int iCond) {
    int iRet;

    iBeginIdx = 0;
    iEndIdx = 0;

    if (m_pstDirtyCore == NULL || m_pstChnIndexTab == NULL ||
        m_pstDirtyEngRec == NULL)
        return kErrNoCore;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    iRet = OI_Dirty_Chn_Check(iLevel, (unsigned char *)pszContent,
                              iDirtyLevel, iBeginIdx, iEndIdx, iCond);
    if (iRet > 0)
        return 1;

    iRet = OI_Dirty_Eng_Check(iLevel, (unsigned char *)pszContent,
                              iDirtyLevel, iBeginIdx, iEndIdx, iCond);
    if (iRet > 0)
        return 2;

    iRet = OI_Dirty_Mix_Check(iLevel, (unsigned char *)pszContent,
                              iDirtyLevel, 0, iCond);
    if (iRet > 0)
        return 3;
    return 0;
}

int CDirtyCheck::OI_Dirty_Rreplace(int iLevel, char *pszContent,
                                   int &iDirtyLevel, int iCond) {
    int iBegIdx = 0;
    int iEndIdx = 0;
    int iRet;

    if (m_pstDirtyCore == NULL || m_pstChnIndexTab == NULL ||
        m_pstDirtyEngRec == NULL)
        return kErrNoCore;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    while ((iRet = OI_Dirty_Chn_Check(iLevel, (unsigned char *)pszContent,
                                      iDirtyLevel, iBegIdx, iEndIdx,
                                      iCond)) > 0) {
        for (int i = iBegIdx; i < iEndIdx; ++i)
            pszContent[i] = '*';
    }
    if (iRet < 0)
        return iRet;

    while ((iRet = OI_Dirty_Eng_Check(iLevel, (unsigned char *)pszContent,
                                      iDirtyLevel, iBegIdx, iEndIdx,
                                      iCond)) > 0) {
        for (int i = iBegIdx; i < iEndIdx; ++i)
            pszContent[i] = '*';
    }
    if (iRet < 0)
        return iRet;

    while ((iRet = OI_Dirty_Mix_Check(iLevel, (unsigned char *)pszContent,
                                      iDirtyLevel, 1, iCond)) > 0) {
    }
    if (iRet < 0)
        return iRet;
    return 0;
}

int CDirtyCheck::OI_Dirty_Reg_Check(int iLevel, unsigned char *sCheckStr,
                                    int &iDirtyLevel, int &iBeginIdx,
                                    int &iEndIdx, int iCond) {
    regmatch_t subs;
    char errbuf[128];
    int err;

    iBeginIdx = 0;
    iEndIdx = 0;
    if (sCheckStr == NULL)
        return kErrNullString;
    if (m_pstDirtyCore->iEnable == 0)
        return kErrNotEnabled;

    OI_DIRTY_REG_RECORD *pstRegRecord = m_pstDirtyCore->astRegDirtyRec;
    for (int i = 0; i < m_pstDirtyCore->aiRegWordCount; ++i, ++pstRegRecord) {
        err = regexec(&pstRegRecord->stRegular, (char *)sCheckStr, 1, &subs, 0);
        if (err == REG_NOMATCH)
            continue;
        if (err != 0) {
            regerror(err, &pstRegRecord->stRegular, errbuf, sizeof(errbuf));
            continue;
        }

        {
            int bLevelOk;
            if (iCond < 0)
                bLevelOk = (iLevel >= pstRegRecord->iLevel);
            else if (iCond == 0)
                bLevelOk = (iLevel <= pstRegRecord->iLevel);
            else
                bLevelOk = (pstRegRecord->iLevel >= iLevel);
            if (!bLevelOk)
                continue;
        }

        iDirtyLevel = pstRegRecord->iLevel;
        iBeginIdx = subs.rm_so;
        iEndIdx = subs.rm_eo;
        return 1;
    }
    return 0;
}

void CDirtyCheck::TrimStr(char *pszString) {
    if (pszString == NULL)
        return;

    int iTempLength = (int)strlen(pszString);
    if (iTempLength == 0)
        return;

    char *pb = pszString;
    while (*pb == ' ' || *pb == '\t' || *pb == '\n' || *pb == '\r')
        ++pb;

    char *pe = pszString + iTempLength - 1;
    if (pb <= pe) {
        while (pe >= pb) {
            if (*pe == ' ' || *pe == '\t' || *pe == '\n' || *pe == '\r')
                --pe;
            else
                break;
        }
    }
    *(pe + 1) = 0;
    strcpy(pszString, pb);
}

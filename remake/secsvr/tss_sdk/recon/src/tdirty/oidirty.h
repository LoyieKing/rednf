// Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/OIDirty.h).
// Layouts and member offsets come from DWARF; see AGENT_REPORT_tdirty.md.
#ifndef RECON_TDIRTY_OIDIRTY_H_
#define RECON_TDIRTY_OIDIRTY_H_

#include <regex.h>

#define OI_DIRTY_CHN_INDEX_SIZE   65536
#define OI_DIRTY_CHN_RECORD_SIZE  20000
#define OI_DIRTY_ENG_RECORD_SIZE  2000
#define OI_DIRTY_REG_RECORD_SIZE  2000

struct _OI_DIRTY_CHN_INDEX {
    short iDirtyIndexTable[OI_DIRTY_CHN_INDEX_SIZE];
};
typedef _OI_DIRTY_CHN_INDEX OI_DIRTY_CHN_INDEX;

struct _OI_DIRTY_CHN_RECORD {
    unsigned char sDirtyStr[257];
    unsigned char sKeyWord[3];
    unsigned char bMix;
    short iKeyOffset;
    short iNextKey;
    int iLevel;
};
typedef _OI_DIRTY_CHN_RECORD OI_DIRTY_CHN_RECORD;

struct _OI_DIRTY_ENG_RECORD {
    unsigned char sDirtyStr[257];
    int iLevel;
};
typedef _OI_DIRTY_ENG_RECORD OI_DIRTY_ENG_RECORD;

struct _OI_DIRTY_REG_RECORD {
    unsigned char sDirtyStr[257];
    int iLevel;
    regex_t stRegular;
};
typedef _OI_DIRTY_REG_RECORD OI_DIRTY_REG_RECORD;

struct _OI_DIRTY_CORE {
    int iEnable;
    int lChnUpdateTime;
    int lEngUpdateTime;
    int lSwUpdateTime;
    OI_DIRTY_CHN_INDEX stChnDirtyIndex;
    int aiChnWordCount;
    int aiEngWordCount;
    int aiSigWordCount;
    int aiRegWordCount;
    OI_DIRTY_CHN_RECORD astChnDirtyRec[OI_DIRTY_CHN_RECORD_SIZE];
    OI_DIRTY_ENG_RECORD astEngDirtyRec[OI_DIRTY_ENG_RECORD_SIZE];
    OI_DIRTY_REG_RECORD astRegDirtyRec[OI_DIRTY_REG_RECORD_SIZE];
    unsigned int ulChnVer;
    unsigned int ulEngVer;
    char sReserve[112];
};
typedef _OI_DIRTY_CORE OI_DIRTY_CORE;

class CDirtyCheck {
public:
    CDirtyCheck();
    ~CDirtyCheck();

    int OI_Dirty_Init();
    int OI_Dirty_Fini();
    void OI_Dirty_Report();
    void OI_Dirty_Attach(void *ptrAddr);
    int OI_Dirty_Malloc();

    int OI_Dirty_Eng_GetFromLineBuff(const char *pszBuff);
    int OI_Dirty_Chn_GetFromLineBuff(const char *pszBuff);
    int OI_Dirty_Reg_GetFromLineBuff(const char *pszBuff);

    int OI_Dirty_Eng_GetFromFile(const char *pszEngFile);
    int OI_Dirty_Chn_GetFromFile(const char *pszChnFile);
    int OI_Dirty_Regular_GetFromFile(const char *pszRegFile);

    int OI_Dirty_Rreplace(int iLevel, char *pszContent, int &iDirtyLevel,
                          int iCond);
    int OI_Dirty_Check(int iLevel, char *pszContent, int &iDirtyLevel,
                       int &iBeginIdx, int &iEndIdx, int iCond);
    int OI_Dirty_Chn_Check(int iLevel, unsigned char *sCheckStr,
                           int &iDirtyLevel, int &iBeginIdx, int &iEndIdx,
                           int iCond);
    int OI_Dirty_Eng_Check(int iLevel, unsigned char *sCheckStr,
                           int &iDirtyLevel, int &iBeginIdx, int &iEndIdx,
                           int iCond);
    int OI_Dirty_Reg_Check(int iLevel, unsigned char *sCheckStr,
                           int &iDirtyLevel, int &iBeginIdx, int &iEndIdx,
                           int iCond);
    int OI_Dirty_Mix_Check(int iLevel, unsigned char *sCheckStr,
                           int &iDirtyLevel, int iReplace, int iCond);

private:
    void TrimStr(char *pszString);
    int OI_Dirty_Mixed(unsigned char *sCheckStr);
    int OI_Drity_Chn_Add_Item(unsigned char *sDirtyWord,
                              unsigned char *sKeyWord, int iCount, int iLevel);
    int OI_Dirty_Chn_Check_Word(int iKeyOff, unsigned char *sCheckStr,
                                unsigned char *sReservBuff,
                                unsigned char *sDirtyStr, int &iRealCount);
    int OI_Dirty_Eng_Check_Word(unsigned char *sCheckStr,
                                unsigned char *sDirtyStr, int &iRealCount);

    OI_DIRTY_CHN_INDEX *m_pstChnIndexTab;
    OI_DIRTY_CORE *m_pstDirtyCore;
    OI_DIRTY_ENG_RECORD *m_pstDirtyEngRec;
};

#endif // RECON_TDIRTY_OIDIRTY_H_

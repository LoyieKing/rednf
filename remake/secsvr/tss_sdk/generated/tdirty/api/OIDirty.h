// Auto-generated header stub from DWARF info
// Original path: tdirty/api/OIDirty.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_TDIRTY_API_OIDIRTY_H_H_
#define SECSVR_TDIRTY_API_OIDIRTY_H_H_

#include <regex.h>

// sizeof = 131072
struct _OI_DIRTY_CHN_INDEX { // line 44
public:
short int iDirtyIndexTable[];
};
typedef _OI_DIRTY_CHN_INDEX OI_DIRTY_CHN_INDEX;
// sizeof = 272
struct _OI_DIRTY_CHN_RECORD { // line 49
public:
unsigned char sDirtyStr[];
unsigned char sKeyWord[];
unsigned char bMix;
short int iKeyOffset;
short int iNextKey;
int iLevel;
};
typedef _OI_DIRTY_CHN_RECORD OI_DIRTY_CHN_RECORD;
// sizeof = 264
struct _OI_DIRTY_ENG_RECORD { // line 61
public:
unsigned char sDirtyStr[];
int iLevel;
};
typedef _OI_DIRTY_ENG_RECORD OI_DIRTY_ENG_RECORD;
// sizeof = 296
struct _OI_DIRTY_REG_RECORD { // line 68
public:
unsigned char sDirtyStr[];
int iLevel;
regex_t stRegular;
};
typedef _OI_DIRTY_REG_RECORD OI_DIRTY_REG_RECORD;
// sizeof = 6691224
struct _OI_DIRTY_CORE { // line 85
public:
int iEnable;
int lChnUpdateTime;
int lEngUpdateTime;
int lSwUpdateTime;
OI_DIRTY_CHN_INDEX stChnDirtyIndex;
int aiChnWordCount;
int aiEngWordCount;
int aiSigWordCount;
int aiRegWordCount;
OI_DIRTY_CHN_RECORD astChnDirtyRec[];
OI_DIRTY_ENG_RECORD astEngDirtyRec[];
OI_DIRTY_REG_RECORD astRegDirtyRec[];
unsigned int ulChnVer;
unsigned int ulEngVer;
char sReserve[];
};
typedef _OI_DIRTY_CORE OI_DIRTY_CORE;
// sizeof = 12
struct CDirtyCheck { // line 114
private:
OI_DIRTY_CHN_INDEX *m_pstChnIndexTab;
OI_DIRTY_CORE *m_pstDirtyCore;
OI_DIRTY_ENG_RECORD *m_pstDirtyEngRec;
public:
CDirtyCheck(); // line 116
~CDirtyCheck(); // line 117
int OI_Dirty_Init(); // line 119
int OI_Dirty_Fini(); // line 120
void OI_Dirty_Report(); // line 122
void OI_Dirty_Attach(void *arg0); // line 123
int OI_Dirty_Malloc(); // line 125
int OI_Dirty_Eng_GetFromLineBuff(const char *arg0); // line 126
int OI_Dirty_Chn_GetFromLineBuff(const char *arg0); // line 127
int OI_Dirty_Reg_GetFromLineBuff(const char *arg0); // line 128
int OI_Dirty_Eng_GetFromFile(const char *arg0); // line 130
int OI_Dirty_Chn_GetFromFile(const char *arg0); // line 131
int OI_Dirty_Regular_GetFromFile(const char *arg0); // line 132
int OI_Dirty_Rreplace(int arg0, char *arg1, int &arg2, int arg3); // line 134
int OI_Dirty_Check(int arg0, char *arg1, int &arg2, int &arg3, int &arg4, int arg5); // line 135
int OI_Dirty_Chn_Check(int arg0, unsigned char *arg1, int &arg2, int &arg3, int &arg4, int arg5); // line 137
int OI_Dirty_Eng_Check(int arg0, unsigned char *arg1, int &arg2, int &arg3, int &arg4, int arg5); // line 138
int OI_Dirty_Reg_Check(int arg0, unsigned char *arg1, int &arg2, int &arg3, int &arg4, int arg5); // line 139
int OI_Dirty_Mix_Check(int arg0, unsigned char *arg1, int &arg2, int arg3, int arg4); // line 140
private:
void TrimStr(char *arg0); // line 144
int OI_Dirty_Mixed(unsigned char *arg0); // line 146
int OI_Drity_Chn_Add_Item(unsigned char *arg0, unsigned char *arg1, int arg2, int arg3); // line 147
int OI_Dirty_Chn_Check_Word(int arg0, unsigned char *arg1, unsigned char *arg2, unsigned char *arg3, int &arg4); // line 148
int OI_Dirty_Eng_Check_Word(unsigned char *arg0, unsigned char *arg1, int &arg2); // line 149
};

#endif // SECSVR_TDIRTY_API_OIDIRTY_H_H_

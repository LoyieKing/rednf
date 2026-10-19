// Auto-generated header stub from DWARF info
// Original path: sandyxie/dirty/tsf4g_proj/trunk/include/taa/tagentapi.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SANDYXIE_DIRTY_TSF4G_PROJ_TRUNK_INCLUDE_TAA_TAGENTAPI_H_H_
#define SECSVR_SANDYXIE_DIRTY_TSF4G_PROJ_TRUNK_INCLUDE_TAA_TAGENTAPI_H_H_

// sizeof = 64
struct tagExchnageHead { // line 76
public:
int iExchangeSize;
int iBlockSize;
int iCreateTime;
int iMagic;
int iUsed;
int iVersion;
char szDesc[];
};
typedef tagExchnageHead EXCHANGEHEAD;
// sizeof = 12
struct tagExchangeMng { // line 95
public:
char *pShm;
void *aSem;
EXCHANGEHEAD *pstExcHead;
};
typedef tagExchangeMng EXCHANGEMNG;

#endif // SECSVR_SANDYXIE_DIRTY_TSF4G_PROJ_TRUNK_INCLUDE_TAA_TAGENTAPI_H_H_

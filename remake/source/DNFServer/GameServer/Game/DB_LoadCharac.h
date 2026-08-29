#ifndef GAME_DB_LOADCHARAC_H_
#define GAME_DB_LOADCHARAC_H_

// ============================================================================
// df_game_r 还原 —— DB_LoadCharac（SIG_LOAD_CHARAC DB 装载分发器，DB 线程侧）。
// ORIG 13 符号（/tmp/only_orig/DB_LoadCharac.txt）：
//   C1/C2 0x844d8c6（调 DBDispatcher::DBDispatcher 后设 vptr，对象尺寸 4）
//   D0/D1/D2 由类定义自然产出（虚析构，D0 deleting）
//   dispatch 0x08408a3c | _getGameDB 0x08408c0a | _sendResult 0x0840a4ce
//   _getCharacInfo 0x08408cf4 | _getCharacStat 0x084093f6
//   _GetTagCharacInfo 0x08409fa6（static，无 this）| _UpdateSchoolCharac 0x08408c30
//   _LoadEventBringUpInfo 0x08409d2e
// 实现：DB_LoadCharac.cpp（单 TU）。每函数按
// docs/class_func_reports/DB_LoadCharac/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_CHARAC 权威定义见 SigTypes_C.h（+0xc CHARAC_DATA / +0x686 tag
// charac no / +0x68b TAG_CHARAC_DATA；本 TU 内其余字段按文档化偏移访问）。
// ============================================================================

#include "DBDispatcher.h"

class Stream;
class MySQL;
struct SIG_LOAD_CHARAC;
struct TAG_CHARAC_DATA;

class DB_LoadCharac : public DBDispatcher
{
public:
    // ORIG 0x844d8c6 _ZN13DB_LoadCharacC1Ev (size 0x0000001c)  报告: docs/class_func_reports/DB_LoadCharac
    DB_LoadCharac();
    // ORIG 0x844d912 _ZN13DB_LoadCharacD0Ev (size 0x0000001e)  报告: docs/class_func_reports/DB_LoadCharac
    virtual ~DB_LoadCharac();
    // ORIG 0x8408c0a _ZN13DB_LoadCharac10_getGameDBEv (size 0x00000025)  报告: DB_LoadCharac/_getGameDB.md
    MySQL* _getGameDB();
    // ORIG 0x840a4ce _ZN13DB_LoadCharac11_sendResultEiiP15SIG_LOAD_CHARAC (size 0x00000136)  报告: DB_LoadCharac/_sendResult.md
    void _sendResult(int, int, SIG_LOAD_CHARAC*);
    // ORIG 0x8408cf4 _ZN13DB_LoadCharac14_getCharacInfoEP15SIG_LOAD_CHARAC (size 0x00000702)  报告: DB_LoadCharac/_getCharacInfo.md
    bool _getCharacInfo(SIG_LOAD_CHARAC*);
    // ORIG 0x84093f6 _ZN13DB_LoadCharac14_getCharacStatEP15SIG_LOAD_CHARAC (size 0x00000937)  报告: DB_LoadCharac/_getCharacStat.md
    bool _getCharacStat(SIG_LOAD_CHARAC*);
    // ORIG 0x8409fa6 _ZN13DB_LoadCharac17_GetTagCharacInfoEjP15TAG_CHARAC_DATA (size 0x00000527)  报告: DB_LoadCharac/_GetTagCharacInfo.md  【static】
    static bool _GetTagCharacInfo(unsigned int, TAG_CHARAC_DATA*);
    // ORIG 0x8408c30 _ZN13DB_LoadCharac19_UpdateSchoolCharacEP15SIG_LOAD_CHARAC (size 0x000000c4)  报告: DB_LoadCharac/_UpdateSchoolCharac.md
    bool _UpdateSchoolCharac(SIG_LOAD_CHARAC*);
    // ORIG 0x8409d2e _ZN13DB_LoadCharac21_LoadEventBringUpInfoEP15SIG_LOAD_CHARAC (size 0x00000278)  报告: DB_LoadCharac/_LoadEventBringUpInfo.md
    bool _LoadEventBringUpInfo(SIG_LOAD_CHARAC*);
    // ORIG 0x8408a3c _ZN13DB_LoadCharac8dispatchEiiP6Stream (size 0x000001ce)  报告: DB_LoadCharac/dispatch.md
    virtual bool dispatch(int, int, Stream*);
};

#endif  // GAME_DB_LOADCHARAC_H_

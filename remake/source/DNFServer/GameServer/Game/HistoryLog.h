#ifndef DNF_GAME_HISTORY_LOG_H_
#define DNF_GAME_HISTORY_LOG_H_

// df_game_r 还原 —— HistoryLog（G2-4 批次，历史日志文件写入）。
// 原类定义位于 HistoryLog.cpp 内部；因 CGameManager.cpp 需真实调用
// LogClose（原 asm 桥 _ZN10HistoryLog8LogCloseEP8_IO_FILE），拆到本头
// 作为唯一声明点（HistoryLog.cpp 亦 include）。
// 静态成员：+0x00 StaticPool<_IO_FILE,50> logfiles_（0x30）| int count_
// | char time[0x10]。

#include <cstdio>

#include "CGameManager.h"  // StaticPool / _IO_FILE typedef

class HistoryLog
{
public:
    static bool Init();
    static FILE* LogOpen(unsigned int chNo);
    static void LogClose(FILE* file);
    static bool CheckFD(FILE* file);

    static void WriteCreateCh(FILE* file, char* name, int job, int level, int growType);
    static void WriteDeleteCh(FILE* file, char* name, int job, int level, int growType);
    static void WriteSelectCh(FILE* file, const char* name, int job, int level,
                              int growType);
    static void WriteLevelUp(FILE* file, int level);
    static void WriteChat(FILE* file, int type, char* content, int level);
    static void WriteFineQuest(FILE* file, int questIdx);
    static void WriteLearnSkill(FILE* file, int skillIdx, int level);
    static void WriteDunStart(FILE* file, int job, int level, int growType);
    static void WriteKillMob(FILE* file, int mobType);
    static void WriteDie(FILE* file);
    static void WriteGiveUp(FILE* file, const char* reason);
    static void WriteUseCoin(FILE* file, const char* reason);
    static void WriteDunClear(FILE* file);
    static void WriteTradeUp(FILE* file, int itemIdx, int count);
    static void WriteTradeDown(FILE* file, int itemIdx, int count);
    static void WriteDisJoint(FILE* file, int itemIdx);
    static void WriteComposition(FILE* file, int itemIdx);
    static void WriteBuyItem(FILE* file, int itemIdx, int count);
    static void WriteSellItem(FILE* file, int itemIdx, int count);

    static StaticPool<_IO_FILE, 50> logfiles_;  // 0943dd00
    static int count_;                          // 0943dd30
    static char time[0x10];                     // 0943dd34
};

#endif  // DNF_GAME_HISTORY_LOG_H_

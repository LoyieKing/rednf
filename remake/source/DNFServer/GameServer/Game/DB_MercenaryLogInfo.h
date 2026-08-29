#ifndef GAME_DB_MERCENARYLOGINFO_H_
#define GAME_DB_MERCENARYLOGINFO_H_

#include "DBDispatcher.h"

// df_game_r Game/ DB_MercenaryLogInfo（雇佣兵竞技 DB 请求封装；独立叶子头）。
//
// ORIG（df_game_r, elf32-i386）：
//   _ZN19DB_MercenaryLogInfo11makeRequestEjjjjccciii  T 0x08446340（static，无 this）
//   _ZN19DB_MercenaryLogInfoC1Ev                      W 0x0844faea（base ctor+vptr）
//   _ZN19DB_MercenaryLogInfo8dispatchEiiP6Stream      T 0x08446196（虚槽 2 覆盖；
//     定义在 W8LT_DBTask.cpp）
// 调用点 CHARAC_LOAD_MERCENARY::Competition/Reward（CUserMissingA2.cpp）以
// (get_acc_id, characNo, startSec, endSec, status, periodIdx, areaIdx, gold,
//  itemIdx, rarity) 十实参直调。
class DB_MercenaryLogInfo : public DBDispatcher
{
public:
    DB_MercenaryLogInfo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08446196
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            unsigned int startSec, unsigned int endSec,
                            char status, char periodIdx, char areaIdx,
                            int gold, int itemIdx, int rarity);
};

#endif  // GAME_DB_MERCENARYLOGINFO_H_

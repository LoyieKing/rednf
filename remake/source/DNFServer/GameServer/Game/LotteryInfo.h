// ============================================================================
// LotteryInfo.h — 抽奖信息 POD 唯一权威声明（ORIG sizeof 0x20）
//
// 依据 ORIG nm/objdump：
//   ctor W 0x083706d4（构造 +0x04 vector<pair<int,int>> / +0x14 vector<int> 后
//                     调 clear）/ clear W 0x0837069c / dtor W 0x08374cfc
//   createLotteryInfo(LotteryInfo*, std::vector<int>*) T 0x089bab64。
//
// 此前 CStackableItem.h 与 DisJoint.h 各持同名同布局定义（字段名不同），
// 同 TU 同时包含两链头时重定义冲突；统一为本头，方法定义归属 CStackableItem.cpp。
// ============================================================================
#ifndef GAME_LOTTERY_INFO_H_
#define GAME_LOTTERY_INFO_H_

#include <utility>
#include <vector>

class LotteryInfo
{
public:
    LotteryInfo();   // ORIG _ZN11LotteryInfoC1Ev W 0x083706d4
    ~LotteryInfo();  // ORIG _ZN11LotteryInfoD1Ev W 0x08374cfc
    void clear();    // ORIG _ZN11LotteryInfo5clearEv W 0x0837069c

    int m_defaultIdx;                              // +0x00（clear 置 0）
    std::vector<std::pair<int, int> > m_items;     // +0x04
    int m_defaultCount;                            // +0x10（clear 置 0）
    std::vector<int> m_counts;                     // +0x14
};  // sizeof == 0x20（ctor 双 vector 构造实证）

#endif  // GAME_LOTTERY_INFO_H_

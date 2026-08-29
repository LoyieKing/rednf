// stNeoplePCBANG —— 网吧 PC 房增益脚本结构（ORIG 顶层类型）。
// ORIG（mangled _ZN14stNeoplePCBANG...）：
//   C1 W 0x0891acce、C1ERKS_ W 0x089276f8、D1 W 0x0891c6c2、operator= W 0x089277be、
//   clear W 0x0891ac9a（+0x00/+0x04/+0x08 int=0 + vector<pair> clear）。
// importPCBANGScript（T 0x08905773）实证：
//   +0x04 [level]、+0x08 [people count] ScanInt；+0x0c vector<pair<ENUM_CHANGE_STATUS_TYPE,
//   int>>（[physical attack] 等 16 段，first=状态枚举值、second=ScanInt）。
#ifndef DNF_GAME_STNEOPLEPCBANG_H_
#define DNF_GAME_STNEOPLEPCBANG_H_

#include <utility>
#include <vector>

#include "EquipmentParameterInfo.h"  // ENUM_CHANGE_STATUS_TYPE

struct stNeoplePCBANG
{
    stNeoplePCBANG();                            // W 0x0891acce
    stNeoplePCBANG(stNeoplePCBANG const&);       // W 0x089276f8
    ~stNeoplePCBANG();                           // W 0x0891c6c2
    stNeoplePCBANG& operator=(stNeoplePCBANG const&);  // W 0x089277be
    void clear();                                // W 0x0891ac9a

    int m_field0;      // +0x00（clear 置 0；importPCBANGScript 未写入）
    int m_level;       // +0x04（[level]）
    int m_peopleCount; // +0x08（[people count]）
    std::vector<std::pair<ENUM_CHANGE_STATUS_TYPE, int> > m_status;  // +0x0c
};  // sizeof 0x18

#endif  // DNF_GAME_STNEOPLEPCBANG_H_

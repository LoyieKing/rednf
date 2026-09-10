// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CReliablePerson/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CReliablePerson13InitTradeGoldEjjj`（ORIG 0x86454a8）。
#include <iostream>

// CReliablePerson — 交易金币可信度统计（ORIG 0x86453b8..0x86454a8；
// 权威参照 docs/class_func_reports/CReliablePerson.md）
// 布局（+0x00..+0x0f）：
//   +0x00 total（累计交易金；GetTradeGoldTotal；>1e9 时溢出到 billion）
//   +0x04 billion（累计溢出的十亿位；GetTradeGoldTotalBillion）
//   +0x08 daily（日交易金；GetTradeGoldDaily / SetTradeGoldDaily）
//   +0x0c schoolPoint（CUserCharacInfo.h 侧声明）
// 原始工程中 3 个 Get* + SetTradeGoldDaily 为头内联（ORIG W），
// 此处统一落地为本 TU 定义（符号体逐字节一致）。

class CReliablePerson
{
public:
    void InitTradeGold(unsigned int daily, unsigned int billion,
                       unsigned int total);
    unsigned int GetTradeGoldDaily() const;
    unsigned int GetTradeGoldTotal() const;
    unsigned int GetTradeGoldTotalBillion() const;
    void SetTradeGoldDaily(unsigned int gold);
    void IncrementTradeGold(unsigned int gold);

    unsigned int m_totalTradeGold;   // +0x00
    unsigned int m_billionTradeGold; // +0x04
    unsigned int m_dailyTradeGold;   // +0x08
    int m_schoolPoint;              // +0x0c
};

#include "CReliablePerson/InitTradeGold.inc"
}

#include "CReliablePerson/GetTradeGoldDaily.inc"
}

#include "CReliablePerson/GetTradeGoldTotal.inc"
}

#include "CReliablePerson/GetTradeGoldTotalBillion.inc"
}

#include "CReliablePerson/SetTradeGoldDaily.inc"
}

#include "CReliablePerson/IncrementTradeGold.inc"
}

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StatisticsToObserveCrash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— StatisticsToObserveCrash（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsToObserveCrash.md + ORIG 反汇编。
// 布局：+0x00 int | +0x04 Mutex | +0x1c/0x34/0x4c 三个 map<int,int>。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsToObserveCrash
{
public:
    StatisticsToObserveCrash();
    ~StatisticsToObserveCrash();

    void reset();
    int getAbnormalDownUserCount();
    void setAbnormalDownUserCount(int count);
    void incAbnormalDownUserCount();
    void incUserLastCrashPacketID(int userIdx);
    int getUserLastCrashPacketID(int userIdx);
    void incUserLastCrashDungeonID(int userIdx);
    int getUserLastCrashDungeonID(int userIdx);
    void incUserLastCrashPlayStatus(int userIdx);
    int getUserLastCrashPlayStatus(int userIdx);

    int m_abnormalDownUserCount;        // +0x00
    Mutex m_mutex;                      // +0x04
    std::map<int, int> m_crashPacketID;   // +0x1c
    std::map<int, int> m_crashDungeonID;  // +0x34
    std::map<int, int> m_crashPlayStatus; // +0x4c
};

#include "StatisticsToObserveCrash/StatisticsToObserveCrash.inc"
}

#include "StatisticsToObserveCrash/dtor_StatisticsToObserveCrash.inc"

#include "StatisticsToObserveCrash/reset.inc"
}

#include "StatisticsToObserveCrash/getAbnormalDownUserCount.inc"
}

#include "StatisticsToObserveCrash/setAbnormalDownUserCount.inc"
}

#include "StatisticsToObserveCrash/incAbnormalDownUserCount.inc"
}

// [b28d-62] AE：value_type 直构 + `(*it).second`（P167：去 make_pair 转换 ctor
//   与 ptEv）。ORIG 0x0860bb88
#include "StatisticsToObserveCrash/incUserLastCrashPacketID.inc"
}

#include "StatisticsToObserveCrash/getUserLastCrashPacketID.inc"
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bd2e
#include "StatisticsToObserveCrash/incUserLastCrashDungeonID.inc"
}

#include "StatisticsToObserveCrash/getUserLastCrashDungeonID.inc"
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bed4
#include "StatisticsToObserveCrash/incUserLastCrashPlayStatus.inc"
}

#include "StatisticsToObserveCrash/getUserLastCrashPlayStatus.inc"
}

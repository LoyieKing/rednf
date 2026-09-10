// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StatisticsShusiaService/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— StatisticsShusiaService（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsShusiaService.md + ORIG 反汇编。
// 布局：+0x00 Mutex | +0x18 map<int,int> | +0x30 map<int,int>。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();

    void reset();
    void incDungeonFailure(int dungeonIdx);
    void incServiceUsage(int serviceIdx);
    int getDungeonFailure(int dungeonIdx);
    int getServiceUSage(int serviceIdx);

    Mutex m_mutex;              // +0x00
    std::map<int, int> m_dungeonFailure;  // +0x18
    std::map<int, int> m_serviceUsage;    // +0x30
};

#include "StatisticsShusiaService/StatisticsShusiaService.inc"
}

#include "StatisticsShusiaService/dtor_StatisticsShusiaService.inc"

#include "StatisticsShusiaService/reset.inc"
}

#include "StatisticsShusiaService/incDungeonFailure.inc"
}

#include "StatisticsShusiaService/incServiceUsage.inc"
}

#include "StatisticsShusiaService/getDungeonFailure.inc"
}

#include "StatisticsShusiaService/getServiceUSage.inc"
}

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCubeStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CCubeStatistic 缺失符号批次
// 参照 docs/class_func_reports/CCubeStatistic/*.md（sendCubeStatistic.md、
// findCubeStuff.md、collectCubeStatistics.md、collectCubeStatistics_0860e07a.md）、
// _lt_global_gt_/GetInstanceCubeStatistic.md 与 ORIG objdump 逐函数核对。
// 同时补齐 SECRET_SHOP_STATISTIC_DATA ctor（该 struct 在 CSecretShop.cpp 声明，
// ORIG ctor 清零 m_data[5]，原注释称定义于 Statics/GMAccounts.cpp 有误——实际
// 该 mangled 符号属 game 构建，须在本域定义）。
// ============================================================================
#include <map>
#include <string>

#include "CCubeStatistic.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CUser.h"
#include "CEnvironment.h"
#include "Packet_Cube_Statistic.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"

// ORIG _ZN26SECRET_SHOP_STATISTIC_DATAC1Ev @ 0x85fd45a：清零 m_data[0..4]（5 int）。
struct SECRET_SHOP_STATISTIC_DATA
{
    SECRET_SHOP_STATISTIC_DATA();
    int m_data[5];   // [0]=dungeonIdx [1]=showCount [2]=showPrice
                     // [3]=buyCount [4]=buyPrice
} __attribute__((packed));

#include "CCubeStatistic/SECRET_SHOP_STATISTIC_DATA.inc"
}

// ---- getItemGroupNameTable（ORIG 0x88c0409）：__cxa_guard 单例 map ----
// findCubeStuff 拷贝本表后查 key "cube stuff"。ORIG 单例体仅默认构造空 map
//（条目由其它加载逻辑填充），此处保持一致。
std::map<std::string, int>& getItemGroupNameTable()
{
    static std::map<std::string, int> mapItemGroupTable;
    return mapItemGroupTable;
}

// ---- GetInstanceCubeStatistic（ORIG 0x860de50）：单例 ----
CCubeStatistic* GetInstanceCubeStatistic()
{
    static CCubeStatistic instance;
    return &instance;
}

// ---- CCubeStatistic 空构造（ORIG 0x86103b8，函数体仅有 ret） ----
#include "CCubeStatistic/CCubeStatistic.inc"
}

// ---- findCubeStuff（ORIG 0x860de96）：查表 "cube stuff" 值是否 == 入参 ----
#include "CCubeStatistic/findCubeStuff.inc"
}

// ---- sendCubeStatistic（ORIG 0x860e0d8）：组装 Packet_Cube_Statistic 发出 ----
#include "CCubeStatistic/sendCubeStatistic.inc"
}

// ---- collectCubeStatistics(int,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860dfb8） ----
#include "CCubeStatistic/collectCubeStatistics.inc"
}

// ---- collectCubeStatistics(CItem*,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860e07a） ----
#include "CCubeStatistic/collectCubeStatistics_1.inc"
}
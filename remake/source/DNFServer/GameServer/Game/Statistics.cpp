// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Statistics/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10StatisticsC2Ev`（ORIG 0x860ff82）。
#include <iostream>

#include <string.h>

#include "Statistics.h"
#include "MSG_STAT_RANKRES.h"   // 权威 MSG_STAT_RANKRES/GameResultType（唯一声明点）

// ---- 外部符号声明（对应 TU 翻译后移除） ----
class StatisticsShusiaService
{
public:
    void incDungeonFailure(int value);
    void incServiceUsage(int value);
};

class StatisticsPvP
{
public:
    void incPvPRoomResult(int a, int b, int c);
};

class GameWorld
{
public:
    StatisticsShusiaService* getStatisticsShusiaService();
    StatisticsPvP* getStatisticsPvP();
};

GameWorld* G_GameWorld();

// [b28c-4] ORIG 紧凑槽（Composition 0x0860b6a2：-0x15/-0x11/-0xd/-0xc，len 0xd）
// ⇒ 原码是 pack(1) 载荷结构体：4.1 对 pack(1) 紧凑排布，4.4 对独立标量按 4 对齐
// 分配槽（探针 /tmp/l4/pk.cpp C1 与 ORIG 逐条全等）。
struct StatCompositionData
{
    unsigned int a;
    unsigned int b;
    char type;
    int c;
} __attribute__((packed));

// DungeonClear/GiveUp 共用布局（ORIG 0x0860b81c/0x0860b868：-0x21/-0x1d/-0x19/
// -0x15/-0x11/-0xd/-0x9，len 0x19）——成员序 a,b,c,d,e,f,type，char 在末。
struct StatDungeonData
{
    unsigned int a;
    unsigned int b;
    int c;
    int d;
    int e;
    int f;
    char type;
} __attribute__((packed));

#include "Statistics/Statistics.inc"
#include "Statistics/dtor_Statistics.inc"

#include "Statistics/BreakJar.inc"


#include "Statistics/Composition.inc"


#include "Statistics/Decomposition.inc"


#include "Statistics/UseCoin.inc"


#include "Statistics/PvpPlay.inc"


#include "Statistics/RankResult.inc"


#include "Statistics/DungeonClear.inc"


#include "Statistics/DungeonGiveUp.inc"


#include "Statistics/DungeonFail.inc"


#include "Statistics/UseShusiaService.inc"


#include "Statistics/PVPPlay2.inc"


#include "Statistics/SendSignal.inc"

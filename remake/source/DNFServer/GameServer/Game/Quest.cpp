// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Quest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r 任务模板 Quest 实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/Quest.md（Ghidra C）+ ORIG 反汇编。
// ============================================================================
// b28d-solo-Quest 判词汇总（Quest.cpp 独占 TU，ORIG 权威地址见各行；
// 上一轮 b28d-20 lane20 判词已按本轮实测翻案/更新）：
//  [1] dungeonEventHanaseStruct::~dungeonEventHanaseStruct（定义在 Quest.h:195）
//      ORIG 0x08a61164 —— TU 口径 AE（52=52 助记符/槽位全等，仅 .o 重定位标签差）。
//  [2] Quest::check_possible 0x08352d86 —— AE（本轮：内层 if 加 else{return 0;}
//      后尾部四岛序 ret0,ret0,ret1,ret1 与 ORIG 全等；N93附 判词作废）。
//  [3] makeRewardItemInfo(pair) 0x083520c8 —— AE/IDENTICAL。
//  [4] stQuestTriggerState_t::setTrigger 0x0836dce0 —— N18a 封顶（4.1 每字段多发
//      `mov %edx,%ecx; and $0x1ff,%cx` 冗余截断，4.4 恒折单次 and；本轮再试
//      |= 形 32/41、无 cast 形 38/41、原形 38/41 ⇒ 无源码解）。
//  [5] Quest::CheckKillMonster 0x083535d6 —— N18b 封顶（enum 形参比较：ORIG
//      `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax` mem 直比，4.4 恒载寄存器
//      reg,reg；本轮 18 形探针（(int)/(enum) cast/交换/取址视图）全败）。
//  [6] Quest::get_appearmap 0x083536f2 —— AE（本轮：while(percent>=rand){return}
//      形复现 setge 物化 + %ebx 跨 call 保活；N7/P197④ 判词作废）。
//  [7] makeRewardItemInfo(RewardItemInfo) 0x08352203 —— AE/IDENTICAL。
//  [8] Quest::ConvertRewardSelectIndex 0x083537c2 —— AE（本轮：goto 尾标签复现
//      `je` 直达 return 岛 + RewardItemInfoJobView enum 视图复现 m_job eax→ebx
//      中转；P223 单源 goto 负例判词作废）。
//  [9] Quest::get_init_trigger 0x083530d6 —— AE（本轮：state 局部 aligned(4)
//      复现 4 对齐槽 -0x4c；168=168）。
//  [10] Quest::get_clear_itemlist 0x08353404 —— AE/IDENTICAL。
//  [11] Quest::set_quest 0x0835233e —— AE（本轮：QuestScript::m_field108/
//      m_field130/m_field22d bool 化 + m_powerSide uchar 化 + 循环1 it 前置声明
//      + 循环2 i/n int + trace 全表达式临时链；449=449 ndiff=0；N82 判词作废）。
// ============================================================================
#include <utility>
#include <vector>

#include "Quest.h"
#include "CUser.h"
#include "CDataManager.h"
#include "GlobalData.h"
#include "LogManager.h"

// [P2S1PvPQuest 2026-09-11 P139] ORIG prett 串直证（df_game_r）：
//   `void makeRewardItemInfo(const INT_VECTOR&, RewardItemInfoVector&)` @0x8c346e0
//   size 0x42 ⇒ 第二参拼写 = RewardItemInfoVector（GameTypes.h 未收录其名，此处新增；
//   底层 std::vector<RewardItemInfo>，mangled `...RS_I14RewardItemInfoSaIS4_EE` 唯一确定）。
//   IntPairVector/INT_VECTOR 已由 GameTypes.h（经 CUser.h/CDataManager.h）可见。
//   ⚠ 只新增本行 typedef，不改任何既有 TU 级实体/include 顺序。
typedef std::vector<RewardItemInfo> RewardItemInfoVector;

// ============================================================================
// 其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================


// 全局对象声明统一走权威头 GlobalData.h（s_DailyScheduleManager 为指针，
// ORIG 0x8a5... 调用点 `mov <A>,%eax; mov %eax,(%esp)` 装载指针值实证）。

#include "Quest/makeRewardItemInfo.inc"

#include "Quest/makeRewardItemInfo_1.inc"

#include "Quest/get_rand_int.inc"

#include "Quest/setTrigger.inc"


// ============================================================================
// Quest
// ============================================================================

#include "Quest/Quest.inc"


#include "Quest/dtor_Quest.inc"


#include "Quest/exposeQuest.inc"


#include "Quest/check_possible.inc"


#include "Quest/check_power_side.inc"


#include "Quest/checkPossibleJob.inc"


#include "Quest/_isNeededCheckGrowType.inc"


#include "Quest/checkMatchedGrowType.inc"


#include "Quest/check_clear_item.inc"


#include "Quest/CheckKillMonster.inc"


#include "Quest/check_clear_send.inc"


#include "Quest/get_init_trigger.inc"


#include "Quest/get_init_achievement_trigger.inc"


#include "Quest/get_title_reward.inc"


#include "Quest/isRepeatableQuest.inc"


#include "Quest/get_appearmap.inc"



// b28d-solo-Quest AE：ORIG 0x083537c2。
// ① [P223 正例] `if (index == count) goto return_index;` + 函数尾
//    `return_index: return i;` 复现 ORIG `cmp -0xc(%ebp),%eax; je +0x123` 直达
//    尾 return 岛（b28d-20「P223 单源 goto 尾岛负例」判词作废）。
// ② [P254/P235] ORIG 0x08353844 `mov 0x8(%eax),%eax; mov %eax,%ebx`（enum 字段
//    保留 eax→ebx 中转）⇒ 同布局 enum 视图读 m_job（RewardItemInfo 四 int）。
struct RewardItemInfoJobView
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    ENUM_CHARACTERJOB m_job;  // +0x08
    int m_growType;  // +0x0c
};

#include "Quest/ConvertRewardSelectIndex.inc"


#include "Quest/get_clear_itemlist.inc"


#include "Quest/IsOpenScheduleQuest.inc"



#include "Quest/set_quest.inc"

// ============================================================================
// 仅 ORIG 弱符号发射（DungeonClearItem，Quest.h 内联真实定义）：
//   C1/C2 0x0836de02（体=call clear）／clear 0x0836ddc2（itemIdx=0,
//   dungeonIdx=-1, rewardItem=0, count=0, percent=0, field14=-1）／
//   D1/D2 0x08a6866c（空体）。Quest.cpp 现有代码只经 vector::at/operator[]
//   拷贝或引用访问元素，未 odr-use 默认构造/clear/析构 → 弱符号不发射，
//   此处真实使用强制发射（不改语义、不造符号）。
namespace
{
#include "Quest/emitDungeonClearItemWeakSymbols.inc"

#include "Quest/emitQuestListGetWeakSymbol.inc"

}

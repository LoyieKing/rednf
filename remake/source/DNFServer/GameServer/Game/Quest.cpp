// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Quest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// 其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================


// 全局对象声明统一走权威头 GlobalData.h（s_DailyScheduleManager 为指针，
// ORIG 0x8a5... 调用点 `mov <A>,%eax; mov %eax,(%esp)` 装载指针值实证）。

// ORIG 0x8352203：src 按相邻元素配对填充 RewardItemInfo{field0,field4}；
// 奇数元素时记录错误并返回
// b28d-20 AE：it++ 后置 + cMyTrace 全表达式临时链（83=83，槽位对齐；
// 残差仅 .o 重定位立即数）。
#include "Quest/makeRewardItemInfo.inc"
}
// ORIG 0x83520c8：src 按相邻元素配对填充 std::pair<int,int>{first,second}；
// 奇数元素时 cMyTrace 记录 "[%s][%d]_it == data.rewardSelectItems_.end()"
// （0x1031 行，PRETTY __PRETTY_FUNCTION__）。
// 与 RewardItemInfo 版同 TU 相邻（0x83520c8/0x8352203），ORIG T 强符号，无调用点（导出）。
// b28d-20 AE：同上形（it++ + 全表达式临时链），83=83 槽位对齐。
#include "Quest/makeRewardItemInfo_1.inc"
}
#include "Quest/get_rand_int.inc"
#include "Quest/setTrigger.inc"
}

// ============================================================================
// Quest
// ============================================================================

#include "Quest/Quest.inc"
}

#include "Quest/dtor_Quest.inc"
}

#include "Quest/exposeQuest.inc"
}

// b28d-solo-Quest AE：ORIG 0x08352d86，162=162。尾部块序翻正形=内层 if 只保留
// 早返 `if(expertJobLevel<m_field140) return 0;`，把内层否定路径改为
// `else { return 0; }` 再 fall-through `return 1;` ⇒ 四岛序 ret0,ret0,ret1,ret1
// 与 ORIG 全等（b28d-20 N93附「四形全败」判词作废）。
#include "Quest/check_possible.inc"
}

// SIGFIX:Quest::check_power_side —— ORIG __PRETTY_FUNCTION__ 串 "bool Quest::check_power_side(char)"
// 定返回 bool（返回类型不入 mangled；P174）。
#include "Quest/check_power_side.inc"
}

#include "Quest/checkPossibleJob.inc"
}

#include "Quest/_isNeededCheckGrowType.inc"
}

#include "Quest/checkMatchedGrowType.inc"
}

#include "Quest/check_clear_item.inc"
}

// b28d-solo-Quest 判词 N18b 封顶：ORIG 0x083535d6。P223 多岛复现成功（3 个
// nop+jmp 岛 + 尾 nop，与 ORIG 0x83536a9/ac/af 逐址对应）。残 1 条 `mov`：
// ORIG `mov -0x2c(%ebp),%eax; cmp 0x18(%ebp),%eax`（mem 直比），4.4 对 enum
// 形参比较恒载寄存器 `mov -0x2c,%edx; mov 0x18(%ebp),%eax; cmp %eax,%edx`。
// 本轮 18 形探针（无 cast/(int)/(ENUM) cast/交换/取址视图/一元+ 等，4.4.7 -m32
// -O0）全败；同函数 int 形参（mapIdx/dungeonIdx/killCount）比较已 mem 直比 ⇒
// 差异根因=ENUM_QUEST_ENEMY_TYPE 全非负 ⇒ 4.4 底层 unsigned、比较走寄存器。
// [F2e-Singles2 C05② 复核 2026-09-10] 4 处比较对象全为**形参**（0xc/0x10/0x14/0x18
//   (%ebp)，含 ENUM_QUEST_ENEMY_TYPE）⇒ C05②「两个具名局部比较」命中形**不成立**
//   （边界表 ❌ 形参子型），维持 N18b（18 形探针已穷举）。our 94 vs ORIG 93（gap −1）。
#include "Quest/CheckKillMonster.inc"
}

#include "Quest/check_clear_send.inc"
}

// b28d-solo-Quest AE：ORIG 0x083530d6。`*it++` 后置形 + `{0}` 两写清零 + 拆嵌套
// if + 两处 state 局部 `__attribute__((aligned(4)))`（复现 4.1 对 8 字节聚合的
// 4 对齐槽 -0x4c/-0x54；4.4 默认按 short 自然对齐 2 落 -0x4a/-0x52）⇒ 168=168。
#include "Quest/get_init_trigger.inc"
}

#include "Quest/get_init_achievement_trigger.inc"
}

#include "Quest/get_title_reward.inc"
}

#include "Quest/isRepeatableQuest.inc"
}

// b28d-solo-Quest AE：ORIG 0x083536f2。`while (m_percent >= get_rand_int(100))
// { return m_appearMap; }` 形同时复现 4.1 的 `cmp %eax,%ebx; setge %al;
// test %al,%al; je` 条件物化与 m_percent 升格 %ebx 跨 call 保活（两处分支各一）
// ⇒ 47=47（b28d-20 N7/P197④「无源码形态」判词作废；P225/P226 边界）。
#include "Quest/get_appearmap.inc"
}


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

// 单一短路表达式 + P226 语句表达式承载条件。**真语义修正**：ORIG 循环
// 正常结束返回 i（== size()）而非 0xffffffff（早退才返回 -1），已按 ORIG 照抄。
#include "Quest/ConvertRewardSelectIndex.inc"
}

// b28d-20 AE：ORIG 0x08353404。声明序 item/out/p + pItem/randValue 循环外
// 具名（ORIG -0x2c/-0x28 清 0 实证）+ 正向 `if (is_stackable())`（消 xor $1）
// + pair 字段直赋（消 make_pair 临时）；134/134 槽位对齐。
#include "Quest/get_clear_itemlist.inc"
}

#include "Quest/IsOpenScheduleQuest.inc"
}


// b28d-solo-Quest AE：ORIG 0x0835233e，449=449 ndiff=0（p167 调用点差集 45=45）。
// 本轮翻正项：① QuestScript::m_field108/m_field130/m_field22d bool 化 +
// m_powerSide unsigned char 化（P300/档②、P265/档③，消 3 处 setne 与 1 处
// eax 中转）；② 循环1 iterator 先于 pair 声明（P28/4.4 先声明占浅槽）；③ 循环2
// n/i 改 int（P2/P209，消 jae→jge）；④ 块内 cMyTrace 改全表达式临时链（P221，
// 复现 trace 浅槽 -0x30）⇒ 上一轮 N82 封顶判词作废。
#include "Quest/set_quest.inc"
}
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
}
// QuestList::getQuestList（CDataManager.h 类内内联，ORIG W 0x08374c7a：
// 8 字节 = mov this,%eax; ret）。Quest.cpp 现有代码不 odr-use 它
// （重建访问 QuestList 均经 G_CDataManager()->m_questList 成员直访）→
// 弱符号不发射；此处真实使用强制发射（不改语义、不造符号）。
#include "Quest/emitQuestListGetWeakSymbol.inc"
}
}

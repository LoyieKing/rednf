// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkSkillGM_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWorkSkillGM_A.cpp — WongWorkA 批次 G5：CSkillChanger 追加成员 + CGMAccounts
//   独占 TU；勿在此实现 SkillSlot.cpp 已有的 10 个 CSkillChanger 符号。
// 证据来源：docs/class_func_reports/WongWork__CSkillChanger/*.md、
//           WongWork__CGMAccounts/*.md 及 objdump df_game_r（缺报告符号已标注）。
//
// 字段布局（ORIG 0x092 对象，4 组 kind 数据各 0x24）：
//   +0x00 int counts[9]        —— 用户持有 SP 书计数（m_data.m_skillBookCount）
//   +0x24 int ability[9]       —— g_nElixerAbility 表值（m_data.m_elixerAbility）
//   +0x48 int needCount[9]     —— 需求洗点书数量（union 内无具名，按偏移访问）
//   +0x6c int needAbility[9]   —— 需求洗点书能力表
//   +0x90 m_state.m_initialized —— SkillInitialize/GetCurCharacTotalSP 的 bool 入参
//                                （_addSPRelatedItem 0x0860a8a8 movzbl 0x90 读取的门控）
//   +0x91 m_state.m_reserved91  —— CountElixer 置 1 / SkillInitialize+
//                                GetCurCharacTotalSP 置 0（movb $?,0x91）
// ============================================================================

#include <string.h>

#include "SkillSlot.h"          // CSkillChanger 类定义 / CUser / NumberToString / G_CDataManager
// [Lane37CUser 2026-09-10 解环] SkillSlot.h 已把 `#include "CUser.h"` 改为前向声明
//   （解 CUser.h ⇄ SkillSlot.h 环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"              // CUser 完整定义（本 TU 解引用 CUser 成员）
#include "Quest.h"              // Quest（+0xb8 字段）/ RewardItemInfo 完整型
#include "QuestClear.h"         // WongWork::CQuestClear::enumQuestClear 回调链
#include "CDataManager.h"       // find_quest / get_level_exp
#include "CMyFileLog.h"         // AppendGM_Sys 日志
#include "CGuildServerProxy.h"  // WongWork::CGMAccounts / stGMInfo_t 权威声明

namespace WongWork
{

// ORIG T 0x0810933c loadGMAccounts(char const*)：恒返回 1。
#include "WongWorkSkillGM_A/loadGMAccounts.inc"
}

// ============================================================================
// ORIG T 0x08609d10 CheckCondition(CUser const*)
// exp < get_level_exp(level) || get_level_exp(level+1) < exp → 不可执行
// ============================================================================
#include "WongWorkSkillGM_A/CheckCondition.inc"
}
// ============================================================================
// ORIG T 0x08609d9c CountElixer(CUser*)：重置四组数据（8/组首）、置就绪标志，
// 统计用户物品并应用双树 kind 数据。
// ============================================================================
#include "WongWorkSkillGM_A/CountElixer.inc"
}

// ORIG W 0x0869780e GetElixerCount()：返回第一组计数数组（即 this）。
#include "WongWorkSkillGM_A/GetElixerCount.inc"
}

// ORIG W 0x08697816 GetNeedElixerCount()：返回 this+0x48。
#include "WongWorkSkillGM_A/GetNeedElixerCount.inc"
}

// ORIG W 0x08697822 GetNeedElixerAbility()：返回 this+0x6c。
#include "WongWorkSkillGM_A/GetNeedElixerAbility.inc"
}

// ============================================================================
// ORIG T 0x0860a868 _addSPRelatedItem(int*,int*,int,int)
// 0x407 → *a += count；0x40e → *b += count；
// 其余且 initialized：命中 SP 书索引时 needCount[i] += count、
// needAbility[i] = g_nElixerAbility[i]（直接赋值，与 ORIG store 一致）。
// ============================================================================
#include "WongWorkSkillGM_A/_addSPRelatedItem.inc"
}

// ============================================================================
// ORIG T 0x0860a918 _addSFPRelatedItem(int*,int*,int,int)
// 0x4b4 → *a += count；0x4b5 → *b += count；其余无操作。
// ============================================================================
#include "WongWorkSkillGM_A/_addSFPRelatedItem.inc"
}

// ============================================================================
// ORIG T 0x0860a6da static _fnEnumQuestClear(uint,char*)：enumQuestClear 回调。
// va 打包区依次：int& spA,int& spB,int& sfpA,int& sfpB,this,user。
// 查任务；仅限 m_fieldB8 == 0 或 25（一般/重复类任务）统计其奖励物品；
// SP 书（0x407/0x40e 与表格项）与 SFP 书（0x4b4/0x4b5）分别累计。恒返回 true。
// [A28 round5 定稿] 本轮修：rewards 预清两句式（ORIG 0x0860a781
//   `movl $0x0,-0x14(%ebp)` 后才 call，P198①）。
//   残余判 N107+N82（gap -2，p167 调用点一致）：
//   ① N107：ORIG 六组 `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)`
//      （2 条）vs 4.4 恒 `addl $0x4,0xc(%ebp)`（1 条）。本轮实测四形全产 addl：
//      `args += 4` / `args = args + 4` / `args = &args[4]` /
//      `(char*)((int)args+4)`（该形多 1 条重载）/ `(char*)((int**)args+1)`。
//   ② N82：迭代器槽 it/end ORIG -0x34/-0x38 vs 4.4 -0x3c/-0x34（具名 it 形、
//      for-init 直构形两种均实测同槽）。
// ============================================================================
#include "WongWorkSkillGM_A/_fnEnumQuestClear.inc"
}

// ============================================================================
// ORIG T 0x0860a680 _CountQuestSPBook(const CUser*,int&,int&,int&,int&)
// 经 UserQuest+0x04 的 CQuestClear 子对象遍历已完成任务并回调统计。
// ============================================================================
#include "WongWorkSkillGM_A/_CountQuestSPBook.inc"
}

// ============================================================================
// CGMAccounts 三缺失成员
// ============================================================================

// ORIG T 0x081093ee appendGM(unsigned,unsigned)：恒返回 0。
#include "WongWorkSkillGM_A/appendGM.inc"
}

// ORIG T 0x08109524 AppendGM_Sys(unsigned,char)：入列 {mid,(int)(signed char)}
// 并记 "./log/Init" 文件日志 "GM List Add mid:%s"。
#include "WongWorkSkillGM_A/AppendGM_Sys.inc"
}

}  // namespace WongWork

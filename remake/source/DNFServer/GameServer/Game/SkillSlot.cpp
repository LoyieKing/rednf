// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SkillSlot/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 玩家技能槽 SkillSlot 还原（G1 玩家域第一批）。
// 逐函数对照 docs/class_func_reports/SkillSlot.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
#include <string.h>

#include <utility>

#include "SkillSlot.h"
#include "CSkill.h"
#include "LogManager.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "CCharacter.h"
#include "STExpertJobScript.h"
#include "PacketGuard.h"
#include "CInventory.h"

// ---- _Charac_info +0x29 低 4 位有符号位域（ORIG：movzbl; shl $4; sar $4,%al）----
struct _Charac_info_nibble_field
{
    unsigned char m_pad[0x29];
    signed char m_specialNibble : 4;
    signed char m_padNibble : 4;
};

// ---- [b28d-solo-SkillSlot] STGuildDBInfoOnly 尾段组件视图（P161）----
// ORIG master_new_guild_skill 0x0860387e 段把 `m_skills + i*5` 折成
// `lea base+i*5; add $0x40; mov 0x5(%eax)`（int 读）/ `add $0x49`（字节读）：
// 把 +0x40 处建模为 Section（内含 +0x05 起的 5B 条目数组），4.4-O0 即发同形。
// 纯 POD 局部视图（无 ctor/方法），不发射符号；sizeof/偏移断言见下。
struct STGuildSkillEntry5
{
    int m_skillIdx;             // +0x00
    char m_level;               // +0x04
} __attribute__((packed));
struct STGuildSkillSection
{
    char m_pad[5];              // +0x00..+0x04（Section 起点 +0x40）
    STGuildSkillEntry5 m_entries[16];   // +0x05..+0x54
} __attribute__((packed));
struct STGuildDBTailView
{
    char m_pad[0x40];
    STGuildSkillSection m_section;
} __attribute__((packed));
static_assert(sizeof(STGuildSkillEntry5) == 5, "STGuildSkillEntry5");
static_assert(sizeof(STGuildDBTailView) == 0x40 + 5 + 0x50, "STGuildDBTailView");

#include "SkillSlot/set_skill_slot.inc"
}

// ---- 依赖的全局函数（真实定义在本 TU，ORIG 符号）----
// ORIG T 0x8604fe2：创建角色时按 job 追加默认技能。
// 逻辑（对照 0x8604fe2 反汇编）：
//   job ∉ [0,10] → 直接返回；从 G_CDataManager()->m_characList + job*0x7dc
//   + 0x240 拷贝该职业默认技能表（vector<pair<int,int>>，pair = 技能idx/等级）；
//   逐条 find_skill(job, idx)；CSkill 为主动技能时优先写 [0,6) 空槽，
//   否则（含主动技能无空槽）按 get_group() 落入对应区间
//   （g0:[6,0x36) g1:[0x36,0x66) g2:[0x66,0x96) g3:[0x96,0xc6)），
//   在区间内写第一个空槽；其余 group 值打 cMyTrace 日志。
#include "SkillSlot/addSkillOnCreateCharacter.inc"
}
// ORIG 0x88923f1：skill>0x95→0；否则 (level+flag)*(skill*10)/10
#include "SkillSlot/getSkillChecksum.inc"
}
// ORIG T 0x8892445
// ORIG T 0x8892445（0x26 字节，objdump 转写：报告缺失）
// if (sum == 0) return 0; *sum ^= key; return *sum;
#include "SkillSlot/finishSkillChecksum.inc"
}
// ORIG T _Z22getSkillSpendSPOnStealii（0x8a9afa2，0x53 字节）：
// f = penalty / 100.0f（.rodata 0x8e2a180 = 0x42c80000）；r = (int)(sp * f)
// （fistpl，控制字 chop 截断向零）；返回 std::max(1, r)。
#include "SkillSlot/getSkillSpendSPOnSteal.inc"
}

#include "SkillSlot/_Mastered_skill.inc"
}

#include "SkillSlot/stSPReturnInfo_t.inc"
}

// ==================== 构造 / 析构 / 拷贝（ORIG 0x08694d6a 族） ====================

#include "SkillSlot/SkillSlot.inc"
}

#include "SkillSlot/dtor_SkillSlot.inc"
}

// NOTE(主 agent 协调)：ORIG 的 SkillSlot 拷贝构造/赋值是编译器隐式生成
// （weak 符号 0x86996f6/0x869d91e，逐字节 AE 已由影子验证确认）。显式定义
// 已在 SkillSlot.cpp 删除；需同步从 SkillSlot.h 移除声明（L241/L243）并移除
// m_pad9 成员（ORIG 该处为自然 padding，隐式拷贝不复制），方可最终 identical。

#include "SkillSlot/set_parent.inc"
}

#include "SkillSlot/set_remain_sp.inc"
}

#include "SkillSlot/get_remain_sp.inc"
}

// ==================== 简单查询 ====================

// 【P210】FindSkill(kind) 定义前置：调用者须在 get_skillslot_buf 定义之前，
// 否则 4.4 按 callee 帧精确继承 outgoing，0x28 保守大帧塌缩为 0x18。
#include "SkillSlot/FindSkill.inc"
}

#include "SkillSlot/get_skillslot_buf.inc"
}

#include "SkillSlot/get_cur_remain_sp.inc"
}

#include "SkillSlot/get_temp_remain_sp.inc"
}

#include "SkillSlot/get_temp_remain_sfp.inc"
}

#include "SkillSlot/get_temp_skill_slot.inc"
}

#include "SkillSlot/GetSkillTreeIndex.inc"
}

#include "SkillSlot/IsLoadStrikerSkill.inc"
}

#include "SkillSlot/SetLoadStrikerSkill.inc"
}

#include "SkillSlot/IsEmptySkillTree.inc"
}

#include "SkillSlot/get_skill_count.inc"
}

#include "SkillSlot/get_skill_index.inc"
}

#include "SkillSlot/get_skill_level.inc"
}

#include "SkillSlot/get_skillslot_group.inc"
}

#include "SkillSlot/getSkillSlotIndex.inc"
}

#include "SkillSlot/get_remain_sp_at_index.inc"
}

#include "SkillSlot/get_remain_sfp_at_index.inc"
}

#include "SkillSlot/set_remain_sp_at_index.inc"
}

#include "SkillSlot/set_remain_sfp_at_index.inc"
}

#include "SkillSlot/get_skill_slot.inc"
}

#include "SkillSlot/get_skill_slot_at_index.inc"
}

#include "SkillSlot/set_skill_slot_at_index.inc"
}

#include "SkillSlot/FindSkill_1.inc"
}

// [b28d-solo-SkillSlot] int→bool（头声明同步）：ORIG check_skill_list 调用点
// 0x08604d2a `call; xor $0x1,%eax; test %al,%al; je` = P292/P248 档①。
#include "SkillSlot/check_skill_mastered.inc"
}

#include "SkillSlot/check_skill_mastered_1.inc"
}

#include "SkillSlot/get_skillslot_no.inc"
}

/* b28d-36（P210 前置法）：本函数 ORIG 帧 0x24（callee get_skillslot_no 已见定义、
 * outgoing 精确继承）；callee 定义后置时 4.4 撑保守帧 0x38、slot 槽 -0xc vs
 * ORIG -0x4。把 get_skillslot_no(_Mastered_skill*,...) 定义前置到本函数之前即
 * 逐条复现（NEAR→AE）。代价：callee 自身 .o 层 IDENT→AE（本地跳转偏移巧合丢失，
 * clean 口径仍等价）。 */
#include "SkillSlot/check_skill_mastered_2.inc"
}

#include "SkillSlot/get_skillslot_no_1.inc"
}

#include "SkillSlot/checkComboSkillInsertQuickSlot.inc"
}

#include "SkillSlot/checkMoveComboSkillSlot.inc"
}

#include "SkillSlot/insert_skill.inc"
}

#include "SkillSlot/insert_skill_1.inc"
}

#include "SkillSlot/insert_skill_with_slot_index.inc"
}

#include "SkillSlot/delete_skill.inc"
}

#include "SkillSlot/clear_all_skills.inc"
}

#include "SkillSlot/clear_all_skills_both.inc"
}

#include "SkillSlot/clear_sfp_skills.inc"
}

#include "SkillSlot/backupSkill.inc"
}

#include "SkillSlot/initialize_sfp_skill.inc"
}

#include "SkillSlot/makeCheckSum.inc"
}

#include "SkillSlot/growtype_skill.inc"
}

#include "SkillSlot/skill_move.inc"
}

#include "SkillSlot/check_skill_list.inc"
}

#include "SkillSlot/calcReturnSpSfpPerOneSkill.inc"
}

#include "SkillSlot/calcUsedSP.inc"
}

#include "SkillSlot/get_give_skill.inc"
}

#include "SkillSlot/master_new_skill.inc"
}

#include "SkillSlot/try_master_new_skills.inc"
}

#include "SkillSlot/master_new_guild_skill.inc"
}

#include "SkillSlot/refund_skill.inc"
}

#include "SkillSlot/try_refund_skill.inc"
}

#include "SkillSlot/InitSkillHistoryLog.inc"
}

#include "SkillSlot/SendSkillInfo.inc"
}

#include "SkillSlot/checkskillSlotEmptyCount.inc"
}

// ==== SkillSlot::checkOverLevelSkill @ 0x08605d2a（报告 docs/class_func_reports/SkillSlot/checkOverLevelSkill.md）====
// 检查超等级技能：对每槽技能计算当前等级可达到的最大技能等级（含偷学/超限加成），
// 超出部分返回 SP/SFP 并写回；返回 1 表示有技能被降级/补发。
#include "SkillSlot/checkOverLevelSkill.inc"
}

#include "SkillSlot/debugCheckGrowTypeSkill.inc"
}

#include "SkillSlot/debugCommandResetSkill.inc"
}

#include "SkillSlot/loadComboSkill.inc"
}

#include "SkillSlot/getComboSkillSaveData.inc"
}

#include "SkillSlot/getComboSkillSendInfo.inc"
}

#include "SkillSlot/setComboSkillInfo.inc"
}

#include "SkillSlot/resetComboSkill.inc"
}

#include "SkillSlot/verifyComboSkill.inc"
}

#include "SkillSlot/verifyComboSkillTree.inc"
}

#include "SkillSlot/eraseDuplicationComboSKill.inc"
}

#include "SkillSlot/eraseDuplicationComboSKillTree.inc"
}

#include "SkillSlot/moveComboSkillExtensionQuickSlot.inc"
}

#include "SkillSlot/pullComboSkillExtensionQuickSlot.inc"
}


// ==== SkillSlot::InitSkill @ 0x08608120（报告 docs/class_func_reports/SkillSlot/InitSkill.md）====
// PvP 树频道：givePvPSkillTree + 脚本点数 set_remain_sp_at_index(+日志)。
// 普通树：建号技能补发 → get_give_skill 表 → growtype_skill 循环 →
// ExpertJob 增益技循环 → CSkillChanger::SkillInitialize → 日志/发包。
#include "SkillSlot/InitSkill.inc"
}

namespace WongWork
{
// ORIG D 0x093acdc0：{8,0x40f,0x410,0x411,0x412,0x413,0x414,0x415,0x416}（[0]=计数）
int CSkillChanger::g_nStatUpItemIdx[9] = {8, 0x40f, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416};
// ORIG D 0x093ace00：{8,4,6,5,7,2,3,8,9}（[0]=计数，其余为对应能力值）
int CSkillChanger::g_nElixerAbility[9] = {8, 4, 6, 5, 7, 2, 3, 8, 9};

#include "SkillSlot/CSkillChanger.inc"
#include "SkillSlot/dtor_CSkillChanger.inc"

#include "SkillSlot/_ResetSkillPoint.inc"
}

#include "SkillSlot/_ResetSFPoint.inc"
}

// [b23S3C] ORIG 0x0860a34a 尾 `leave; ret` 无 mov $0,%eax ⇒ 返回 void；
// getCurCharacInvenR/CargoR 结果 ORIG 直落 0x4(%esp) 实参槽（无 -off 往返）⇒ P48 去具名局部；
// 第一段 ORIG 先 Inven_Item ctor(-0x4d) 再 sret 临时(-0xd8)+逐字段赋值 ⇒ 拆 ctor/赋值两句
// （初始化形被 copy-elision 直构进 item 槽，丢 ctor call + 16 条拷贝 = gap 30 的真因）
#include "SkillSlot/_GetUserItemCount.inc"
}

// ORIG 0x0860a634：累加 GetSpAtLevelUp(1..charLevel)。
#include "SkillSlot/_GetBaseSkillPoint.inc"
}

// ORIG 0x0860a6d0：固定返回 0。
#include "SkillSlot/_GetQuestSkillPoint.inc"
}

// ORIG 0x0860a1e6：GetCurCharacTotalSP(CUser const*, bool)。
#include "SkillSlot/GetCurCharacTotalSP.inc"
}

// ORIG 0x0860a950 真实空实现（仅 push/mov/pop/ret，无逻辑）。
#include "SkillSlot/_ApplyUserData.inc"
}

#include "SkillSlot/_CountSkillBook.inc"
}

#include "SkillSlot/SkillInitialize.inc"
}
}

// ---- SkillSlot 拷贝构造/赋值（ORIG W 弱符号：拷贝构造 0x086996f6 /
// operator= 0x0869d91e，均为编译器隐式生成形态——标量逐字段 movzbl +
// ComboSkill C1ERKS/aSERKS + 三组 _Mastered_skill[204] 逐元素循环 + 尾部标量）。
// 本 TU 无值语义调用点，GCC -O0 不实例化隐式成员 → 匿名命名空间 odr-use
// 强制生成两个弱符号，保证真实成员拷贝可用（P74：ORIG 弱拷贝优先隐式生成）。
namespace
{
#include "SkillSlot/SkillSlotCopyForceUse.inc"
}
}

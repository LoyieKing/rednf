// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W7_LinkTailGrowth/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W7_LinkTailGrowth.cpp —— W7G_LinkTail：CUser::decide_growth_power_reward_system
// ORIG T 0x0868d780 _ZN5CUser33decide_power_reward_system（完整 mangled：
//   _ZN5CUser33decide_growth_power_reward_systemEv），报告
//   docs/class_func_reports/CUser/decide_growth_power_reward_system.md 全文转写。
// 报告中"文本级别名"已用 nm 核出真实符号：
//   080da2b8 W CUserCharacInfo::get_charac_level() const
//   0822fb00 W CUserCharacInfo::get_growth_power_reward() const
//   0822fb50 W CUserCharacInfo::set_growth_power_exp_reward_right(bool)
//   080cc19b W G_CDataManager()
//   080cb8e6/8fc/920/958 W InterfacePacketBuf::{clear,put_header,put_byte,finalize}
//   vptr+0x34 槽 = CEventBase::IsEventing(CUser*) const（slot 13）。
// ============================================================================
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"      // CEventBase / CEventManager 权威声明
#include "CDataManager.h"           // G_CDataManager / m_seriaBlessing / m_growthPower
#include "STExpReward.h"
#include "PacketGuard.h"

// [b28d-solo-W7_LinkTailGrowth 判词·DIFF/N104+N7] 语义正确：事件(0x5d)开启→
//   IsEventing 双调用→seria is_possible_lv→set_growth_power_exp_reward_right(true)
//   +breakAway section 判定；breakAway||seria→PacketGuard(0x178,3 byte)+Send；
//   未开启→breakAway 判定→同包(byte 1,1)+set_growth_power_exp_reward_right(true)。
//   本轮修：① 第二 IsEventing+is_possible_lv 两条件改 P226 语句表达式（复现 ORIG
//   +0x8b..+0x9b 两 `je` 同址 0x816 的共享短路物化岛）；② 尾部
//   set_growth_power_exp_reward(get_charac_level()) 拆两份物理独立块（P165，ORIG
//   +0x1ed/+0x32a 各一份）。修后 231/231 指令数全等、帧 0x40 与全部槽号一致、
//   p167 调用点差集 42/42 CLEAR；唯一残差 = 第二分支 PacketGuard::~D1 的相对序
//   rotate（ORIG 正常路径 D1 在 `jmp +0x32a` 之前；4.4 恒把该 D1 挪到 unwind
//   landing 之后，块内容逐条全等）⇒ N104 封顶；另 N7 虚调用目标寄存器
//   %eax↔%edx（ORIG %edx；P230=4.4.6 后端可修，路由表不动）。
#include "W7_LinkTailGrowth/decide_growth_power_reward_system.inc"
}

// ============================================================================
// set_growth_power_exp_reward 的依赖 producer（此前 repo 无 owner；
// 引用方 decide_growth_power_reward_system 强依赖，随本批一并落地）。
// ============================================================================

// stSeriaBlessingScript::get_value(int, Value&) const
// ORIG W 0x8693ad6：FindOneByLv(lv)（C1 0x822aebe / operator() 0x822aecc）
// 谓词 std::find_if 遍历 m_map0；谓词条件 = (m_minLv==m_maxLv && m_minLv==lv)
// 或 (m_minLv<=lv && lv<m_maxLv)。命中把 Value 五 int 拷入 out 返回 true。
// ============================================================================
#include "W7_LinkTailGrowth/get_value.inc"
}

// ============================================================================
// STGrowthPowerData::get_idx_break_away_section(int)（ORIG 非 const，mangled 无 K）
// ORIG W 0x822b136：遍历 m_vec0，首个满足 (unsigned char)m_field1 <= lv 且
// lv < (unsigned char)m_field2 的下标；未命中返回 -1。
// [b28d-solo-W7_LinkTailGrowth 判词·AE] ORIG 0x0822b136（52/52）：加具名
// `const STBreakAwaySection& sec = *it;`（ORIG de() 每轮一次落 -0xc 槽、两字段从槽重读，
// P0 槽往返 ⇒ 原码有具名局部）＋ 把 `&&` 拆成嵌套 if（ORIG 两守卫 `jg`/`jle` 同落
// 循环增量块，P133/P225 推广；合并形被 4.4 物化成 setcc 双物化）后与 ORIG 全等；
// p167 调用点差集 CLEAR。
// ============================================================================
#include "W7_LinkTailGrowth/get_idx_break_away_section.inc"
}

// ============================================================================
// CUser::set_growth_power_exp_reward(int)
// ORIG T 0x0868dace _ZN5CUser27set_growth_power_exp_rewardEi：
// [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x0868dace（111/111 指令数全等，
// p167 调用点差集 14/14 CLEAR）：① `&&` 条件改 P226 语句表达式，复现 ORIG
// +0x6d..+0x8f 的短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）；
// ② Value→m_reward 的字节写改 STExpReward union 具名视图（§4.3，删裸 char* 指针视图）；
// ③ 下标比较改有符号 `idx < (int)sz`（ORIG +0xf3 cmpl/jge，无符号版产 jae=P196 隐患）。
// 残差：N7 虚调用目标寄存器 %eax↔%edx（ORIG %edx；P230 判据=4.4.6 后端可翻，
// 路由表不改，故 4.4.7 封顶）＋ 尾部对齐 nop（§6.1 豁免）＋ `m_reward.m_reward=`
// 段 4.4 RHS-first vs ORIG LHS-first（P211 档③ packed 字段侧旋钮；STExpReward.h
// 换型会波及全消费点，按边界封顶）。callee is_growth_power_exp_reward_right
// 返回类型 unsigned char→bool 由 Main 代落（P248 档①，ORIG 调用点 xor $1 实证），
// 两处 xor 已归位。
//   ev = s_event_manager->GetRepeatEvent(0x5d);
//   if (ev->IsEventing(0))：                                  // +0x09..+0xb0
//     Value value;                                            // ctor 0x8693aa0
//     if (m_seriaBlessing.get_value(level,value) &&
//         !is_growth_power_exp_reward_right())
//       写 m_expReward 字节 [+2]=value.m_fieldC、[+3]=value.m_field10、
//                          [+1]=value.m_field8                // 0x8d24f/50/4e
//     else memset(&m_reward, 0, 4);                        // 0x8d24d
//   else：                                                    // +0xe7..+0x15f
//     idx = m_growthPower.get_idx_break_away_section(level);
//     sz = m_vecC.size();                                     // dm+0x871c
//     if (idx >= 0 && idx < sz && !is_growth_power_exp_reward_right())
//       m_reward = m_vecC[idx].m_reward;          // 0x8d24d int 写
//     else memset(&m_reward, 0, 4);
// ============================================================================
#include "W7_LinkTailGrowth/set_growth_power_exp_reward.inc"
}

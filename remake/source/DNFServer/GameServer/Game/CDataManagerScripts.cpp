// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDataManagerScripts/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CBoosterGage.h"  // BoosterGageData 权威布局（须先于 CDataManager.h）
#include "CDataManager.h"
#include "cMyTraceNoop.h"  // add_lottery_needMoney 命中 trace（ORIG 0x83650f5）
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include "DNFLexWrapper.h"   // ScanType/ScanInt/ScanStr/ScanFloat/loadRDARScriptFile/
                             // unloadRDARScriptFile（RDAR 脚本扫描接口）
#include "Statuepredicate.h"  // pieceQuestRewardStatus 合并谓词（getRewardStatus）
#include "CDungeon.h"        // advancealtar::{StageDifficulty,RewardItemType}
                             // （importAchievementRewardParameter 解析用）
#include <cstring>

// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 0x088bbd77 T（定义 STScriptFileList.cpp；DNFLexWrapper.h 未含此声明）。
#include "CDataManagerScripts/loadRDARScriptListFile.inc"
#include "CDataManagerScripts/IS_PVP_CHANNEL.inc"
#include <algorithm>     // std::find（CWorldMap::hasDungeon）

// [b28d-solo-CDataManagerScripts-6 Main 盲区排查] 本 TU **无任何 goto 语句**（grep 实证，
// 命中全为判词注释）⇒「`goto scan_exit` 落进 `if(!ScanType){...}` 体内导致退出循环」
// 的模式结构上不适用；全部 continue/break 均位于循环体内（C++ 语义保证回边/退出正确）。
// jmp/jcc 目标方向（回边 B / 前跳 F）逐位核对（objdump 提 (addr,op,target) 分类）：
// createStackableLotteryInfo(AE) 6/6、get_break_away_section_data(AE) 4/4、
// getRewardStatus(NEAR) 12/12 全 0 mismatch；两 import 大件（DIFF，非 AE 假匹配）
// 方向错位全部由「ORIG 每 continue 独立 `nop;jmp 循环头` 岛 vs 4.4 tail-merge 共享回边」
// 解释，回边语义一致。
// [b28d-solo-CDataManagerScripts-3] dual_rank gap 榜首位（gap 15）复核结论：
// `_ZNSt12_Vector_baseIN12advancealtar9FieldType1TESaIS2_EED1Ev` 为 libstdc++ 头
// 内模板弱符号，本 TU 副本尺寸 0x67 与 ORIG 0x67 **逐条全等**（含 4.1 的
// `_M_deallocate` 后 EH landing pad）；链接后二进制取到的是
// advancealtar_misc.cpp.o 的 0x43 副本（该 TU 走 446 路由头集，dtor 无 landing），
// 属弱符号多副本链接选择差异（P93/N89 类），非本 TU 源码可修。

// [A16 round5 复核] 本 TU 9 件非 identical 全数复核（fresh check_tu_routed 19:17 +
//   p167 调用点差集）：**9 件全部 CALLSITE CLEAR**（差集仅 C1/C2·D1/D2 同址别名
//   P93 伪差）⇒ 语义缺口 0，残差全为跨版本 codegen 形态。逐件判词（均附 ORIG 地址）：
//   · importAchievementRewardParameter 0x0889afb0（gap -12）：N82 帧/槽分区（§6.2 三件套）。
//   · importAdvanceAltarShopParameter 0x088a10cc（gap -6）：**本波已修 P196 符号性**
//     —— 3 处下标循环 `for (size_t fi = 0; fi < v.size(); ++fi)`（`size_t` ⇒ 无符号
//     比较 ⇒ `seta`）改为 `for (int fi = 0; fi < (int)v.size(); ++fi)`，复现 ORIG
//     三处 `cmp -0x48/-0x18/-0xc(%ebp),%eax; setg %al`（**有符号**；ORIG 0x088a156f 段
//     等）⇒ 实测 OUR setg ×3 = ORIG，`seta` 归零（P196：signed/unsigned 语义分叉）。
//     残余 = 我方多 `sete`×3 / `setne`×1（ORIG 直跳形）+ N82 帧/槽分区（gap 不变）。
//   · AddRewardItemInfo 0x0890cfda（gap -2）：N107 —— ORIG 0x0890d04f
//     `lea (%ecx,%eax,1),%eax; 0x0890d052 lea 0xc(%eax),%edx; mov val,(%edx)`，
//     我方 `lea (%ecx,%eax,1),%edx; mov val,0xc(%edx)`（4.4 恒折 disp；下标形与
//     指针算术形两写法实测同形，判词原文见函数头）。
//   · GetRewardItem 0x0890d326（gap +14）：N107/N7 —— ORIG 单条 scaled
//     `mov 0xc(%eax,%ecx,1),%eax`，我方 `lea(%ecx,%eax,1); add $0xc` ×2 + 多
//     push %esi 与 3-pop epilogue（寄存器角色族，P333 边界外的地址形成差）。
//   · reseal_table_t::get_reseal_cost 0x08900b02（gap +3）：P209 附（unsigned 比较
//     恒载寄存器；S1 一次性旋钮已用 —— idx 改 unsigned 实测劣化并回滚）+ N18a
//     （`mov %eax,%ecx` 单段 regmove 不可造）。
//   · _getIntegratedPvPItemAttr 0x084fc541（gap +3）：P9 —— callee GetReSealCount
//     返回 uchar ⇒ 4.4 调用点恒插 `movzbl %al,%eax` ×2（ORIG 无）；改共享头返回
//     类型须连动 Inven_Item.cpp body 与全调用点，保留。
//   · isBoundaryExpValue 0x0849f420（NEAR，gap +1）：N22/N18b —— ORIG
//     `cmp %eax,%ebx`（dst=局部 e）vs 4.4 `cmp %ebx,%eax`（dst=字段/call 侧）；
//     交换源码操作数实测同形、P248 三档试尽；尾 nop 属 §6.1 对齐豁免。
//   · getRewardStatus 0x08a6be66（NEAR，gap +1）：N126/N95 —— 全帧槽号恒浅 0xc
//     （版本首槽基址常数，P332「永封部分」）+ 1 nop 豁免。
//   · getPvPSkillPointNext 0x08a5df0e（gap -1）：N93/N112 —— 槽号整体平移 4
//     （jcc 30/30 无极性反写）。
//   [A16 round5 · C05 四范式复查留痕（fresh 19:38-19:39）] 本 TU 9 件机械扫描：
//   ① F2（ORIG `cmp -N(%ebp),%eax` + setcc）命中 **0** 处 ⇒ 无 P209 反向可用形；
//   ② 尾 fail 块（尾 `mov $0/$1,%eax`）命中 **0** 处；
//   ③ 岛差：importAchievementRewardParameter / importAdvanceAltarShopParameter 的
//      ORIG 岛目标重复（0x889b05f×9；0x88a12ab×4 等 8 目标各 2~3 次）⇒ C05③ 负例
//      （4.4 合并/跳板不可控），封 N；其余 7 件 ORIG/我方岛数相等（0~1，已对上）；
//   ④ 数据区解码假象本 TU 未命中。⇒ 无 C05 可落地件，维持 N 判词。
//   [补·本波新增两轴复核（Main 配方：cv 限定符 / 循环符号性）]
//   轴A `const` 常量传播：本 TU 9 件 ORIG/OUR 的 `cmpl $imm,-N(%ebp)` 计数**全等**
//      （0/0 … 2/2），ORIG 段 `cmp -N(%ebp),%e` 槽读形 0 处 ⇒ 无「我方多折叠」件；
//   轴B 循环符号性：**已命中并修**（importAdvanceAltarShopParameter 3 处，见上），
//      其余 8 件 setcc 符号序与 ORIG 一致（扫描数据见本块上位判词）。
//   [补·Main 细化口径] `test` 零比较子型亦复核：本 TU 9 件 ORIG 段
//   `mov -N(%ebp),%reg; test %reg,%reg` 对**全 0 处**；我方 `cmpl $0x0,-N(%ebp)`
//   2 处（_getIntegratedPvPItemAttr / importAdvanceAltarShopParameter）两侧同形
//   ⇒ 无 `unsigned`→`int` 可翻转点，勿重试。

// ============================================================================
// 脚本 POD 生命周期（ORIG 语义，见 docs/class_func_reports 与 ORIG 反汇编）。
// 各 ctor 构造 STL 容器成员（必要时调 clear()），dtor 逆序析构。
// ============================================================================

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}

#include "CDataManagerScripts/AdvanceAltarShopParameter.inc"
}

#include "CDataManagerScripts/QuestParameterScript.inc"
}
#include "CDataManagerScripts/dtor_QuestParameterScript.inc"

#include "CDataManagerScripts/clear.inc"
}

// ============================================================================
// QuestParameterScript 奖励段查询（ORIG T 0x08a6bdc6 / 0x08a6be66）。
// getRewardLevel：遍历 m_map3c（map<int,int>），取首个 second<=level 的
// first（初始 0）；getRewardStatus：getRewardLevel(level) 后遍历 m_map54 中
// key<=level 的各 vector，按 Statuepredicate（+0x0/+0x4 两 int）合并：
// 命中段累加 m_value（+0x8 float），否则 push_back（sret 返回）。
// ============================================================================
#include "CDataManagerScripts/getRewardLevel.inc"
}

// [b28d-solo-CDataManagerScripts] NEAR：p167 CLEAR（28/28）—— 本轮修两处：
// ①section 按值拷贝（ORIG 0x8a6bee3 `operator->(it)→+4→vector C1ERKS(-0x54)` 实证，
//   补 MISSING C1ERKS×1 + D1Ev×2）；②`(*found).m_value += (*sit).m_value` 的
//   operator*（deEv）形（P167：MISSING deEv×2 / EXTRA ptEv×2）。帧 0x8c=ORIG；
//   残余仅槽号文本差（-0x3c vs ORIG -0x48 族，N86/N93 分配策略）；ORIG 0x08a6be66。
#include "CDataManagerScripts/getRewardStatus.inc"
}

// ---- GuildParameterScript（ORIG C1 0x837058a / clear 0x83704ae / D1 0x837d542）----
#include "CDataManagerScripts/GuildParameterScript.inc"
}
#include "CDataManagerScripts/dtor_GuildParameterScript.inc"
#include "CDataManagerScripts/clear_1.inc"
}

// ORIG C1 0x8372480：仅构造 +0x08 map 后调 clear()（+0x1c call；+0/+4 由 clear 内清零）。
#include "CDataManagerScripts/AccountCargoScript.inc"
}
#include "CDataManagerScripts/dtor_AccountCargoScript.inc"
#include "CDataManagerScripts/PvPSkillTreeParameterScript.inc"
}
#include "CDataManagerScripts/dtor_PvPSkillTreeParameterScript.inc"

#include "CDataManagerScripts/clear_2.inc"
}
// ============================================================================
// getPvPSkillPointNext（ORIG T 0x08a5df0e，const）：
// m_map18[grade] 与 m_map18[grade-1] 中 (job*5+firstGrow) 的投入点数差，
// 再减 m_map30[(job,firstGrow)] 的 m_pointCost。secondGrow 不参与
// （ORIG 写入 0 后仅用于 key 第三字节，恒 0）。
// ============================================================================
// [b28d-solo-CDataManagerScripts-2] 真 bug 修正：ORIG 0x08a5df66 `je +0xde` ⇒
// m_map18 无 grade 时直接 `mov $0,%eax; jmp end` **返回 0**（Ghidra：`if (==) local_10=0
// else { 全部计算 }`）；旧实现仍算 prevPoints/cost 并返回 `0-prevPoints-cost`（负值）。
// 带参 `PvPSkillTreeKey key(job,firstGrow,secondGrow)` + `secondGrow=0` 写回形参槽保留
// （ORIG C1Ehhh 0x08a5e768）。
// [b28d-solo-CDataManagerScripts-3] P220/P47 块序修正：ORIG 0x08a5df5f 调
// `_Rb_tree_const_iterator::ne`（operator!=）后 `test %al; je 0x08a5dfec`——return-0
// 块（`mov $0,%eax; jmp leave`）位于函数**中部**、主体直落尾；旧 `if (it == end) return 0;`
// 早退形在 4.4 下把 return-0 块排到主体之前（块序相反）。改正极包裹
// `if (gradeIt != m_map18.end()) { …; return …; } return 0;` 后布局与 ORIG 同向。
// 残余 gap 1 + 槽号整体平移 4（gradeIt -0x38 vs ORIG -0x34；P112/N93 块级槽向）；ORIG 0x08a5df0e。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 183 / ours 182；jcc 30/30
// （序列错位源于块序/岛，无极性反写）⇒ 维持 N93/N112。
#include "CDataManagerScripts/getPvPSkillPointNext.inc"
}

// ---- IndependentDropParameterScript（ORIG C1 0x8372d46 / clear 0x8372dfa / D1 0x837daac）----
#include "CDataManagerScripts/IndependentDropParameterScript.inc"
}
#include "CDataManagerScripts/dtor_IndependentDropParameterScript.inc"
#include "CDataManagerScripts/clear_3.inc"
}

// ---- stMercenarySystemInfo（ORIG C1 0x8373328 / D1 0x837db42）----
#include "CDataManagerScripts/stMercenarySystemInfo.inc"
}
#include "CDataManagerScripts/dtor_stMercenarySystemInfo.inc"
#include "CDataManagerScripts/channel_script_t.inc"
#include "CDataManagerScripts/dtor_channel_script_t.inc"
#include "CDataManagerScripts/stBroadCastItemScript.inc"
}
#include "CDataManagerScripts/dtor_stBroadCastItemScript.inc"
#include "CDataManagerScripts/isUnCommonItem.inc"
}

// ORIG checkCommonItem 0x8918cdc：遍历 m_vec0 找 itemIdx
// [b28d-solo-CDataManagerScripts-2] DIFF→：原判「dead-bool XOR 哨兵非源码可表达」
// 被探针推翻——ORIG 的 `bool b` 哨兵在 4.4 下逐条可复现（/tmp/probe_cdms9.cpp k2：
// 帧 0x38、入口 `cmpb $0,-0xd; jne ret0`、槽 -0xd/-0x18/-0x1c/-0xc/-0x14 与 ORIG 全等）。
// 关键形 = 空 then + else 主体（`if(b){}else{…}`），既得入口直跳又无 goto 尾岛。
// 语义等价：b 初值 false，仅在条件下 `^=1`（0→1）且内层置回 0 ⇒ 守卫恒真，
// 遍历与命中返 true 的原语义不变。ORIG 0x08918cdc。
#include "CDataManagerScripts/checkCommonItem.inc"
}

// ORIG checkUnCommonNestedItem 0x8918dfe：m_mapC.find(itemIdx) 命中后遍历其子 vector 找 subItemIdx
// [b28d-solo-CDataManagerScripts-2] DIFF→：同 checkCommonItem 的 dead-bool XOR 哨兵已证
// 可复现（空 then + else 主体 + `it != e && (b ^= 1, b)` + 内层 `while(b)` + `&(*it)` 缓存）。
// 语义等价（b 恒真守卫）；p167 CLEAR（10/10）保持。ORIG 0x08918dfe。
#include "CDataManagerScripts/checkUnCommonNestedItem.inc"
}

// [b28d-solo-CDataManagerScripts] AE：map C1 + CSlotBoundChecker C1（+0x18 真实内嵌成员）+ clear；fresh check_tu_routed clean（39=39）；ORIG 0x083738a2。
#include "CDataManagerScripts/eventReward.inc"
}
// [b28d-solo-CDataManagerScripts] AE：map D1 + CSlotBoundChecker D1（+0x18 真实内嵌成员，ORIG 0x0837dfb2 调 _ZN17CSlotBoundCheckerD1Ev）；fresh check_tu_routed clean（27=27）；ORIG 0x0837dfb2。
#include "CDataManagerScripts/dtor_eventReward.inc"
}

// ---- ConditionLevelChkDungeon / FairPvPStatScript / RegenerationROI ----
// ORIG C1 0x837c9ac / D1 0x837c9c0（本 TU 唯一 owner）：仅构造/析构 +0x00 的
// vector<int>；objdump 直转，标量成员无动作。
#include "CDataManagerScripts/ConditionLevelChkDungeon.inc"
}
#include "CDataManagerScripts/dtor_ConditionLevelChkDungeon.inc"
#include "CDataManagerScripts/FairPvPStatScript.inc"
}
#include "CDataManagerScripts/dtor_FairPvPStatScript.inc"
#include "CDataManagerScripts/clear_4.inc"
#include "CDataManagerScripts/RandomOptionItem_Regenerator.inc"
}

#include "CDataManagerScripts/RegenerationROI.inc"
}
#include "CDataManagerScripts/dtor_RegenerationROI.inc"
}
// ORIG C1/D1：map<int,Value> 默认构造/析构（原 pad 占位时的 ZERO_POD 已随真实布局移除）。
#include "CDataManagerScripts/stSeriaBlessingScript.inc"
#include "CDataManagerScripts/dtor_stSeriaBlessingScript.inc"
#include "CDataManagerScripts/is_possible_lv.inc"
}

// ORIG C1 0x83724f0 / FindCharacStat 0x8693c66：map.find(CharacterKind(job,grow))，
// 命中返回 &it->second，未命中返回 0（迭代器 != end 判定）。
#include "CDataManagerScripts/FindCharacStat.inc"
}

// ORIG W 0x822b07e get_break_away_section_data(level)：sret；
// 返回最后一个 elem.m_field1 <= level 的段拷贝（memmove 3 字节；
// 一旦 m_field1>level 即退出，未命中返回清零段）。
#include "CDataManagerScripts/get_break_away_section_data.inc"
}

// ORIG W 0x822afd6 get_break_away_section_low_level(lv)：返回最后一个
// m_field1<=lv 段的 m_field1；首个 m_field1>lv 即返回 -1（循环内直返）。
#include "CDataManagerScripts/get_break_away_section_low_level.inc"
}

// ORIG C1 0x8371194 / 0x83714c6：map 构造后 clear()。
#include "CDataManagerScripts/stLevelUpRewardItemScript.inc"
}
#include "CDataManagerScripts/dtor_stLevelUpRewardItemScript.inc"
#include "CDataManagerScripts/stStepRewardStackableItemScript.inc"
}
#include "CDataManagerScripts/dtor_stStepRewardStackableItemScript.inc"
#include "CDataManagerScripts/stGrowthCapsuleScript.inc"
}
#include "CDataManagerScripts/CLottery_NeedMoney.inc"
#include "CDataManagerScripts/dtor_CLottery_NeedMoney.inc"

#include "CDataManagerScripts/isNeedMoney.inc"
}

#include "CDataManagerScripts/add_lottery_needMoney.inc"
}

#include "CDataManagerScripts/getChannelInfo.inc"
}

#include "CDataManagerScripts/GetPvPChannelNumber.inc"
}

// [b28d-solo-CDataManagerScripts] AE：①拆两个独立 null 守卫（P133，删 || 合并短路）；②删 vector 引用缓存（P183 逐点重读 info->m_pBonusRates）；③size_t i 头部前置 ⇒ info/-0xc、i/-0x10 与 ORIG 对齐；ORIG 0x088dce3c。
#include "CDataManagerScripts/getBonusRate.inc"
}

// ORIG T 0x08581680：hash_map 命中返回 CNPCScript*，未命中返回 0。
#include "CDataManagerScripts/find.inc"
}

// ORIG _ZNK14CNPCScriptList16getMaxFavorValueEv W 0x8582364：
// movzwl 0x14(%eax)（m_npcCommon.m_maxFavorExp）+ movzwl %ax 零扩展，int 返回。
// ORIG _ZNK14CNPCScriptList13getFavorLevelEt 0x85816e4：favorExp 低于上限时按
// m_favorExp[1..3] 阈值表（ushort 无符号比较）返回 1..3，否则 4。
// 【P210】定义前置：调用者须在 getMaxFavorValue 定义之前，否则 4.4 按
// callee 帧精确继承 outgoing，0x34 保守大帧塌缩。
#include "CDataManagerScripts/getFavorLevel.inc"
}

// ORIG _ZNK14CNPCScriptList18getFavorLevelValueEj 0x858174e：level 0..3 返
// m_favorExp[level]（level 0 返 0），越界（>3）返 getMaxFavorValue()。
// [b28d-solo-CDataManagerScripts] AE：①返回类型 unsigned short（ORIG 0x0858177d
// 单条 `movzwl` 直返；int 返回 4.4 恒多一条 `movzwl %ax,%eax`，P9/探针
// /tmp/b28d_s160_p1.cpp 实证）；②本定义置于 getMaxFavorValue 之前（P210：callee
// 定义在后 ⇒ 4.4 走保守 `sub $0x18`，与 ORIG 0x08581751 一致）；ORIG 0x0858174e。
#include "CDataManagerScripts/getFavorLevelValue.inc"
}

#include "CDataManagerScripts/getMaxFavorValue.inc"
}

// ORIG _destroy 0x8581784：遍历 hash_map delete 各 CNPCScript*（调 D1 + operator delete），
// 再 clear()。
// [b28d-solo-CDataManagerScripts] AE：①缓存 end 转换构造 `const_iterator e(m_npcMap.end())`（P13）；②无条件 `delete it->second`（P141 删显式判空，消 -0xc 槽）⇒ 帧/槽与 ORIG 对齐；ORIG 0x08581784。
#include "CDataManagerScripts/_destroy.inc"
}

#include "CDataManagerScripts/dtor_CNPCScriptList.inc"
}

namespace WongWork
{
#include "CDataManagerScripts/dtor_CItemGeneratorMgr.inc"
#include "CDataManagerScripts/setDropRatio.inc"
}
// getGenerator 已由 CItemGeneratorMgr.cpp 真实定义，不重复。
}

#include "CDataManagerScripts/getItemGenerator.inc"
}
// get_event_script_mng 已由 CDataManager.cpp 定义，不重复。
#undef ZERO_POD

// ============================================================================
// AccountCargoScript 查询（ORIG 0x88c8056 / 0x88c80ba / 0x88c8124）。
// 全部基于 +0x8 std::map<int, UpgradeInfo>：
//   GetCreateInfo：begin() 的 value（首个升级档），空 map 返回 0；
//   GetCurrUpgradeInfo(idx)：find(idx) 的 value，未找到返回 0；
//   GetNextUpgradeInfo(idx)：find(idx) 的下一项 value，无下一项返回 0。
// 返回 void* 以匹配 CAccountCargo.cpp 的 asm 别名调用。
// ============================================================================
#include "CDataManagerScripts/GetCreateInfo.inc"
}

#include "CDataManagerScripts/GetCurrUpgradeInfo.inc"
}

#include "CDataManagerScripts/GetNextUpgradeInfo.inc"
}

// ============================================================================
// SecretShopScript::GetNpcIdx / GetNpcByDungeonIdx / GetNpcByDungeonLev
// （ORIG 0x8a82998 / 0x8a828f8 / 0x8a8293c）。
// GetNpcIdx：遍历 equal_range 区间，累加 stDungeonNpc::m_rate（+0x8），
// 首次累计 > target 时输出 m_npcIdx（+0x4）并返回 1，遍历完返回 0。
// ============================================================================
#include "CDataManagerScripts/GetNpcIdx.inc"
}

#include "CDataManagerScripts/GetNpcByDungeonIdx.inc"
}

#include "CDataManagerScripts/GetNpcByDungeonLev.inc"
}

// ============================================================================
// DeleteInvalidItemScript 生命周期与查询。
// C1/D1（0x8ac1d40/0x8ac1dce）：构造 string +0x0/string +0x4/map +0x8 后
// clearScript()；IsInvalidItem2Delete（0x8ac2096）：map.find(idx) != end。
// ============================================================================
#include "CDataManagerScripts/DeleteInvalidItemScript.inc"
}
// ORIG D1 0x8ac1dce W：clearScript() 后 string +0x0/+0x4 与 map +0x8 由编译器按成员序析构
// （P12 真实析构，非空桩——成员析构仍生成）。
#include "CDataManagerScripts/dtor_DeleteInvalidItemScript.inc"
}

// ORIG 0x08ac2066 T：清 +0x00/+0x04 两 string 与 +0x8 map（loadScript 前置）。
#include "CDataManagerScripts/clearScript.inc"
}

#include "CDataManagerScripts/IsInvalidItem2Delete.inc"
}

// ORIG 0x8ac20f8：map.find(idx)；命中 → rewardIdx=data.m_field0、rewardCnt=data.m_field4。
#include "CDataManagerScripts/GetRewardItemInfo.inc"
}

// ORIG 0x8ac2180：返回脚本 +0x00/+0x04 两字符串（发件人名/正文模板）。
#include "CDataManagerScripts/GetMailInfo.inc"
}
// ============================================================================
// createStackableLotteryInfo（ORIG 0x89bacc1）：与 createLotteryInfo 同型，
// 但前两元素写入 m_defaultIdx/m_defaultCount，之后每三元素为一组
// (item, count, weight) 写入 m_items/m_counts。
// ============================================================================
// [b28d-19] AE（P28 后置 it++ 修复：p167 MISSING operator++(int) 归零，130=130）；ORIG 0x089bacc1。
#include "CDataManagerScripts/createStackableLotteryInfo.inc"
}

// ============================================================================
// createGlobalEffectInfo（ORIG 0x89bae94）：源为 int 三元组
// (field0, field4, rate*10000)；size 非 3 的倍数返回 0。修正了原桩错误的
// vector<char>& 形参（ORIG mangled 为 vector<stGlobalEffect_t>&）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①CStackableItem.h stGlobalEffect_t m_field4→float / m_field8→int（R2 广播，sizeof/偏移/mangled 不变）；②赋值序 field0→field4(fstps)→field8 + 后置 `*it++` ⇒ 逐条对齐；ORIG 0x089bae94。
#include "CDataManagerScripts/createGlobalEffectInfo.inc"
}

// ============================================================================
// SetLightServerFlag / GetLightServerFlag（ORIG 0x8a8cba5 / 0x822ad3a）。
// ORIG 全局标志位于 0x940b558（.bss）；SetLightServerFlag 经
// GetLightServerFlag() 取指针后写入 bool。
// ============================================================================

// probe 实证（/tmp/wk/probe12.cpp）：调用点若见 GetLightServerFlag 的"定义"，
// 4.4 将 caller 帧收紧为 0x14/spill -0x14；只见原型则 pad[12] 生效得
// ORIG 帧 0x28/spill -0x1c/p -0xc（0x8a8cba8-0x8a8cbbe）⇒ 定义置于调用点之后。
bool* GetLightServerFlag();
#include "CDataManagerScripts/SetLightServerFlag.inc"
}

bool* GetLightServerFlag()
{
    static bool light_server_flag = false;  // ORIG _ZZ18GetLightServerFlagvE17light_server_flag V @0x940b558
	return &light_server_flag;
}
// stLimitItemUsageInfoEx 权威定义在 CDataManager.h（含 ctor/dtor 声明），
// 此处不再重复定义。
// ============================================================================
// _checkTimeoutItem（ORIG 0x867cf8d T，G3 任务域辅助）
// CTitleBook 通过 _checkTimeoutItem 直接调用：检查物品是否过期。
// ORIG 逻辑（逐步核对 0x867cf8d 反汇编）：
//   虚表 +0x10（IsAvatarItem，@4）非 0 → 0；
//   虚表 +0x48（PeriodAfterUnsealing，@18）非 0 且 item.m_field0 != 0 → 1；
//   虚表 +0xc（GetItemType，@3）== 0xa 时：expirationDate <= now → 1；
//   否则（其他类型）：item.m_fieldb * 86400 + 0x44a54a80 <= now → 1；
//   其余 → 0。
//   注：ORIG vtable（0x8c96da8）实证 @4=IsAvatarItem、@18=PeriodAfterUnsealing、
//   @19=IsRandomOption（NEW 原实现误用 IsCreatureItem/IsRandomOption，已纠正）。
// [b28d-solo-CDataManagerScripts] AE：①P52 链式 find_item（实参先物化 %ebx）；②P226 条件向语句表达式复现短路物化岛（`mov $1/$0 + test/je`，零槽）；③真语义 bug 修正：`PeriodAfterUnsealing() && m_field0!=0` 路径 ORIG 0x0867d03c 走 +0xb1 `mov $0,%eax` = 返回 **0**（原实现误写 return 1）；fresh clean 101=101；ORIG 0x0867cf8d。
#include "CDataManagerScripts/_checkTimeoutItem.inc"
}

// ============================================================================
// _getIntegratedPvPItemAttr（ORIG T 0x84fc541，G3 PvP 物品属性）
// ORIG 反汇编（84fc541）：
//   it = G_CDataManager()->find_item(item.m_addInfo)；it==0 → 0；
//   虚表 +0xc（GetItemType）== 0xa → item.GetReSealCount() << 5；
//   0xb < GetItemType() <= 0x15 → item.GetReSealCount() << 5；
//   其余 → item.GetItemAttr()。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（5/5）。P9 —— GetReSealCount/GetItemAttr 返回 uchar（ORIG body 0x822b456 `movzbl 0x6(%eax),%eax; shr $0x5,%al` 实证），4.4 调用点恒插 `movzbl %al,%eax`（ORIG 无）；改共享头返回类型会动 Inven_Item.cpp body 与本 TU 全部调用点，保留；ORIG 0x084fc541。
#include "CDataManagerScripts/_getIntegratedPvPItemAttr.inc"
}

// ============================================================================
// GetIntegratedPvPItemAttr（ORIG T 0x84fc5ff）
// ORIG 反汇编（84fc5ff）：
//   G_GameWorld()->IsIntegratedPvPBaseChannel() 且
//   G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0 → _getIntegratedPvPItemAttr(item)
//   否则 → item.GetItemAttr()
// ============================================================================
// ORIG 0x84fc5ff：返回类型为 byte（函数体只经最后 call 的 %eax/%al 透传，
//   else 分支 `call GetItemAttr; leave; ret` 无 movzbl 扩宽）。声明 unsigned char
//   ⇒ 两分支均不透传扩展（true 支 int 直落、else 支 byte 无扩），byte 全等 ORIG。
//   调用方（CAccountCargo/CCargo/CTitleBook）均 `(int)Get...(item)&0xff`，低字节一致。
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        return _getIntegratedPvPItemAttr(item);
    }
    return item.GetItemAttr();
}

// ============================================================================
// GameStubs.cpp 迁移：纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// 权威声明见 CDataManager.h（seria/growth/create/use/rank/death/blood/village/
// questShop/WorldDrop/Slang/Invalid/GrowthPower/Booster/AvatarColor 等）、
// CStackableItem.h（RecipeInfo/MultiBoxLotteryInfo）、CEquipItem.h（RandomItemTable）、
// CItemList.h（BaseItemKey/BaseItemValue）。
// ============================================================================

// ---- RecipeInfo（CStackableItem.h；ORIG C1 0x8513c6a / clear 0x8513bf2 / D1 0x85144f4）----
#include "CDataManagerScripts/RecipeInfo.inc"
}
#include "CDataManagerScripts/dtor_RecipeInfo.inc"
#include "CDataManagerScripts/clear_5.inc"
}

#include "CDataManagerScripts/changeOutput.inc"
}

// ---- RandomItemTable（CEquipItem.h；ORIG W 0x8515468 / 0x8514b46）----
#include "CDataManagerScripts/RandomItemTable.inc"
#include "CDataManagerScripts/dtor_RandomItemTable.inc"
#include "CDataManagerScripts/BaseItemKey.inc"
}
// [b28d-Main·检查点21] BaseItemValue ctor 已删（ORIG 无符号，POD）。

// ---- seriaRoomDecoEventScript（ORIG C1 0x8372ef4：构造 vector@0、+0xc=0、
//      map@+0x10、vector@+0x28、+0x34=0、+0x38=0 后三容器各自 clear）----
#include "CDataManagerScripts/seriaRoomDecoEventScript.inc"
}
#include "CDataManagerScripts/dtor_seriaRoomDecoEventScript.inc"
#include "CDataManagerScripts/growthEquipmentScript.inc"
}
#include "CDataManagerScripts/dtor_growthEquipmentScript.inc"
#include "CDataManagerScripts/createChracScript.inc"
#include "CDataManagerScripts/dtor_createChracScript.inc"
#include "CDataManagerScripts/rewardInfoList.inc"
}
// ORIG C1 0x8373098：map 构造后 clear()。
#include "CDataManagerScripts/useCharacterJobFatigue.inc"
}
#include "CDataManagerScripts/dtor_useCharacterJobFatigue.inc"
#include "CDataManagerScripts/CharacterCreateCreatureGiftScript.inc"
}
#include "CDataManagerScripts/dtor_CharacterCreateCreatureGiftScript.inc"
#include "CDataManagerScripts/seriaRoomDecoAniEventScript.inc"
#include "CDataManagerScripts/dtor_seriaRoomDecoAniEventScript.inc"

#include "CDataManagerScripts/stDungeonRankSystem_t.inc"
}
#include "CDataManagerScripts/dtor_stDungeonRankSystem_t.inc"
#include "CDataManagerScripts/clear_6.inc"
}

// ORIG C1 0x8370eba：构造子对象 + 4 容器后调 clear()（clear 内写 +0x74）。
#include "CDataManagerScripts/stRankSystemInfo.inc"
}
#include "CDataManagerScripts/dtor_stRankSystemInfo.inc"
#include "CDataManagerScripts/clear_7.inc"
}

// ---- stRankSystemInfo 嵌套子类（ORIG W 符号）----
// stDungeonClearTimeData：C1 0x0891b6f2（调 clear）/ clear 0x0891b706
// （memset(this,0,0x1c)）/ D1 0x0893fbc6（空体）。
#include "CDataManagerScripts/stDungeonClearTimeData.inc"
}
#include "CDataManagerScripts/clear_8.inc"
}
// [b28d-Main 代落·S334 P317] 空析构定义删除（声明端同步）。

// stComboIntervalBoundary：C1 0x0891b72a / clear 0x0891b73e
// （memset(this,0,0x10)）/ D1 0x08940176（空体）。
#include "CDataManagerScripts/stComboIntervalBoundary.inc"
}
#include "CDataManagerScripts/clear_9.inc"
}
// [b28d-Main 代落·S334 P317] 空析构定义删除（声明端同步）。

// ---- stBloodDungeon_t（ORIG C1 0x8370a3a / clear 0x8370b02 / D1 0x8370ab0）----
#include "CDataManagerScripts/stBloodDungeon_t.inc"
}
#include "CDataManagerScripts/dtor_stBloodDungeon_t.inc"
#include "CDataManagerScripts/clear_10.inc"
}
// stVillageAttacked_t 现为纯 vector 包装（CDataManager.h），ctor/dtor 隐式生成
// （ORIG W 0x837cf1e/0x837cf32 即隐式符号），无显式定义。

// ---- stDeathTower_t（ORIG C1 0x08370970）----

// ORIG C1 0x8370970：四个 200 元素表初始化 + 尾部字段，非全量 memset。
// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i < 200; ++i)` 复现 ORIG 两段寄存器尾测；fresh clean 45=45；ORIG 0x08370970。
#include "CDataManagerScripts/stDeathTower_t.inc"
}

// live_server_info_t ctor ORIG 0x0837dd64：仅 map 成员构造（自动）。
#include "CDataManagerScripts/live_server_info_t.inc"
#include "CDataManagerScripts/dtor_live_server_info_t.inc"

#include "CDataManagerScripts/stQuestShop.inc"
}
#include "CDataManagerScripts/dtor_stQuestShop.inc"
#include "CDataManagerScripts/InvalidCharData.inc"
}
#include "CDataManagerScripts/dtor_InvalidCharData.inc"
#include "CDataManagerScripts/SlangNameData.inc"
}
#include "CDataManagerScripts/dtor_SlangNameData.inc"
#include "CDataManagerScripts/STGrowthPowerData.inc"
}
#include "CDataManagerScripts/dtor_STGrowthPowerData.inc"
#include "CDataManagerScripts/AvatarColorInfo.inc"
}
#include "CDataManagerScripts/dtor_AvatarColorInfo.inc"
#include "CDataManagerScripts/BoosterGageData.inc"
}
#include "CDataManagerScripts/dtor_BoosterGageData.inc"
#include "CDataManagerScripts/reseal_table_t.inc"
}
#include "CDataManagerScripts/dtor_reseal_table_t.inc"
#include "CDataManagerScripts/get_reseal_cost.inc"
}

// ORIG get_oneshot_reseal_cost @0x08900be0（T）。见头声明注释。
#include "CDataManagerScripts/get_oneshot_reseal_cost.inc"
}

// ---- InGameAdvertisementScript（本 TU 唯一 owner；InitScriptProducers_7 重复已删）----
// ORIG C1 0x83732f0：仅调 clear()。
#include "CDataManagerScripts/InGameAdvertisementScript.inc"
}

// ORIG clear 0x8373304：memset(this, 1, 0x97)——前 0x97 字节置 1，尾字节保持原值。
#include "CDataManagerScripts/clear_11.inc"
}

// ---- PcBangItemRentarData（ORIG D1 0x837c2c0）----
#include "CDataManagerScripts/PcBangItemRentarData.inc"
}
#include "CDataManagerScripts/dtor_PcBangItemRentarData.inc"
#include "CDataManagerScripts/DimensionActivationData.inc"
}
#include "CDataManagerScripts/dtor_DimensionActivationData.inc"
#include "CDataManagerScripts/stUnlimitChallengeInfo.inc"
}
#include "CDataManagerScripts/dtor_stUnlimitChallengeInfo.inc"
#include "CDataManagerScripts/stLimitItemUsageInfoEx.inc"
}
#include "CDataManagerScripts/dtor_stLimitItemUsageInfoEx.inc"
#include "CDataManagerScripts/CNPCRelationEventManager.inc"
}
// ORIG 0x890ce34：遍历 delete 节点后直接析构 map（无显式 clear()）。
#include "CDataManagerScripts/dtor_CNPCRelationEventManager.inc"
}

// ---- CNPCRelationEventManager::AddNPCInfo（ORIG 0x890ceea：
//      make_pair(idx, info) 后 map insert）----
#include "CDataManagerScripts/AddNPCInfo.inc"
}

// ---- CNPCRelationEventManager::AddFavorItemInfo（ORIG 0x890cf44：
//      find(idx)，非 end 时将 {itemId,count} push_back 到 node+kind*0x14
//      即 m_favor[kind].m_items；步长 0x14 为 stNPCFavorAndRewardInfo）----
#include "CDataManagerScripts/AddFavorItemInfo.inc"
}

// ---- CNPCRelationEventManager::AddRewardItemInfo（ORIG 0x890cfda：
//      find(idx)，非 end 时写 node+kind*0x14+0xc / +0x10 两奖励值）----
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（4/4），gap=2、P=1（gap'=4）。
// 本轮复核：`(node->m_favor + kind)->m_rewardA/B` 数组名算术形（P214①）在 4.4 下
// 产出 `lea (%ecx,%eax,1),%edx; mov val,0xc(%edx)`（折 disp），ORIG 为两步
// `lea (%ecx,%eax,1),%eax; lea 0xc(%eax),%edx; mov val,(%edx)`（0x0890d052/0x0890d06d）；
// 另实参装载序（ORIG kind 先于 node）与 %ebx 角色差。属 N107（add-vs-lea 折 disp）
// + N7/N76（装载序）封顶；ORIG 0x0890cfda。
#include "CDataManagerScripts/AddRewardItemInfo.inc"
}

// ---- CNPCRelationEventManager::AddDislikeItemInfo（ORIG 0x890d07a：
//      find(idx)，非 end 时向 node+0x3c 的 vector<int> push_back）----
// [b28d-solo-CDataManagerScripts] AE：删具名 `int item`（P0 右值直传 push_back(itemId) 的 const& 临时由编译器生成）⇒ it/end 槽 -0x14/-0x10 与 ORIG 对齐；ORIG 0x0890d07a。
#include "CDataManagerScripts/AddDislikeItemInfo.inc"
}

// ---- CNPCRelationEventManager::GetFavorIndex（ORIG 0x890d3d0：
//      strncmp 前 10 字节分派 [good]/[soso]/[bad]→0/1/2，其余 3）----
#include "CDataManagerScripts/GetFavorIndex.inc"
}

// ============================================================================
// IsDislikeItem（ORIG T 0x0890d100）：m_npcEventMap.find(idx) 未命中 → 0；
// node->m_dislikeItems（+0x3c vector<int>）任一元素 == itemId → 1。
// ============================================================================
#include "CDataManagerScripts/IsDislikeItem.inc"
}

// ============================================================================
// GetItemKind（ORIG T 0x0890d1c0，sret stTempRewardInfo*）：
// m_npcEventMap.find(idx) 未命中 → 默认值；遍历 node->m_favor[kind].m_items
// （kind=0..2，步长 0x14）：item.m_itemId==itemId 且 item.m_count<=count：
//   count==0 → 默认值（不写）；
//   否则 → out.m_kind=kind、out.m_count=ROUND(count/item.m_count)。
// [N类·跨版本豁免] 已还原 ORIG 真形（our 113 vs 112）：残 1 指令差为 gcc4.1/4.4 的
// outgoing-frame / 死栈槽产物（范式库『跨版本』），助记符序列已对齐；语义完全一致。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：p167 CLEAR（7/7）。本轮两处翻正：
// ①`stNPCEventItemInfo::m_count` int→unsigned int（CDataManager.h，P254 档③）——
//   复现 ORIG 0x0890d2a8 `mov $0,%edx; mov %eax,-0x38; mov %edx,-0x34; fildll`
//   的无符号零扩展（旧 int 字段产 `fildl`）；②相等比较按 ORIG 操作数序写
//   `itemId == (unsigned int)item.m_itemId` ⇒ 复现 `mov -0x24(%ebp),%eax;
//   cmp 0x14(%ebp),%eax`（旧写法多一条形参装载）。帧/全部槽号与 ORIG 逐一对齐；ORIG 0x0890d1c0。
#include "CDataManagerScripts/GetItemKind.inc"
}

// ORIG C1 0x837cf46：仅构造 +0x4/+0x10/+0x1c/+0x28 四 vector，+0x00 不初始化。
#include "CDataManagerScripts/stCharacLinkSystem_t.inc"
}
#include "CDataManagerScripts/dtor_stCharacLinkSystem_t.inc"
#include "CDataManagerScripts/GetRewardItem.inc"
}

// ---- advancealtar（StageTimeLineParameter D1 0x8899e88 /
//      AdvanceAltarShopParameter D1 0x88a0fa2：析构 map 成员；
//      RewardParameter 清 pad；ConfigParameter 按 ORIG 0x889b662）----
#include "CDataManagerScripts/StageTimeLineParameter.inc"
}
#include "CDataManagerScripts/dtor_StageTimeLineParameter.inc"
#include "CDataManagerScripts/dtor_AdvanceAltarShopParameter.inc"
#include "CDataManagerScripts/BuyUpgradeData.inc"
}
#include "CDataManagerScripts/RewardParameter.inc"
}
#include "CDataManagerScripts/dtor_RewardParameter.inc"
#include "CDataManagerScripts/BuyShopData.inc"
}
#include "CDataManagerScripts/ConfigParameter.inc"
}
#include "CDataManagerScripts/dtor_ConfigParameter.inc"
#include "CDataManagerScripts/reset.inc"
}

// ============================================================================
// eventReward::importRewardTable（ORIG 0x080ef29c T）
// 列表装载失败返 0x77；逐文件 ImportRewardScript 失败返 0x71；成功返 0。
// ORIG STScriptFileList 即 +0 的 map<int,string>（NpcScript.h 占位版同尺寸），
// 故直接以本地 map 重解释传入，避免与本 TU 已含头中的占位定义冲突。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①P214① 正极性包裹；②真实 STScriptFileList 局部（ORIG 调 _ZN16STScriptFileListC1Ev；原 map+reinterpret_cast 调错 C1 符号且槽深 4）；③P13 缓存 end（ORIG 循环前调一次 end 存 -0x28）；④`!ImportRewardScript(...)` 复现 ORIG `test %eax,%eax; je` 直分支；fresh clean 82=82；ORIG 0x080ef29c。
#include "CDataManagerScripts/importRewardTable.inc"
}

// ============================================================================
// DeleteInvalidItemScript::loadScript（ORIG 0x08ac1e60 T）
// tw_DeleteInvalidItem.etc：[delete item] 三元组 (idx, rewardIdx, count)
// 以 idx 为 key、{idx,rewardIdx,count} 为值 insert；[send mail title]/
// [send mail content] 分别 ScanStr 进 +0/+4 字符串，ScanStr 失败直接结束；
// ScanType 耗尽正常结束返回 1。
// ============================================================================
#include "CDataManagerScripts/DeleteInvalidItemData.inc"
}
#include "CDataManagerScripts/init.inc"
}

// [b28d-solo-CDataManagerScripts] AE：函数头声明序 `token, ok, idx, data`（P212/P309：4.4 头部声明按先声明浅 ⇒ data 落最深 -0x50、idx -0x44，与 ORIG 一致）；fresh clean 149=149；ORIG 0x08ac1e60。
#include "CDataManagerScripts/loadScript.inc"
}

// ============================================================================
// advancealtar::ConfigParameter::importConfigParameter（ORIG 0x0889b6cc T）
// 逐段还原 ORIG ## 汇编 + ## 反编译 C：
//  · [gage up rate] 行 (idx, P, Q)：0<=idx<=3 时 this+8(idx*4)=Q(m_gageUpRate)、
//    this+0x1c(idx*4)=P(m_gageUpAux)（ORIG 0x0889b742/0x0889b757：+8 存第 3 个
//    ScanInt=local_18，+0x1c 存第 2 个=local_1c，与直觉相反，原实现写反已修）；
//    循环带一个 ORIG dead 计数 local_24（0..每行 ++，从不读）。P 失败→0x3e9、
//    Q 失败→0x3ec。
//  · [gage upgrade cost percent]：(float)(long double)ScanFloat 存 m_4(+4)，失败 0x3f8；
//    [daily refill ticket]：ScanInt 存 m_0(+0)，失败 0x3ff。
//  · 三个开标签均不匹配时（ORIG 最内 else 0x0889b84f..）短路比对
//    [/gage up rate]、[/gage upgrade cost percent]、[/daily refill ticket]（结果丢弃）。
//    原实现缺这三条比较 → we-smaller 主因。成功/文件打开失败均返 0（非零=Fail）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（17/17）。本轮加 P232
// `char framePad[4]`（首声明）⇒ 帧 sub $0x40=ORIG、token -0x28 / ok -0x29 /
// lineNo -0x20 / idx -0x1c / p -0x18 / q -0x14 / fv -0x10 / tv -0xc 全部与 ORIG 对齐；
// 残余 = 尾部闭标签 6 条（ORIG `test %al,%al; jne <循环头岛>` 正极性 vs 4.4
// `test; je 下一块; jmp 岛` 反极性——continue/嵌套包裹/goto 尾标签三形实测均不翻，
// P222/P223 边界）；ORIG 0x0889b6cc。
#include "CDataManagerScripts/importConfigParameter.inc"
}

// ============================================================================
// advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter
// （ORIG 0x088a10cc T，恒返 0）
// 语义（ORIG 汇编逐段还原，见 importAdvanceAltarShopParameter.md ## 汇编/## 反编译 C）：
//  · [star reset fee gold] 段：每轮先 ctor _StarRestFeeByGold(-0x238)，key=ScanInt，
//    fee=ScanInt，任一失败仅结束本段回外层扫描头；成功 insert(key,fee)。
//  · 每个 [ridable id] 块：ridableId=ScanInt；!ok 或 !isValidRidableId ⇒ bVar4=假
//    ⇒ 不插入直接返 0（原实现误无条件插入，已修正）。valid 时 shop.m_nField0=id，
//    进 LAB_088a12ab 段循环，[unit]/[skill]/[tower] 各自内联展开三份独立解析块。
//  · item [id] 子块字段序：unit/tower=[unlock stage index],[name],[field name],[list];
//    skill 额外在 unlock 与 name 间插 [is settable slot]。
//  · 关键：push_back(item) 绑定在 [list] 处理尾（非 [/id]），故 item 收集入
//    shop.m_vecT0/T1/T2 由 [list] 触发；[list] 逐行读入 item.m_listMap(key=FieldType-0
//    行 m_value)，直到某行 ScanStr/ScanInt 失败。[/ridable id] 或 EOF ⇒ 插入 shop。
// [b28c-18-unbind] 32 处 bool-callee 检查点的 register 绑定已全域清除（P0 硬约束），
// 统一落 N96+N18a+P168a④ 判词；xor 物化形为跨版本寄存器分配产物，源码不可复现。
// 语义已全量还原为 ORIG 三段内联真形（our_insns
// 708→1403→1431 vs ORIG 1441，gap 727→38→10）。残留 10 指令差为 gcc 4.1.2 相对
// 4.4.7 的 EH landing-pad 与 BuyShopData/BuyUpgradeData 双份 dtor epilogue vs merge、
// deferred-write 寄存器轮换（技巧24 落点岛/技巧31 dead 撑帧无法从源码复现）
// （范式库「负例 N 类·跨版本：delete[]+成员 dtor EH cleanup / EH 双份 dtor epilogue vs
// merge」），非源码可表达；证据 ORIG 0x088a18de/0x088a2731/0x088a2704 CatchHandler 区段。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（201/201）—— 本轮修 6 处调用点
// 符号差：①`item.m_fieldTypes.push_back(具名 FieldType::T 左值)` ×3（ORIG 调
// `push_back(const FieldType::T&)` RKS2_，右值临时会选 `push_back(FieldType::T&&)` OS2_）；
// ②`!(token == "[id]")` ×3（ORIG 调 operator==，写 != 会发 operator!=）。
// 残余 = N82 超大件帧槽（本轮复核：帧 sub $0x320 vs ORIG $0x350（Δ0x30=12 槽）、
// token -0x208 vs ORIG -0x230（Δ0x28），而 unkC -0x64 与 ORIG 同位 ⇒ 差集中在
// 深区局部区，属 4.1/4.4 聚合/标量槽分区策略差；gap 7、p167 201/201 CLEAR，
// 按 §6.2 三件套封顶）；ORIG 0x088a10cc。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 1441 / ours 1435；p167 201/201
// （差集全为 C1/C2·D1/D2 别名 P93 伪差）；jcc ORIG 84 / 我方 86，上下文差集全部为
// `test;je;jmp`（ORIG 岛形）↔ `test;jne;jmp`（我方）+ 槽平移对齐伪影 ⇒ 维持 N82/P164。
#include "CDataManagerScripts/importAdvanceAltarShopParameter.inc"
}

// ---- advancealtar::BuyUpgradeData::getFieldDataPoint（ORIG T 0x088a2e4c）----
// m_listMap.find(level) 未命中→0；it->second.find(ft) 未命中→0；命中返回
// &fit->second（FieldData*）。报告 advancealtar__BuyUpgradeData/getFieldDataPoint.md。
#include "CDataManagerScripts/getFieldDataPoint.inc"
}

// ---- advancealtar::BuyShopData::getBuyUpgradeDataVec（ORIG T 0x088a105c）----
// type 0/1/2 → &m_vecT0/T1/T2；其它 → 0。报告 advancealtar__BuyShopData/。
std::vector<advancealtar::BuyUpgradeData>*
#include "CDataManagerScripts/getBuyUpgradeDataVec.inc"
}

// ---- advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec
// （ORIG T 0x088a0fe8）：m_buyShopMap.find(key) 未命中→0；命中转调
// BuyShopData::getBuyUpgradeDataVec(type)。报告同名 md。
std::vector<advancealtar::BuyUpgradeData>*
#include "CDataManagerScripts/getBuyUpgradeDataVec_1.inc"
}

// ============================================================================
// advancealtar::RewardParameter::importAchievementRewardParameter
// （ORIG 0x0889afb0 T）[reward data]…[/reward data] 块解析进
// AchievementRewardData 后按 m_type 插入 m_rewardMap。错误码：
// type 扫描 0x362 / type<=0 0x364 / 难度串 0x36a / 难度枚举 0x36d /
// 清空次数 0x373 / 图标 0x37a / 奖励计数 0x391 / 标题 0x399 /
// 邮政标题 0x39e / 正文 0x3a3。[reward item] 三元组仅 {type,_,itemId}
// 进入向量（第三值写入越界临时不入拷贝，见报告注记）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（48/48）。历轮修正：
// ①`if (token == "[reward data]") { … }` 正向包裹（ORIG 调 operator==）；
// ②P232 `int unkC` ⇒ -0xc 与 ORIG 对齐（帧 0xc0=ORIG）。
// ③[本轮] [stage difficulty] 去具名 `StageDifficulty::T diff`，出参直传
//   `(StageDifficulty::T&)data.m_difficulty`（ORIG 0x0889b1d5 `lea -0xb4; add $0x4`
//   实证）⇒ 消除 2 条 store/load 并使后续局部槽整体回浅 4。
// ④[本轮] [reward item] 段用独立 bool `itemOk`（ORIG 0x0889b339 `mov %al,-0x89`、
//   0x0889b368/0x0889b38e `movzbl -0x89`）⇒ item -0x84 / typeName -0x88 /
//   itemOk -0x89 / value1 -0x80 / value2 -0x7c 与 ORIG 逐槽对齐。
// 残余 gap 12 = ORIG 8 个 continue 中继岛（0x0889b046..b05e 共 8×(nop;jmp)，落点
// 均为内层循环头 0x0889b05f）+ 少量装载序；4.4 对 continue 恒 jump-thread 直跳
// （我方仅 5 岛），属 P164/P223/N82 控制流布局族，源形不可择；ORIG 0x0889afb0。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 386 / ours 374；p167 48/48
// （差集全为 C1/C2·D1/D2 同址别名 P93 伪差）；jcc 30/30、逐位错位 10（岛/块序所致，
// 两侧同序号 jcc 语义指令均存在，无极性反写）⇒ 维持 N。
// [本波 fresh 复核 · 硬证据] orig 373 / our 367（我方少 6 条），ORIG-only
// {xor:2, jmp:4}。ORIG 报告 `## 汇编` 0x0889b044..0x0889b05c 有 **9 条连续 jmp
// 全部指向同一目标 0x0889b05f**（每 3 字节一条，+0x94/+0x97/…/+0xac）＝ 4.1 为
// 每个 `continue` 各发一条独立岛；4.4 恒 jump-thread 合并（P223 边界：同落点
// 多岛不可造，仅「不同落点连续尾标签」可造）。同族 2 条 xor 为 `while(ScanType)`
// 条件形差（P292 表反例：改 for+if(!) 经 regen 件实测劣化 raw 300→301，已回滚）。
// ⇒ 判 N（4.1 多岛 vs 4.4 merge，ORIG 0x0889b044..0x0889b05c 实证）。
#include "CDataManagerScripts/importAchievementRewardParameter.inc"
}

// ORIG C1 0x08371a26：构造 7 容器成员后调 clear()（+0x84 call clear）。
#include "CDataManagerScripts/PowerParameterScript.inc"
}
#include "CDataManagerScripts/dtor_PowerParameterScript.inc"
}

// ---- PowerParameterScript 查询（ORIG T 0x08a59c6e / 0x08a59d58）----
// GetPenalty：遍历 m_vecBC（+0xbc），取首个 m_field4 >= victory 的条目按值返回；
// 无匹配时返回最后一条（ORIG memmove(&vec[size-1], 0x10)）；空表未定义。
// [b28d-solo-CDataManagerScripts] AE：P13 反向 —— ORIG 0x08a59ce3 在循环条件处每轮调 end()（end 临时 -0xc，不缓存），改 for-init 内 begin + 条件 end() 后逐条对齐；fresh clean 71=71；ORIG 0x08a59c6e。
#include "CDataManagerScripts/GetPenalty.inc"
}

// GetBreakBonus（ORIG T 0x08a59d58）：m_vecC8（+0xc8 连破奖励表）；
// winCount < size → v[winCount]，否则 v[size-1]（越界取末元素）。
#include "CDataManagerScripts/GetBreakBonus.inc"
}

// ============================================================================
// checkPowerWarServer（ORIG T 0x08a59db4）：遍历 m_powerWarServerList
// （+0x9c，vector<pair<int,int>>），pair.first==a && pair.second==b → 1，
// 否则 0。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①`std::pair<int,int> p;` 默认构造于 begin 前（P50 复活；ORIG 0x81b5c8e=C1Ev）；②end 缓存（P13）；③`p = *it` 结构赋值替代两次 ptEv；ORIG 0x08a59db4。
#include "CDataManagerScripts/checkPowerWarServer.inc"
}

// ---- SecretShopScript（成员默认构造）----

// ---- SecretShopScript（成员默认构造）----
#include "CDataManagerScripts/SecretShopScript.inc"
}
#include "CDataManagerScripts/dtor_SecretShopScript.inc"
#include "CDataManagerScripts/stTitleBookInfo.inc"
}
#include "CDataManagerScripts/dtor_stTitleBookInfo.inc"
#include "CDataManagerScripts/stItemMakingSkill.inc"
}
#include "CDataManagerScripts/dtor_stItemMakingSkill.inc"
#include "CDataManagerScripts/clear_12.inc"
}

// ---- ChoiceItemInfo / ImageCommunicationData（无析构声明）----
#include "CDataManagerScripts/ChoiceItemInfo.inc"
}

#include "CDataManagerScripts/clear_13.inc"
}
#include "CDataManagerScripts/ImageCommunicationData.inc"
}

// ---- ARAD::SCRIPT::AradJumping_Script----
// W7_BuildHarmonize 去重：生命周期唯一 owner 为 Arad_Script.cpp
// （同文件含 getRDARScriptData/importScript 等同类方法）。

// ---- EventEtcScript（ORIG C1 0x8372e66 / clear 0x8372ece / D1 0x837dda0）----
#include "CDataManagerScripts/EventEtcScript.inc"
}
#include "CDataManagerScripts/dtor_EventEtcScript.inc"
#include "CDataManagerScripts/clear_14.inc"
}

// ---- stReturnUserRewardScript / stBingoScript / stNewAccountLevelUpToJobScript----
// ORIG C1 0x8371544：两个 int 置 0 后构造 + clear 内嵌 stLevelUpRewardItemScript。
#include "CDataManagerScripts/stReturnUserRewardScript.inc"
}
#include "CDataManagerScripts/dtor_stReturnUserRewardScript.inc"
#include "CDataManagerScripts/stBingoScript.inc"
}
#include "CDataManagerScripts/dtor_stBingoScript.inc"
#include "CDataManagerScripts/stNewAccountLevelUpToJobScript.inc"
}
#include "CDataManagerScripts/dtor_stNewAccountLevelUpToJobScript.inc"
#include "CDataManagerScripts/CWorldMapList.inc"
#include "CDataManagerScripts/dtor_CWorldMapList.inc"
}

// ORIG 0x8374752：遍历 delete 各 CWorldMap*（D1 + operator delete），最后清空 map。
// [b28d-solo-CDataManagerScripts] AE：①`iterator it;` 默认构造 + `it = m_map.begin()`（P50 复活）+ `iterator e = m_map.end()` 缓存；②无条件 `delete worldMap`（P141）⇒ 51=51 全等；ORIG 0x08374752。
#include "CDataManagerScripts/destroy.inc"
}

// ORIG 0x83647a2：map<int,CWorldMap*>::find(idx)，命中返回 second，否则 0。
#include "CDataManagerScripts/find_world_map.inc"
}

// ---- CWorldMap 访问器（ORIG 无独立报告；按 objdump 直读字节字段实现） ----
#include "CDataManagerScripts/IsInHellDungeon.inc"
}

#include "CDataManagerScripts/hasDeathTower.inc"
}

// ORIG W 0x0822b514 _ZNK9CWorldMap18getDungeonMinLevelEv（const）：返回
// m_dungeonIndex（this+0x8）；StartGame::check_error 消费。
#include "CDataManagerScripts/getDungeonMinLevel.inc"
}

// ORIG W 0x822b52c _ZNK9CWorldMap10hasDungeonEj（0x85 字节）：
// std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) != end()。
// 实参为 unsigned int（find 模板 _ZSt4findI...jE...）；调用方
// DisPatcher_SelectDungeon::process 传 (int)(short)MSG_CHARAC_SELECT+0xd。
#include "CDataManagerScripts/hasDungeon.inc"
}

#include "CDataManagerScripts/StageMapList.inc"
#include "CDataManagerScripts/dtor_StageMapList.inc"
#include "CDataManagerScripts/WarAreaCounter.inc"
}

// ORIG W 0x083707a4：memset +0x000/0x60/0xc0 三段数组、清 +0xe8..+0xf4 四个
// int、memset +0x0f8 段、map(+0x120).clear()、清 +0x140；其余（wpPenalty/
// monsterExpConst/warRoomCount/pad_2d4）不动。
#include "CDataManagerScripts/clear_15.inc"
}
#include "CDataManagerScripts/dtor_WarAreaCounter.inc"
#include "CDataManagerScripts/stEquipmentGradeSample.inc"
}
#include "CDataManagerScripts/dtor_stEquipmentGradeSample.inc"
#include "CDataManagerScripts/stOneEmotionData.inc"
}
// ORIG 0x891b640（stOneEmotionData::operator=）：逐成员复制
// 0x00/0x04 int、0x08/0x0c/0x14 string、0x10/0x11/0x12 char、0x18 vector。
// 注意：+0x13 为 padding（m_pad13），ORIG 不复制；镜像布局须显式定义以
// 覆盖编译器隐式逐成员赋值（隐式版会复制 m_pad13）。
stOneEmotionData& stOneEmotionData::operator=(const stOneEmotionData& rhs)
{
    m_0 = rhs.m_0;
    m_4 = rhs.m_4;
    m_8 = rhs.m_8;
    m_c = rhs.m_c;
    m_10 = rhs.m_10;
    m_11 = rhs.m_11;
    m_12 = rhs.m_12;
    m_14 = rhs.m_14;
    m_18 = rhs.m_18;
    return *this;
}
// ORIG W 0x8370bb0（0x9b 字节）：成员自动析构，标量无操作。
#include "CDataManagerScripts/dtor_stOneEmotionData.inc"
#include "CDataManagerScripts/clear_16.inc"
}
// ORIG 0x8370c4c：构造两个 vector 后 clear
#include "CDataManagerScripts/stEmotionTypeTagsInfo.inc"
}
#include "CDataManagerScripts/dtor_stEmotionTypeTagsInfo.inc"
#include "CDataManagerScripts/clear_17.inc"
}

// ORIG C1 0x837c8c6：仅构造 +0x8/+0x20/+0x38 三容器，+0x0/+0x4 不初始化。
#include "CDataManagerScripts/stConditionEventInfo.inc"
}
#include "CDataManagerScripts/dtor_stConditionEventInfo.inc"
#include "CDataManagerScripts/pvp_channel_info_t.inc"
}

// ============================================================================
// STExpertJobScript::isBoundaryExpValue（TSV 修复，ORIG 0x0849f420 T）。
// ORIG：遍历 vector<ExpertJobExp>（元素尺寸 12，见 _ZNSt6vectorI12ExpertJobExpSaIS0_EEixEj
// 乘法 i*12），任一元素首整数字段 == exp 返回 true，否则 false。
// 权威声明见 STExpertJobScript.h（CUser::GetCurExpertJobLevel 真实调用）。
// ============================================================================

// [b28d-solo-CDataManagerScripts] NEAR(1)：p167 CLEAR（4/4）。N22 —— 4.4 恒
// `cmp %ebx,%eax`（dst=call 结果/字段侧），ORIG 4.1 `cmp %eax,%ebx`（dst=局部 e）；
// 本轮 fresh 复验：交换源码操作数（`e == m_expVec[i].m_exp`）仍产同形，P248 三档试尽；
// 语义等价；ORIG 0x0849f420。
#include "CDataManagerScripts/isBoundaryExpValue.inc"
}

// STExpertJobScript::GetLevel（TSV 修复发明符号：_ZN17STExpertJobScript8GetLevelEj，ORIG 0x08693ba0）。
// ORIG：遍历 vector<ExpertJobExp>，返回首个 v[i].m_exp > exp 的 i+1；无则返回 size()。
#include "CDataManagerScripts/GetLevel.inc"
}

// STExpertJobScript::GetCompoundRate（ORIG 0x0849f366 W）。
// 遍历 m_compoundRateVec（+0x1f4）：返回首个 stCompoundRate.m_level >= level 的元素；
// 全表都 < level 时返回末元素；空表返回 0。调用方（GetSuccessRate）读 +4/+8/+0xc。
// [b28d-solo-CDataManagerScripts] AE：①P13 缓存 end（`iterator e = m_compoundRateVec.end();` 具名初始化）⇒ begin/end 槽 -0xc/-0x10 对齐；②P167 —— ORIG 三处调 `operator*`(deEv)，`it->m_level` 改 `(*it).m_level` 消 ptEv 差集；fresh clean 56=56；ORIG 0x0849f366。
#include "CDataManagerScripts/GetCompoundRate.inc"
}

// STExpertJobScript::GetRecipeInfoIndex（ORIG 0x0832b726 W）。
// 在 m_recipeIndexMap（+0x218，map<unsigned long,int>）按 itemIndex 查；
// 找到返回 mapped int（recipe 信息索引），否则 -1。
#include "CDataManagerScripts/GetRecipeInfoIndex.inc"
}

// STExpertJobScript::GetRecipeInfo（ORIG 0x0832b6b6 W）。
// 在 m_recipeInfoMap（+0x200，map<int,stRecipeInfo>）按 index 查；
// 找到返回 &it->second，否则 0。
#include "CDataManagerScripts/GetRecipeInfo.inc"
}
// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
// ---- GuildParameterScript::getGuildExpWeight（ORIG T 0x089795de）----
// m_map44.find(level) 命中返回 second，否则 0（CUser::FatigueUp 公会经验权重，
// 与 0x08979864 getExpWeight 的 m_vecE4 扫表路径不同；ORIG 返回 int）。
#include "CDataManagerScripts/getGuildExpWeight.inc"
}

// ---- GuildParameterScript::getGuildAgitUpgradeInfo（ORIG T 0x089796dc）----
// m_vec2c（STAgitUpgradeInfo[agit upgrade info]）取下标 level-1；
// level-1 >= size 返回 NULL（ORIG cmp/setl 分支一致）。
#include "CDataManagerScripts/getGuildAgitUpgradeInfo.inc"
}

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----
// 自高向低扫 m_vecE4（vector<pair<int,double>>，等级阈值→经验权重），
// 首个 first<=guildLevel 的 second；全不命中取 [0].second（ORIG 无判空，照抄）。
#include "CDataManagerScripts/getExpWeight.inc"
}

// ---- GuildParameterScript::getGuildLevelUpParam（ORIG T 0x08979648）----
// level∈[1,16] 返回 m_levelUpCost+(level-1)*8 的地址（按 int 口径），否则 0。
#include "CDataManagerScripts/getGuildLevelUpParam.inc"
}

// ---- GuildParameterScript::getLevelupPrice（ORIG T 0x089797a4）----
// 自高向低扫 m_vecF0，首个 first<=memberCnt（members 阈值）的
// second[level]；全不命中取 [0].second[level]；结果 ×10000。
// [b28d-solo-CDataManagerScripts] AE：删 tbl 引用缓存与 priceVecIdx（P183 每点全表达式 m_vecF0[i] + 命中直接 return）⇒ 循环变量 -0xc 复用、无 -0x14/-0x10 缓存槽；ORIG 0x089797a4。
#include "CDataManagerScripts/getLevelupPrice.inc"
}

// ---- GuildParameterScript::getGuildExpBook（ORIG T 0x08979672）----
// m_map10.find(key) 命中返回 second，否则 0（W7-BigThree increase_status case 0x10 依赖）。
#include "CDataManagerScripts/getGuildExpBook.inc"
}
// ORIG _Z14IS_PVP_CHANNELi 0x088dd255（W）：线性 cmpl/je 链，命中集合
// {4,6,5,7,8,9,10,13,14,15,16} 返回 1，否则 0（P45 短路线性判定同构）。
// 定义落本 TU：声明同文件（CDataManagerScripts.cpp 顶部），调用点在
// GetPvPChannelNumber 0x088dcc28（按 bool 语义消费）。
#include "CDataManagerScripts/IS_PVP_CHANNEL_1.inc"
}

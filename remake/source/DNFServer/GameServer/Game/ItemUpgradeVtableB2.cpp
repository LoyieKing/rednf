// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ItemUpgradeVtableB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ItemUpgradeVtableB2.cpp — WongWork::CItemUpgrade::ProcUpgrade（ORIG vtable
// _ZTVN8WongWork12CItemUpgradeE @0x08c9d310 槽 0）及其升级域依赖闭环。
//
// 本 TU 定义的 ORIG 符号（nm -C /home/wangyilei/下载/df_game_r 全部核实为真实
// T/W；报告路径 docs/class_func_reports/…，## 汇编 + ## 反编译 C 均已完整读取）：
//
//   WongWork::CItemUpgrade:
//     ProcUpgrade(MSG_ITEM_UPGRADE)   T 0x0854779c  WongWork__CItemUpgrade/ProcUpgrade.md
//     _getUpgradeCountByUpgradeItem   T 0x08548e68  …/_getUpgradeCountByUpgradeItem.md
//     _getUpgradeRateByUpgradeItem    T 0x08548ffa  …/_getUpgradeRateByUpgradeItem.md
//     _CheckTargetUsableLv            T 0x08548c3c  …/_CheckTargetUsableLv.md
//     _ProcPortableUpgradeItem        T 0x08548d3c  …/_ProcPortableUpgradeItem.md
//     _DoProcUpgrade                  T 0x0854755a  …/_DoProcUpgrade.md
//     _ProcPenalty                    T 0x08546bba  …/_ProcPenalty.md
//     _getAdditionalDisjointItem      T 0x08546938  …/_getAdditionalDisjointItem.md
//   expert_job::CAlchemist（静态）:
//     IsExistContinuousEffectItem     T 0x085d121e  expert_job__CAlchemist/IsExistContinuousEffectItem.md
//     GetItemUpgradeSuccessRate       T 0x085d1360  expert_job__CAlchemist/GetItemUpgradeSuccessRate.md
//   DisJoint:
//     GetResult                       T 0x084733f6  DisJoint/GetResult.md
//     _GetAdditionalItem              T 0x084732b4  DisJoint/_GetAdditionalItem.md
//     _GetAdditionalItemInfo          T 0x08473224  DisJoint/_GetAdditionalItemInfo.md
//   CEquipItem::getUpgradeProbIncrease W 0x0854b552（14 字节访问器；_DoProcUpgrade.md
//     引用、无独立报告——调用方报告已读后按 objdump 忠实转写）
//   DisJointResult::DisJointResult   W 0x082347a2（无独立报告；调用方 GetResult/
//     _ProcPenalty 报告已读后按 objdump 忠实转写）
// [b28d-solo-ItemUpgradeVtableB2 判词汇总 · 单文件泳道]
// 本 TU 8 件（明细 /tmp/b28d_solo_ItemUpgradeVtableB2_result.tsv）：
//   _ProcPortableUpgradeItem 85/85 **IDENTICAL**（前轮 AE + 本轮 stack 改 const
//     ⇒ 调 const 版 vector::operator[]，符号与 ORIG 全等，TU identical +1）；
//   DisJointResult::C1 19/19 **AE**（本轮翻正旧 N4：DisJoint.h 加 union 双视图
//     m_f[10]/{m_a[5],m_b[5]}，ctor 改写 m_a[i]/m_b[i] ⇒ 复现 ORIG
//     `add $0x4,%edx; movl $0,0x4(%eax,%edx,4)`）；
//   IsExistContinuousEffectItem 66/66 **AE**（本轮翻正旧 N18b：同布局视图
//     ContinuousItemIdxView 的 unsigned 首字段 ⇒ 复现两段装载 mem→%edx/param→%eax）；
//   DisJoint::GetResult 105/105 **AE**（本轮翻正旧 N85：内联 rarity、int 槽复用
//     sellPrice、内联 sell、去 money 具名槽、if 形最小值）；
//   _getAdditionalDisjointItem 108/102 N85①（4.1 float 拷贝经 flds/fstps、4.4 恒
//     mov；volatile 实测反劣）+ N82（槽号）+ 尾 3 岛（4.1 多出口 jmp+nop）；
//     P167 CLEAR 8/8、条件方向已用 jb 复现、make_pair 嵌套形/unsigned span/去 +0.5 已落；
//   _DoProcUpgrade 166/163 N82（槽号重排）+ rollWin setcc-fold（4.1 mov $1/$0 岛）；
//     P167 CLEAR 14/14；
//   _ProcPenalty 634/620 N82（§6.2 三件套：P167 CLEAR 87/87、gap 14、帧同）；
//   ProcUpgrade 1242/1244 N82 + P292（P167 CLEAR 115/115、gap -2；
//     _GetUpgradeInfo int→bool 属 CItemUpgrade.cpp 属主，跨 TU 不改）。
// 全部 8 件 P167 调用点差集 CLEAR（helper EXTRA×2 已删：missing_in_game 40→36）。
//   getItemSellPrice(int,int,int,bool) T 0x0898c86e（无独立报告；调用方 GetResult/
//     _ProcPenalty 报告已读后按 objdump 忠实转写）
//   g_alchemistInfo                 B 0x094f7778（数据符号；nm 尺寸 8，唯一读者按
//     低 32 位 int 读取）
//
// vtable 闭环说明：GCC 以类内首个虚函数为 key function；本 TU 定义 ProcUpgrade
// 后由编译器产出 _ZTVN8WongWork12CItemUpgradeE，消除当前链接的
// "undefined reference to `vtable for WongWork::CItemUpgrade'"
// （CItemUpgrade.cpp.o 析构与 CtorProducers_2C.cpp.o 构造两处引用）。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CGlobalEffectManager.h"   // s_globalEffectManager_ 完整类型
#include "CEquipItem.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager / stADDINFO
#include "CInventory.h"           // CInventory / Store / CValueStatistic
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"             // CMTRand（+0x4e8 randInt）
#include "EventUpgradeJarInfo.h"  // Event_Upgrade_Jar
#include "CCubeStatistic.h"
#include "DisJoint.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "CUserProc.h"            // APSystem::CUserProc
#include "CEnvironment.h"
// WongWork::CItemUpgradeTable（CItemUpgrade 表区子对象；ProcUpgrade 取表直调
// 其 getUpgradeInfoTable W 0x0854b382，禁 inline 视图代返）。
#include "CItemGenerator.h"
// MSG_ITEM_UPGRADE 完整类型（CItemUpgrade_Separate.h 仅前向声明；ProcUpgrade
// 需具名读 m_upgradeType/m_targetSlot/m_targetItemIdx/m_ticketSlot/
// m_materialSlot/m_25/m_27/m_28）。
#include "PacketTypes.h"

#include <cmath>
#include <cstring>
// std::max（_getAdditionalDisjointItem 0x08546983 模板实调）
#include <algorithm>
#include <utility>
#include <vector>

extern int itemPriceRates_[3];    // B 0x094f7bfc（InitScriptProducers_4.cpp 定义）

// B 0x094f7778（nm 尺寸 8）。源码此前无任何定义/写点；唯一读者
// GetItemUpgradeSuccessRate 以 fildl 取低 32 位。按 int 定义并置零。
extern int g_alchemistInfo[2];   // B 0x094f7778（定义于 CExpertJobList.cpp int[2]）


namespace expert_job
{

// [b28d-solo-ItemUpgradeVtableB2] ContinuousItemInfo 同布局具名视图：
// ORIG IsExistContinuousEffectItem 0x085d12a2 `call deEv; mov (%eax),%edx;
// mov 0xc(%ebp),%eax; cmp %eax,%edx` 是**两段装载**比较 ⇒ 字段与 int 形参
// 异型（P248 档③/P274）；权威头 ContinuousItemInfo::m_itemIdx 为 int 且跨 TU
// 消费点众多（改头需全消费段联判），故本 TU 以同布局视图（unsigned 首字段）
// 表达，零头改动、零 ABI/布局风险。
struct ContinuousItemIdxView
{
    unsigned int m_itemIdx;      // +0x00（同 ContinuousItemInfo::m_itemIdx）
    unsigned int m_expireTime;   // +0x04
};

// ---- CAlchemist 静态成员（此前无声明；仅本 TU 使用）----
class CAlchemist
{
public:
    static bool IsExistContinuousEffectItem(CUser* user, int itemId);
    static float GetItemUpgradeSuccessRate();
};

}  // namespace expert_job

// ============================================================================
// expert_job::CAlchemist::IsExistContinuousEffectItem @ 0x085d121e
//   getCurCharacR 为空 → false；遍历当前角色持续道具表，命中 itemId 即以
//   「当前秒 < 到期秒」判定存在性。
// [b28d-solo-ItemUpgradeVtableB2] **AE 66/66**：旧判 N18b（成员==参数
//   mem-cmp 折叠）已翻正——ContinuousItemIdxView 同布局视图（unsigned 首字段
//   vs int 形参，P248 档③/P274）令 4.4 复现 ORIG 两段装载
//   `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（探针 g 实证；
//   视图指针必须内联在条件里，具名会多地址槽）。
// ============================================================================
#include "ItemUpgradeVtableB2/IsExistContinuousEffectItem.inc"
}

// ============================================================================
// expert_job::CAlchemist::GetItemUpgradeSuccessRate @ 0x085d1360
//   return (float)g_alchemistInfo / *(float*)0x08cbfa3c (= 9840.0f)
// ============================================================================
#include "ItemUpgradeVtableB2/GetItemUpgradeSuccessRate.inc"
}

// ============================================================================
// getItemSellPrice @ 0x0898c86e（自由函数；nm 实查 size 0x8e）
//   【切片转写】game_func_report 对本符号的 ORIG 段取错（撞名，§6.3 假阳性清单
//   已报 Main），故按 objdump 0x0898c86e..0x0898c8fb 人工转写：
//     0x0898c874 `mov 0x14(%ebp),%eax; mov %al,-0x14(%ebp)` bool 实参落本地槽；
//     0x0898c87a `movb $0,-0x1` + `cmpb $0,-0x14; je` + `movb $2,-0x1` ⇒ sel 形；
//     0x0898c888 `cmp 0x10,%eax; jle; mov 0x10→0xc` ⇒ a=min(a,b)（写回形参槽）；
//     0x0898c896 `cmpl $0,0x10; jne` 后**两分支各自完整重发**
//     `movzbl sel; mov rates(,%eax,4); imul price; 0x10624dd3 magic /1000`
//     （0x0898c89c 与 0x0898c8c6 各 13 条）⇒ 提公共 base 局部会丢整段 13 insn
//     （P48/P183：-O0 无 CSE）。
// ============================================================================
#include "ItemUpgradeVtableB2/getItemSellPrice.inc"
}

// ============================================================================
// CStackableItem 强化/增幅券访问器（0xf 字节级访问器；声明见 CStackableItem.h）
//   IsReinforceTicket      W 0x084b4222：movzbl +0x30f（m_char30f）
//   GetReinforceRate       W 0x084b4232：movzbl +0x312（m_char312）
//   IsAmplifyReinforceTicket W 0x084b4242：movzbl +0x313（m_char313）
//   GetAmplifyReinforceRate  W 0x084b4252：movzbl +0x316（m_char316）
//   GetReinforceVaule      T 0x0854b4f8：movzwl +0x310（m_short310）
//   GetAmplifyReinforceValue T 0x0854b508：movzwl +0x314（m_short314）
// ============================================================================
#include "ItemUpgradeVtableB2/IsReinforceTicket.inc"
}

#include "ItemUpgradeVtableB2/GetReinforceRate.inc"
}

#include "ItemUpgradeVtableB2/IsAmplifyReinforceTicket.inc"
}

#include "ItemUpgradeVtableB2/GetAmplifyReinforceRate.inc"
}

#include "ItemUpgradeVtableB2/GetReinforceVaule.inc"
}

#include "ItemUpgradeVtableB2/GetAmplifyReinforceValue.inc"
}

// ============================================================================
// DisJointResult ctor @ 0x082347a2：循环 i=0..4 将两组 5 int 清零
// （等价整体清零 10 个 int）。
// [b28d-solo-ItemUpgradeVtableB2] 旧判 N4（P30 add-vs-lea）已翻正：ORIG 第二组
//   `add $0x4,%edx; movl $0,0x4(%eax,%edx,4)` 的偏移 0x14 = 索引 add 4（16B）
//   + disp 4，只在**独立成员数组**（m_b@+0x14）下标形下由 4.4 复现（探针 c5/c7）；
//   单数组 m_f[i+5] 恒折 `lea 5(%eax)`（c1）。DisJoint.h 已加 union 双视图
//   m_f[10] / {m_a[5], m_b[5]}（sizeof 0x28、全偏移不变）。
// ============================================================================
#include "ItemUpgradeVtableB2/DisJointResult.inc"
}

// ============================================================================
// DisJoint::GetResult @ 0x084733f6
//   out.m_f[5] = m_cubeIndexVecs[4][0]；
//   out.m_f[0] = ROUND(((ROUND(1.1f*sellPrice) 经 getItemSellPrice 折算 ×1000 ÷
//                m_creationConst) * m_creationRate[rarity]) ÷ 1000.0f)，最小 1；
//   _GetAdditionalItem(item, &out.m_f[6], &out.m_f[1])。
//   物品缺失走 LogManager::logFormat(1,…,0x6e,"[DisJoint::GetResult] Null Item\n")
//   并返回 0x11。
// [b28d-solo-ItemUpgradeVtableB2] **AE 105/105**：旧判 N85 已翻正——①rarity
//   无槽（get_rarity 直入 %edx 参与 0x14c 索引）②sellPrice 用 int 槽复用
//   （fildl→*1.1f→RC=trunc fistpl 写回同槽，禁 float 局部）③getItemSellPrice
//   返回值无槽（直进 imul $0x3e8）④base 具名槽 -0xc ⑤money 不设具名局部
//   （4.4 临时槽形与 ORIG -0x20 同构）⑥最小值用 if 形（三目折 cmovg）。
// ============================================================================
#include "ItemUpgradeVtableB2/GetResult.inc"
}

// ============================================================================
// CReduceUpgradeItemPay —— 强化减费重复事件（CEventBase 派生；与
// CItemUpgrade.cpp 内同形内联定义，弱符号合并）。
// ORIG GetReduceRate W 0x854b56e：返回 +0x04 short 经 cwtl 符号扩展。
// ============================================================================
class CReduceUpgradeItemPay : public CEventBase
{
public:
    int GetReduceRate() { return (short)(m_addInfo.m_info0); }
};

// [b28d-solo-ItemUpgradeVtableB2] P49/P167：原匿名 namespace 的
// upgradeRand/randIntBound/embeddedTable/embeddedUpgradeTable 四个 inline
// helper 已全部删除——-O0 下它们是**真实 call**（ORIG 无的 EXTRA 符号），
// 调用点改为 m_rand 字段直读 / 基类 getUpgradeInfoTable() 成员直调后
// p167 差集归零（missing_in_game 40→36）。
namespace WongWork
{

// ============================================================================
// _getUpgradeCountByUpgradeItem @ 0x08548e68
//   [b28d-58] AE（P46 叶块排放序 = case 书写序）：原 case 序 1d29/1c6a/1c6b/1c69…
//   使各 je 落点与 ORIG 叶块错位；改 ORIG 排放序 1c69/1c6a/1c6b/1d29/1e3f/1e40/1e86
//   后逐条全等（ORIG 0x08548ecd..0x08548ee5）。
//   默认按材料 idx 映射强化档位（仅非增幅物品）；材料为堆叠物时改读
//   ReinforceTicket/AmplifyReinforceTicket 数值（[0,0x1f] 越界归零）。
//   增幅券在目标无增幅能力时报 ENUM_ERROR 0x13。
// ============================================================================
#include "ItemUpgradeVtableB2/_getUpgradeCountByUpgradeItem.inc"
}

// ============================================================================
// _getUpgradeRateByUpgradeItem @ 0x08548ffa
//   同上结构；映射命中直接返回 100；票券读 Rate 字段（[0,100] 越界归零）。
// ============================================================================
#include "ItemUpgradeVtableB2/_getUpgradeRateByUpgradeItem.inc"
}

// ============================================================================
// _CheckTargetUsableLv @ 0x08548c3c（const）
//   材料（param_1）脚本可堆叠且 +0x320 标志为 1 时，要求目标（param_2）为
//   可装备类型且其可用等级落在材料脚本 [m_int318, m_int31c] 区间内。
// ============================================================================
#include "ItemUpgradeVtableB2/_CheckTargetUsableLv.inc"
}

// ============================================================================
// _ProcPortableUpgradeItem @ 0x08548d3c（const）
//   便携带升级券（action 类型 0x35）：校验通过后把 info 改写为
//   {recipe=材料 idx, count=1, discount=0} 并返回 0。
// [b28d-solo-ItemUpgradeVtableB2] **AE 85/85**（前轮判词复验，本轮改动无回归）。
// ============================================================================
#include "ItemUpgradeVtableB2/_ProcPortableUpgradeItem.inc"
}

// ============================================================================
// _DoProcUpgrade @ 0x0854755a
//   以 info+0x20 的失败概率经事件/炼金/装备三段修正后掷骰：
//   roll >= prob → IncUpgrade 且返回 true（强化成功）；否则保持等级并返回
//   false（调用方走 _ProcPenalty）。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 166/163 封顶（N82 + setcc-fold）：
//   P167 CLEAR 14/14，帧同（0x94）。已修：删 range 缓存（m_randMax 每用点
//   重读）、两处 `&&` 拆嵌套 if（复现双 test/je 短路口）、a0/a1 独立 max 局部、
//   reduced 左操作数改 m_randMax（复现 fildl→fstps→call→fmuls 求值序）、
//   内联 invenR/roll、恢复 wornIdx 具名槽。残余：①prob 等局部槽号整体重排
//   （4.4 首次使用序 vs 4.1 声明序，N82）②`rollWin = roll < prob` 4.4 恒
//   setb+槽+xor，ORIG 是 mov $1/$0 + test 岛（P197④ setcc-fold，call 结果
//   寄存器直比不可翻）。
// [2026-09-10 SweepCEquipItem2] 本轮复核（未改）：gap 3（orig 166 / our 163），
// p167 14=14 CLEAR；非槽差异仅 `setb %al` 一处（ORIG 0x085476xx 的
// `jae; mov $1,%eax; jmp; mov $0,%eax` setcc-fold 岛，P197④/N 封顶），
// 其余为 prob/wornIdx 等局部槽号重排（N82）。语义与 ORIG 一致。
// ============================================================================
#include "ItemUpgradeVtableB2/_DoProcUpgrade.inc"
}

// ============================================================================
// ProcUpgrade @ 0x0854779c —— 强化主流程（vtable 槽 0）。
// 返回 0 成功；其余为错误码（4/8/9/0xd/0x10/0x11/0x13/0x16/0x17/0xea/0x3f4…）。
//
// 【batch-11 验收标注（§6.2 超大函数口径）】orig 1242 / ours 1246（gap 4）。
//   ① 调用点差集（P167）：ORIG direct 115 + indirect 5 ↔ ours 115 + 5，**归零**
//      （仅 Inven_Item C2×4 / upgrade_info_t C2×1 ↔ 4.4 发射 C1×5 的同体符号名
//      对称差，函数体等价，属跨 GCC 版本发形）；
//   ② 逐块比对补齐：4 处 Inven_Item「默认构造+赋值」物化块、map operator-> ×3、
//      GetRepeatEvent ×2、use_money 双分支各自重取 invenW、减费装备改走
//      getCurCharacInvenR(0,0xb)、回包 add_info 改取材料槽且重取 invenW、
//      randInt 上界改字面 99、去掉 ORIG 不存在的 GetRepeatEvent 判空防御
//      （gap 156 → 4）；
//   ③ 残余 4 insn 集中在 ticket 级 bool 汇聚岛（0x08547c51）与
//      /100 magic 除法展开（0x08548a98）⇒ 判**语义完成、byte 层
//      N76/N82/N18b 封顶**，勿再耗轮次。
// [b28d-solo-ItemUpgradeVtableB2 复核] DIFF 1242/1244 封顶（N82 + P292）：
//   ①P167 CLEAR 115/115（本轮删匿名 namespace helper×2，EXTRA 归零）；
//   ②gap -2（我方多 2 条，含 1 nop）；③帧 0x32c 同、残余为槽号整体重排
//   （N82）与两处 bool 物化（`_GetUpgradeInfo` int 返回的 `test/sete` vs
//   ORIG `xor $0x1`；callee 定义在 CItemUpgrade.cpp 属主 TU，P292 跨 TU
//   返回类型冲突不改）⇒ 语义完成、byte 层封顶。
//   MSG_ITEM_UPGRADE +0x1a/+0x1b/+0x1d/+0x21/+0x23 字段化已由 Main 代落
//   PacketTypes.h:807-812；本函数无裸偏移读（CEnvironment+0x378 已按 §4.3
//   具名化为 m_serverEnvir.m_serverGroup）。
// [2026-09-10 SweepCEquipItem2] 本轮复核（未改）：gap 1（fresh .o orig 1242 /
//   our 1241），p167 115=115 CLEAR（仅 Inven_Item/upgrade_info_t 的 C1/C2 同址
//   别名伪差）。虚槽 5/5 与 ORIG 逐处一致：0x44 / 0x30 / 0x34 / 0x34 / 0x4c
//   （ORIG 0x08547a63 / 0x08547e12 / 0x08547e4d / 0x085481be / 0x08548aae）。
//   非槽差异为 setcc-fold（setne/setge 岛）与浮点书写序（P243）；帧差 4 来自
//   ORIG 多一条 `push %edi`（N7 寄存器分配）。按 §6.2 三件套判语义完成。
//
// 物化口径（P183/P167 实证）：ORIG 对材料/保护券槽各发
//   Inven_Item C2（0x080cb854）+ GetInvenSlot 临时槽（-0x2c8）+ 15×movl+1×movb
//   逐字段拷贝，共四处独立物化块（-0x15e/-0x19b/-0x1d8/-0x215）；重建端一律
//   「默认构造 + 赋值」两步形，禁 `T x = f()` 直构（会折叠 sret 目标，丢整块）。
// ============================================================================
#include "ItemUpgradeVtableB2/ProcUpgrade.inc"
}

// ============================================================================
// _getAdditionalDisjointItem @ 0x08546938
//   grade 与表内基准差值达到阈值且强化级足够时，按 rarity 权重浮点计算
//   附加分解物数量并压入 items（{item=table[0x238], count}）。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 108/102 封顶（N85① + N82 + 尾岛）：
//   P167 CLEAR 8/8；已修 grade 内联（%ebx 保活）、max 实参序、clamped>=gap
//   方向（复现 ORIG jb）、unsigned span（复现零扩展 fildll）、去 +0.5（ORIG
//   RC=trunc 纯截断）、make_pair 嵌套形（三连 call）。残余：①weight 的
//   `flds mem; fstps` vs 4.4 mov（volatile 实测反劣 +1）②槽号重排（N82）
//   ③fistpll 后 4.4 DCE 掉 `mov -0x3c,%edx` ④尾部 3 个 jmp+nop 岛（4.1
//   多出口，4.4 jump-thread 合并）。
// [2026-09-10 SweepCEquipItem2] 本轮复核（未改）：gap 7（orig 108 / our 101），
// p167 8=8 CLEAR。逐条核对 Ghidra C 与汇编：weight 字段确为 float
// （CItemAmplifier.h `float m_additionalWeight[6]` @+0x240），4.4 对 float 局部
// 拷贝恒 `mov` 位直搬、4.1 用 `flds/fstps`（0x085469bf，N85① 封顶）；
// 寻址 `add $0x90,(%eax,%edx,4)` vs `add $0x8c; mov 0x10(%eax,%edx,4)`
// 等价（0x8c*4+0x10 = 0x240，P30 拆分差）；fistpll 后 ORIG 多读高 32 位
// （0x08546a3x，N92）；尾 3 岛为 4.1 多出口 jmp+nop（N/P223 负例）。
// 语义（pow 底数 up-m_additionalMinUpgrade、截断两段、make_pair 转换链、
// count!=0 门禁）与 ORIG 逐点一致，判 N。
// ============================================================================
#include "ItemUpgradeVtableB2/_getAdditionalDisjointItem.inc"
}

// ============================================================================
// _ProcPenalty @ 0x08546bba
//   惩罚分派：1=降级（保底）、2=降级（可到 0）、3=破坏（保护券或分解返料）。
//
// 【batch-16 验收标注（§6.2 口径）】orig 634 / ours 633（gap 41 → 1）。
//   本轮补齐：① 返料三元组遍历由 size()+operator[] 索引形改为 ORIG 的
//   begin/end 迭代器形（恢复 4 个迭代器调用、去掉 3×ix 与 1×size，P175）；
//   ② GetSellItemPrice 的短参改直读 Inven_Item::m_addInfo2（ORIG 无
//   get_add_info() 调用）；③ penaltyType==3 的增幅表改走真实成员调用
//   CItemUpgradeTable::getUpgradeInfoTable（inline 视图代返会丢该 call）。
//   残余 1 insn 与差集里 operator+ ×3 ↔ operator++ ×1 的符号名形差同源
//   （Ghidra 把 libstdc++ 的 operator+(difference_type) 渲染为 operator++），
//   已判语义完成、byte 层封顶。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 634/620 封顶（N82，§6.2 三件套）：
//   ①P167 CLEAR 87/87（本轮补：三元组循环 `*it++`×3 复现 ppEi×3、SetUpgrade
//   两分支各自重调 GetUpgrade、insert_event_items 前重调 getCurCharacInvenW、
//   保护券分支去 invenW 缓存）；②gap 14（2.2%）；③帧 0x180 同、残余为槽号
//   重排与 `info->m_prob20[2]=1` 的 4.1 地址中转（-0x3c 指针槽，4.4 折单条
//   movl）+ bool 物化（setge vs jge/jns）。已修：penaltyType 正极性重排、
//   toZero 改 `A || B` 短路表达式（复现 jge/jns 双跳）。
// [2026-09-10 SweepCEquipItem2] 本轮 gap 15→1（fresh .o orig 634 / our 633），
// p167 87=87 CLEAR，四数零回退。修法：
//   ① **真 bug（缺条件守卫）**：ORIG 0x08546da1 `cmpl $0x3,-0x40(%ebp);
//      jne 0854754b`（直接返回 0），Ghidra C 亦为 `else if (local_44 == 3) {…}`；
//      旧译文在 penaltyType==2 return 后无条件执行破坏流程 ⇒ penaltyType∉{1,2,3}
//      时仍 delete_item/返料。已补 `if (penaltyType == 3) { … }` 包裹。
//   ② **真 bug（数组下标差 1）**：ORIG 0x085471ab `mov -0xa0(%ebp,%eax,4),%eax`
//      （eax=i+4、result 基址 -0xa4）⇒ 条件是 **m_f[i+5]**，与 0x085471dc 的
//      make_pair 第 1 参 &m_f[i+5] 自洽；旧写 m_f[i+4] 漏末元素且误纳 m_f[4]。
//   ③ m_prob20[2]=1 改具名指针局部（复现 0x08546cbb 的 -0x3c 指针槽）；
//      保护券链改 P198① 预清两句式 + if/else-if 链（复现三条 -1 预置）；
//      savedUp 改 unsigned int + 掩码（复现 `movzbl %al,%eax; mov %eax,-0x38`）。
// 残余 1 条为槽基平移 8（-0x40↔-0x48，N95/N126 版本常数）；`call *` 1 处
// vptr+0x8（make_item）与 ORIG 0x0854743d 一致。
// ============================================================================
#include "ItemUpgradeVtableB2/_ProcPenalty.inc"
}

}  // namespace WongWork

// ============================================================================
// CEquipItem::getUpgradeProbIncrease @ 0x0854b552（W，14 字节访问器）
//   return *(int*)((char*)this + 0x1d0)（CEquipItem::m_field1d0）。
// ============================================================================
#include "ItemUpgradeVtableB2/getUpgradeProbIncrease.inc"
}

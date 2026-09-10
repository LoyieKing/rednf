// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEquipItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CEquipItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CEquipItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// STEquipmentScript 内联完整布局为 I1 工作布局（脚本表批次细化），
// 依赖的辅助类（CodePage/CSystemTime/CMTRand 等）由 GameStubs.cpp 提供桩。

#include "CEquipItem.h"
#include "GamePODTypes.h"

#include <set>
#include <math.h>
#include <string.h>

#include "CSystemTime.h"
#include "GameTypes.h"

// ===================== 外部依赖（其它 TU 提供） =====================

#include "CEquipItem/get_rand_int.inc"

#include "CEnvironment.h"  // m_serverEnvir.m_gcNo（+0x198）
#include "GlobalData.h"    // s_systemTime_（+0x941f714；make_item getCurSec 调用）
extern CEnvironment* G_CEnvironment();

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
};
extern CDataManager* G_CDataManager();

// ORIG 0x898c79d：线性插值
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x0898c79d；帧 0x68）：
//   ① vSpan/vLevel/vDiff 三个 int 槽按 ORIG 顺序先算（vDiff=d-c 在 x87 段之前，槽 -0x24）；
//   ② ratio 的钳位是 `ratio <= 0.0`（ORIG `fucompp;fnstsw;test $0x45,%ah;sete;test;je`），
//      不是旧写的 `ratio == 0.0`（后者 4.4 产 fucomip+jne/jp 双跳）；
//   ③ `double diff = (double)vDiff;` 具名局部复现 ORIG `fildl -0x24;fstpl -0x50`。
//   x87 比较 lowering 需 -march=i486 才出 ORIG 的 fnstsw/sahf 形（P230 补；已上报 Main 路由）。
// [b28e 三形实测] 帧/槽：①无 pad ⇒ `sub $0x58`（ORIG 0x68）、diff -0x38、floor 临时 -0x40；
//   ②pad 置首（本形）⇒ `sub $0x68` = ORIG、diff -0x38、临时 -0x50；③`double diff` 迁到
//   三个 int 之后 ⇒ 全槽整体平移（vSpan -0x34 等）、更劣已回退。ORIG 形（diff -0x50、
//   临时 -0x40、帧 0x68）要求 4.1 的「具名 double 深槽 / 临时浅槽」方向，4.4 恒反向
//   （具名 -0x38 / 临时 -0x50），与 0x.. 帧差无关 ⇒ 残差 = 两 double 槽互换，N82附/P112 族。
#include "CEquipItem/getLevelLinearAbility.inc"
}

#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t 权威定义
#include "../../ServerCommon/DNFFunctionLib.h"

namespace user_creature
{
class CCreatureScriptMgr
{
public:
    void RegisterCreatureIdAndItemId(int creatureId, int itemId);
};
CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

namespace ExtreamDungeon
{
class CCompound_ExtreamDun_Item
{
public:
    void ForceResultItemRule_Setting(int idx, bool flag, int value);
};
}

// STEquipmentScript 权威布局见 STEquipmentScript.h（class STEquipmentScript : public STItemScript，
// [b28d-D43] N82/N89 大函数（669/705）（ORIG 0x0850fd84）。
// 基类 0x178 + 派生。CEquipItem::set_item(STEquipmentScript&) 直接使用权威成员名访问，
// 不再保留本文件的 flat 工作布局副本（消除与权威头的重复定义）。
// ===================== 报告函数（CEquipItem.md） =====================

// ORIG WpBonusPointData/ExpBonusPointData ctor/clear 为独立符号（0x08513a6e/0x08513a82 等），
// 由 CEquipItem::C1/STEquipmentScript::C1 调用；故在此 out-of-line 定义以发射符号。
#include "CEquipItem/WpBonusPointData.inc"
#include "CEquipItem/clear.inc"
#include "CEquipItem/ExpBonusPointData.inc"
#include "CEquipItem/clear_1.inc"
#include "CEquipItem/CEquipItem.inc"
}

#include "CEquipItem/dtor_CEquipItem.inc"

#include "CEquipItem/IsAvatarColorVariation.inc"
}

#include "CEquipItem/IsAvatarOptionValid.inc"
}

// [b28d-solo-CEquipItem] ORIG 0x0851388e（34 条，N 封顶）：值全程寄存器直传（无槽往返）
// ⇒ P0 判原码为内联表达式右值；4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为单条
// `mov 0x234(%eax),%eax`（cmp 与 shl 共用 %eax）。4.4 -O0 无该复用：内联形恒发第二次读
// （36 条），register 局部形（P231 合法）改出 `mov %eax,%ebx; cmp %ebx; ja; mov %ebx,%eax`
// 两条中转（同 36 条，掩码段寄存器分配差异）。两形实测（/tmp/p1/t.cpp f_inline/f_reg、
// t2.cpp g1..g7、t3.cpp h1..h6、4.4.6 后端同形）均差 2 条 ⇒ 跨版本 CSE 结构性封顶。
#include "CEquipItem/checkItemUpgradePossible.inc"
}

#include "CEquipItem/check_able_To_equip.inc"
}

#include "CEquipItem/check_eType.inc"
}
#include "CEquipItem/getExpBonusPoint.inc"
}

#include "CEquipItem/getGrowthGrade.inc"
}

#include "CEquipItem/getGrowthRepairCost.inc"
}

// ============================================================================
// CEquipItem::getUpgradeCostDiscount @ 0x822ca80
//   返回 +0x1cc 的 float 折扣率（强化成本折扣；ORIG flds 位读）。
// ============================================================================
#include "CEquipItem/getUpgradeCostDiscount.inc"
}

#include "CEquipItem/getWpBonusPoint.inc"
}
#include "CEquipItem/get_aura_avatar_option_value.inc"
}

#include "CEquipItem/isAmplifyPollutionItem.inc"
}

#include "CEquipItem/isExistUpgradableLevelTag.inc"
}

#include "CEquipItem/isGrowthType.inc"
}

#include "CEquipItem/isUpgradableLevel.inc"
}

#include "CEquipItem/is_aura_avatar_option.inc"
}

#include "CEquipItem/verify.inc"
}

// ===================== make_item（ORIG 0x851098a） =====================

// [b28d-solo-CEquipItem] ORIG 0x0851098a（182/184）：仅剩两处结构性残差，N 封顶。
//   ① 位掩码段（0x08510b7f 段）ORIG 为单次读 `mov 0x234(%eax),%eax`（cmp 与 shl 共用
//      %eax）：4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为一次；4.4 -O0 内联形
//      恒发第二次 `mov 0x8(%ebp),%eax; mov 0x234(%eax),%eax`（+2 条），register 局部形
//      换成 `mov %eax,%ebx`+`mov %ebx,%eax` 两条中转（净差同）⇒ 跨版本同表达式字段读
//      CSE 差异，封顶（探针 /tmp/p1/t.cpp f_inline/f_reg）。
//   ② G_CEnvironment 段（0x08510ba3 段）ORIG 值先入 %eax、&item+0x15 入 %edx；4.4 恒
//      互换（助记符序列全等仅寄存器角色互换 = N7；本 TU 已路由 4.4.6 后端仍存）。
#include "CEquipItem/make_item.inc"
}

// ===================== set_item（ORIG 0x850fd84） =====================

// [b28d-solo-CEquipItem] ORIG 0x0850fd84（669=669 AE；p167 32/32 CLEAR）修复要点：
//   ① 真语义 bug：`(T*)0x941f750` 把全局变量**地址**当对象指针 ⇒ 改取
//      `GlobalData::s_pCompoundBindCube` 的**指针值**（ORIG `mov 0x941f750,%eax`，P237）；
//   ② bool 族源字段化（P136/P300）：STItemScript/STEquipmentScript 的 11 个 char 字段
//      （m_usable[0xb]、m_fieldd0/d9/da/10f、m_6f4/5/6、m_7ac/7bc/888）改 bool 后直存；
//   ③ cMyTrace 全表达式临时链（P227）复现 ctor 前右起求 varargs + 跨 ctor 保活；
//   ④ m_field1d4 包匿名 union float 视图复现单条位拷贝（N107 取址形恒 add+mov）；
//   ⑤ Wp/ExpBonus 类型统一（stBonusPointData_t 副本删除，§4.4）+ trivial op= 整体赋值
//      复现 P246 的 dest 缓存 %ecx 块拷贝；
//   ⑥ m_880 char→unsigned char 复现两段零扩展装载；
//   ⑦ m_experienceAmount float 视图（替旧 `(int)` 转换 7 条）、m_compound110/
//      m_additionalExp120 结构整体赋值（P246）、m_auraOptionType 直存。
#include "CEquipItem/set_item.inc"
}


// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

#include "CEquipItem/GetItemType.inc"
}

#include "CEquipItem/setAvatarOptionSize.inc"
}

#include "CEquipItem/getAvatarOptionSize.inc"
}

#include "CEquipItem/IsAvatarItem.inc"
}

#include "CEquipItem/IsCreatureItem.inc"
}

#include "CEquipItem/IsClearAvatar.inc"
}

#include "CEquipItem/IsEnchantalbe.inc"
}

#include "CEquipItem/getUsableItemType.inc"
}

#include "CEquipItem/isEnableAvatarFunc.inc"
}

#include "CEquipItem/GetGuildPowerWarPoint.inc"
}

#include "CEquipItem/GetUserPowerWarPoint.inc"
}

#include "CEquipItem/IsImpossibleUpgrade.inc"
}

#include "CEquipItem/IsImpossibleAmplify.inc"
}

#include "CEquipItem/IsImpossibleDisjoint.inc"
}

#include "CEquipItem/IsImpossibleGift.inc"
}

#include "CEquipItem/IsRandomOption.inc"
}

#include "CEquipItem/GetRandomOptionGrade.inc"
}

#include "CEquipItem/set_parent_booster_item_id.inc"
}

#include "CEquipItem/get_parent_booster_item_id.inc"
}

#include "CEquipItem/get_endurance.inc"
}

#include "CEquipItem/getAvatarTypeSelect.inc"
}
#include "CEquipItem/getAvatarPeriod.inc"
}

// ORIG 0x8150f36：填充 emblem 的小孔类型。
// 1) 先将 m_vec180（vector<ushort>）逐项写入 emblem.stEmblemSocket[i].emblemSocketType_
// 2) 依 slot 选择 m_avatarSelect 项并写入其 socketType 数组（index 续接）
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x8150f36）：
//   ① 第一循环 ORIG 无 index<5 上限（P140：ORIG 无防御就照抄直访）且 end() 循环前求值一次
//      （P13 缓存 end）；
//   ② slot 范围检查与 empty() 检查是**两个独立 if**（ORIG 各出一段 bool 物化，
//      P133 独立守卫不可合并）；
//   ③ slot==-1 分支同样缓存 end()；
//   ④ 残余：ORIG `add $0x8,%ecx; movzwl 0x4(%eax,%ecx,2)` 是 4.1 对 +0x14 数组下标的
//      add+disp 拆形，4.4 恒折单 disp 0x14(%eax,%edx,2)（P30/N4 结构性伪影，封顶）。
#include "CEquipItem/getAvatarSocket.inc"
}

#include "CEquipItem/GetUsableMaxLevel.inc"
}

// ORIG _ZNK10CEquipItem20getEnchantSystemDataEv W 0x0854b544：返回 +0x1b0（m_enchantVec）
#include "CEquipItem/getEnchantSystemData.inc"
}

// ORIG 0x833eecc：返回 this+0x160（脚本写入的 sub type 选择值，见 set_item 报告）
#include "CEquipItem/get_sub_type.inc"
}

// ORIG W 0x833eea6：flds [this+0x158]，即 m_randomTable.m_field10 的 float 位模式。
#include "CEquipItem/get_experience_amount.inc"
}

// ORIG W 0x833eee8：返回 this+0x168（m_randomTable.m_field20）。
#include "CEquipItem/get_creature_minimum_level.inc"
}

// ORIG W 0x0833eebe：返回 this+0x15c 的 creature species。
#include "CEquipItem/get_creature_species.inc"
}

// ORIG _ZNK10CEquipItem17get_hatch_item_idEv W 0x0833eeda size 0xe（nm -S + objdump
// 实证：push ebp/mov esp,ebp/mov 0x8(%ebp),%eax/mov 0x164(%eax),%eax/pop ebp/ret），
// 即 m_randomTable.m_field1c（蛋孵化物品 id，set_item case 1 由脚本 m_764 写入）。
// 本 TU 提供该 W 符号的强定义；调用方 user_creature::CEgg::Hatch（0x833912a）
// 按 ORIG 形态外联调用它（非内联）。
#include "CEquipItem/get_hatch_item_id.inc"
}

// ===================== RandomItemTable::Set（G6 物品表；ORIG T 0x89bba40）=====================
// [b28d-solo-CEquipItem] ORIG 0x089bba40 逐条还原：
//   1. m_vec4.clear(); m_field0 = 0;
//   2. 遍历 source：pair 默认构造；pr.first = *it；it++；
//      若 it == end 则 break；否则 m_field0 += *it；
//      **pr.second = m_field0**（累计权重上界，GetRandomItemIndex 依此判定区间；
//      旧重建漏写该赋值 = 真语义 bug）；m_vec4.push_back(pr)；it++。
// 增量用后置 it++（ORIG 增量段为 operator++(int)/ppEi，P28 增量侧推论）。
#include "CEquipItem/Set.inc"
}

// ===================== RandomItemTable::GetRandomItemIndex（ORIG 0x089bbb6c，const）=====================
// 遍历 m_vec4（pair<int,int> 数组，+0x04）：找第一个 it->second > index 的元素，
// 返回其 it->first；未命中返回 0。SetEggItem 依赖此把随机权重映射到物品 index。
#include "CEquipItem/GetRandomItemIndex.inc"
}

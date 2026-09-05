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
// ============================================================================
bool expert_job::CAlchemist::IsExistContinuousEffectItem(CUser* user,
                                                         int itemId)
{
    if (((const CUserCharacInfo*)user)->getCurCharacR() == 0)
    {
        return false;
    }
    const std::vector<ContinuousItemInfo>* list =
        ((const CUserCharacInfo*)user)->GetCurCharacEffectItemListR();
    for (std::vector<ContinuousItemInfo>::const_iterator it = list->begin();
         it != list->end(); ++it)
    {
        if (it->m_itemIdx == itemId)
        {
            return (unsigned)GlobalData::s_systemTime_.getCurSec() <
                   it->m_expireTime;
        }
    }
    return false;
}

// ============================================================================
// expert_job::CAlchemist::GetItemUpgradeSuccessRate @ 0x085d1360
//   return (float)g_alchemistInfo / *(float*)0x08cbfa3c (= 9840.0f)
// ============================================================================
float expert_job::CAlchemist::GetItemUpgradeSuccessRate()
{
    static const float divisor = 9840.0f;  // ORIG .rodata 0x08cbfa3c
    return (float)g_alchemistInfo[0] / divisor;   // 合并后 int[2]；低半区 [0] 为原标量
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
int getItemSellPrice(int price, int a, int b, bool flag)
{
    char sel = 0;
    if (flag)
    {
        sel = 2;
    }

    if (a > b)
    {
        a = b;
    }

    if (b == 0)
    {
        return itemPriceRates_[sel] * price / 1000;
    }
    return itemPriceRates_[sel] * price / 1000 * a / b;
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
bool CStackableItem::IsReinforceTicket()
{
    return m_char30f;
}

unsigned char CStackableItem::GetReinforceRate()
{
    return (unsigned char)m_char312;
}

bool CStackableItem::IsAmplifyReinforceTicket()
{
    return m_char313;
}

unsigned char CStackableItem::GetAmplifyReinforceRate()
{
    return (unsigned char)m_char316;
}

unsigned short CStackableItem::GetReinforceVaule()
{
    return m_short310;
}

unsigned short CStackableItem::GetAmplifyReinforceValue()
{
    return m_short314;
}

// ============================================================================
// DisJointResult ctor @ 0x082347a2：循环 i=0..4 将 m_f[i] 与 m_f[i+1] 清零
// （等价整体清零 10 个 int）。
// ============================================================================
DisJointResult::DisJointResult()
{
    for (int i = 0; i <= 4; ++i)
    {
        m_f[i] = 0;
        m_f[i + 5] = 0;
    }
}

// ============================================================================
// DisJoint::GetResult @ 0x084733f6
//   out.m_f[5] = m_cubeIndexVecs[4][0]；
//   out.m_f[0] = ROUND(((ROUND(1.1f*sellPrice) 经 getItemSellPrice 折算 ×1000 ÷
//                m_creationConst) * m_creationRate[rarity]) ÷ 1000.0f)，最小 1；
//   _GetAdditionalItem(item, &out.m_f[6], &out.m_f[1])。
//   物品缺失走 LogManager::logFormat(1,…,0x6e,"[DisJoint::GetResult] Null Item\n")
//   并返回 0x11。
// ============================================================================
int DisJoint::GetResult(int itemIdx, DisJointResult& out)
{
    CItem* script = G_CDataManager()->find_item(itemIdx);
    if (script == 0)
    {
        LogManager::logFormat(1, "DisJoint.cpp",
                              __PRETTY_FUNCTION__,
                              0x6e, "[DisJoint::GetResult] Null Item\n");
        return 0x11;
    }

    const int rarity = script->get_rarity();
    // m_creationRate[rarity]（+0x538+rarity*4）
    const float rate = *reinterpret_cast<const float*>(
        reinterpret_cast<const char*>(this) + rarity * 4 + 0x538);
    out.m_f[5] = m_cubeIndexVecs[4][0];

    static const float priceBias = 1.1f;    // ORIG .rodata 0x08c6ff50
    static const float thousand = 1000.0f;  // ORIG .rodata 0x08c6ff54

    const float scaled = priceBias * (float)script->GetSellPrice();
    // ORIG 0x08473490 区：fnstcw/fldcw 置截断模式后 fistpl —— 截断（非四舍五入）。
    const int sell = getItemSellPrice((int)scaled, 0, 0,
                                      script->IsRandomOption());
    // ORIG：(sell*1000)/creationConst 为整数除法后再进浮点，fistpl 截断。
    const int money = (int)(((float)((sell * 1000) / m_creationConst) *
                             rate) / thousand);  // 截断（ORIG 无 +0.5）
    out.m_f[0] = money < 1 ? 1 : money;

    _GetAdditionalItem(script, out.m_f[6], out.m_f[1]);
    return 0;
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

// ============================================================================
// 类内裸区访问辅助（约定同 CtorProducers_2C.cpp：表区 this+0x04、
// 随机器指针 this+0x4e8、randInt 上界 this+0x4ec）。
// ============================================================================
namespace
{

inline CMTRand* upgradeRand(const WongWork::CItemUpgrade* self)
{
    return *reinterpret_cast<CMTRand* const*>(
        reinterpret_cast<const char*>(self) + 0x4e8);
}

inline const unsigned long& randIntBound(
    const WongWork::CItemUpgrade* self, unsigned long& storage)
{
    storage = *reinterpret_cast<const unsigned long*>(
        reinterpret_cast<const char*>(self) + 0x4ec);
    return storage;
}

// 表区（this+0x04）以 upgrade_table_t 视图访问：ORIG
// WongWork::CItemUpgradeTable::getUpgradeInfoTable W 0x0854b382 即返回自身。
inline const upgrade_table_t& embeddedTable(const WongWork::CItemUpgrade* self)
{
    return *reinterpret_cast<const upgrade_table_t*>(
        reinterpret_cast<const char*>(self) + 0x04);
}

// 表区子对象视图（+0x04）：ORIG ProcUpgrade 0x08547ca4 `add $0x4,%eax` +
// call _ZNK8WongWork17CItemUpgradeTable19getUpgradeInfoTableEv（W 0x0854b382）。
// 必须走真实成员函数调用（inline 视图代返会丢该 call，P167 差集实证）。
inline const WongWork::CItemUpgradeTable* embeddedUpgradeTable(
    const WongWork::CItemUpgrade* self)
{
    return reinterpret_cast<const WongWork::CItemUpgradeTable*>(
        reinterpret_cast<const char*>(self) + 0x04);
}

}  // namespace

namespace WongWork
{

// ============================================================================
// _getUpgradeCountByUpgradeItem @ 0x08548e68
//   默认按材料 idx 映射强化档位（仅非增幅物品）；材料为堆叠物时改读
//   ReinforceTicket/AmplifyReinforceTicket 数值（[0,0x1f] 越界归零）。
//   增幅券在目标无增幅能力时报 ENUM_ERROR 0x13。
// ============================================================================
unsigned short CItemUpgrade::_getUpgradeCountByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    short count = 0;
    const bool amplify = target.m_amp.hasAbility();
    if (!amplify)
    {
        switch (material.m_addInfo)
        {
            case 0x1d29: count = 7;  break;
            case 0x1c6a: count = 10; break;
            case 0x1c6b: count = 12; break;
            case 0x1c69: count = 7;  break;
            case 0x1e40: count = 5;  break;
            case 0x1e86: count = 13; break;
            case 0x1e3f: count = 3;  break;
            default:     count = 0;  break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG 0x08548f70 `cwtl` + int 槽 -0x10 + `cmpl` 区间判定，末了
            // `mov %ax,-0x1a` 落 short ⇒ 票面值先入 int 局部（P10/P11）。
            const int value = (short)stack->GetReinforceVaule();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!amplify)
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (short)stack->GetAmplifyReinforceValue();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
    }
    return (unsigned short)count;
}

// ============================================================================
// _getUpgradeRateByUpgradeItem @ 0x08548ffa
//   同上结构；映射命中直接返回 100；票券读 Rate 字段（[0,100] 越界归零）。
// ============================================================================
unsigned char CItemUpgrade::_getUpgradeRateByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    char rate = 0;
    const bool amplify = target.m_amp.hasAbility();
    if (!amplify)
    {
        // ORIG 0x0854901d..0x08549059 是 switch 二分决策树（首比较 0x1d29、
        // `sub $0x1c69; cmp $0x2; ja` 小区间、0x1e3f/0x1e40 jb/jbe、0x1e86），
        // 命中共享叶 `movb $0x64,-0x19; movzbl; jmp 返回`；default 叶
        // `movb $0,-0x19` 后**继续**执行（不返回）⇒ 必须写 switch + case 升序
        // （P46/P163），布尔区间链会退化成 cmp 序列且丢 18 insn。
        switch (material.m_addInfo)
        {
            case 0x1c69:
            case 0x1c6a:
            case 0x1c6b:
            case 0x1d29:
            case 0x1e3f:
            case 0x1e40:
            case 0x1e86:
                rate = 100;
                return (unsigned char)rate;
            default:
                rate = 0;
                break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG `movsbl` 到 int 槽后 cmpl 判定，末了 movb 落 char ⇒ int 中转。
            const int value = (char)stack->GetReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = (char)value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!amplify)
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (char)stack->GetAmplifyReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = (char)value;
            }
        }
    }
    return (unsigned char)rate;
}

// ============================================================================
// _CheckTargetUsableLv @ 0x08548c3c（const）
//   材料（param_1）脚本可堆叠且 +0x320 标志为 1 时，要求目标（param_2）为
//   可装备类型且其可用等级落在材料脚本 [m_int318, m_int31c] 区间内。
// ============================================================================
bool CItemUpgrade::_CheckTargetUsableLv(const Inven_Item& material,
                                        const Inven_Item& target) const
{
    // ORIG（Ghidra uVar5）各分支写同一结果槽 + 统一出口（P63）：散布 return 让
    // 4.4 为每个出口生成独立 epilogue 中继，实测 result 汇聚形 78/73 优于
    // 散布形 78/71；余 5 insn 为 m_char320 的 xor$1/test 布尔物化（4.1 对
    // 值域 [0,1] 字节比较的发形，4.4 恒 cmp $0x1 → N6/N18b 族）。
    bool result;
    CItem* matScript = G_CDataManager()->find_item(material.m_addInfo);
    if (matScript == 0)
    {
        result = false;
    }
    else if (!matScript->is_stackable())
    {
        result = true;
    }
    else
    {
        const CStackableItem* stack = (const CStackableItem*)matScript;
        if (stack->m_char320 != 1)
        {
            result = true;
        }
        else
        {
            CItem* tgtScript = G_CDataManager()->find_item(target.m_addInfo);
            if (tgtScript == 0)
            {
                result = false;
            }
            else if (!target.isEquipableItemType())
            {
                result = false;
            }
            else
            {
                // 目标可用等级须位于材料脚本的区间 [m_int318, m_int31c]。
                const int usable = tgtScript->getUsableLevel();
                result = !(usable < stack->m_int318 ||
                           stack->m_int31c < usable);
            }
        }
    }
    return result;
}

// ============================================================================
// _ProcPortableUpgradeItem @ 0x08548d3c（const）
//   便携带升级券（action 类型 0x35）：校验通过后把 info 改写为
//   {recipe=材料 idx, count=1, discount=0} 并返回 0。
// ============================================================================
int CItemUpgrade::_ProcPortableUpgradeItem(const Inven_Item& material,
                                           const Inven_Item& target,
                                           upgrade_info_t& info) const
{
    static const int MaxPortableItemActionIntCount = 2;  // ORIG _ZZNK8WongWork12CItemUpgrade24_ProcPortableUpgradeItem…E29MaxPortableItemActionIntCount
    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script == 0)
    {
        return 4;
    }
    if (!script->is_stackable())
    {
        return 0;
    }
    CStackableItem* stack = (CStackableItem*)script;
    if (!stack->verifyStackableAction((StackableAction::Type)0x35))
    {
        return 0;
    }
    const std::vector<int>& levels = stack->m_vec3a0;
    if (levels.size() != MaxPortableItemActionIntCount)
    {
        return 4;
    }
    const int minUp = levels[0];
    const int maxUp = levels[1];
    const int up = (int)target.GetUpgrade();
    if (up < minUp || maxUp < up)
    {
        return 0x13;
    }
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&info) + 0x34) = 0;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&info) + 0x30) = 1;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&info) + 0x2c) =
        material.m_addInfo;
    return 0;
}

// ============================================================================
// _DoProcUpgrade @ 0x0854755a
//   以 info+0x20 的失败概率经事件/炼金/装备三段修正后掷骰：
//   roll >= prob → IncUpgrade 且返回 true（强化成功）；否则保持等级并返回
//   false（调用方走 _ProcPenalty）。
// ============================================================================
bool CItemUpgrade::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                  const upgrade_info_t* info)
{
    unsigned long range = *reinterpret_cast<const unsigned long*>(
        reinterpret_cast<const char*>(this) + 0x4ec);

    unsigned int prob =
        *reinterpret_cast<const unsigned int*>(
            reinterpret_cast<const char*>(info) + 0x20);
    GlobalData::s_pEvent_UpGrade_Jar->GetProbability(
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(info) +
                                      0x2c),
        prob);

    if (prob != 0 &&
        GlobalData::s_globalEffectManager_->isAffectedEffect(3))
    {
        const float effect =
            GlobalData::s_globalEffectManager_->getEffectValue(3);
        // ORIG：shifted = fistp(range*effect)；delta = shifted - range；
        //       prob = max(prob - delta, 0)（有符号钳零）。
        const int shifted = (int)((float)(int)range * effect);  // fistp
        const int delta = shifted - (int)range;
        int adjusted = (int)prob - delta;
        if (adjusted < 0)
        {
            adjusted = 0;
        }
        prob = (unsigned int)adjusted;
    }

    if (prob != 0 &&
        expert_job::CAlchemist::IsExistContinuousEffectItem(user, 0x27ac59))
    {
        const int reduced =
            (int)((float)expert_job::CAlchemist::GetItemUpgradeSuccessRate() *
                  (float)(int)range);  // fistp 四舍五入
        int adjusted = (int)prob - reduced;
        if (adjusted < 0)
        {
            adjusted = 0;
        }
        prob = (unsigned int)adjusted;
    }

    const CInventory* invenR = ((CUserCharacInfo*)user)->getCurCharacInvenR();
    Inven_Item worn = invenR->GetInvenSlot(0, 0xb);
    CItem* equipScript = G_CDataManager()->find_item(worn.m_addInfo);
    if (equipScript != 0 && prob != 0)
    {
        // W 0x0854b552：失败概率 -= 装备 +0x1d0 强化概率提升值。
        prob -= (unsigned)((CEquipItem*)equipScript)->getUpgradeProbIncrease();
        if (*reinterpret_cast<const unsigned*>(
                reinterpret_cast<const char*>(this) + 0x4ec) < prob)
        {
            prob = 0;
        }
    }

    bool rollWin = false;
    if (prob != 0)
    {
        // ORIG：mov 0x4ec（m_randMax）存栈 → mov 0x4e8（m_rand）→ randInt(&bound)。
        unsigned long boundStorage = (unsigned long)m_randMax;
        const unsigned int roll = (unsigned int)m_rand->randInt(boundStorage);
        rollWin = roll < prob;
    }
    if (!rollWin)
    {
        item->IncUpgrade();
    }
    return !rollWin;
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
//   MSG_ITEM_UPGRADE +0x1a/+0x1b/+0x1d/+0x21/+0x23 字段化已由 Main 代落
//   PacketTypes.h:807-812；本函数无裸偏移读（仅 CEnvironment+0x378 待补）。
//
// 物化口径（P183/P167 实证）：ORIG 对材料/保护券槽各发
//   Inven_Item C2（0x080cb854）+ GetInvenSlot 临时槽（-0x2c8）+ 15×movl+1×movb
//   逐字段拷贝，共四处独立物化块（-0x15e/-0x19b/-0x1d8/-0x215）；重建端一律
//   「默认构造 + 赋值」两步形，禁 `T x = f()` 直构（会折叠 sret 目标，丢整块）。
// ============================================================================
int CItemUpgrade::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                              PacketGuard* pg)
{
    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();

    Inven_Item target = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                             msg->m_targetSlot);
    if (target.isEmpty())
    {
        return 4;
    }
    if (target.m_addInfo != msg->m_targetItemIdx)
    {
        return 4;
    }

    Inven_Item material;
    material = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot);
    if (material.isEmpty())
    {
        return 0x16;
    }

    Event_Upgrade_Jar* jar = GlobalData::s_pEvent_UpGrade_Jar;
    if (!jar->CheckMaxLuckyLevel(target, material.m_addInfo))
    {
        return 0x3f4;
    }
    jar->CheckMaterialIndex(material.m_addInfo);

    ENUM_ERROR err = ENUM_ERROR_NONE;              // -0xdc
    msg->m_25 = _getUpgradeCountByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }
    msg->m_27 = _getUpgradeRateByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }

    CItem* script = G_CDataManager()->find_item(target.m_addInfo);
    if (script == 0)
    {
        return 4;
    }

    Inven_Item ticket;                             // -0x19b
    char ticketFlag = 0;                           // -0x45
    if (msg->m_ticketSlot != 0xffff)
    {
        if (!script->IsPossibleKiriProtection())   // CItem vtable+0x44
        {
            return 0xea;
        }
        ticket = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticket.isEmpty())
        {
            return 4;
        }
        std::map<unsigned int, stUpgradeItemTicketParam_t>& tickets =
            G_CDataManager()->m_serverParameter.m_ticket468;
        std::map<unsigned int, stUpgradeItemTicketParam_t>::iterator it =
            tickets.find(ticket.m_addInfo);
        if (it == tickets.end())
        {
            return 9;
        }
        // ORIG 每次 it->second 都重调 _Rb_tree_iterator::operator->（3 次：
        // 0x08547c15/0x08547c3b/0x08547c6b）、GetUpgrade 重调 2 次 ⇒ 表达式内
        // 逐点写，禁 &it->second 缓存（P183）。
        if (target.GetUpgrade() < it->second.m_view.m_minUpgrade ||
            it->second.m_view.m_maxUpgrade < target.GetUpgrade())
        {
            return 8;
        }
        ticketFlag = (char)it->second.m_view.m_ticketFlag;
    }

    // 升级表：增幅物品用 CItemAmplifier 表，否则表区子对象的 getUpgradeInfoTable。
    const upgrade_table_t* table;
    if (!target.m_amp.hasAbility())
    {
        table = embeddedUpgradeTable(this)->getUpgradeInfoTable();
    }
    else
    {
        table = GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    }

    upgrade_info_t info;                           // -0x284（ctor 自清零）
    const int level = ((const CUserCharacInfo*)user)->get_charac_level();
    if (!_GetUpgradeInfo(&target, script, *table, &info, level))
    {
        return 0xd;
    }

    // 【ORIG 怪癖】payBase/matCount 仅 upCount!=0 时赋值，其余路径沿用旧栈值。
    int payBase;                // -0x250 金钱基数
    int matCount;               // -0x254 材料消耗数量
    int ticketMatIdx;           // -0x258（ORIG 写入后无读者：死写入照抄，P171）
    if (msg->m_25 != 0)
    {
        payBase = 0;
        matCount = 1;
        ticketMatIdx = (int)material.m_addInfo;
    }
    (void)ticketMatIdx;

    if (!_CheckTargetUsableLv(material, target))
    {
        return 0x13;
    }
    const int portable = _ProcPortableUpgradeItem(material, target, info);
    if (portable != 0)
    {
        return portable;
    }

    if (ticket.m_addInfo == 0x28d30e || ticket.m_addInfo == 0x28d30f)
    {
        payBase = 0;
    }

    jar->SettingRecipeNUpConunt(material.m_addInfo, info,
                                target.GetUpgrade());

    if (msg->m_upgradeType == 0 && script->IsImpossibleUpgrade())
    {   // CItem vtable+0x30
        return 0x13;
    }
    if (msg->m_upgradeType == 1 && script->IsImpossibleAmplify())
    {   // CItem vtable+0x34
        return 0x13;
    }

    // 特殊券覆盖材料消耗数（第一处 @0x08547e89，删除前第二次见 @0x085483b4）。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketAgain;                    // -0x1d8
        ticketAgain =
            invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        // ORIG 各分支直发 movl $imm,-0x254（0x08547f9e/0x08547faa/0x08547fce/
        // 0x08547fda）⇒ if/else 独立赋值，禁三目（三目物化「先备值再 store」
        // 两条，本函数 4 处共 +4 insn）。
        if (ticketAgain.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketAgain.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
    }

    int cond = 0;                                  // -0x3c
    cond = _CheckCondition(user, target, script, material, *table, info);
    if (cond != 0)
    {
        return cond;
    }
    if (msg->m_upgradeType == 0 && target.m_amp.hasAbility())
    {
        return 0x17;
    }
    if (msg->m_upgradeType == 1 && !target.m_amp.hasAbility())
    {
        return 0x17;
    }

    pg->put_byte(1);

    const int oldUp = (int)target.GetUpgrade();    // -0x38（int 槽）
    bool upgraded = false;                         // -0x32
    char winFlag = 0;                              // -0x31

    if (msg->m_25 != 0)
    {
        upgraded = true;
        // ORIG 上界是字面量 99（movl $0x63,-0xd0），非 _DoProcUpgrade 的 +0x4ec。
        unsigned long bound = 99;
        if (upgradeRand(this)->randInt(bound) < msg->m_27)
        {
            target.SetUpgrade((unsigned char)msg->m_25);
            winFlag = 1;
        }
        else
        {
            target.SetUpgrade((unsigned char)oldUp);
            winFlag = 0;
        }
    }
    else
    {
        upgraded = _DoProcUpgrade(user, &target, &info);

        int pay = payBase;                         // -0x2c
        // ORIG 无判空防御（0x085481bc 直接解引用返回值，P140）。
        CEventBase* repeat =
            GlobalData::s_event_manager->GetRepeatEvent(0x32);
        if (repeat->IsEventing((CUser*)0) && !target.m_amp.hasAbility())
        {
            // 第二次 GetRepeatEvent（0x0854820c）：ORIG 不缓存返回值（P48）。
            const int rate =
                ((CReduceUpgradeItemPay*)
                    GlobalData::s_event_manager->GetRepeatEvent(0x32))
                    ->GetReduceRate();
            if (rate > 0)
            {
                pay = payBase - (payBase * rate) / 100;
                if (pay < 0)
                {
                    pay = 0;
                }
            }
        }
        else
        {
            pay = payBase;
        }

        // 减费装备来自**右槽**（0x08548ad3 getCurCharacInvenR，type=0/slot=0xb）。
        Inven_Item worn =
            ((const CUserCharacInfo*)user)
                ->getCurCharacInvenR()
                ->GetInvenSlot((INVEN_TYPE)0, 0xb);
        const int wornIdx = (int)worn.m_addInfo;   // -0x28
        CItem* discScript = G_CDataManager()->find_item(wornIdx);
        if (discScript != 0)
        {
            const float discount =
                ((CEquipItem*)discScript)->getUpgradeCostDiscount();
            // _DAT_08c9d304 = -100.0f（objdump flds 立即数直读，P185）。
            pay = (int)((discount * (float)pay) / -100.0f + (float)pay);
        }

        if (target.m_amp.hasAbility())
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0x2e, true) != 1)
            {
                return 10;
            }
        }
        else
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0xd, true) != 1)
            {
                return 10;
            }
        }
    }

    // 特殊券覆盖材料消耗数（第二处 @0x085483b4）+ 删券。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketDel;                      // -0x215
        ticketDel = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticketDel.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketDel.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
        if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, msg->m_ticketSlot, 1, (eItemDelReason)3,
                true) != 1)
        {
            return 0x11;
        }
        user->m_historyLog.apply_Item(ticket.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)0);
    }

    if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, msg->m_materialSlot, matCount,
            (eItemDelReason)0xc, true) != 1)
    {
        return 0x11;
    }
    if (msg->m_25 != 0)
    {
        user->m_historyLog.apply_Item(material.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)1);
    }

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, (unsigned int)payBase);

    pg->put_byte((int)msg->m_upgradeType);
    pg->put_short((int)msg->m_materialSlot);
    // 回包的 add_info 取自**材料槽**当前物品，且 ORIG 在此重取 invenW
    // （0x08548ee7 call getCurCharacInvenW → 0x08548f0b GetInvenSlot(1, +0x23)）。
    pg->put_int(((CUserCharacInfo*)user)
                    ->getCurCharacInvenW()
                    ->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot)
                    .get_add_info());
    pg->put_short((int)msg->m_ticketSlot);

    if (msg->m_25 == 0)
    {
        if (jar->isNewNPC())
        {
            pg->put_byte(2);
        }
        else if (jar->isFortune())
        {
            pg->put_byte(3);
        }
        else
        {
            pg->put_byte(0);
        }
    }
    else
    {
        pg->put_byte(1);
    }
    pg->put_byte(oldUp);

    if (table->m_bodyView.m_named.m_noticeUpgradeLevel <=
            (int)target.GetUpgrade() &&
        (msg->m_25 == 0 || winFlag != 0))
    {
        _NoticeUpgrade(user, target, upgraded);
    }

    if (target.GetUpgrade() > 12)
    {
        const unsigned int logUpgraded = (unsigned int)upgraded;  // -0x2d4
        const unsigned int logUpgrade =
            (unsigned int)target.GetUpgrade();                    // -0x2d0
        const unsigned int envField =
            *reinterpret_cast<const unsigned int*>(
                reinterpret_cast<const char*>(G_CEnvironment()) +
                0x378);  // [裸偏移] CEnvironment+0x378（具名字段待补）
        DB_ReqItemUpgradeLog::makeRequest(
            envField, target.m_addInfo,
            ((const CUserCharacInfo*)user)->getCurCharacNo(),
            ((const CUserCharacInfo*)user)->getCurCharacName(), logUpgrade,
            logUpgraded != 0);
    }

    if (upgraded)
    {
        if (msg->m_25 == 0)
        {
            pg->put_byte(0);
        }
        else if (winFlag == 0)
        {
            pg->put_byte(1);
        }
        else
        {
            pg->put_byte(0);
        }
        pg->put_byte((int)target.GetUpgrade());
        pg->put_short((int)msg->m_targetSlot);
        if (msg->m_25 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0xc, 0,
                (ENUM_PACKETCLASS)0);
        }
    }
    else
    {
        int sellPrice = 0;                         // -0xe4
        G_Store()->GetSellItemPrice(target, script,
                                    (short)target.m_addInfo2, false,
                                    sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);

        const int penalty =
            _ProcPenalty(user, &target, script, msg->m_targetSlot, *table,
                         &info, pg, &ticket, ticketFlag);
        if (penalty != 0)
        {
            if (penalty != 0xd8)
            {
                return penalty;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)msg->m_materialSlot);
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, 0);
            return penalty;
        }
    }

    user->m_historyLog.UpgradeItem(script->get_index(), oldUp,
                                   (int)target.GetUpgrade(), upgraded);

    if (script->IsRandomOption())                  // CItem vtable+0x4c
    {
        user->makeGuildItemUpgradMessage(msg->m_28, oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }
    else
    {
        user->makeGuildItemUpgradMessage(script->GetItemName(), oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }

    if (target.m_addInfo != 0)
    {
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_targetSlot, target);
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    return 0;
}

// ============================================================================
// _getAdditionalDisjointItem @ 0x08546938
//   grade 与表内基准差值达到阈值且强化级足够时，按 rarity 权重浮点计算
//   附加分解物数量并压入 items（{item=table[0x238], count}）。
// ============================================================================
void CItemUpgrade::_getAdditionalDisjointItem(unsigned int up,
                                              const CItem* itemScript,
                                              std::vector<std::pair<int, int> >& items,
                                              const upgrade_table_t& table)
{
    // 【切片转写 0x08546938..0x08546a95】表项全部走具名字段直访（inline
    // 访问器在 -O0 每点额外发 call）；**语义纠错**：pow 的底数是
    // `up - table.m_additionalMinUpgrade`（0x085469db..0x085469f8：先取 +0x258，
    // 与形参 up(0xc(%ebp)) 相减、符号扩展成 long long 后 fildll），
    // 此前误用钳零后的 grade 差。
    const int grade = itemScript->get_grade();

    // 0x0854696f..0x0854698a：`lea -0x24(&diff); lea -0x20(&zero); call max;
    // mov (%eax)` ⇒ ORIG 用 std::max 模板（两实参各取地址，故 zero 必须是
    // 具名 int 局部）；写成 if 钳零会少 6 insn。
    // get_rarity **两次独立调用**（0x0854695a 算 grade 差、0x085469bf 选权重
    // 槽）⇒ 不命名 rarity 局部（P48/P183）。
    int diff = grade -
               table.m_bodyView.m_named.m_baseGrade[itemScript->get_rarity()];
    int zero = 0;
    const int clamped = std::max(diff, zero);

    if ((unsigned int)table.m_bodyView.m_named.m_additionalGradeGap <=
            (unsigned int)clamped &&
        (unsigned int)table.m_bodyView.m_named.m_additionalMinUpgrade < up)
    {
        // ORIG 的 make_pair 实参是 `Rm Rj`（unsigned long& / unsigned int&
        // 左值）⇒ pair 由转换 ctor C2<unsigned long,unsigned int> 落成
        // pair<int,int>（0x08546a5c/0x08546a71 两个 call）⇒ 两量必须是有类型
        // 的具名局部，不能是表达式直传。
        unsigned long itemId =
            (unsigned long)table.m_bodyView.m_named.m_additionalItemId;
        const float weight = table.m_bodyView.m_named
                                 .m_additionalWeight[itemScript->get_rarity()];

        // pow(up - +0x258, 2.0)：64 位底数（`fildll -0x40`）+ _DAT_08c9d308=2.0；
        // 两级取整分别是 fistpl（32 位）与 fmuls weight 后 fistpll（64 位），
        // 均切 RC=nearest 控制字 ⇒ +0.5 近似（P176 同类，无合法 ROUND 源形）。
        const long long span =
            (long long)up -
            (long long)table.m_bodyView.m_named.m_additionalMinUpgrade;
        const double powered = std::pow((double)span, 2.0);
        unsigned int count =
            (unsigned int)((float)(int)(powered + 0.5) * weight + 0.5f);
        if (count != 0)
        {
            items.push_back(std::make_pair(itemId, count));
        }
    }
}

// ============================================================================
// _ProcPenalty @ 0x08546bba
//   惩罚分派：1=降级（保底）、2=降级（可到 0）、3=破坏（保护券或分解返料）。
// ============================================================================
int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               const upgrade_table_t& table,
                               const upgrade_info_t* info, PacketGuard* pg,
                               Inven_Item* ticketCopy, char ticketFlag)
{
    int penaltyType;
    if (ticketCopy->m_addInfo == 0)
    {
        penaltyType = table.GetPenaltyType(item->GetUpgrade(),
                                           (ENUM_RARITY)itemScript->get_rarity(),
                                           info);
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            penaltyType = 2;
        }
    }
    else
    {
        penaltyType = (ticketFlag == 0) ? 1 : 2;
    }
    pg->put_byte((int)(char)penaltyType);

    if (penaltyType == 1)
    {
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    if (penaltyType == 2)
    {
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            *reinterpret_cast<int*>(
                const_cast<char*>(reinterpret_cast<const char*>(info) +
                                  0x28)) = 1;
        }
        const int drop =
            *reinterpret_cast<const int*>(
                reinterpret_cast<const char*>(info) + 0x28);
        const unsigned char up = item->GetUpgrade();
        bool toZero;
        if (drop >= (int)up)
        {
            toZero = true;
        }
        else
        {
            toZero = ticketFlag < 0;
        }
        if (toZero)
        {
            item->SetUpgrade(0);
        }
        else if (ticketFlag <= 0)
        {
            item->SetUpgrade((unsigned char)((int)up - drop));
        }
        else
        {
            item->SetUpgrade((unsigned char)((int)up - (int)ticketFlag));
        }
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    // ---- penaltyType == 3：破坏 ----
    if (!item->m_amp.hasAbility())
    {
        // 保护券链：0x28be7e → 0x1f72 → 0x20ba，末位 0x292091ac 优先覆盖。
        int protectSlot =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x28be7e);
        int slot172 =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x1f72);
        int slot20ba =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x20ba);
        protectSlot = protectSlot != -1
                          ? protectSlot
                          : (slot172 != -1 ? slot172
                                           : (slot20ba != -1 ? slot20ba : -1));
        const int slot292 =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x292091ac);
        if (slot292 != -1)
        {
            protectSlot = slot292;
        }
        if (protectSlot != -1)
        {
            CInventory* invenW =
                ((CUserCharacInfo*)user)->getCurCharacInvenW();
            Inven_Item used = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, protectSlot);
            if (used.m_addInfo != 0x28be7e && used.m_addInfo != 0x1f72 &&
                used.m_addInfo != 0x20ba && used.m_addInfo != 0x292091ac)
            {
                return 0x11;
            }
            if (invenW->delete_item(INVEN_TYPE_EQUIP, protectSlot, 1,
                                    eItemDelReason_3, true) != 1)
            {
                return 0x11;
            }
            user->m_historyLog.apply_Item(used.m_addInfo, item->m_addInfo,
                                          eApplyItemReason_0);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 protectSlot);
            item->SetUpgrade(0);
            ((CUserCharacInfo*)user)->getCurCharacInvenW()->update_item(
                INVEN_TYPE_EQUIP, slot, *item);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)slot);
            return 0xd8;
        }
    }

    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();
    if (invenW->delete_item(INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_0xd,
                            true) != 1)
    {
        return 0x11;
    }
    const unsigned char savedUp = item->GetUpgrade();
    item->SetUpgrade(0);
    pg->put_byte(0);
    pg->put_short((int)slot);

    DisJointResult result;
    if (G_CDataManager()->m_disJoint->GetResult(item->m_addInfo, result) != 0)
    {
        pg->put_byte(0);
        return 0;
    }
    item->m_addInfo = 0;

    std::vector<std::pair<int, int> > items;
    items.reserve(6);
    for (int i = 0; i <= 4; ++i)
    {
        if (result.m_f[i + 4] != 0)
        {
            items.push_back(std::make_pair(result.m_f[i + 5], result.m_f[i]));
        }
    }
    _getAdditionalDisjointItem(savedUp, itemScript, items, table);
    if (item->m_amp.hasAbility())
    {
        _getAdditionalDisjointItem(savedUp, itemScript, items,
                                   embeddedTable(this));
    }

    std::vector<int> slots;
    // ORIG 尾参 a/b 为 .rodata 空串 0x08c9c3a0（邮件标题/正文查找键）。
    static const char kEmptyKey[1] = "";
    const int rc = invenW->insert_event_items(items, slots,
                                              (eMoneyAddReason)7,
                                              eItemAddReason_0x13, false,
                                              kEmptyKey, kEmptyKey);
    pg->put_byte(rc);

    // insert_event_items 输出三元组 {slot, itemIdx, addInfo2/count}。
    for (size_t k = 0; k < slots.size(); k += 3)
    {
        const int outSlot = slots[k];
        const int outIdx = slots[k + 1];
        const int outCnt = slots[k + 2];
        pg->put_short(outSlot);
        pg->put_item_idx((unsigned long)outIdx);
        pg->put_int(outCnt);

        CItem* outScript = G_CDataManager()->find_item(outIdx);
        if (outScript != 0)
        {
            Inven_Item tmp;
            tmp.m_addInfo = outIdx;
            tmp.set_add_info(outCnt);
            outScript->make_item(tmp);
            GetInstanceCubeStatistic()->collectCubeStatistics(
                outScript, outCnt, user, (CUBE_STATISTIC_FIELD)1);
            int price = 0;
            G_Store()->GetSellItemPrice(tmp, outScript,
                                        (short)tmp.get_add_info(), false,
                                        price);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)9, user, (unsigned int)price);
        }
    }
    return 0;
}

}  // namespace WongWork

// ============================================================================
// CEquipItem::getUpgradeProbIncrease @ 0x0854b552（W，14 字节访问器）
//   return *(int*)((char*)this + 0x1d0)（CEquipItem::m_field1d0）。
// ============================================================================
int CEquipItem::getUpgradeProbIncrease() const
{
    return m_field1d0;
}

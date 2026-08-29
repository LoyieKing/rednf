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

#include <cmath>
#include <cstring>
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
// getItemSellPrice @ 0x0898c86e（自由函数）
//   sel = flag ? 2 : 0；a = min(a,b)；base = itemPriceRates_[sel]*price/1000；
//   b==0 → base；否则 base*a/b。（ORIG 的魔数除法即有符号 /1000 截断语义。）
// ============================================================================
int getItemSellPrice(int price, int a, int b, bool flag)
{
    const unsigned char sel = flag ? 2 : 0;

    if (a > b)
    {
        a = b;
    }

    const int base = itemPriceRates_[sel] * price / 1000;
    if (b == 0)
    {
        return base;
    }
    return base * a / b;
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
        m_f[i + 1] = 0;
    }
}

// ============================================================================
// ============================================================================
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
    // fistp 四舍五入。
    const int sell = getItemSellPrice((int)(scaled + 0.5f), 0, 0,
                                      script->IsRandomOption());
    // ORIG：(sell*1000)/creationConst 为整数除法后再进浮点。
    const int money = (int)(((float)((sell * 1000) / m_creationConst) *
                             rate) / thousand + 0.5f);  // fistp 四舍五入
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
            count = (short)stack->GetReinforceVaule();
            if (count < 0 || 0x1f < count)
            {
                count = 0;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!amplify)
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            count = (short)stack->GetAmplifyReinforceValue();
            if (count < 0 || 0x1f < count)
            {
                count = 0;
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
        const unsigned idx = (unsigned)material.m_addInfo;
        if (idx == 0x1d29 || (idx >= 0x1c69 && idx <= 0x1c6b) ||
            (idx >= 0x1e3f && idx <= 0x1e40) || idx == 0x1e86)
        {
            return (unsigned char)100;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            rate = (char)stack->GetReinforceRate();
            if (rate < 0 || 100 < rate)
            {
                rate = 0;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!amplify)
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            rate = (char)stack->GetAmplifyReinforceRate();
            if (rate < 0 || 100 < rate)
            {
                rate = 0;
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
    CItem* matScript = G_CDataManager()->find_item(material.m_addInfo);
    if (matScript == 0)
    {
        return false;
    }
    if (!matScript->is_stackable())
    {
        return true;
    }
    const CStackableItem* stack = (const CStackableItem*)matScript;
    if (stack->m_char320 != 1)
    {
        return true;
    }
    CItem* tgtScript = G_CDataManager()->find_item(target.m_addInfo);
    if (tgtScript == 0)
    {
        return false;
    }
    if (!target.isEquipableItemType())
    {
        return false;
    }
    // 目标可用等级须位于材料脚本的区间 [m_int318, m_int31c]。
    const int usable = tgtScript->getUsableLevel();
    if (usable < stack->m_int318 || stack->m_int31c < usable)
    {
        return false;
    }
    return true;
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
        unsigned long boundStorage;
        const unsigned int roll =
            (unsigned int)upgradeRand(this)->randInt(
                randIntBound(this, boundStorage));
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
// ============================================================================
int CItemUpgrade::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                              PacketGuard* pg)
{
    char* mb = reinterpret_cast<char*>(msg);
    const unsigned char type = *(const unsigned char*)(mb + 0x1a);
    const unsigned short tSlot = *(const unsigned short*)(mb + 0x1b);
    const unsigned int expectIdx = *(const unsigned int*)(mb + 0x1d);
    const unsigned short ticketSlot = *(const unsigned short*)(mb + 0x21);
    const unsigned short matSlot = *(const unsigned short*)(mb + 0x23);
    unsigned short upCount = *(const unsigned short*)(mb + 0x25);

    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();

    Inven_Item target = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, tSlot);
    if (target.isEmpty())
    {
        return 4;
    }
    if (*(const int*)(reinterpret_cast<const char*>(&target) + 2) !=
        (const int)expectIdx)
    {
        return 4;
    }

    Inven_Item material = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, matSlot);
    if (material.isEmpty())
    {
        return 0x16;
    }

    Event_Upgrade_Jar* jar = GlobalData::s_pEvent_UpGrade_Jar;
    if (!jar->CheckMaxLuckyLevel(target, (unsigned long)material.m_addInfo))
    {
        return 0x3f4;
    }
    jar->CheckMaterialIndex((unsigned long)material.m_addInfo);

    ENUM_ERROR err = ENUM_ERROR_NONE;
    *(unsigned short*)(mb + 0x25) = upCount =
        _getUpgradeCountByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }
    *(unsigned char*)(mb + 0x27) =
        _getUpgradeRateByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }

    CItem* script = G_CDataManager()->find_item(target.m_addInfo);
    if (script == 0)
    {
        return 4;
    }

    Inven_Item ticketFirstCopy;  // ORIG 先默认构造，仅在 ticketSlot 有效时装填
    char ticketFlag = 0;
    if (ticketSlot != 0xffff)
    {
        if (!script->IsPossibleKiriProtection())  // CItem vtable+0x44
        {
            return 0xea;
        }
        ticketFirstCopy = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, ticketSlot);
        if (ticketFirstCopy.isEmpty())
        {
            return 4;
        }
        std::map<unsigned int, stUpgradeItemTicketParam_t>& tickets =
            G_CDataManager()->m_serverParameter.m_ticket468;
        std::map<unsigned int, stUpgradeItemTicketParam_t>::iterator it =
            tickets.find((unsigned int)ticketFirstCopy.m_addInfo);
        if (it == tickets.end())
        {
            return 9;
        }
        const unsigned char* param =
            reinterpret_cast<const unsigned char*>(&it->second);
        const unsigned char up = target.GetUpgrade();
        if (up < param[0] || param[1] < up)
        {
            return 8;
        }
        ticketFlag = (char)param[2];
    }

    // 升级表：增幅物品用 CItemAmplifier 表，否则类内嵌表（this+0x04）。
    const upgrade_table_t& table =
        target.m_amp.hasAbility()
            ? *GlobalData::s_itemAmplifier_->getUpgradeInfoTable()
            : embeddedTable(this);

    upgrade_info_t info;
    std::memset(&info, 0, sizeof(info));  // ORIG upgrade_info_t ctor 全清零
    const int level = ((const CUserCharacInfo*)user)->get_charac_level();
    if (!_GetUpgradeInfo(&target, script, table, &info, level))
    {
        return 0xd;
    }

    // 【ORIG 怪癖】payBase/matCount 仅在部分分支赋值，其余路径沿用栈上旧值。
    int payBase;   // -0x250：金钱基数
    int matCount;  // -0x254：材料消耗数量
    if (upCount != 0)
    {
        payBase = 0;
        matCount = 1;
        // （dead store：materialIdx 写入 -0x258 后无读者，忠实省略。）
    }

    if (!_CheckTargetUsableLv(material, target))
    {
        return 0x13;
    }
    const int portable =
        _ProcPortableUpgradeItem(material, target, info);
    if (portable != 0)
    {
        return portable;
    }

    if (ticketFirstCopy.m_addInfo == 0x28d30e ||
        ticketFirstCopy.m_addInfo == 0x28d30f)
    {
        payBase = 0;
    }

    jar->SettingRecipeNUpConunt(material.m_addInfo, info,
                                target.GetUpgrade());

    if (type == 0 && script->IsImpossibleUpgrade())
    {  // CItem vtable+0x30
        return 0x13;
    }
    if (type == 1 && script->IsImpossibleAmplify())
    {  // CItem vtable+0x34
        return 0x13;
    }

    // 特殊券（0x28d30e/0x28d30f）覆盖材料消耗数（两处相同逻辑，忠实保留）。
    if (ticketSlot != 0xffff)
    {
        Inven_Item t = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, ticketSlot);
        if (t.m_addInfo == 0x28d30f)
        {
            matCount = (type == 0) ? 0x32 : 10;
        }
        else if (t.m_addInfo == 0x28d30e)
        {
            matCount = (type == 0) ? 100 : 20;
        }
    }

    const int cond =
        _CheckCondition(user, target, script, material, table, info);
    if (cond != 0)
    {
        return cond;
    }
    if (type == 0 && target.m_amp.hasAbility())
    {
        return 0x17;
    }
    if (type == 1 && !target.m_amp.hasAbility())
    {
        return 0x17;
    }

    pg->put_byte(1);

    const unsigned char oldUp = target.GetUpgrade();
    bool upgraded = false;
    char winFlag = 0;

    if (upCount == 0)
    {
        upgraded = _DoProcUpgrade(user, &target, &info);

        int pay = payBase;  // -0x2c
        CEventBase* repeat =
            GlobalData::s_event_manager->GetRepeatEvent(0x32);
        if (repeat != 0 && repeat->IsEventing((CUser*)0) &&
            !target.m_amp.hasAbility())
        {
            const int rate = ((CReduceUpgradeItemPay*)repeat)->GetReduceRate();
            if (rate > 0)
            {
                pay = payBase - (payBase * rate) / 100;
                if (pay < 0)
                {
                    pay = 0;
                }
            }
        }

        Inven_Item worn = invenW->GetInvenSlot(0, 0xb);
        CItem* discScript = G_CDataManager()->find_item(worn.m_addInfo);
        if (discScript != 0)
        {
            const float discount =
                ((CEquipItem*)discScript)->getUpgradeCostDiscount();
            // _DAT_08c9d304 == -100.0f（ORIG 分母常量）。
            pay = (int)((discount * (float)pay) / -100.0f + (float)pay);
        }

        const eMoneySubReason reason =
            target.m_amp.hasAbility() ? eMoneySubReason_0x2e
                                      : eMoneySubReason_0xd;
        if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                pay, reason, true) != 1)
        {
            return 10;
        }
    }
    else
    {
        upgraded = true;
        unsigned long boundStorage;
        const unsigned int roll =
            (unsigned int)upgradeRand(this)->randInt(
                randIntBound(this, boundStorage));
        if (roll < (unsigned int)*(const unsigned char*)(mb + 0x27))
        {
            target.SetUpgrade((unsigned char)upCount);
            winFlag = 1;
        }
        else
        {
            target.SetUpgrade(oldUp);
        }
    }

    // 特殊券覆盖材料消耗数（删除前第二次，同 +0x06dc 段逻辑）。
    if (ticketSlot != 0xffff)
    {
        Inven_Item t = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, ticketSlot);
        if (t.m_addInfo == 0x28d30f)
        {
            matCount = (type == 0) ? 0x32 : 10;
        }
        else if (t.m_addInfo == 0x28d30e)
        {
            matCount = (type == 0) ? 100 : 20;
        }
        if (((CUserCharacInfo*)user)->getCurCharacInvenW()
                ->delete_item(INVEN_TYPE_EQUIP, ticketSlot, 1,
                              eItemDelReason_3, true) != 1)
        {
            return 0x11;
        }
        user->m_historyLog.apply_Item(ticketFirstCopy.m_addInfo,
                                      target.m_addInfo,
                                      eApplyItemReason_0);
    }

    if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, matSlot, matCount, eItemDelReason_0xc,
            true) != 1)
    {
        return 0x11;
    }
    if (upCount != 0)
    {
        user->m_historyLog.apply_Item(material.m_addInfo, target.m_addInfo,
                                      eApplyItemReason_1);
    }

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, (unsigned int)payBase);

    pg->put_byte((int)type);
    pg->put_short((int)matSlot);
    {
        Inven_Item cur = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, tSlot);
        pg->put_int(cur.get_add_info());
    }
    pg->put_short((int)ticketSlot);

    if (upCount == 0)
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
    pg->put_byte((int)(char)oldUp);

    if (table.intAtConst(0x234) <= (int)target.GetUpgrade() &&
        (upCount == 0 || winFlag != 0))
    {
        _NoticeUpgrade(user, target, upgraded);
    }

    if (target.GetUpgrade() > 12)
    {
        const unsigned int envField =
            *reinterpret_cast<const unsigned int*>(
                reinterpret_cast<const char*>(G_CEnvironment()) +
                0x378);  // [裸偏移] CEnvironment+0x378（具名字段待补）
        DB_ReqItemUpgradeLog::makeRequest(envField, target.m_addInfo,
                                          ((const CUserCharacInfo*)user)
                                              ->getCurCharacNo(),
                                          ((const CUserCharacInfo*)user)
                                              ->getCurCharacName(),
                                          target.GetUpgrade(), upgraded);
    }

    if (upgraded)
    {
        if (upCount == 0)
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
        pg->put_short((int)tSlot);
        if (upCount == 0)
        {
            APSystem::_ActionGroupIndex group;
            group.m_value = 0xc;
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, group, 0, (ENUM_PACKETCLASS)0);
        }
    }
    else
    {
        int sellPrice = 0;
        G_Store()->GetSellItemPrice(target, script,
                                    (short)target.get_add_info(), false,
                                    sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);

        const int penalty =
            _ProcPenalty(user, &target, script, tSlot, table, &info, pg,
                         &ticketFirstCopy, ticketFlag);
        if (penalty != 0)
        {
            if (penalty != 0xd8)
            {
                return penalty;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)matSlot);
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, 0);
            return penalty;
        }
    }

    user->m_historyLog.UpgradeItem(script->get_index(), (int)oldUp,
                                   (int)target.GetUpgrade(), upgraded);

    if (script->IsRandomOption())
    {
        user->makeGuildItemUpgradMessage(mb + 0x28, (int)oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }
    else
    {
        user->makeGuildItemUpgradMessage(script->GetItemName(), (int)oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }

    if (*(const int*)&target != 0)
    {
        invenW->update_item(INVEN_TYPE_EQUIP, tSlot, target);
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
    const int grade = itemScript->get_grade();
    const int rarity = itemScript->get_rarity();

    const int baseGrade = table.intAtConst(rarity * 4 + 0x25c);
    int diff = grade - baseGrade;
    if (diff < 0)
    {
        diff = 0;
    }
    if ((unsigned int)table.intAtConst(0x23c) <= (unsigned int)diff &&
        (unsigned int)table.intAtConst(600) < up)
    {
        const int itemId = table.intAtConst(0x238);
        const float weight = table.floatAt(rarity * 4 + 0x240);

        // pow(diff, 2.0)（_DAT_08c9d308），两级 fistp 取整。
        const double powered = std::pow((double)(unsigned)diff, 2.0);
        int count = (int)(powered + 0.5);           // fistp 四舍五入
        count = (int)((float)count * weight + 0.5f);  // fistpll 四舍五入
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

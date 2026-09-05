// ============================================================================
// InventoryLCB.cpp —— LinkClosureB.BattleItem 独占 TU。
// 忠实转写 CInventory ORIG 符号：
//   CheckEquipmentItemCondition(INVEN_TYPE, short, stEquipConditionForChronicle&)
//     const
//   T _ZNK10CInventory27CheckEquipmentItemConditionE10INVEN_TYPEsR28stEquipCondi
//     tionForChronicle @0x085083d0 (0x165)
// 报告：docs/class_func_reports/CInventory/CheckEquipmentItemCondition_085083d0.md
//
// 返回错误码 int：0=成功；0x11=槽空/找不到脚本道具；0x13=强化选项未鉴定；
// 0xd8=耐久不符（ORIG eax 直接值；bool 声明会截断错误码，已改 int）。
// stEquipConditionForChronicle 写入布局证据见 CInventory.h 注释与该报告。
// ============================================================================

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CEquipItem.h"      // CEquipItem::get_endurance（ORIG 非虚 thiscall）
#include "CDataManager.h"    // G_CDataManager / CDataManager::find_item
#include "CInventory.h"
#include "CItemAmplifier.h"  // stAmplifyOption_t::getAbilityType/isIdentified/getAbilityValue
#include "Inven_Item.h"

int CInventory::CheckEquipmentItemCondition(INVEN_TYPE type, short slot,
                                            stEquipConditionForChronicle& cond) const
{
    // +0x00: GetInvenSlot(type, slot) —— 返回 Inven_Item 值拷贝。
    Inven_Item item = GetInvenSlot(static_cast<int>(type), slot);

    // GetItemType(item idx) != 1(EQUIP) → return 0x11
    int itemType = GetItemType(item.m_addInfo);
    if (itemType != 1)
        return 0x11;

    // find_item(idx)：ORIG 无类型转换直接调 CEquipItem::get_endurance，
    // 用 static_cast<CEquipItem*> 保持 ABI 等价。
    CEquipItem* equip =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(item.m_addInfo));
    if (!equip)
        return 0x11;

    // ORIG 顺序：先取 abilityType 存栈(-0xd)，再 isIdentified 判定。
    char abilityType = item.m_amp.getAbilityType();
    if (!item.m_amp.isIdentified())
        return 0x13;

    // item+0x0b（endurance ushort）== equip->get_endurance() 才通过。
    int endurance = equip->get_endurance();
    if (static_cast<int>(item.m_fieldb) != endurance)
        return 0xd8;

    // ---- 输出块（对应 asm 逐偏移写）----
    cond.m_upgrade = item.GetUpgrade();                     // cond[0] ← Inven_Item::GetUpgrade
    if (abilityType == 0)
    {
        cond.m_hasAmplify = 0;                              // cond[1]
        cond.m_amplifyType = 0;                             // cond[2]
        cond.m_amplifyValue = 0;                            // *(ushort*)&cond[4]
    }
    else
    {
        cond.m_hasAmplify = 1;                              // cond[1]
        cond.m_amplifyType =
            static_cast<unsigned char>(abilityType);        // cond[2]（原字节）
        cond.m_amplifyValue = item.m_amp.getAbilityValue(); // *(ushort*)&cond[4]
    }

    // cond[6]：Inven_Item 字节0（m_field0）非零 → 1，否则 0。
    if (item.m_field0)
        cond.m_hasField0Item = 1;
    else
        cond.m_hasField0Item = 0;

    // cond[7] ← Inven_Item::GetReSealCount()
    cond.m_reSealCount = item.GetReSealCount();

    // cond+8(int) ← Inven_Item+0x0d 联合整体读（m_fieldd）。
    cond.m_fieldd = item.m_fieldd;

    return 0;
}

// ============================================================================
// CInventory::CheckEquipmentItemCondition(vector<pair<int,int>>, stEquipCondition&,
//                                         Inven_Item&) const
// ORIG T 0x08507f1a _ZNK10CInventory27CheckEquipmentItemConditionERKSt6vectorISt4pair
//   IiiESaIS2_EER16stEquipConditionR10Inven_Item（0x4b6 字节）。
// 语义（逐指令对照）：
//   - items 空 → 0；遍历 items：pair.first 经 GetItemType 非 1(EQUIP) 跳过；
//     pair.second > 1 → 1；StartEndPos(type,&start,&end)；
//   - 段1 slot=start..end、段2 slot=3..8 扫 m_pEquipSlot(+0x650, 步进 0x3d)：
//     m_addInfo 匹配计数 >1 → 0x12；整槽 0x3d 字节拷贝到 outItem；
//     m_pParent->CheckItemLock(slot,1) 锁定 → 0xd5；
//   - 循环后 outItem.m_addInfo==0 → 0；find_item 空 → 0x11；
//     m_amp.getAbilityType()==0x80 → 0x13；m_fieldb != get_endurance → 0xd8；
//   - 输出：cond.m_upgrade=1、m_tableIdx=GetUpgrade()；abilityType==0 →
//     amplifySelect/pad3/field4 清零，否则 1/type/value；
//     m_flag6 = GetAttachType()==0 ? 1 : (m_field0 ? 1 : 0)；m_reSealCount。
// 返回错误码 int（0x11/0x12/0x13/0xd5/0xd8，bool 声明会截断，与 CInventory.h 一致）。
// ============================================================================
int CInventory::CheckEquipmentItemCondition(
    const std::vector<std::pair<int, int> >& items, stEquipCondition& cond,
    Inven_Item& outItem) const
{
    if (items.empty())
        return 0;

    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const int itemIdx = it->first;
        const int count = it->second;
        const int type = GetItemType(itemIdx);
        if (type != 1)
            continue;                 // 非 EQUIP：++it 跳过
        if (count > 1)
            return 1;                 // 同 key 需求 >1 → 1
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        int found = 0;
        int slot = start;
        for (; slot <= end; ++slot)
        {
            const Inven_Item& src = m_pEquipSlot[slot];
            if (src.m_addInfo != itemIdx)
                continue;
            if (++found > 1)
                return 0x12;
            outItem = src;
            if (m_pParent->CheckItemLock(slot, 1))
                return 0xd5;
        }
        slot = 3;
        for (; slot <= 8; ++slot)
        {
            const Inven_Item& src = m_pEquipSlot[slot];
            if (src.m_addInfo != itemIdx)
                continue;
            if (++found > 1)
                return 0x12;
            outItem = src;
            if (m_pParent->CheckItemLock(slot, 1))
                return 0xd5;
        }
    }

    if (outItem.m_addInfo == 0)
        return 0;

    CItem* item = G_CDataManager()->find_item(outItem.m_addInfo);
    if (item == 0)
        return 0x11;

    const char abilityType = outItem.m_amp.getAbilityType();
    if (abilityType == 0x80)
        return 0x13;

    CEquipItem* equip = static_cast<CEquipItem*>(item);
    if (static_cast<int>(outItem.m_fieldb) != equip->get_endurance())
        return 0xd8;

    cond.m_upgrade = 1;
    cond.m_tableIdx = outItem.GetUpgrade();
    if (abilityType == 0)
    {
        cond.m_amplifySelect = 0;
        cond.m_pad3 = 0;
        cond.m_field4 = 0;
    }
    else
    {
        cond.m_amplifySelect = 1;
        cond.m_pad3 = static_cast<unsigned char>(abilityType);
        cond.m_field4 = outItem.m_amp.getAbilityValue();
    }

    if (item->GetAttachType() == 0)
        cond.m_flag6 = 1;
    else
        cond.m_flag6 = outItem.m_field0 ? 1 : 0;

    // cond+0x07 ← GetReSealCount()（stEquipCondition::m_pad7 承载 reseal 计数）
    cond.m_pad7 = outItem.GetReSealCount();
    return 0;
}

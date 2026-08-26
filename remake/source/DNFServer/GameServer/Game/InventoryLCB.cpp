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
    if (GetItemType(item.m_addInfo) != 1)
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
    if (static_cast<int>(item.m_fieldb) != equip->get_endurance())
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

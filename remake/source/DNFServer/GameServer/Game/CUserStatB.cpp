// ============================================================================
// CUserStatB.cpp —— CUser 脚本补给/任务碎片批次 B（独立 TU，仅承载两符号）
//   1. CUser::RefillItemByScript   ORIG _ZN5CUser18RefillItemByScriptERSt6vectorISt4pairIiiESaIS2_EE
//                                  @0x086890be (T)
//   2. CUser::sendCharacQuestPiece ORIG _ZN5CUser20sendCharacQuestPieceEv @0x0868af2c (T)
// 报告依据（汇编 + Ghidra 反编译全文已读）：
//   docs/class_func_reports/CUser/RefillItemByScript.md
//   docs/class_func_reports/CUser/sendCharacQuestPiece.md
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "CQuestShop.h"

// ---------------------------------------------------------------------------
// CUser::RefillItemByScript —— ORIG 0x086890be，报告逐段转写。
// 遍历 CDataManager::GetRefillItemInfo()（= m_limitItemUsage.m_refill18.m_list，
// ORIG this+0x4d10）。对到期秒晚于当前秒的每个条目：
//   count==0：向调用方 vector 压入 pair(itemId, 0)（供下游 deleteSpecificItem）；
//   count!=0：find_item 查定义，按背包有无分流：
//     无（GetInvenData<0）→ vtable 槽 2 make_item 填默认 Inven_Item 后，
//        强制写 pack(1) +0x02 物品索引=entry.m_itemId、+0x07 数量=entry.m_count
//        （ORIG 栈槽 -0x67/-0x62 即这两字段），再插入背包
//        insertItemIntoInventory(item, reason 0xd, bLog=true, b=false)；
//     有 → 先 ItemAdd(INVEN_TYPE 1, 现有数量, 物品索引, item, 0xd) 记日志，
//        再把 +0x07 数量改为 entry.m_count 后 update_item 回写；
//   插入/更新成功（slot>=0）→ SendUpdateItemList(eSendTarget_1, 空间 0, slot)。
// ---------------------------------------------------------------------------
void CUser::RefillItemByScript(std::vector<std::pair<int, int> >& list)
{
    std::vector<stRefillItemInfo::RefillItem>* refill =
        G_CDataManager()->GetRefillItemInfo();
    const int nowSec = GlobalData::s_systemTime_.getCurSec();

    for (unsigned int i = 0; i < refill->size(); ++i)
    {
        const stRefillItemInfo::RefillItem& entry =
            refill->operator[](i);
        if (nowSec >= entry.m_endSec)      // ORIG：endSec<=curSec 跳过（jle）
            continue;
        if (entry.m_count == 0)
        {
            // std::make_pair<int const&, int>(itemId, 0) → push_back
            list.push_back(std::make_pair(entry.m_itemId, 0));
            continue;
        }

        CItem* def = G_CDataManager()->find_item(entry.m_itemId);
        if (def == 0)
            continue;

        Inven_Item item;                   // ORIG Inven_Item::Inven_Item() W 0x80cb854
        int slot = getCurCharacInvenR()->GetInvenData(entry.m_itemId, item);
        if (slot < 0)
        {
            def->make_item(item);              // ORIG *(*(def)+8)(def,&item)：vtable 槽 2
            item.m_addInfo2 = entry.m_count;   // +0x07 数量（ORIG -0x62 槽，先写）
            item.m_addInfo = entry.m_itemId;   // +0x02 物品索引（ORIG -0x67 槽，后写）
            slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, false);
        }
        else
        {
            m_historyLog.ItemAdd((INVEN_TYPE)1, item.m_addInfo2, item.m_addInfo,
                                 item, eItemAddReason_0xd);
            item.m_addInfo2 = entry.m_count;
            getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, item);
        }
        if (slot >= 0)
            SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
}

// ---------------------------------------------------------------------------
// CUser::sendCharacQuestPiece —— ORIG 0x0868af2c。
// 取角色扩展槽 0xb（CQuestShop），非空则转发 sendCharacQuestPiece(this)。
// ---------------------------------------------------------------------------
void CUser::sendCharacQuestPiece()
{
    CQuestShop* shop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->sendCharacQuestPiece(this);
    return;
}

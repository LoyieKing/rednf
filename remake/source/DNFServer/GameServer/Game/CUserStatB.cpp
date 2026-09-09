// ============================================================================
// CUserStatB.cpp —— CUser 脚本补给/任务碎片批次 B（独立 TU，仅承载两符号）
//   1. CUser::RefillItemByScript   ORIG _ZN5CUser18RefillItemByScriptERSt6vectorISt4pairIiiESaIS2_EE
//                                  @0x086890be (T)
//   2. CUser::sendCharacQuestPiece ORIG _ZN5CUser20sendCharacQuestPieceEv @0x0868af2c (T)
// 报告依据（汇编 + Ghidra 反编译全文已读）：
//   docs/class_func_reports/CUser/RefillItemByScript.md
//   docs/class_func_reports/CUser/sendCharacQuestPiece.md
// ============================================================================
// [SweepCUserStatB 2026-09-09 复核，verdict = check_tu_routed.sh，fresh .o]：
//   本 TU 已路由 c6446r（4.4.6 后端 + 4.4.6 头集，dnf_game_cxx.sh:133）。
//   四数：identical=24 ae=33 near=1 diff=1 missing_in_game=6
//         （diff 仅 _Z41__static_initialization_and_destruction_0ii=N123 噪音）。
//   RefillItemByScript 0x086890be  NEAR(204/204 助记符全等)；p167 18/18，
//     唯一差 Inven_Item C1Ev↔C2Ev 系同址别名（ORIG nm 080cb854 C1/C2 双名，P93）。
//     虚调用槽核对：ORIG `mov (%eax),%eax; add $0x8,%eax; mov (%eax),%edx; call *%edx`
//       = vptr+0x08；_ZTV5CItem@0x8c96da0 word4(+0x10)=0x08724fe0=__cxa_pure_virtual
//       （CItem::make_item 纯虚，CItem.h:8 注释「vtable 0x8c96da8，make_item 纯虚 @2」），
//       且 _ZTV14CStackableItem+0x10=0x0850f8b8=_ZNK14CStackableItem9make_itemER10Inven_Item
//       ⇒ 槽号/目标双证一致；我方 4.4.6 亦发 add $0x8 + call *%edx。
//     jcc 逐位：jle/jne/je/jns/js/seta+jne 六处条件跳转方向与落点块（nop/jmp 岛）
//       两侧全等（唯一残差非 jcc）。
//     残差 = vector::operator[](i) 的 receiver/index 装载序（ORIG 0x086890f3
//       `mov -0x20(%ebp),%eax; mov -0x18(%ebp),%edx` = receiver 先；4.4.6/4.4.7 恒
//       `mov -0x18(%ebp),%eax; ...; mov -0x20(%ebp),%eax` = index 先）。实测探针
//       （/tmp/pr/probe*.o，同 TU 旗标）：指针/引用/`(*p)[i]`/`p->operator[](i)`/
//       int 下标/`static_cast`/逗号/三目/`(p+0)` 共 12+ 形均 index 先；仅 `(*&refill)`
//       与 `volatile` 指针可翻装载序，但会把 refill 槽 -0x20→-0x2c 并使 make_pair
//       两槽 -0x24/-0x2c→-0x20/-0x28（四数不降，NEAR 依旧）⇒ 判跨版本求值序
//       N（P75/N7 邻域）封顶，保留最自然指针形。
//   sendCharacQuestPiece 0x0868af2c  identical（17 条 + 尾部 nop）；p167 2/2 CLEAR。
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
    // ORIG 0x086890be：经 const 指针走 const operator[]（ix 常量符号）
    const std::vector<stRefillItemInfo::RefillItem>* refill =
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

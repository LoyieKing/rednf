#include "Dispatchers.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "CDataManager.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CGameManager.h"

#include <vector>

// Event_Upgrade_Jar 权威类型来自 EventUpgradeJarInfo.h（经 GlobalData.h 引入）。
// GlobalData::s_pEvent_UpGrade_Jar 权威类型为 Event_Upgrade_Jar*（nm B 0941f758）。
// ORIG 自由函数 _Z8LineFunciPKcij = LineFunc(int, char const*, int, unsigned int)
// @0x085908d7；非本 TU 定义，由上游/主代理链接提供。
int LineFunc(int code, const char* file, int line, unsigned int flag);

// ============================================================================
// 装备升级卡合成（ORIG 0x08227e00 T，约 710 指令）
//
// 包协议（回包 header 0x169）：
//   材料 = 升级卡(slotCard) + 目标装备(slotEquip)（合成配方），放入槽 slotTarget。
//   流程见 docs/class_func_reports/Dispatcher_Compound_Equipment_Upgrade_Card。
// ============================================================================
int Dispatcher_Compound_Equipment_Upgrade_Card::dispatch_sig(CUser* user,
                                                             PacketBuf& packet)
{
    // ---- 1. 角色/状态校验：state==3 且存在当前角色才放行 ----
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    // ---- 2. 频道校验：channel type 7（赛事/特殊频道）拒绝 ----
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return LineFunc(0xe4c3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // ---- 3. 读包：slotCard / slotEquip / slotTarget ----
    unsigned short slotCard = 0;
    unsigned short slotEquip = 0;
    unsigned short slotTarget = 0;
    packet.get_short(slotCard);
    packet.get_short(slotEquip);
    packet.get_short(slotTarget);

    // ---- 4. 取两个材料的物品 ----
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item itemCard = invenR->GetInvenSlot((INVEN_TYPE)1, (int)slotCard);
    Inven_Item itemEquip = invenR->GetInvenSlot((INVEN_TYPE)1, (int)slotEquip);

    // ---- 5. 依升级卡索引查合成目标（booster）索引；材料数量须 >0 ----
    unsigned long cardIdx = (unsigned long)itemCard.m_addInfo;
    unsigned long equipIdx = (unsigned long)itemEquip.m_addInfo;
    unsigned long boosterIdx = GlobalData::s_pEvent_UpGrade_Jar
                                   ->getBoosterIndex_byUpgradeCardIndex(cardIdx, equipIdx);
    if (boosterIdx == 0 || itemCard.m_addInfo2 <= 0 || itemEquip.m_addInfo2 <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x169, 0x13);
        return 0;
    }

    // ---- 6. 校验目标装备的合成配方 ----
    Inven_Item itemTarget = invenR->GetInvenSlot((INVEN_TYPE)1, (int)slotTarget);
    if (!GlobalData::s_pEvent_UpGrade_Jar->check_requiredRecipe(itemTarget))
    {
        return 0;
    }

    // ---- 7. 结果脚本必须是可堆叠助力（stackable booster，GetItemType==0xf） ----
    CItem* boosterScript = G_CDataManager()->find_item((int)boosterIdx);
    if (boosterScript == 0 || boosterScript->GetItemType() != 0xf)
        return 0;

    CStackableItem* booster = static_cast<CStackableItem*>(boosterScript);
    std::vector<stStackableBoosterItemInfo_t> out;
    booster->getBoosterItem(out);

    PacketGuard pg;
    pg.put_header(1, 0x169);
    std::vector<int> insertedSlots;

    if (out.empty() || out.size() >= 2)
    {
        // 结果异常 → 回失败包：put_byte(0), put_byte(0x13)
        pg.put_byte(0);
        pg.put_byte(0x13);
    }
    else
    {
        // ---- 合成成功：删除 3 个材料 ----
        CInventory* invenW = user->getCurCharacInvenW();
        invenW->delete_item((INVEN_TYPE)1, (int)slotTarget, 1,
                            (eItemDelReason)0x29, true);
        invenW->delete_item((INVEN_TYPE)1, (int)slotCard, 1,
                            (eItemDelReason)0x29, true);
        invenW->delete_item((INVEN_TYPE)1, (int)slotEquip, 1,
                            (eItemDelReason)0x29, true);

        // 删后重读各槽剩余数量，按 ORIG set_ReturnSendPacket 格式写回（put_byte(0);
        // put_short(slot); put_int(quantity)），此处内联（头未声明该方法）。
        const CInventory* invenAfter = user->getCurCharacInvenR();
        Inven_Item rTarget = invenAfter->GetInvenSlot((INVEN_TYPE)1, (int)slotTarget);
        Inven_Item rCard = invenAfter->GetInvenSlot((INVEN_TYPE)1, (int)slotCard);
        Inven_Item rEquip = invenAfter->GetInvenSlot((INVEN_TYPE)1, (int)slotEquip);

        pg.put_byte(1);
        pg.put_byte(0);
        pg.put_short((int)slotTarget);
        pg.put_int(rTarget.m_addInfo2);
        pg.put_byte(0);
        pg.put_short((int)slotCard);
        pg.put_int(rCard.m_addInfo2);
        pg.put_byte(0);
        pg.put_short((int)slotEquip);
        pg.put_int(rEquip.m_addInfo2);

        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 out.begin();
             it != out.end(); ++it)
        {
            int itemIdx = it->m_itemIdx;
            CItem* itemScript = G_CDataManager()->find_item(itemIdx);
            if (itemScript == 0)
                continue;

            Inven_Item rew;
            itemScript->make_item(rew);
            // ORIG 0x08228757：booster 结构 +8 处 ushort 作为 add_info。
            // stStackableBoosterItemInfo_t 权威布局（+8 = m_addInfo，具名字段）。
            rew.set_add_info((int)it->m_addInfo);

            short slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                rew, (eItemAddReason)0x2b, true, true);
            if (slot < 0)
            {
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    user->getCurCharacName(), rew, 0, user->getCurCharacNo(),
                    0, 0, 0, user->GetServerGroup(), 0, 0);
            }
            else
            {
                insertedSlots.push_back((int)slot);
            }
            pg.put_item_idx(itemIdx);
        }
    }

    pg.finalize(true);
    user->Send(pg);

    for (std::vector<int>::const_iterator it = insertedSlots.begin();
         it != insertedSlots.end(); ++it)
    {
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, *it);
    }
    return 0;
}

// ORIG 为弱符号（约 5 指令，vtable 占位），return 0 与 ORIG identical。
int Dispatcher_Compound_Equipment_Upgrade_Card::process(CUser* user,
                                                        MSG_BASE& msg,
                                                        ParamBase& param)
{
    (void)user; (void)msg; (void)param; return 0;
}

int Dispatcher_Compound_Equipment_Upgrade_Card::read(PacketBuf& packet,
                                                     MSG_BASE& msg)
{
    (void)packet; (void)msg; return 0;
}
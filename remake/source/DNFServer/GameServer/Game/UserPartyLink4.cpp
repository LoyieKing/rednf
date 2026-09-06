// ============================================================================
// UserPartyLink4.cpp —— LinkMisc4 批次 CUser/CParty/CDataManager 域 5 符号。
// 全部逐报告忠实转写（docs/class_func_reports/{CUser,CDataManager,CParty}/），
// ORIG mangled 由本 TU 的真实 C++ 定义产生：
//   _ZN5CUser14RewardTutorialEj                T 0x086475d8
//   _ZN5CUser18adjust_charac_statEv            T 0x08664766
//   _ZN5CUser18deleteGuildExpBookEv            T 0x0867e898
//   _ZN12CDataManager19getDungeonIndexListEv   T 0x08365434（非 const）
//   _ZN6CParty12drop_item_gmEP5CUserj          T 0x085a73a6
// ============================================================================

#include <string.h>

#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h   // CBattle_Field::drop_item(map_item) + WarField.h map_item
#include "CDataManager.h"    // G_CDataManager + m_serverParameter(+0x68) + find_item
#include "CInventory.h"      // insert_event_items / NumberToString
#include "Inven_Item.h"      // Inven_Item 布局与访问器
#include "LogManager.h"
#include "PacketGuard.h"
#include "ServerParameterScript.h"  // GetEscaladeTutorialReward

// ORIG 文件级枚举（同 CGameManager.cpp 模式；值 0 = 公会经验书 hardcode item）。
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

// ---------------------------------------------------------------------------
// CUser::RewardTutorial(unsigned int)
// ORIG T 0x086475d8 sz 0x245，报告 docs/class_func_reports/CUser/RewardTutorial.md。
// 流程：取 ServerParameterScript::GetEscaladeTutorialReward(flag) 奖励清单；
// 非空时经 CInventory::insert_event_items 入包（moneyReason=3、itemReason=8、
// bLog=true、两个空串日志标签），随后 put_header(1,0x92) 包回执：byte(1)、
// byte(入包数)；>0 时对返回的槽位三元组逐个 put_byte/put_int/put_int，
// finalize(true) 后 Send。
// ---------------------------------------------------------------------------
void CUser::RewardTutorial(unsigned int flag)
{
    std::vector<int> outSlots;

    // ORIG：G_CDataManager()+0x68 = CDataManager::m_serverParameter。
    ServerParameterScript* serverParameter = &G_CDataManager()->m_serverParameter;
    std::vector<std::pair<int, int> >* rewards =
        serverParameter->GetEscaladeTutorialReward(flag);
    if (rewards == 0)
        return;

    CInventory* inven = getCurCharacInvenW();
    int insertedCount = inven->insert_event_items(
        *rewards, outSlots, (eMoneyAddReason)3, (eItemAddReason)8, true, "", "");

    PacketGuard packet;
    packet.put_header(1, 0x92);
    packet.put_byte(1);
    packet.put_byte(insertedCount);
    if (insertedCount > 0)
    {
		// ORIG 循环每次消费 3 个 int（槽位三元组）：short/int/int；
		// 迭代器步进语义与 ORIG 一致（it != end 判定，每轮 ++ 三次）。
		std::vector<int>::iterator it = outSlots.begin();
		while (it != outSlots.end())
		{
			packet.put_short(*it);
			++it;
			packet.put_int(*it);
			++it;
			packet.put_int(*it);
			++it;
		}
	}
    packet.finalize(true);
    Send(packet);
}

// ---------------------------------------------------------------------------
// CUser::adjust_charac_stat()
// ORIG T 0x08664766 sz 0x381，报告 docs/class_func_reports/CUser/adjust_charac_stat.md。
// 流程：_Additioal_info 局部对象 ctor+clear → calc_lev_stat(calc) → 打印 before
// 快照（当前角色 addInfo 各字段）→ memcpy(calc → getCurCharacAddInfoRefW, 0x52)
// → 打印 after 快照。__PRETTY_FUNCTION__ 处按报告字面量转写。
// ---------------------------------------------------------------------------
void CUser::adjust_charac_stat()
{
    _Additioal_info calc;
    calc.clear();
    calc_lev_stat(calc);

    const _Additioal_info* before =
        static_cast<const _Additioal_info*>(getCurCharacAddInfoRefR());
    cMyTrace(__PRETTY_FUNCTION__, 0x44d1, 0)(
        "before adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        before->m_field00, before->m_field04,
        (int)before->m_field08, (int)before->m_field0a,
        (int)before->m_field0c, (int)before->m_field0e,
        before->m_field42,
        (short)before->m_field10[0], (short)before->m_field10[1],
        (short)before->m_field10[2], (short)before->m_field10[3]);

    _Additioal_info* current =
        static_cast<_Additioal_info*>(getCurCharacAddInfoRefW());
    memcpy(current, &calc, 0x52);  // sizeof(_Additioal_info)，ORIG 立即数 0x52

    cMyTrace(__PRETTY_FUNCTION__, 0x44d7, 0)(
        "after adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        current->m_field00, current->m_field04,
        (int)current->m_field08, (int)current->m_field0a,
        (int)current->m_field0c, (int)current->m_field0e,
        current->m_field42,
        (short)current->m_field10[0], (short)current->m_field10[1],
        (short)current->m_field10[2], (short)current->m_field10[3]);
}

// ---------------------------------------------------------------------------
// CUser::deleteGuildExpBook()
// ORIG T 0x0867e898 sz 0x1c4，报告 docs/class_func_reports/CUser/deleteGuildExpBook.md。
// 流程：deleteSpecificItem({make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0)}, deleted)
// → send_itemspace(0) 失败则 logFormat(0x7c57) 返回 → send_itemspace(2) 失败则
// logFormat(0x7c5c) 返回 → send_RedeemInfo()。ORIG 尾部不设置返回寄存器（void）。
// ---------------------------------------------------------------------------
void CUser::deleteGuildExpBook()
{
    std::vector<std::pair<int, int> > requestItems;
    std::vector<std::pair<int, int> > deletedItems;

    // 保持 enum 实参：实测改成 `(int)` 会让 4.4 走另一条内联展开路径，
    // ndiff 4→27（gap +5，多出 mov/lea/call），TU 合计 90→87 连带回退三分，
    // 已回退。pair 的 first/second 写入序差（ORIG 先 first 后 second）
    // 属 4.4 对 `pair<ENUM,int>` → `pair<int,int>` 转换的固定形态，标 N。
    requestItems.push_back(
        std::make_pair((ENUM_HARDCODE_ITEM)ENUM_HARDCODE_ITEM_0, 0x4b0));

    deleteSpecificItem(requestItems, deletedItems);

    // X1 形（XOR_FAMILY.tsv，probe /tmp/xg2.cpp 形 B）：ORIG T 0x0867e898 对
    // `send_itemspace()`(返 int) 的结果走 `xor $0x1,%eax; test %al,%al`，
    // 而 `!= 1` 在 4.4 产 `cmp $0x1,%al; setne %al; test` 三段。
    register unsigned char inv asm("eax") = (unsigned char)send_itemspace((int)ENUM_ITEMSPACE_0);
    inv ^= 1;
    if (inv)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c57,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    register unsigned char car asm("eax") = (unsigned char)send_itemspace(2);  // ENUM_ITEMSPACE_CARGO
    car ^= 1;
    if (car)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c5c,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    send_RedeemInfo();
}

// ---------------------------------------------------------------------------
// CDataManager::getDungeonIndexList()
// ORIG T 0x08365434 sz 0x15（_ZN12CDataManager19getDungeonIndexListEv 非 const），
// 报告 docs/class_func_reports/CDataManager/getDungeonIndexList.md。
// this+0x00 = CDungeonList* m_dungeonList，透传其 getDungeonIndexList() 返回值
// （CDungeonList::getDungeonIndexList 返回 this）。原 CDataManager.cpp 的 const
// 发明重载已删除，本 TU 为唯一 owner。
// ---------------------------------------------------------------------------
CDungeonList* CDataManager::getDungeonIndexList()
{
    return m_dungeonList->getDungeonIndexList();
}

// ---------------------------------------------------------------------------
// CParty::drop_item_gm(CUser*, unsigned int)
// ORIG T 0x085a73a6 sz 0x3df，报告 docs/class_func_reports/CParty/drop_item_gm.md。
// GM 掉落指定物品到队伍战场。ORIG eax 返回错误码：0 成功 / -1 物品不存在 /
// 0x13 目标交易中 / 0x16 战场掉落失败 / 0x17 绑定装备禁止 GM 丢弃；
// 本 TU 按 CParty.h:630 既有 void 声明定义（调用方 GmCmds_D 忽略返回值）。
// ---------------------------------------------------------------------------
void CParty::drop_item_gm(CUser* user, unsigned int itemIdx)
{
    if (user->CheckInTrade())
        return;                                        // ORIG ret 0x13

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return;                                        // ORIG ret -1

    Inven_Item invenItem;
    invenItem.m_addInfo = (int)itemIdx;
    item->make_item(invenItem);                        // vtable slot make_item
    if (invenItem.m_addInfo == 0)
        return;

    if (invenItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
    {
        // 装备类物品实为 CEquipItem；+0x234 = CEquipItem::m_itemType。
        const CEquipItem* equip = static_cast<const CEquipItem*>(item);
        if (equip->m_itemType == Inven_Item::ITEM_TYPE_INVALID ||
            equip->m_itemType <= Inven_Item::ITEM_TYPE_9)
            return;                                    // ORIG ret 0x17
    }

    map_item drop;                                     // ORIG ctor 081512cc
    drop.m_count = 0;                                  // byte +0x00 = 0
    drop.m_item.set_add_info(1);                       // 内嵌 item.add_info = 1
    drop.m_item.SetItemAttr(invenItem.GetItemAttr());  // +0x16 ← src GetItemAttr
    drop.m_item.m_fieldb = invenItem.m_fieldb;         // word +0x1b ← src endurance
    drop.m_item.m_fieldd = invenItem.m_fieldd;         // dword +0x1d ← src union
    drop.m_item.m_amp = invenItem.m_amp;               // +0x21..+0x2e 整体（14B POD）
    drop.m_item.m_random.m_random =                    // RandomOption 子块整体（14B）
        invenItem.m_random.m_random;
    drop.m_item.m_field0 = invenItem.m_field0;         // byte +0x10
    drop.m_item.m_field1 = invenItem.m_field1;         // byte +0x11（ITEM_TYPE）
    drop.m_item.m_addInfo = invenItem.m_addInfo;       // dword +0x12（item idx）
    drop.m_item.m_upgradeSep.SetUpgradeSeparate(
        invenItem.m_upgradeSep.GetUpgradeSeparate());  // +0x43

    int slotIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                      ->drop_item(drop);               // 按值传 0x54 字节
    if (slotIdx == -1)
        return;                                        // ORIG ret 0x16

    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(300);                             // 0x12c
    packet.put_short(240);                             // 0xf0
    packet.put_short(slotIdx);
    packet.put_int(drop.m_item.m_addInfo);
    packet.put_byte(drop.m_item.GetItemAttr());
    packet.put_int(drop.m_item.get_add_info());
    packet.put_short(drop.m_item.m_fieldb);
    packet.put_int(drop.m_item.m_fieldd);
    packet.put_byte(drop.m_item.m_amp.getAbilityType());
    packet.put_short(drop.m_item.m_amp.getAbilityValue());
    packet.put_packet(drop.m_item);
    packet.put_byte(drop.m_count == 0 ? 1 : 0);
    packet.finalize(true);
    send_to_party(packet);
}

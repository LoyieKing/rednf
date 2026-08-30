// df_game_r Redeem_Item::CRedeemItem —— 恢复实现（独立 TU）。
// 语义依据 docs/class_func_reports/Redeem_Item__CRedeemItem/（ORIG 反汇编 + Ghidra C）。
// 仅实现 CUser::Add_RedeemInfo 调用的两个方法 + 私有辅助 _isFPItem；
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/GetItemPrice/RedeemItem 等）未建模。

#include "Redeem_Item__CRedeemItem.h"

#include "CDataManager.h"
#include "CItem.h"
#include "GameWorld.h"        // G_GameWorld / IsEnchantRevisionChannel
#include "PacketGuard.h"

namespace Redeem_Item
{

// ORIG 命名空间内 static 常量（_ZN11Redeem_ItemL...E，r 段；值实测
// 0x08cd21c8..）：MAX_COUNT=21 / THROW_FEE=1000 / BUY_FEE=1000。
static const int MAX_COUNT = 21;
static const int THROW_FEE = 1000;
static const int BUY_FEE = 1000;

// ORIG 0x085f7c04：itemId 在 (0x1963, 0x1b57] 区间 → true（"FP 物品"判定）。

bool CRedeemItem::_isFPItem(const Inven_Item& item) const
{
    int itemId = item.m_addInfo;
    return itemId > 0x1963 && itemId <= 0x1b57;
}

// ORIG 0x085f76f0：空物品/FP 物品/固定 ID 表/物品不存在/低稀有度可升级
// 装备/禁兑换物品 → 0；否则 1。
bool CRedeemItem::isAddableFilter(const Inven_Item& item)
{
    if (item.isEmpty())
        return 0;
    if (_isFPItem(item))
        return 0;
    switch (item.m_addInfo)
    {
    case 0x114c:
    case 0xcf1:
    case 0xbdd:
    case 0x1057:
    case 0x10fe:
    case 0x1157:
    case 0x1159:
    case 0x1973:
    case 0x1ee1:
    case 0x28e770:
        return 0;
    default:
        break;
    }
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    bool bVar2;
    if (!item.isEquipableItemType() || ci->get_rarity() != 0 ||
        item.GetUpgrade() != 0)
    {
        bVar2 = false;
    }
    else
    {
        bVar2 = true;
    }
    if (bVar2)
        return 0;
    // ORIG 0x085f77f4 调用 CItem::IsBanRedeemItem()（非内联，生成 call）
    if (ci->IsBanRedeemItem() != 0)
        return 0;
    return 1;
}

// ORIG 0x085f71a8：构造 Inven_Item_Expand_RedeemInfo（复制物品前 0x3c 字节 +
// 末字节 + count=param2+1000/1000），alter()，list（this+8）超 0x14 项时
// pop_front，push_back，返回 1。
int CRedeemItem::AddRedeemList(const Inven_Item& item, int param2, bool param3)
{
    Inven_Item_Expand_RedeemInfo info;
    std::memcpy(&info.m_data, &item, 0x3c);
    info.m_byte3c = *((const unsigned char*)&item + 0x3c);
    info.m_count40 = param3 ? (param2 + THROW_FEE) : BUY_FEE;
    alter();
    if (m_redeemList.size() > 0x14)
        m_redeemList.pop_front();
    m_redeemList.push_back(info);
    return 1;
}

// 未达 end()），逐项整体赋值 0x44 字节到 pData + i*0x44 + 8（ORIG 为
// rep movsl 0x11，结构整体赋值即内联该复制）。
void CRedeemItem::getData(char* pData) const
{
    char* dst = pData;
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        m_redeemList.begin();
    *(int*)(dst + 4) = (int)m_redeemList.size();
    int i = 0;
    for (; i <= 0x14 && it != m_redeemList.end(); ++it, ++i)
        *(Inven_Item_Expand_RedeemInfo*)(dst + i * 0x44 + 8) = *it;
}


// ORIG T 0x085f73ee _ZN11Redeem_Item11CRedeemItem15Send_RedeemListEP5CUser：
// header(0,0x121)/byte(list size)/逐项（≤0x15 项）：+0x40 计数、Inven_Item
// 字段展开、附魔通道分支、增幅属性、3rd Chronicle 子包与 put_packet，最后
// finalize + Send 并返回 1。
int CRedeemItem::Send_RedeemList(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x121);
    packet.put_byte((int)m_redeemList.size());

    int count = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::iterator it =
             m_redeemList.begin();
         count < MAX_COUNT && it != m_redeemList.end(); ++it, ++count)
    {
        const Inven_Item_Expand_RedeemInfo& entry = *it;

        // Inven_Item_Expand_RedeemInfo 前 0x3c 字节即 Inven_Item 镜像
        // （AddRedeemList 拷贝实证），按字段口径逐项写出。
        Inven_Item item;
        std::memcpy(&item, &entry.m_data[0], 0x3c);
        unsigned char lastByte = entry.m_byte3c;

        packet.put_int(entry.m_count40);                    // +0x40 兑换计数
        packet.put_int(item.m_addInfo);                     // +0x02 物品 idx
        packet.put_int(item.m_addInfo2);                    // +0x07 数量/强化值
        packet.put_byte((int)item.GetItemAttr());           // 属性字节
        packet.put_short((int)item.m_fieldb);               // +0x0b 耐久
        (void)lastByte;
        packet.put_byte((int)(unsigned char)item.m_field0); // +0x00 绑定标记

        if (!G_GameWorld()->IsEnchantRevisionChannel())
            packet.put_int(item.m_fieldd);                  // +0x0d 强化信息
        else
            packet.put_int(0);

        packet.put_byte((int)item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_amp.getAbilityValue() & 0xffff);

        WongWork::CItemUpgrade* upgrade = G_CDataManager()->m_itemUpgrade;
        upgrade->make3rdChroniclePacket(user, item, &packet);
        packet.put_packet(item);
    }

    packet.finalize(true);
    user->Send(packet);
    return 1;
}

}  // namespace Redeem_Item

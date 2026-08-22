// df_game_r Game/ WongWork::CItemUpgrade 实现。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"

#include <cstring>

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
namespace WongWork
{
CItemUpgrade::~CItemUpgrade()
{
    // ORIG 0x85462c6：析构 +0x4e8 的 CMTRand*（构造时 new，见 0x8546247）。
    void** pRand = reinterpret_cast<void**>(reinterpret_cast<char*>(this) + 0x4e8);
    if (*pRand)
    {
        ::operator delete(*pRand);
        *pRand = 0;
    }
}

float CItemUpgrade::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    // ORIG 0x8549148：
    //   flag==true  → GlobalData::s_itemAmplifier_->getUpgradeInfoTable()（=+0x100）
    //   flag==false → this+0x04（CItemUpgradeTable 子对象，getUpgradeInfoTable 返回自身）
    //   返回 table[idx + 0x9c] 的第 4 字节偏移处 float。
    const char* table;
    if (flag)
        table = reinterpret_cast<const char*>(GlobalData::s_itemAmplifier_) + 0x100;
    else
        table = reinterpret_cast<const char*>(this) + 0x04;
    float rate = 0.0f;
    memcpy(&rate, table + (idx + 0x9c) * 4 + 4, sizeof(rate));
    return rate;
}

void CItemUpgrade::make3rdChroniclePacket(CUser* user,
                                          const Inven_Item& item,
                                          PacketGuard* packet)
{
    if (user == 0 || packet == 0)
        return;
    const unsigned char* fields =
        reinterpret_cast<const unsigned char*>(&item) + 0x1f;
    int count = (fields[0] & 1) != 0 ? 1 : 0;
    if (count != 0 && (fields[1] & 1) != 0)
        count = 2;
    if (count == 0)
    {
        packet->put_byte(0);
        return;
    }
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return;
    packet->put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        const unsigned char field = fields[i];
        const int kind = (field >> 1) & 3;
        const int enchantId = kind == 0 ? 0x4e6 :
                              kind == 1 ? 0x4e7 :
                              kind == 2 ? 0x4e8 : 0x4e9;
        packet->put_int(enchantId);
        packet->put_byte(user->get_charac_job());
        packet->put_byte(user->getCurCharFirstGrowType());
        packet->put_byte(itemScript->GetItemType());
        packet->put_byte(field >> 3);
    }
}
}

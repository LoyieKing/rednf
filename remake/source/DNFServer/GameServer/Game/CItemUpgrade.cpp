// df_game_r Game/ WongWork::CItemUpgrade 实现。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"


namespace WongWork
{
class CItemUpgrade
{
public:
    void make3rdChroniclePacket(CUser* user, const Inven_Item& item,
                                PacketGuard* packet);
};

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

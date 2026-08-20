// df_game_r Game/ item_lock::CItemLock 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CExpandEquipslot.h"
#include "CUser.h"
#include <vector>

namespace item_lock
{

struct stItemLockRef
{
    char m_field0;       // +0x00
    char m_space;        // +0x01
    unsigned short m_slot;  // +0x02
};

class CItemLock
{
public:
    std::vector<stItemLockRef>& GetItemLockRefVec();
    void SendItemLockList(CUser* user);
    bool CheckItemLock(unsigned char b) const;
    void MakeItemLockPacket(PacketGuard& packet, unsigned char b) const;
    static int CheckItemLock(CExpandEquipslot* data);
};

std::vector<stItemLockRef>& CItemLock::GetItemLockRefVec()
{
    static std::vector<stItemLockRef> s;
    return s;
}

void CItemLock::SendItemLockList(CUser*)
{
}

bool CItemLock::CheckItemLock(unsigned char) const
{
    return false;
}

void CItemLock::MakeItemLockPacket(PacketGuard&, unsigned char) const
{
}

int CItemLock::CheckItemLock(CExpandEquipslot*)
{
    return 0;
}

}  // namespace item_lock

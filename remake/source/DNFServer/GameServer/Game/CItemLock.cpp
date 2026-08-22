// df_game_r Game/ item_lock::CItemLock 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CExpandEquipslot.h"
#include "CUser.h"
#include "CTitleBook.h"   // item_lock::stItemLockRef 权威定义
#include <vector>

namespace item_lock
{

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

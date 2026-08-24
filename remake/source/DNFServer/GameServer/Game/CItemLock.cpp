// df_game_r Game/ item_lock::CItemLock 实现（S5 语义对齐批次）。
// 依据 docs/class_func_reports/item_lock__CItemLock/*.md：
//   SendItemLockList 0x85427a0 / MakeItemLockPacket 0x85426a6 /
//   CheckItemLock(uchar) 0x8541a96 / GetItemLockInfo 0x854261e /
//   UpdateItem 0x8541e4e / UpdateAvatarItem 0x8541d42 / UpdateCreatereItem
//   0x8541c36。
#include "CTitleBook.h"   // item_lock::stItemLockRef / CItemLock 权威定义

#include <string.h>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "CUser.h"
#include "CInventory.h"
#include "PacketGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CSystemTime.h"
#include "Inven_Item.h"

namespace item_lock
{

std::vector<stItemLockRef>& CItemLock::GetItemLockRefVec()
{
    static std::vector<stItemLockRef> s;
    return s;
}

// ORIG 0x854261e：map 查找；命中返回 state 并写解锁时刻，未命中返回 0。
unsigned char CItemLock::GetItemLockInfo(unsigned char lock, int& outSec) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end())
        return 0;
    outSec = it->second.m_unlockSec;
    return it->second.m_state;
}

bool CItemLock::CheckItemLock(unsigned char b) const
{
    // ORIG：map.find(b) != end()。const_cast 与 ORIG 行为一致（仅查找）。
    return m_lockMap.find(b) != m_lockMap.end();
}

// ORIG 0x85426a6：未锁定 → put_byte(0)；否则 put_byte(state)，state==2 时
// 再 put_int(剩余秒数 = unlockSec + 0x3f480 - now，负值截 0)。
void CItemLock::MakeItemLockPacket(PacketGuard& packet, unsigned char b) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(b);
    if (it == m_lockMap.end())
    {
        packet.put_byte(0);
        return;
    }
    packet.put_byte(it->second.m_state);
    if (it->second.m_state == 2)
    {
        int remain = it->second.m_unlockSec + 0x3f480 -
                     GlobalData::s_systemTime_.getCurSec();
        if (remain < 0)
            remain = 0;
        packet.put_int(remain);
    }
}

// ---- UpdateAvatarItem（ORIG 0x8541d42）：包 0x1c2 = uid、item+0x1c、lock ----
static void UpdateAvatarItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xbf), true);
    **guard << 0x1c2;
    **guard << (int)user->GetUID();
    **guard << *(int*)((char*)item + 0x1c);
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- UpdateCreatereItem（ORIG 0x8541c36）：包 0x1c1，同构 ----
static void UpdateCreatereItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xb5), true);
    **guard << 0x1c1;
    **guard << (int)user->GetUID();
    **guard << *(int*)((char*)item + 0x1c);
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x8541e4e：item 空间字节(+0x01)==8 → creature 包；==5 → avatar 包。
void CItemLock::UpdateItem(CUser* user, Inven_Item* item)
{
    unsigned char space = *(unsigned char*)((char*)item + 1);
    if (space == 8)
        UpdateCreatereItem(user, item);
    else if (space == 5)
        UpdateAvatarItem(user, item);
}

// ORIG 0x85427a0：包 0xfb（put_header(0,0xfb)），short 计数占位后逐项写；
// 已解锁项清锁并 UpdateItem；倒计时项按频道 equipslot 开关重映射 space
// (3<->0x12)，写 space/slot/state，state==2 再写剩余秒数；末尾回填计数，
// finalize 后发送并清空 m_lockList。
void CItemLock::SendItemLockList(CUser* user)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->clear();
    buf->put_header(0, 0xfb);
    int countIndex = buf->get_index();
    int count = 0;
    buf->put_short(0);

    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        stItemLockRef& ref = *it;
        int remainSec = 0;
        unsigned char state = GetItemLockInfo((unsigned char)ref.m_lock,
                                              remainSec);
        if (state == 0)
        {
            // 已解锁：清除物品上的锁位并同步。
            CInventory* inven = user->getCurCharacInvenW();
            if (inven != 0)
            {
                int invenType =
                    GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)ref.m_space);
                Inven_Item* slot = inven->GetInvenRef(invenType, ref.m_slot);
                if (slot != 0)
                {
                    slot->m_amp.SetLock(0);
                    UpdateItem(user, slot);
                }
            }
        }
        else
        {
            unsigned char outSpace = (unsigned char)ref.m_space;
            if (user->getCurChannelEquipslotSwitch() == 1 &&
                (ref.m_slot > 9 || ref.m_slot < 0x16))
            {
                if (ref.m_space == 3)
                    outSpace = 0x12;
                else if (ref.m_space == 0x12)
                    outSpace = 3;
            }
            buf->put_byte(outSpace);
            buf->put_short(ref.m_slot);
            buf->put_byte(state);
            if (state == 2)
            {
                int remain = remainSec + 0x3f480 -
                             GlobalData::s_systemTime_.getCurSec();
                if (remain < 0)
                    remain = 0;
                buf->put_int(remain);
            }
            ++count;
        }
    }
    buf->put_byte(countIndex, count);
    buf->finalize(true);
    user->Send(packet);
    m_lockList.clear();
}

}  // namespace item_lock
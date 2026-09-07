// df_game_r Game/ item_lock::CItemLock + CItemLockTimer 实现（W6_TailA 批）。
// 依据 docs/class_func_reports/item_lock__CItemLock/*.md（汇编为准）与
// docs/class_func_reports/item_lock__CItemLockTimer/*.md；worklist
// /tmp/only_orig/item_lock.txt 全部 33 个符号由本 TU 与共享头产生。
//
// 对象布局（ORIG ctor 0x854156a）：
//   charac_expand::CData 基类（vptr +0x00 / m_bIsSave +0x04）
//   +0x08 map<unsigned char, stItemLockInfo>   当前锁表
//   +0x20 stack<unsigned char, deque<uchar> >  空闲 lock 号池（IsAbleItemLock 判空）
//   +0x48 vector<stItemLockRef>                SendItemLockList 待发列表
//   +0x54 CItemLockTimer                       解锁倒计时计数
// 载体为角色扩展数据区（跨 TU reinterpret_cast，见 CUser/CAccountCargo/CGameManager）。
#include "CTitleBook.h"   // item_lock::stItemLockRef / stItemLockInfo /
                          // CItemLock / CItemLockTimer 权威定义

#include <string.h>
#include <utility>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "LogManager.h"
#include "SigTypes.h"
#include "TimerQueue.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "PacketGuard.h"
// ---- 最小类型镜像（仓库既有模式，见 CExpandEquipslot.cpp:36）----
// CItemLock.cpp 不 include CUser.h/CInventory.h：二者的联合依赖域含先行批次
// 的 GCC4.4 assert/枚举缺口；此处仅镜像本 TU 调用的方法签名（声明），外部
// 链接定义仍由权威 TU 提供，mangled ABI 完全一致。
// [hc N 记 · b20] CUser 具名字段 m_historyLog（+0x79700）在权威 CUser.h 存在，
// 但本 TU 为镜像隔离 TU（不 include CUser.h，否则与本文件镜像 `class CUser`
// 重复定义 + 先行批次联合依赖缺口），故 cUserHistoryLog* 取址仍按 §4.3「跨 TU
// 隔离」例外保留 `(cUserHistoryLog*)((char*)user + 0x79700)` 裸偏移形态，记 N。
// 证据：CUser.h 内 `cUserHistoryLog m_historyLog;  // +0x79700`；ORIG 各取址点
// lea 0x79700(%reg)。
class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);      // ORIG（槽位访问）
};

class CUserCharacInfo
{
public:
    int getCurCharacNo() const;                            // ORIG 0x080cbc4e
    CInventory* getCurCharacInvenW();                      // ORIG（继承转发）
    unsigned char getCurChannelEquipslotSwitch();          // ORIG 0x086863? 语义一致
    bool FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot,
                      CUser* user) const;                  // ORIG 0x08680e04
};

class cUserHistoryLog
{
public:
    void ItemLockUsingLog(unsigned char lockType, int a, int b, int c,
                          int d);                          // ORIG 0x08689808
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;                           // ORIG 0x080c8c96
    int get_charac_no(int chIdx);                          // ORIG 0x0864dfa0
    Inven_Item* GetCurCharacInventoryRef(unsigned short area,
                                         unsigned short slot); // ORIG 0x08680f2e
    void Send(PacketGuard& packet);                        // ORIG 0x086485ba
};

int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);   // ORIG W 0x080f7845（switch 映射；权威定义 CTitleBook.cpp）
// ORIG 匿名常量（rodata 内联实例 _ZN9item_lockL20CHECK_ITEM_LOCK_TIMEE
namespace item_lock
{

// ORIG C1/C2 0x854156a：CData 基类构造后依次默认构造 map/stack/vector 与
// 计时器成员（全部零态）。
CItemLock::CItemLock()
{
}

// = 0x3c、_ZN9item_lockL24ITEM_UNLOCK_WAITING_TIMEE = 0x3f480，08b25970 取值）。
static const int CHECK_ITEM_LOCK_TIME     = 0x3c;      // 计时器周期 60 秒
static const int ITEM_UNLOCK_WAITING_TIME = 0x3f480;   // 解锁等待期 3 天

// ===========================================================================
// CItemLockTimer（ORIG 方法均 W 内联）
// ===========================================================================

// ORIG 0x8543044：m_count = 0。
CItemLockTimer::CItemLockTimer()
    : m_count(0)
{
}

// ORIG 0x8543052：m_count = 0。
void CItemLockTimer::Reset()
{
    m_count = 0;
}

// ORIG 0x854303a：返回 m_count（非 const）。
int CItemLockTimer::GetTimerCount()
{
    return m_count;
}

// ORIG 0x854147c：计数为 0 时注册 TIMER_MESSAGE 0x88 定时器（周期 60s、
// 参数 uid / characNo）；无论如何计数 +1。
void CItemLockTimer::IncreaseItemLockTimer(CUser* user)
{
    if (m_count == 0)
    {
        // ORIG：getCurCharacNo→%esi、GetUID→%ebx 跨 G_TimerQueue call 缓存，
        // 实参内联复现（P24/P52，禁具名局部槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x88, CHECK_ITEM_LOCK_TIME,
                                    0, (unsigned int)user->getCurCharacNo());
    }
    ++m_count;
}

// ORIG 0x85414f0：计数 -1。
void CItemLockTimer::DecreaseItemLockTimer()
{
    --m_count;
}

// ORIG 0x8541502：计数非 0 时重新注册 TIMER_MESSAGE 0x88 定时器（保证链式触发）。
void CItemLockTimer::NextItemLockTimer(CUser* user)
{
    if (m_count != 0)
    {
        // ORIG：getCurCharacNo→%esi、GetUID→%ebx 跨 G_TimerQueue call 缓存，
        // 实参内联复现（P24/P52，禁具名局部槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x88, CHECK_ITEM_LOCK_TIME,
                                    0, (unsigned int)user->getCurCharacNo());
    }
}

// ===========================================================================
// CItemLock 生命周期
// ===========================================================================

// ORIG D1/D2 0x8541644 / D0 0x85416f6：基类 vptr 回填后容器逆序析构；
// 编译器自动生成等价序列。
CItemLock::~CItemLock()
{
}

std::vector<stItemLockRef>& CItemLock::GetItemLockRefVec()
{
    return m_lockList;
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
    // ORIG 0x8541a96：map.find(b) != end()。
    return m_lockMap.find(b) != m_lockMap.end();
}

// ORIG 0x85426a6：未锁定 → put_byte(0)；否则 put_byte(state)，state==2 时
// 再 put_int(剩余秒数 = unlockSec + 3天 - now，负值截 0)。
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
        int remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
                     GlobalData::s_systemTime_.getCurSec();
        if (remain < 0)
            remain = 0;
        packet.put_int(remain);
    }
}

// ===========================================================================
// DB 同步包（StreamPool → MsgQueueMgr DB_Q）
// ===========================================================================

// ORIG 0x8541d42：包 0x1c2 = (0x1c2, uid, item.m_addInfo2(+7), GetLock() & 0xff)。
void CItemLock::UpdateAvatarItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xbf), true);
    **guard << 0x1c2;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x8541c36：包 0x1c1，同构。
void CItemLock::UpdateCreatereItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xb5), true);
    **guard << 0x1c1;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x8541e4e：item 空间字节(+0x01)==8 → UpdateAvatarItem(0x1c2)；
// ==5 → UpdateCreatereItem(0x1c1)。（注意 ORIG 判定次序：==8 先，调用的是 Avatar。）
void CItemLock::UpdateItem(CUser* user, Inven_Item* item)
{
    if (item->m_field1 == 8)
        UpdateAvatarItem(user, item);
    else if (item->m_field1 == 5)
        UpdateCreatereItem(user, item);
}

// ORIG 0x85427a0：包 0xfb（put_header(0,0xfb)），short 计数占位后逐项写；
// 已解锁项清锁并 UpdateItem；倒计时项按频道 equipslot 开关重映射 space
// (3<->0x12)，写 space/slot/state，state==2 再写剩余秒数；末尾回填计数，
// finalize 后发送并清空 m_lockList。
void CItemLock::SendItemLockList(CUser* user)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfb);
    int countIndex = packet.get_index();
    int count = 0;
    packet.put_short(0);

    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        stItemLockRef& ref = *it;
        int remainSec = 0;
        unsigned char state = GetItemLockInfo((unsigned char)ref.m_lock,
                                              remainSec);
        if (state == 0)
        {
            // 已解锁：清除物品上的锁位并同步（ORIG 经 GetCurCharacInventoryRef
            // 全分派：含 cargo(space=2)/title(space=0x13..0x16)）。
            Inven_Item* slot = user->GetCurCharacInventoryRef(ref.m_space, ref.m_slot);
            if (slot != 0)
            {
                slot->m_amp.SetLock(0);
                UpdateItem(user, slot);
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
            packet.put_byte(outSpace);
            packet.put_short(ref.m_slot);
            packet.put_byte(state);
            if (state == 2)
            {
                int remain = remainSec + ITEM_UNLOCK_WAITING_TIME -
                             GlobalData::s_systemTime_.getCurSec();
                if (remain < 0)
                    remain = 0;
                packet.put_int(remain);
            }
            ++count;
        }
    }
    packet.put_byte(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
    m_lockList.clear();
}

// ===========================================================================
// IsAble* 门禁（返回码即错误表索引；ORIG 精确常量）
// ===========================================================================

// ORIG 0x8541afe：空闲号池空 → 0x16 可锁；非空 → 0（CTitleBook.h:290 注释一致；
// 参数 lock 未使用）。
int CItemLock::IsAbleItemLock(unsigned char /*lock*/) const
{
    return m_lockStack.empty() ? 0x16 : 0;
}

// ORIG 0x8541b2a：map 未命中或 state!=1 → 0x13；state==1 → 0。
int CItemLock::IsAbleItemUnlock(unsigned char lock, int& outSec) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end() || it->second.m_state != 1)
        return 0x13;
    outSec = it->second.m_unlockSec;
    return 0;
}

// ORIG 0x8541bb0：map 未命中或 state!=2 → 0x13；state==2 → 0。
int CItemLock::IsAbleItemUnlockCancel(unsigned char lock) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end() || it->second.m_state != 2)
        return 0x13;
    return 0;
}

// ===========================================================================
// 锁 / 解锁业务
// ===========================================================================

// ORIG 0x8542192：物品未带锁号时从空闲池取出一个号，写 {unlockSec=now,
// state=1} 入 map；alter 后把新号写回物品并发请求包；日志 reason=0。
int CItemLock::DoItemLock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    if (item->m_amp.GetLock() != 0)
        return 0;
    if (m_lockStack.empty())
        return 0;  // ORIG 池空即无可分配号（栈 top 为 undefined 前提）

    unsigned char newLock = m_lockStack.top();
    m_lockStack.pop();

    stItemLockInfo info;
    memset(&info, 0, sizeof(info));
    info.m_unlockSec = GlobalData::s_systemTime_.getCurSec();
    info.m_state = 1;

    std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
              std::map<unsigned char, stItemLockInfo>::mapped_type> value =
        std::make_pair(newLock, info);
    m_lockMap.insert(value);

    alter();
    item->m_amp.SetLock(newLock);
    UpdateItem(user, item);
    SendRequestItemLock(user, space, slot);

    cUserHistoryLog* history = (cUserHistoryLog*)((char*)user + 0x79700);  // §4.3 例外（见顶注）
    history->ItemLockUsingLog(0, space, slot, item->m_addInfo,
                              item->m_addInfo2);
    return 1;
}

// ORIG 0x854231a：物品已上锁且 map 命中时置 state=2 / unlockSec=now，
// 计时器 +1 并回发剩余秒数；日志 reason=1。
int CItemLock::DoItemUnlock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    if (item->m_amp.GetLock() == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(item->m_amp.GetLock());
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 2;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.IncreaseItemLockTimer(user);

    int remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
                 GlobalData::s_systemTime_.getCurSec();
    if (remain < 0)
        remain = 0;

    SendRequestItemUnlock(user, space, slot, remain);

    cUserHistoryLog* history = (cUserHistoryLog*)((char*)user + 0x79700);  // §4.3 例外（见顶注）
    history->ItemLockUsingLog(1, space, slot, item->m_addInfo,
                              item->m_addInfo2);
    return 1;
}

// ORIG 0x85424c2：物品已上锁且 map 命中时置 state=1 / unlockSec=now，
// 计时器 -1；日志 reason=2。
int CItemLock::DoItemUnlockCancel(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    if (item->m_amp.GetLock() == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(item->m_amp.GetLock());
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 1;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.DecreaseItemLockTimer();

    SendRequestItemUnlockCancel(user, space, slot);

    cUserHistoryLog* history = (cUserHistoryLog*)((char*)user + 0x79700);  // §4.3 例外（见顶注）
    history->ItemLockUsingLog(2, space, slot, item->m_addInfo,
                              item->m_addInfo2);
    return 1;
}

// ORIG 0x8542a6a：定时器到期入口 → PopItemLock，恒返 1。
int CItemLock::OnItemUnlockWaitTimeout(CUser* user)
{
    PopItemLock(user);
    return 1;
}

// ORIG 0x8542a8a：
//   新条目 state==0                → 号回收进空闲池；
//   state==2 且已过等待期          → 清 info 后回收 + alter；
//   其余                           → 继续计时再入 map（state==2 时续上计时器）。
void CItemLock::PushItemLock(CUser* user, unsigned char lock,
                             stItemLockInfo* info)
{
    if (info->m_state == 0)
    {
        m_lockStack.push(lock);
        return;
    }

    if (info->m_state == 2)
    {
        if (info->m_unlockSec <=
            GlobalData::s_systemTime_.getCurSec() - ITEM_UNLOCK_WAITING_TIME)
        {
            info->m_state = 0;
            info->m_unlockSec = 0;
            m_lockStack.push(lock);
            alter();
            return;
        }
        m_lockTimer.IncreaseItemLockTimer(user);
    }

    std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
              std::map<unsigned char, stItemLockInfo>::mapped_type> value =
        std::make_pair(lock, *info);
    m_lockMap.insert(value);
}

// ORIG 0x8542b80：遍历 map，将已过 3 天等待期且 state==2 的条目逐个收尾：
// FindItemLock 定位物品槽 → 清物品锁号 + DB 包；equipslot 开关频道空间重映射
// (3<->0x12)；SendItemUnlock 后号回收进空闲池、erase 条目、计时器 -1、alter、
// 日志 reason=3；FindItemLock 失败直接返回。结束后 NextItemLockTimer 链式续期。
void CItemLock::PopItemLock(CUser* user)
{
    if (m_lockTimer.GetTimerCount() == 0)
        return;

    int now = GlobalData::s_systemTime_.getCurSec();

    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        bool due = (now >= it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME &&
                    it->second.m_state == 2);
        if (!due)
        {
            ++it;
            continue;
        }

        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slotVal = 0;
        if (!((const CUserCharacInfo*)user)->FindItemLock(it->first, space,
                                                          slotVal,
                                                          (CUser*)user))
            return;  // ORIG：定位失败立即退出（保留遍历位置）

        Inven_Item* item =
            user->GetCurCharacInventoryRef((unsigned short)space,
                                           (unsigned short)slotVal);
        if (item != 0)
        {
            item->m_amp.SetLock(0);
            UpdateItem(user, item);
        }

        int sendSpace = (int)space;
        if (user->getCurChannelEquipslotSwitch() == 1 &&
            slotVal > 9 && slotVal < 0x16)
        {
            if ((ENUM_ITEMSPACE)sendSpace == (ENUM_ITEMSPACE)3)
                sendSpace = 0x12;
            else if ((ENUM_ITEMSPACE)sendSpace == (ENUM_ITEMSPACE)0x12)
                sendSpace = 3;
        }

        SendItemUnlock(user, sendSpace, slotVal);
        m_lockStack.push(it->first);
        // GCC4.4 map::erase 返回 void：先备份后继再删除（ORIG erase 前自增同序）。
        std::map<unsigned char, stItemLockInfo>::iterator next = it;
        ++next;
        m_lockMap.erase(it);
        it = next;

        m_lockTimer.DecreaseItemLockTimer();
        alter();

        cUserHistoryLog* history = (cUserHistoryLog*)((char*)user + 0x79700);  // §4.3 例外（见顶注）
        if (item != 0)
            history->ItemLockUsingLog(3, sendSpace, slotVal,
                                      item->m_addInfo, item->m_addInfo2);
    }

    m_lockTimer.NextItemLockTimer(user);
}

// ORIG 0x8542e12：m_lockList 中存在指定 lock 号的引用 → 1，否则 0。
int CItemLock::_findItem(unsigned char lock)
{
    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        if ((unsigned char)(*it).m_lock == lock)
            return 1;
    }
    return 0;
}

// ORIG 0x8542e98：核对 map 中所有锁定/倒计时条目是否仍被 m_lockList 引用；
// 未被引用的（角色扩展数据加载不全时）退回空闲池并从 map 删除（state==2 时
// 同时 -1 计时器）；发生过修正则 alter。
void CItemLock::VerifyItemLock(CUser* user)
{
    if (user == 0)
        return;

    bool changed = false;
    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        unsigned int state = (unsigned int)it->second.m_state;
        if (state != 1 && state != 2)
        {
            ++it;
            continue;
        }

        if (_findItem(it->first))
        {
            ++it;
        }
        else
        {
            m_lockStack.push(it->first);
            std::map<unsigned char, stItemLockInfo>::iterator next = it;
            ++next;
            m_lockMap.erase(it);
            it = next;
            if (state == 2)
                m_lockTimer.DecreaseItemLockTimer();
            changed = true;
        }
    }

    if (changed)
        alter();
}

// ===========================================================================
// 客户端应答包
// ===========================================================================

// ORIG 0x85420e0：包 0xfc = put_header(0,0xfc)、byte(space)、short(slot)。
void CItemLock::SendItemUnlock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfc);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8541ea0：包 0x10f = put_header(1,0x110 族第 1 条)、byte(1)、
// byte(space)、short(slot)。注：ORIG put_header 第二参 0x10f。
void CItemLock::SendRequestItemLock(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x10f);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8541f5a：包 0x110 = put_header(1,0x110)、byte(1)、byte(space)、
// short(slot)、int(remainSec)。
void CItemLock::SendRequestItemUnlock(CUser* user, int space, int slot,
                                      int remainSec)
{
    PacketGuard packet;
    packet.put_header(1, 0x110);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x8542026：包 0x111 = put_header(1,0x111)、byte(1)、byte(space)、
// short(slot)。
void CItemLock::SendRequestItemUnlockCancel(CUser* user, int space, int slot)
{
    PacketGuard packet;
    packet.put_header(1, 0x111);
    packet.put_byte(1);
    packet.put_byte(space);
    packet.put_short(slot);
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// 角色扩展数据生命周期（charac_expand::CData 虚接口）
// ===========================================================================

// ORIG 0x8541714：pData 为 100 × 8 字节 DB 区块；自 i=99 向下 PushItemLock
// （lock 号 = i+1），使低号位最终落在栈顶（与空闲号池 LIFO 序一致）。恒真。
bool CItemLock::loadData(CUser* user, char* pData)
{
    char* data = pData;   // ORIG 0x854171d mov %eax,-0x10 存槽复用
    for (int i = 99; i >= 0; --i)
    {
        PushItemLock(user, (unsigned char)(i + 1),
                     (stItemLockInfo*)(data + i * 8));
    }
    return true;
}

// ORIG 0x8541958：map 逆编号回写 DB 区块（idx = key-1 ∈ [0,99]；每项 8 字节
// = unlockSec + {state,pad} 4 字节整体）；越界记 LogManager 错误。
void CItemLock::getData(char* pData) const
{
    for (std::map<unsigned char, stItemLockInfo>::const_iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x6c, "itemlock error index %d", idx);
            continue;
        }
        *(int*)(pData + idx * 8) = it->second.m_unlockSec;
        // +4 写整 4 字节（state + pad，ORIG mov 0x8(%eax) 整字读写口径）。
        *(unsigned int*)(pData + idx * 8 + 4) =
            *(const unsigned int*)&it->second.m_state;
    }
}

// ORIG 0x854176e：SIG_ITEM_LOCK_UPDATE（0x324 字节）出队至 LOGDB→DB_Q(2)：
// << 0x1b8、<< uid；GetInBuffer 清零后 +0 = get_charac_no(-1)，随后与 getData
// 相同的 idx/key-1 校验与写入；越界 LogManager 错误；put(DB_Q) 后析构。恒真。
bool CItemLock::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0x4b), true);
    **guard << 0x1b8;
    **guard << (int)user->GetUID();

    SIG_ITEM_LOCK_UPDATE* sig = guard->GetInBuffer<SIG_ITEM_LOCK_UPDATE>();
    memset(sig, 0, 0x324);
    *(int*)sig = user->get_charac_no(-1);

    for (std::map<unsigned char, stItemLockInfo>::iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x59, "itemlock error index %d", idx);
            continue;
        }
        *(int*)((char*)sig + idx * 8 + 4) = it->second.m_unlockSec;
        *(unsigned int*)((char*)sig + idx * 8 + 8) =
            *(const unsigned int*)&it->second.m_state;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ORIG 0x8541a3e：清空锁表 / 待发列表，抽干空闲号池，复位计时器。
void CItemLock::_reset()
{
    m_lockMap.clear();
    m_lockList.clear();
    while (!m_lockStack.empty())
        m_lockStack.pop();
    m_lockTimer.Reset();
}

}  // namespace item_lock

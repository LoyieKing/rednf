#include "LogManager.h"
// df_game_r WarRoom（G2-3 PvP 链，战场房间，池化 40）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/WarRoom.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类调用一律 include 权威头 + 真实类型调用（无 extern "C" asm 桥）。
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CUser.h"           // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CGameManager.h"    // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CPrivateStoreMgr.h" // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

// ---- 本地枚举（与 CInventory.h 同 guard，避免同时包含时重定义） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

// ---- 本地最小视图（权威声明见 CValueStatistic.cpp / CEventManager 头 /
//      HistoryLog.cpp / TimerQueue.h，因各自权威头与既有 include 冲突或尚未
//      建立，暂以本地声明 + 真实调用；方法定义在权威 TU，mangled 与 ORIG 一致）----
// 注：CEventManager/CEventBase 权威声明在 CGuildServerProxy.h（经 CExpDoubleEvent.h
//     引入），此处不再本地定义避免重定义冲突。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);  // _ZN10HistoryLog12WriteUseCoinEP8_IO_FILEPKc
};

extern CValueStatistic* GetInstanceValueStatistic();
TimerQueue* G_TimerQueue();  // 权威声明将入 TimerQueue.h（CleanPvPAsm 添加中）
char* NumberToString(unsigned int value, int radix);

// ============================================================================
// 依赖子对象（权威声明见 WarField.h，经 WarRoom.h→PvP_deps.h 引入；
//      构造/析构为真实符号）
// ============================================================================

// ============================================================================
// 构造 / 析构
// ============================================================================
WarRoom::WarRoom()
{
    SetState((WARROOM_STATE)-1);
    for (int i = 0; i <= 5; ++i)
        m_slots[i] = 0;
}

WarRoom::~WarRoom()
{
}

// ============================================================================
// 基础访问器（ORIG weak：GetIndex 0x822ee8a / GetState 0x822ee96 /
//   SetState 0x82a5a56 / GenTimerKey 0x86c0b24 / CheckTimerKey 0x863bd92）
// ============================================================================
int WarRoom::GetIndex()
{
    return m_index;
}

int WarRoom::GetState()
{
    return m_state;
}

void WarRoom::SetState(WARROOM_STATE state)
{
    m_state = state;
}

unsigned int WarRoom::GenTimerKey(TIMER_MESSAGE msg)
{
    return ++m_timerKey[msg - 0x3f];
}

bool WarRoom::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return m_timerKey[msg - 0x3f] == key;
}

void WarRoom::CheckState()
{
    if (GetWaiterCount() == 0)
    {
        ChangeState(WARROOM_STATE_0);
    }
    else if ((GetWaiterCount() < 3) && (GetState() == 1))
    {
        ChangeState(WARROOM_STATE_0);
    }
}

void WarRoom::ChangeState(WARROOM_STATE state)
{
    if (m_state != state)
    {
        OnLeaveState(state);
        m_state = state;
        OnEnterState();
    }
}

int WarRoom::GetLiveCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_liveFlag[i] != 0))
            ++count;
    }
    return count;
}

int WarRoom::GetRemainSeat() const
{
    return 6 - GetWaiterCount();
}

int WarRoom::GetUserSlot(CUser* user)
{
    for (int i = 0; i <= 5; ++i)
    {
        if ((CUser*)m_slots[i] == user)
            return i;
    }
    return -1;
}

int WarRoom::GetWaiterCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ++count;
    }
    return count;
}

void WarRoom::SetIndex(int idx)
{
    m_index = idx;
    Reset();
}

void WarRoom::CurSpawnStepReset()
{
    m_curSpawnStep = 0;
}

int WarRoom::Create(CUser* user, char* name, short idx)
{
    m_state = 0;
    m_slots[0] = (int)user;
    m_slotState[0] = 1;
    user->SetWarRoomIndex((short)m_index);
    return 0;
}

void WarRoom::AddBanList(CUser* user)
{
    if (user->isGMUser() != 0)
        return;
    unsigned int id = user->get_acc_id();
    m_map[id] = GlobalData::s_systemTime_.getCurSec();
}

int WarRoom::IsBanUser(CUser* user)
{
    unsigned int id = user->get_acc_id();
    std::map<unsigned int, int>::iterator it = m_map.find(id);
    std::map<unsigned int, int>::iterator e = m_map.end();
    return it != e;
}

void WarRoom::UpdateBanList()
{
    int cur = GlobalData::s_systemTime_.getCurSec();
    std::map<unsigned int, int>::iterator it = m_map.begin();
    std::map<unsigned int, int>::iterator e = m_map.end();
    while (it != e)
    {
        int t = it->second;
        if ((cur - t) >= G_CDataManager()->m_warAreaCounter.m_banInterval / 1000)
        {
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void WarRoom::BattleReset()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->getHackAnalyzer()->reportHackInfo();
            ((CUser*)m_slots[i])->getHackAnalyzer()->resetHackInfo();
            ((CUser*)m_slots[i])->ResetCoinCount();
        }
        m_liveFlag[i] = 0;
        m_slotData[i] = 0;
        m_slotFp[i] = 0;
        m_hostVotes[i] = -1;
    }
    m_field.Reset();
    memset(m_roleKillCnt, 0, sizeof(m_roleKillCnt));
    m_userCount = 0;
    m_flag10 = 0;
    m_flag11 = 0;
    m_flag12 = 0;
    CurSpawnStepReset();
}

void WarRoom::Reset()
{
    BattleReset();
    m_field8 = 0;
    m_levelBand = 2;
    m_roomSeq = 0;
    m_field30 = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_slots[i] = 0;
        m_slotState[i] = 0xff;
        m_slotData[i] = 0;
    }
}

void WarRoom::InvalidUserCheck()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (((CUser*)m_slots[i])->getCurCharacR() == 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x236, 5)
                    ("WarRoom::InvalidUserCheck slot(%d) warRoomIndex(%d)",
                     i, ((CUser*)m_slots[i])->GetWarRoomIndex());
                m_slotState[i] = 0xff;
                m_slots[i] = 0;
                m_liveFlag[i] = 0;
            }
        }
    }
}

int WarRoom::IsJoinable(CUser* user)
{
    if ((GetState() == 4) || (GetState() == 3) || (GetState() == -1) ||
        (GetState() == 2))
        return 0x13;
    if (user->isCompetitionMercenary() != 0)
        return 0x15;
    if (user->getCurCharacMoney() <
        G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
        return 10;
    if (user->CheckFatigue() == 1)
    {
        int level = user->get_charac_level();
        switch ((unsigned char)m_levelBand)
        {
        case 2:
            if ((level < 0x14) || (0x1d < level))
                return 0xe;
            break;
        case 3:
            if ((level < 0x1e) || (0x27 < level))
                return 0xe;
            break;
        case 4:
            if ((level < 0x28) || (0x31 < level))
                return 0xe;
            break;
        case 5:
            if ((level < 0x32) || (0x3b < level))
                return 0xe;
            break;
        case 6:
            if ((level < 0x3c) || (0x46 < level))
                return 0xe;
            break;
        default:
            return 0xe;
        }
        UpdateBanList();
        if (IsBanUser(user) != 0)
            return 0x1e;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
                return 0;
        }
        return 4;
    }
    return 0x43;
}

int WarRoom::WalkOutUserBySlot(int slot)
{
    if (m_slots[slot] == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__, 0x5cc,
                              "WarRoom::WalkOutUserBySlot no slot(%d)", slot);
        return 4;
    }
    ((CUser*)m_slots[slot])->getHackAnalyzer()->reportHackInfo();
    ((CUser*)m_slots[slot])->getHackAnalyzer()->resetHackInfo();
    m_slotState[slot] = 0xff;
    G_GameWorld()->out_from_warroom((CUser*)m_slots[slot]);
    if (m_pDungeon != 0)
    {
        ((CUser*)m_slots[slot])->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                                           (unsigned char)m_levelBand);
    }
    m_slots[slot] = 0;
    m_liveFlag[slot] = 0;
    PacketGuard packet;
    MakeSlotInfo((char*)&packet, slot);
    G_GameWorld()->send_all(packet);
    return 0;
}

int WarRoom::WalkOutUser(CUser* user)
{
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    return WalkOutUserBySlot(slot);
}

int WarRoom::Destroy(int idx)
{
    int user = m_slots[idx];
    if (user == 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                user = m_slots[i];
                break;
            }
        }
    }
    Reset();
    return user;
}

void WarRoom::Open()
{
    SetState(WARROOM_STATE_0);
}

void WarRoom::Close()
{
    SetState((WARROOM_STATE)-1);
}

int WarRoom::ShutDown()
{
    if (m_state < 1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                CUser* u = (CUser*)m_slots[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)7);
            }
        }
        Close();
        return 1;
    }
    m_field30 = 1;
    return 0;
}

void WarRoom::Prepare()
{
    int dungeonIdx =
        G_CDataManager()->m_warAreaCounter.m_warAreaDungeon[(unsigned char)m_levelBand];
    void* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x66e,
                              "WARROOM G_CDataManager()->find_dungeon( %d) fail", 0);
    }
    else
    {
        m_pDungeon = (CDungeon*)dungeon;
        int mapIdx = m_pDungeon->m_warRoomData.m_10;
        void* map = G_CDataManager()->find_map(mapIdx);
        if (map == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x678,
                                  "WARROOM Find Map[%d] Error",
                                  m_pDungeon->m_warRoomData.m_10);
        }
        else
        {
            m_fp = (int)map;
        }
    }
}

void WarRoom::Start()
{
    PacketGuard packet;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            packet.clear();
            packet.put_header(0, 0x5a);
            packet.finalize(true);
            ((CUser*)m_slots[i])->Send(packet);
            if (m_pDungeon != 0)
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    m_pDungeon->get_index(), false);
            }
        }
    }
    ChangeState(WARROOM_STATE_1);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->getHackAnalyzer()->beginCollectHackInfo(
                (CUser*)m_slots[i]);
        }
    }
}

void WarRoom::ReviveAll()
{
    int i = 0;
    while (i <= 5)
    {
        if (m_slots[i] != 0)
        {
            if (m_liveFlag[i] == 0)
            {
                SetCharacterLive(i, (CUser*)m_slots[i], true, false, 0, 0xff);
            }
        }
        ++i;
    }
}

void WarRoom::RemoveKC()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->InitFinishPoint();
            ((CInventory*)((CUser*)m_slots[i])->getCurCharacInvenW())->RemoveKCItem();
        }
    }
}

void WarRoom::CancelAllTimer()
{
    for (int i = 0x40; i <= 0x47; ++i)
        GenTimerKey((TIMER_MESSAGE)i);
}

void WarRoom::SkipLastRankResultTimer()
{
}

int WarRoom::SendWalkOut(CUser* user, ENUM_WALKOUT_TYPE type)
{
    PacketGuard packet;
    packet.put_header(0, 10);
    packet.put_byte(0);
    packet.put_byte((int)type);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}
void WarRoom::SendWarRoomFailMsg()
{
    // P23：嵌套实参求值序 GenTimerKey→m_index→G_TimerQueue()（ORIG ebx/esi 缓存）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                (TIMER_MESSAGE)0x46, 10,
                                GenTimerKey((TIMER_MESSAGE)0x46), 0);
}
void WarRoom::WarRoomFail()
{
    if (GetLiveCount() != 0)
        return;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            AddBanList((CUser*)m_slots[i]);
            CUser* u = (CUser*)m_slots[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)4);
        }
    }
    ChangeState(WARROOM_STATE_0);
}

int WarRoom::HostChangeWarRoom(CUser* user, char a, unsigned long b)
{
    if ((m_slots[(int)a] != 0) && ((CUser*)m_slots[(int)a] == user))
    {
        m_hostVotes[(int)a] = (int)b;
        if (m_userCount == 0)
        {
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x47, 5,
                                        GenTimerKey((TIMER_MESSAGE)0x47), 0);
        }
        m_userCount += 1;
    }
    return 0;
}
void WarRoom::CheckHostChange()
{
    // ORIG 0x86c0964：found 布尔物化；dtor 在公共汇合点执行后按 found 分派。
    bool found = false;
    if (m_userCount > 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x160);
        unsigned int bestIdx = 0xffffffff;
        int bestSlot = -1;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (m_hostVotes[i] != -1) &&
                ((unsigned int)m_hostVotes[i] <= bestIdx))
            {
                bestIdx = (unsigned int)m_hostVotes[i];
                bestSlot = i;
            }
        }
        found = (bestSlot != -1);
        if (found)
        {
            packet.put_byte(bestSlot);
            packet.finalize(true);
            SendToRoom(packet);
        }
    }
    if (!found)
        return;
    for (int i = 0; i <= 5; ++i)
        m_hostVotes[i] = -1;
    m_userCount = 0;
}

void WarRoom::HandleTimerTimeBomb()
{
    m_field.m_mapInfo.m_monsterMap.clear();
    m_flag10 = 1;
    if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
        ChangeState(WARROOM_STATE_5);
    else
        ChangeState(WARROOM_STATE_4);
}

void WarRoom::HandleTimerResultCount()
{
    ReviveAll();
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_slotData[i] != 0))
        {
            AddBanList((CUser*)m_slots[i]);
            CUser* u = (CUser*)m_slots[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)m_slotData[i]);
        }
    }
    BattleReset();
    CheckFatuigue();
    if (GetWaiterCount() < 3)
        ChangeState(WARROOM_STATE_0);
    else
        ChangeState(WARROOM_STATE_1);
}

void WarRoom::SendStatePacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x58);
    packet.put_short(1);
    MakeStateInfo((char*)&packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

void WarRoom::MakeStateInfo(char* buf)
{
    InterfacePacketBuf& p = *(InterfacePacketBuf*)buf;
    p.put_short(m_index);
    p.put_byte(m_state);
    p.put_short(m_curSpawnStep);
}

void WarRoom::MakeRoomInfo(char* buf)
{
    InterfacePacketBuf* p = (InterfacePacketBuf*)buf;
    p->put_short(m_index);
    p->put_byte((unsigned char)m_field8);
    p->put_byte((unsigned char)m_levelBand);
    p->put_byte((unsigned char)m_roomSeq);
    p->put_byte(m_state);
    if (m_pDungeon == 0)
        p->put_short(0);
    else
        p->put_short(m_pDungeon->m_warRoomData.m_10);
    for (int i = 0; i <= 5; ++i)
    {
        p->put_byte(m_slotState[i]);
        if (m_slots[i] == 0)
            p->put_short(0xffff);
        else
            p->put_short(((CUser*)m_slots[i])->get_unique_id());
    }
}
void WarRoom::MakeSlotInfo(char* buf, int seat)
{
    ((InterfacePacketBuf*)buf)->put_header(0, 0x59);
    ((InterfacePacketBuf*)buf)->put_short(m_index);
    if (seat == 0xffff)
    {
        int count = GetWaiterCount();
        ((InterfacePacketBuf*)buf)->put_byte(count);
        if (count == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x793, "Send Member 0 PvpRoom %d State(%d)", m_index,
                                  m_state);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                ((InterfacePacketBuf*)buf)->put_byte(i);
                ((InterfacePacketBuf*)buf)->put_byte(m_slotState[i]);
                ((InterfacePacketBuf*)buf)->put_short(((CUser*)m_slots[i])->get_unique_id());
            }
        }
    }
    else
    {
        ((InterfacePacketBuf*)buf)->put_byte(1);
        ((InterfacePacketBuf*)buf)->put_byte(seat);
        ((InterfacePacketBuf*)buf)->put_byte(m_slotState[seat]);
        if (m_slots[seat] == 0)
            ((InterfacePacketBuf*)buf)->put_short(0xffff);
        else
            ((InterfacePacketBuf*)buf)->put_short(((CUser*)m_slots[seat])->get_unique_id());
    }
    ((InterfacePacketBuf*)buf)->finalize(true);
}

void WarRoom::SendToRoom(PacketGuard& guard)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ((CUser*)m_slots[i])->Send(guard);
    }
}

// ============================================================================
// 状态机
// ============================================================================
void WarRoom::OnLeaveState(WARROOM_STATE state)
{
    if (state == WARROOM_STATE_0)
        CurSpawnStepReset();
    if ((m_state != 0) && (m_state == 2) && (state == 3))
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int money = ((CUser*)m_slots[i])->getCurCharacMoney();
                int min = std::min(
                    G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand],
                    money);
                ((CInventory*)((CUser*)m_slots[i])->getCurCharacInvenW())
                    ->use_money(min, (eMoneySubReason)0xf, 1);
                if (((CUser*)m_slots[i])->GetFinishPointTotal() > 0)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x55b, 5)
                        ("쟁역 FP누적됨 발생", ((CUser*)m_slots[i])->GetFinishPointTotal());
                    ((CUser*)m_slots[i])->InitFinishPoint();
                }
            }
        }
    }
}

void WarRoom::OnEnterState()
{
    CancelAllTimer();
    InvalidUserCheck();
    SendStatePacket();
    switch (m_state)
    {
    case 0:
        UpdateBanList();
        if (m_field30 != 0)
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                    WalkOutUserBySlot(i);
            }
            Close();
            m_field30 = 0;
            PacketGuard packet;
            packet.put_header(0, 0x58);
            packet.put_short(1);
            MakeStateInfo((char*)&packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        break;
    case 1:
        m_fpBase = GlobalData::s_systemTime_.getCurSec();
        BattleReset();
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                m_liveFlag[i] = 0;
            else
                m_liveFlag[i] = 1;
        }
        if (GetWaiterCount() <= 2)
        {
            bool hasGM = false;
            for (int i = 0; i <= 5; ++i)
            {
                if ((m_slots[i] != 0) && (((CUser*)m_slots[i])->isGMUser() != 0))
                {
                    hasGM = true;
                    break;
                }
            }
            if (!hasGM)
            {
                ChangeState(WARROOM_STATE_0);
                return;
            }
        }
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x41);
            int ms = G_CDataManager()->m_warAreaCounter.m_readyCount;
            G_TimerQueue()->InsertTimerInMilisecond(
                (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x41, ms, key, 0);
        }
        break;
    case 2:
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x42);
            G_TimerQueue()->InsertTimerInMilisecond(
                (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x42, 11000, key, 0);
            m_curSpawnStep = 1;
        }
        break;
    case 3:
        m_flag10 = 0;
        m_flag11 = 0;
        {
            PacketGuard packet;
            packet.clear();
            int count = GetWaiterCount();
            m_field.ConsistMap(&packet, m_pDungeon, (const CMap*)m_fp,
                               (unsigned char)(((unsigned char)m_levelBand + 1) * 10),
                               count);
            SendToRoom(packet);
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    ((CUser*)m_slots[i])->FatigueUp(
                        m_pDungeon->m_warRoomData.m_ec);
                }
            }
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x45);
            int ms = m_pDungeon->m_warRoomData.m_f4;
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x45, ms / 1000, key, 0);
        }
        break;
    case 4:
        ReviveAll();
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x43);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x43, 10, key, 0);
        }
        if (m_curSpawnStep != m_pDungeon->m_warRoomData.m_50)
            m_curSpawnStep += 1;
        break;
    case 5:
        if ((m_flag10 != 0) && (m_field.m_stageIdx ==
                                m_pDungeon->m_warRoomData.m_50))
        {
            PacketGuard packet;
            packet.put_header(0, 0x156);
            packet.put_byte(GetWaiterCount());
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                    if (G_CDataManager()->m_warAreaCounter.m_wpPenalty < fp)
                    {
                        ((CUser*)m_slots[i])->DecreaseWarPoint(
                            G_CDataManager()->m_warAreaCounter.m_wpPenalty);
                    }
                    else
                    {
                        ((CUser*)m_slots[i])->DecreaseWarPoint(0);
                    }
                    packet.put_byte(i);
                    packet.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
                }
            }
            packet.finalize(true);
            SendToRoom(packet);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (((CUser*)m_slots[i])->GetFinishPointTotal() == 0))
                m_slotData[i] = 6;
        }
        ClearReward();
        SendGuildFP();
        RemoveKC();
        {
            unsigned int key = GenTimerKey((TIMER_MESSAGE)0x44);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x44, 0x18, key, 0);
        }
        break;
    default:
        break;
    }
}

void WarRoom::CheckFatuigue()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (!((CUser*)m_slots[i])->CheckFatigue())
            {
                CUser* u = (CUser*)m_slots[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)5);
            }
            else if (((CUser*)m_slots[i])->getCurCharacMoney() <
                     G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
            {
                CUser* u = (CUser*)m_slots[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)8);
            }
        }
    }
}

void WarRoom::CalcGuildFP()
{
    char local_4c[12];
    int local_40[6] = {0};
    int local_28;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    int local_14;
    int local_10;
    local_4c[6] = 0;
    local_4c[7] = 0;
    local_4c[8] = 0;
    local_4c[9] = 0;
    local_4c[10] = 0;
    local_4c[11] = 0;
    for (local_28 = 0; local_28 <= 5; ++local_28)
    {
        if (m_slots[local_28] != 0)
        {
            m_slotFp[local_28] =
                ((CUser*)m_slots[local_28])->GetFinishPointTotal();
            local_40[local_28] =
                ((CUser*)m_slots[local_28])->get_charac_guildkey();
            if (local_40[local_28] == 0)
                local_4c[local_28 + 6] = 1;
        }
    }
    for (local_24 = 0; local_24 <= 5; ++local_24)
    {
        if ((m_slots[local_24] != 0) && (local_4c[local_24 + 6] == 0))
        {
            local_20 = m_slotFp[local_24];
            local_1c = 1;
            local_4c[local_24 + 6] = 1;
            local_4c[0] = 0;
            local_4c[1] = 0;
            local_4c[2] = 0;
            local_4c[3] = 0;
            local_4c[4] = 0;
            local_4c[5] = 0;
            local_4c[local_24] = 1;
            for (local_14 = local_24 + 1; local_14 <= 5; ++local_14)
            {
                if ((m_slots[local_14] != 0) && (local_4c[local_14 + 6] == 0) &&
                    (local_40[local_24] == local_40[local_14]))
                {
                    local_20 += m_slotFp[local_14];
                    local_1c += 1;
                    local_4c[local_14 + 6] = 1;
                    local_4c[local_14] = 1;
                }
            }
            if (local_1c > 1)
            {
                local_18 = local_20 / local_1c;
                for (local_10 = 0; local_10 <= 5; ++local_10)
                {
                    if ((m_slots[local_10] != 0) && (local_4c[local_10] != 0))
                        m_slotFp[local_10] = local_20 / local_1c;
                }
            }
        }
    }
}

void WarRoom::SendGuildFP()
{
    PacketGuard packet;
    packet.put_header(0, 0x60);
    packet.put_byte(GetWaiterCount());
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            if (((CUser*)m_slots[i])->GetFinishPointTotal() <
                G_CDataManager()->m_warAreaCounter.m_kickPoint)
            {
                m_slotData[i] = 9;
            }
        }
    }
    if (GetWaiterCount() > 4)
    {
        int minFp = 0x7fffffff;
        std::vector<int> v;
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                if (fp < minFp)
                {
                    minFp = fp;
                    v.clear();
                    v.push_back(i);
                }
                else if (fp == minFp)
                {
                    v.push_back(i);
                }
            }
        }
        for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            m_slotData[*it] = 10;
        }
    }
    char local_3a[6] = {0};
    int count = 0;
    while (count < GetWaiterCount())
    {
        int bestSlot = -1;
        int bestFp = -1;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (local_3a[i] == 0) &&
                (bestFp <= ((CUser*)m_slots[i])->GetFinishPointTotal()))
            {
                bestFp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                bestSlot = i;
            }
        }
        if (bestSlot == -1)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x430,
                                  "maxPlayer (%d)", 0xffffffff);
        }
        local_3a[bestSlot] = 1;
        packet.put_byte(bestSlot);
        packet.put_byte(m_slotData[bestSlot]);
        ++count;
    }
    packet.finalize(true);
    SendToRoom(packet);
}

void WarRoom::ClearReward()
{
    int totalExp = 0;
    for (int i = 0; i <= 3; ++i)
    {
        totalExp += (int)((float)m_pDungeon->m_warRoomData.m_9c[i] *
                          m_roleKillCnt[i]);
    }
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            PacketGuard packet1;
            PacketGuard packet2;
            int sp = 0;
            int sfp = 0;
            float rate;
            memcpy(&rate, &m_pDungeon->m_warRoomData.m_ac, sizeof(rate));
            int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
            int baseExp = (int)((float)totalExp * ((float)fp * rate + 1.0f));
            float avatarRate = (((CUser*)m_slots[i])->IsEquipAvatar() != 0) ? 0.02f : 0.0f;
            float creatureRate =
                (((CUser*)m_slots[i])->getCurCharacInvenR()->GetCreatureMgrR()
                         ->IsEquippedCreature() != 0)
                    ? 0.05f
                    : 0.0f;
            int channelExp = 0;
            int serverExp = 0;
            int avatarExp = 0;
            int eventExp = 0;
            int creatureExp = 0;
            int premiumExp = 0;
            float expFactor = 0.0f;
            CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(3);
            if (((CExpDoubleEvent*)evt)->IsEventing(0))
            {
                int f = ((CExpDoubleEvent*)GlobalData::s_event_manager->GetRepeatEvent(3))
                            ->GetExpFactor();
                expFactor = (float)f / 100.0f;
            }
            void* channel = G_CDataManager()->GetChannelScript();
            // 渠道组字节：ORIG movzbl G_CEnvironment()+0x378 低字节
            //             = Server_Envir::m_serverGroup(+0x1e0)
            float bonus = ((channel_script_t*)channel)->getBonusRate(
                (unsigned char)G_CEnvironment()->get_server_group(),
                (unsigned int)G_CEnvironment()->get_channel_no(),
                (unsigned int)m_pDungeon->get_index());
            channelExp = (int)((float)baseExp * bonus);
            serverExp = (int)((float)baseExp *
                              ((0.0f + (float)CServerEvent::GetExpRate()) / 100.0f));
            if (avatarRate != 0.0f)
            {
                avatarExp = std::max((unsigned int)((float)baseExp * avatarRate), 1u);
            }
            eventExp = (int)((float)baseExp * expFactor);
            if (creatureRate != 0.0f)
            {
                creatureExp = std::max((unsigned int)((float)baseExp * creatureRate), 1u);
            }
            if (((CUser*)m_slots[i])->IsHavePremiumAdvantage() != 0)
            {
                float premiumRate =
                    (float)(((CUser*)m_slots[i])->GetPremiumInfo()->GetAdvantageExpRate()) /
                    100.0f;
                premiumExp = (int)((float)baseExp * premiumRate);
            }
            int burningBonus = 0;
            CEventBase* burning = GlobalData::s_event_manager->GetRepeatEvent(0xf);
            if (((CBurningFatigueEvent*)burning)->IsEventing(0))
            {
                int rate2 = ((CBurningFatigueEvent*)burning)->getBonusExpRate(
                    ((CUser*)m_slots[i])->getCurCharacUsedFatigue());
                if (rate2 != 0)
                    burningBonus = (rate2 * baseExp) / 100;
            }
            int total = avatarExp + baseExp + premiumExp + serverExp + channelExp +
                        burningBonus + creatureExp;
            int upperExp = ((CUser*)m_slots[i])->gainExpAsUpperMember(baseExp);
            total += upperExp;
            int guildExp = ((CUser*)m_slots[i])->gainGuildSkillExp(baseExp);
            total += guildExp;
            if (((CUser*)m_slots[i])->gain_exp_sp(total, sp, sfp,
                                                  (CUser::eExpAddReason)6, 0, 0) != 0)
            {
                packet2.clear();
                packet2.put_header(0, 2);
                packet2.put_byte(1);
                packet2.put_short(1);
                ((CUser*)m_slots[i])->make_basic_info((char*)&packet2, 1);
                packet2.finalize(true);
                ((CUser*)m_slots[i])->Send(packet2);
            }
            packet1.clear();
            packet1.put_header(0, 0x5c);
            packet1.put_byte(sp);
            packet1.put_byte(sfp);
            packet1.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
            packet1.put_int(baseExp + serverExp + burningBonus);
            packet1.put_int(avatarExp);
            packet1.put_int(premiumExp);
            packet1.put_int(channelExp);
            packet1.put_int(upperExp);
            packet1.put_int(creatureExp);
            packet1.finalize(true);
            ((CUser*)m_slots[i])->Send(packet1);
            int cur = GlobalData::s_systemTime_.getCurSec();
            ((CUser*)m_slots[i])->getHackAnalyzer()->setLastMonsterDeadTime(cur);
        }
    }
}

int WarRoom::Join(CUser* user, int& seat)
{
    int count = GetWaiterCount();
    PacketGuard packet1;
    PacketGuard packet2;
    packet1.put_header(0, 2);
    packet2.put_header(0, 0xb);
    packet1.put_byte(1);
    packet1.put_short(count);
    packet2.put_byte(count);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            ((CUser*)m_slots[i])->make_basic_info((char*)&packet1, 1);
            packet2.put_short(((CUser*)m_slots[i])->get_unique_id());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_inner_ip());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_ip());
            packet2.put_short(((CUser*)m_slots[i])->m_network.get_port());
            packet2.put_int(((CUser*)m_slots[i])->get_acc_id());
            packet2.put_byte(((CUser*)m_slots[i])->m_network.get_nat_type());
            packet2.put_int(((CUser*)m_slots[i])->m_network.get_mtu());
            if (((CUser*)m_slots[i])->getCurCharacR() == 0)
            {
                packet2.put_byte(0);
            }
            else
            {
                packet2.put_byte(((CUser*)m_slots[i])->getCurCharacR()->m_stamina);
            }
        }
    }
    packet1.finalize(true);
    packet2.finalize(true);
    int slot = -1;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
        {
            slot = i;
            m_slots[i] = (int)user;
            user->SetWarRoomIndex((short)m_index);
            seat = i;
            break;
        }
    }
    if (slot == -1)
    {
        return 4;
    }
    user->Send(packet1);
    user->Send(packet2);
    if (m_pDungeon != 0)
    {
        user->m_historyLog.EnterDungeon(m_pDungeon->GetDungeonName(),
                                        (unsigned char)m_levelBand);
    }
    m_liveFlag[slot] = 1;
    packet1.clear();
    packet1.put_header(0, 2);
    packet1.put_byte(1);
    packet1.put_short(1);
    user->make_basic_info((char*)&packet1, 1);
    packet1.finalize(true);
    SendToRoom(packet1);
    packet2.clear();
    packet2.put_header(0, 0xb);
    packet2.put_byte(1);
    packet2.put_short(user->get_unique_id());
    packet2.put_int(user->m_network.get_inner_ip());
    packet2.put_int(user->m_network.get_ip());
    packet2.put_short(user->m_network.get_port());
    packet2.put_int(user->get_acc_id());
    packet2.put_byte(user->m_network.get_nat_type());
    packet2.put_int(user->m_network.get_mtu());
    if (user->getCurCharacR() == 0)
    {
        packet2.put_byte(0);
    }
    else
    {
        packet2.put_byte(user->getCurCharacR()->m_stamina);
    }
    packet2.finalize(true);
    SendToRoom(packet2);
    packet1.clear();
    packet1.put_header(1, 0x5d);
    packet1.put_byte(1);
    packet1.put_int(user->getCurCharacMoney());
    packet1.put_short(GlobalData::s_systemTime_.getCurSec() - m_fpBase);
    packet1.finalize(true);
    user->Send(packet1);
    return 0;
}

void WarRoom::SetCharacterLive(int seat, CUser* user, bool live, bool a, short b, int c)
{
    this->m_liveFlag[seat] = live;
    PacketGuard packet;
    packet.put_header(0, 0x5f);
    packet.put_byte(GetUserSlot(user));
    packet.put_byte(live);
    packet.put_byte(a);
    if (((c == 0xff) || (c < 0)) || (c > 5) || (m_slots[c] == 0))
    {
        packet.put_short(0);
    }
    else if (m_flag12 == 0)
    {
        packet.put_short(m_pDungeon->m_warRoomData.m_d4);
    }
    else
    {
        packet.put_short(0);
    }
    packet.put_short(b);
    packet.finalize(true);
    SendToRoom(packet);
}

int WarRoom::SetSlotState(CUser* user, int seat, ENUM_SEAT_STATE state, CUser** out,
                          bool& a, bool& b, bool& c)
{
    if (user == 0)
        return 0;
    if (state != (ENUM_SEAT_STATE)0xff)
        return 0;
    if (m_slots[seat] == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x590, 5);
        trace("WARAREA  WarRoom::SetSlotState players_[pos] is NULL pos(%d)", seat);
    }
    else
    {
        *out = (CUser*)m_slots[seat];
        m_slotState[seat] = 0xff;
        m_liveFlag[seat] = 0;
        a = true;
        if (GetState() > 2)
        {
            AddBanList(user);
        }
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        if (m_pDungeon != 0)
        {
            user->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                            (unsigned char)m_levelBand);
        }
    }
    return 0;
}

void WarRoom::HandleSetSeatState(CUser* user, int seat, ENUM_SEAT_STATE state)
{
    PacketGuard packet;
    int ret = 0;
    CUser* outUser = 0;
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    ret = SetSlotState(user, seat, state, &outUser, b1, b2, b3);
    if (ret == 0)
    {
        if (outUser != 0)
        {
            G_GameWorld()->out_from_warroom(outUser);
        }
        if (b1 != 0)
        {
            packet.clear();
            MakeSlotInfo((char*)&packet, seat);
            G_GameWorld()->send_all(packet);
        }
        if (b3 != 0)
            return;
    }
    if (ret > 0)
    {
        packet.put_header(1, 0x5e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
    }
}

int WarRoom::HandleDieCharacer(CUser* user, int killerSlot)
{
    int st = GetState();
    if ((st != 3) && (st != 4) && (m_state != 5))
        return 0x13;
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    if (m_liveFlag[slot] == 0)
        return 0x12;
    if ((killerSlot != 0xff) && ((killerSlot < 0) || (killerSlot > 5)))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6d0, 5);
        trace("WarRoom::HandleDieCharacer killerSlotIndex Error(%d)", killerSlot);
        return 1;
    }
    short killerUid = -1;
    if (killerSlot != 0xff)
    {
        if (m_slots[killerSlot] == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x6de, 5);
            trace("WARAREA WarRoom::HandleDieCharacer player_[] is NULL killerSlotIndex(%d)",
                  killerSlot);
            killerUid = -1;
        }
        else
        {
            killerUid = ((CUser*)m_slots[killerSlot])->get_unique_id();
        }
    }
    SetCharacterLive(slot, user, false, false, killerUid, killerSlot);
    if ((killerSlot != 0xff) && (killerSlot >= 0) && (killerSlot <= 5) &&
        (m_slots[killerSlot] != 0))
    {
        if (m_flag12 == 0)
        {
            ((CUser*)m_slots[killerSlot])->WarAreaKill(
                4, m_pDungeon->m_warRoomData.m_d4, 1.0f);
        }
        else
        {
            ((CUser*)m_slots[killerSlot])->WarAreaKill(4, 0, 1.0f);
        }
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x5f);
        packet.put_byte(1);
        packet.put_byte(killerSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    if (m_flag10 == 0)
    {
        if (GetLiveCount() == 0)
            SendWarRoomFailMsg();
    }
    else
    {
        if ((m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50) ||
            (m_flag11 != 0))
        {
            return 0;
        }
        PacketGuard packet;
        packet.put_header(0, 0x156);
        packet.put_byte(GetWaiterCount());
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                int fp = ((CUser*)m_slots[i])->GetFinishPointTotal();
                if (G_CDataManager()->m_warAreaCounter.m_wpPenalty < fp)
                {
                    ((CUser*)m_slots[i])->DecreaseWarPoint(
                        G_CDataManager()->m_warAreaCounter.m_wpPenalty);
                }
                else
                {
                    ((CUser*)m_slots[i])->DecreaseWarPoint(0);
                }
                packet.put_byte(i);
                packet.put_int(((CUser*)m_slots[i])->GetFinishPointTotal());
            }
        }
        packet.finalize(true);
        SendToRoom(packet);
        m_flag11 = 1;
    }
    return 0;
}

int WarRoom::HandleMonsterKill(CUser* user, int monsterIdx, CUser* killer)
{
    map_monster monster;
    PacketGuard packet;
    int result = 0;
    int count = GetWaiterCount();
    if (count != 0)
    {
        int wp = 0;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] != 0) && (killer != 0) && ((CUser*)m_slots[i] == killer))
            {
                wp = m_field.HandleWpPerMonster(killer, monsterIdx, 0, monster);
            }
        }
        if (m_field.HandleMonsterKill(monsterIdx, packet, monster, killer,
                                      m_pDungeon) != 1)
        {
            result = 0;
        }
        else
        {
            m_roleKillCnt[monster.m_roleType] += monster.m_field28;
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    bool isKiller =
                        (killer != 0) && ((CUser*)m_slots[i] == killer);
                    if (isKiller)
                    {
                        killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                        killer->CheckQuestMonster(m_pDungeon->get_index(),
                                                  monster.m_mobIndex,
                                                  (ENUM_QUEST_ENEMY_TYPE)1);
                    }
                    int exp = (int)((float)m_pDungeon->m_warRoomData
                                        .m_9c[monster.m_roleType] *
                                    monster.m_field18);
                    if (isKiller)
                    {
                        exp = (int)(G_CDataManager()->m_warAreaCounter
                                        .m_monsterExpConst *
                                    (float)exp);
                    }
                    ExpBonusPointResult bonus;
                    if (isKiller)
                    {
                        killer->checkBonusPoint();
                        killer->getExpBonusPoint(bonus);
                    }
                    int total = bonus.m_base + exp;
                    exp = (int)((float)bonus.m_add / 100.0f * (float)total) + total;
                    int upperExp = ((CUser*)m_slots[i])->gainExpAsUpperMember(exp);
                    int guildExp = ((CUser*)m_slots[i])->gainGuildSkillExp(exp);
                    int fatigueExp = 0;
                    if (((CUser*)m_slots[i])->getCurCharacFatigueGrownUpBuff() != 0)
                    {
                        fatigueExp = exp;
                        ((CUser*)m_slots[i])->set_charac_fatigue_buf_bonus_exp(exp);
                    }
                    int rewardExp =
                        ((CUser*)m_slots[i])->gainPowerWarRewardExp(exp);
                    int sp = 0;
                    int sfp = 0;
                    if (((CUser*)m_slots[i])->gain_exp_sp(
                            upperExp + exp + guildExp + rewardExp + fatigueExp,
                            sp, sfp, (CUser::eExpAddReason)4, monster.m_mobIndex,
                            0) != 0)
                    {
                        ((CUser*)m_slots[i])->SendNotiPacket(
                            (CUser::eSendTarget)3, (ENUM_NOTIPACKET)2, 1);
                    }
                }
            }
            short killerUid = -1;
            if (killer != 0)
                killerUid = killer->get_unique_id();
            packet.put_byte(1);
            packet.put_short(killerUid);
            packet.put_short(wp);
            packet.finalize(true);
            SendToRoom(packet);
            if (m_field.IsGridClear() != 0)
            {
                for (int i = 0; i <= 5; ++i)
                {
                    if (m_slots[i] != 0)
                    {
                        CUser* u = (CUser*)m_slots[i];
                        if (u->m_fatigueGain > 0)
                        {
                            if (u->getCurCharacInvenR()->GetCreatureMgrR()
                                    ->IsGrowCreature_Equipped_Creature() != 1)
                            {
                                u->getCurCharacInvenW()->GetCreatureMgrW()->GainExp(
                                    u->m_fatigueGain);
                            }
                        }
                        u->m_fatigueGain = 0;
                    }
                }
                GenTimerKey((TIMER_MESSAGE)0x45);
                if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
                {
                    m_flag12 = 1;
                    ChangeState(WARROOM_STATE_5);
                }
                else
                {
                    ChangeState(WARROOM_STATE_4);
                }
            }
            result = 1;
        }
    }
    return result;
}

int WarRoom::HandleWpPerMonster(CUser* user, int a, int b, CUser* killer)
{
    map_monster monster;
    int count = GetWaiterCount();
    if (count == 0)
        return 3;
    int wp = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            bool isTarget = (killer != 0) && ((CUser*)m_slots[i] == killer);
            if (isTarget)
            {
                wp = m_field.HandleWpPerMonster(killer, a, b, monster);
                killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
            }
        }
    }
    short killerUid = -1;
    if (killer != 0)
        killerUid = killer->get_unique_id();
    PacketGuard packet;
    packet.put_header(0, 0x151);
    packet.put_int(a);
    packet.put_short(killerUid);
    packet.put_int(b);
    packet.put_short(wp);
    packet.finalize(true);
    SendToRoom(packet);
    return 0;
}

int WarRoom::HandleGetItem(CUser* user, int idx)
{
    char flag = 1;
    CUser* target = 0;
    map_item fieldItem;
    PacketGuard packet;
    int result = 0;
    bool ok = m_field.GetFieldItem(idx, fieldItem) != 0;
    if (ok != 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        result = 0;
    }
    else if (GetWaiterCount() < 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        result = 0;
    }
    else if (fieldItem.m_item.m_addInfo == 0)
    {
        int money = fieldItem.m_item.get_add_info();
        packet.put_header(0, 0x5d);
        packet.put_short(idx);
        packet.put_short(user->get_unique_id());
        char iter[4] = {0};
        int check = m_field.CheckPickupItem(
            idx, user->get_unique_id(),
            *reinterpret_cast<std::map<int, map_item>::iterator*>(iter));
        if (check < 1)
        {
            int gained = user->getCurCharacInvenW()->gain_money(
                money, (eMoneyAddReason)0xf, 1, 0);
            user->saveTaxMoneyForUpperMember(money);
            if (gained < money)
            {
                if (gained < 0)
                    gained = 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, money, gained);
            }
            m_field.PickupItem(*reinterpret_cast<std::map<int, map_item>::iterator*>(iter));
            packet.put_int(gained);
            packet.finalize(true);
            SendToRoom(packet);
            result = 1;
        }
        else
        {
            packet.put_header(1, 0x2e);
            packet.put_byte(0);
            packet.put_byte(check);
            packet.finalize(true);
            user->Send(packet);
            result = 0;
        }
    }
    else
    {
        int itemIdx = fieldItem.m_item.m_addInfo;
        CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x957, "G_CDataManager()->find_item(%d) fail", itemIdx);
            result = 0;
        }
        else
        {
            target = user;
            if (user == 0)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(0x16);
                packet.finalize(true);
                SendToRoom(packet);
                result = 0;
            }
            else
            {
                bool randomOption = false;
                if (item->IsRandomOption() != 0)
                {
                    if (user->getCheckPickUpRandomOptionItem() == 1)
                        randomOption = false;
                    else
                        randomOption = true;
                }
                if (randomOption)
                {
                    user->setCheckPickUpRandomOptionItem(true);
                    DB_InsertRandomOptionItemInform::makeRequest(
                        user->GetUID(), user->get_acc_id(), 1);
                    PacketGuard notice;
                    notice.clear();
                    notice.put_header(0, 0x171);
                    notice.put_short(user->get_unique_id());
                    notice.put_byte(1);
                    notice.finalize(true);
                    user->Send(notice);
                }
                char iter[4] = {0};
                int check = m_field.CheckPickupItem(
                    idx, user->get_unique_id(),
                    *reinterpret_cast<std::map<int, map_item>::iterator*>(iter));
                if (check < 1)
                {
                    int added = -1;
                    if (fieldItem.m_item.isAvatarItemType() == 0)
                    {
                        added = user->getCurCharacInvenW()->insertItemIntoInventory(
                            fieldItem.m_item, (eItemAddReason)0x16, 1, 1);
                    }
                    else
                    {
                        char ipg[24];
                        WongWork::CGenUniqueNo::genIPGNo(
                            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                            user->getCurCharacNo(), ipg);
                        added = user->addAvatarItem(
                            (unsigned long)fieldItem.m_item.m_addInfo, 0,
                            (char)fieldItem.m_item.m_fieldb, ipg,
                            (eAvatarItemAddReason)0x11);
                    }
                    if (added == -1)
                    {
                        packet.put_header(1, 0x2e);
                        packet.put_byte(0);
                        packet.put_byte(4);
                        packet.finalize(true);
                        user->Send(packet);
                        result = 0;
                    }
                    else
                    {
                        m_field.PickupItem(
                            *reinterpret_cast<std::map<int, map_item>::iterator*>(iter));
                        packet.put_header(0, 0x5d);
                        packet.put_short(idx);
                        packet.put_short(user->get_unique_id());
                        packet.put_short(added);
                        packet.finalize(true);
                        SendToRoom(packet);
                        result = 1;
                    }
                }
                else
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x982, 5);
                    trace("fieldData_.PickupItem( %d/%d ) ", idx, itemIdx);
                    packet.put_header(1, 0x2e);
                    packet.put_byte(0);
                    packet.put_byte(check);
                    packet.finalize(true);
                    user->Send(packet);
                    result = 0;
                }
            }
        }
    }
    return result;
}

int WarRoom::HandleItemDrop(CUser* user, int a, int b, char c, int d, int e)
{
    char dropType = c;
    int st = GetState();
    if ((st != 3) && (st != 4))
        return 0x13;
    if (e < 0)
    {
        user->getCurCharacName();
        unsigned int accId = user->get_acc_id();
        NumberToString(accId, 0);
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__,
                              0xa1a, "user - accid[%s] %s : count<0");
        return 0x11;
    }
    if (user->CheckInTrade() != 0)
        return 0x13;
    map_item field;
    Inven_Item item;
    if (dropType == 2)
    {
        Inven_Item slotItem = user->getCurCharacCargoR()->get_cargo_slot(d);
        item = slotItem;
    }
    else if (dropType == 3)
    {
        Inven_Item slotItem;
        slotItem = user->getCurCharacInvenR()->GetInvenSlot(0, d);
        item = slotItem;
    }
    else if (dropType == 0)
    {
        if (d == 0)
        {
            field.m_count = 0;
            field.m_item.m_field1 = 2;
            field.m_item.m_addInfo = 0;
            field.m_item.set_add_info(e);
            field.m_item.m_fieldb = 0;
            field.m_item.ResetItemAttr();
        }
        else
        {
            Inven_Item slotItem;
            slotItem = user->getCurCharacInvenR()->GetInvenSlot(1, d);
            item = slotItem;
        }
    }
    int price = 0;
    int itemIdx = item.m_addInfo;
    if (itemIdx != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            LogManager::logFormat(
                1, "WarRoom.cpp",
                __PRETTY_FUNCTION__, 0xa4c,
                "[WarRoom::HandleDropItem] Null Item %d");
            return 0xffffffff;
        }
        int attach = pItem->GetAttachType();
        switch (attach)
        {
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (item.m_field0 != 1)
                return 0x17;
            break;
        default:
            break;
        }
        if ((item.isEquipableItemType() != 0) &&
            ((((CEquipItem*)pItem)->m_itemType == 0xb) ||
             (((CEquipItem*)pItem)->m_itemType < 10)))
        {
            return 0x17;
        }
        field.m_count = 0;
        field.m_item.m_field1 = item.m_field1;
        field.m_item.m_addInfo = itemIdx;
        if (item.isEquipableItemType() != 0)
        {
            field.m_item.set_add_info(item.m_addInfo2);
        }
        else
        {
            field.m_item.set_add_info(e);
        }
        field.m_item.SetItemAttr(item.GetItemAttr());
        field.m_item.m_fieldb = item.m_fieldb;
        field.m_item.m_field0 = item.m_field0;
        field.m_item.m_fieldd = item.m_fieldd;
        field.m_item.m_amp = item.m_amp;
        field.m_item.m_random.m_random.m_field0 =
            item.m_random.m_random.m_field0;
        field.m_item.m_random.m_random.m_field3 =
            item.m_random.m_random.m_field3;
        field.m_item.m_random.m_random.m_field6 =
            item.m_random.m_random.m_field6;
        field.m_item.m_random.m_random.m_seed9 =
            item.m_random.m_random.m_seed9;
        field.m_item.m_random.m_random.m_fielda.m_field2 =
            item.m_random.m_random.m_fielda.m_field2;
        field.m_item.m_random.m_random.m_seedd.m_field =
            item.m_random.m_random.m_seedd.m_field;
        field.m_item.m_upgradeSep.SetUpgradeSeparate(
            item.m_upgradeSep.GetUpgradeSeparate());
        int price2;
        G_Store()->GetSellItemPrice(item, pItem, (short)e, false, price2);
        price = price2;
    }
    if (dropType == 2)
    {
        if (user->getCurCharacCargoW()->delete_item(d, e, (eItemDelReason)0x10) != 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xabf,
                5);
            trace("CParty::drop_item , delete_item failed 2, User ch=%d , %d %d",
                  user->getCurCharacNo(), d, b);
            return 0x11;
        }
    }
    else if (dropType < 3)
    {
        if (dropType == 0)
        {
            if (d == 0)
            {
                if (((CInventory*)user->getCurCharacInvenW())
                        ->use_money(e, (eMoneySubReason)6, 1) != 1)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0xa9f, 5);
                    trace("CParty::drop_item , use_money failed, User ch=%d , %d %d",
                          user->getCurCharacNo(), d, e);
                    return 0x11;
                }
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x17, user, (unsigned)e);
            }
            else
            {
                if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, d, e, (eItemDelReason)6, 1) != 1)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0xaae, 5);
                    trace("CParty::drop_item , delete_item failed, User ch=%d , %d %d",
                          user->getCurCharacNo(), d, b);
                    return 0x11;
                }
                GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                              user, (unsigned)price);
            }
        }
    }
    else if (dropType == 3)
    {
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)0, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xaca,
                5);
            trace("CParty::drop_item , delete_item failed 3, User ch=%d , %d",
                  user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
    }
    else if (dropType == 7)
    {
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)3, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xaed,
                5);
            trace("CParty::drop_item , delete_item failed 4, User ch=%d , %d",
                  user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
    }
    if (item.m_field1 != 1)
    {
        if ((field.m_item.m_addInfo != 0) && (field.m_item.get_add_info() > 0x7d0))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xb00,
                4);
            trace("war room drop item(%d) stack size over %d add_info(%d)",
                  field.m_item.m_addInfo, 0x7d0, field.m_item.get_add_info());
        }
    }
    int dropIdx = m_field.DropItem(field);
    if (dropIdx == -1)
        return 0x16;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id());
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(dropIdx);
    packet.put_int(field.m_item.m_addInfo);
    packet.put_byte(field.m_item.GetItemAttr());
    packet.put_int(field.m_item.get_add_info());
    packet.put_short(field.m_item.m_fieldb);
    packet.put_int(field.m_item.m_fieldd);
    packet.put_byte(field.m_item.m_amp.getAbilityType());
    packet.put_short(field.m_item.m_amp.getAbilityValue());
    packet.put_packet(field.m_item);
    packet.finalize(true);
    SendToRoom(packet);
    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)dropType);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

int WarRoom::ReviveUserByCoin(CUser* user, unsigned short uid)
{
    bool invalid = (GetState() == 3) || (GetState() == 4);
    if (invalid)
        return 0x13;
    CUser* target = 0;
    int slot;
    for (slot = 0; slot <= 5; ++slot)
    {
        if (m_slots[slot] != 0)
        {
            if (((CUser*)m_slots[slot])->get_unique_id() == uid)
            {
                if (m_liveFlag[slot] != 0)
                    return 0x12;
                target = (CUser*)m_slots[slot];
                break;
            }
        }
    }
    if (user->GetUsedCoinCount() >= m_pDungeon->m_87c)
        return 0x16;
    HistoryLog::WriteUseCoin((_IO_FILE*)user->m_field796f8,
                             target->getCurCharacName());
    SetCharacterLive(slot, target, true, true, 0, 0xff);
    return 0;
}
// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
//   docs/class_func_reports/_lt_global_gt_/EnterWarRoom.md
//   docs/class_func_reports/_lt_global_gt_/ImportWarRoomWPTable.md
//   docs/class_func_reports/DisPatcher_MakeWarRoomTemp/dispatch_sig.md）
// ============================================================================

// ---- stBloodWarGrowTable（ImportWarRoomWPTable 形参类型，mangled P19stBloodWarGrowTable；
//      派生自 vector<vector<pair<int,int>>> —— GCC 4.4 对 typedef 到模板实例会展开
//      typedef 名，唯派生结构体才能产生 ORIG 的 P19stBloodWarGrowTable）----
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};

// ============================================================================
// EnterWarRoom —— ORIG 0x08201e7a（size 0x424，T）
// 报告 _lt_global_gt_/EnterWarRoom.md 全文转写：战场房间入场入口。
// 顺序：state!=3 → 错误包(0x13)；getCurCharacVill==7 → SendCmdErrorPacket(0x13)；
// 私商忙 → 0x3c；Disjoint 店忙 → 0xbf；幽灵 → 0x13；交易中 → 0x13；
// IsJoinable 非 0 → 错误包(码)；CheckInParty → send_invite_warroom + CheckOutParty；
// Join>0 → 错误包(码)；cmd==0x65 → 广播本人 basic 包；MakeSlotInfo + send_all +
// goto_warroom；人数>2 且 state==0 → Start；返回 1/0。
// ============================================================================
int EnterWarRoom(CUser* user, WarRoom* room, ENUM_CMDPACKET cmd)
{
    int seat;                 // -0x20（ORIG 声明序：seat, packet, ret, party）
    PacketGuard packet;       // -0x1c
    int ret;                  // -0x10（IsJoinable 与 Join 复用同一槽，ORIG 实证）
    CParty* party;            // -0xc
    if (user->get_state() != 3)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->getCurCharacVill() == 7)
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        user->SendCmdErrorPacket(cmd, 0x3c);
        return 0;
    }
    if (user->IsBusyDisjointStore())
    {
        user->SendCmdErrorPacket(cmd, 0xbf);
        return 0;
    }
    if (user->IsCurCharacGhost())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    ret = room->IsJoinable(user);
    if (ret != 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->CheckInParty())
    {
        party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_warroom((unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    seat = 0;
    ret = room->Join(user, seat);
    if (ret > 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (cmd == 0x65)
    {
        packet.put_header(1, 0x65);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    room->MakeSlotInfo((char*)&packet, seat);
    G_GameWorld()->send_all(packet);
    G_GameWorld()->goto_warroom(user);
    if (room->GetWaiterCount() > 2)
    {
        if (room->GetState() == 0)
        {
            room->Start();
        }
    }
    return 1;
}

// ============================================================================
// ImportWarRoomWPTable —— ORIG 0x08902543（size 0x1af，T）
// 报告 _lt_global_gt_/ImportWarRoomWPTable.md 全文转写：载入 Etc/WarRoomWPTable.tbl。
// 失败 → logFormat(0, __FUNCTION__) 返回 0；"[WP TABLE]" 段内每行解析 5 组
// (key,val) pair，ok 时 push 到 table，继续下一行（do-while(ok)），失败回外层
// ScanType；返回 1。韩文日志与 __FUNCTION__ 字符串按 ORIG 原样（CP949 源字节）。
// ============================================================================
int ImportWarRoomWPTable(stBloodWarGrowTable* table)
{
    if (!loadRDARScriptFile("", "Etc/WarRoomWPTable.tbl"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0xdb8,
                              "CRITICAL ERROR : 쟁역 카운터 스크립트 로드 실패!");
        return 0;
    }
    std::string token;
    bool ok = false;
    bool again;
    while (ScanType(token, true))
    {
        if (token == "[WP TABLE]")
        {
            do
            {
                std::vector<std::pair<int, int> > row;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    row.push_back(p);
                }
                if (ok)
                {
                    if (table != 0)
                        table->push_back(row);
                    again = true;
                }
                else
                {
                    again = false;
                }
            } while (again);
        }
    }
    return 1;
}

// ============================================================================
// DisPatcher_MakeWarRoomTemp 族（ORIG vtable @0x8bd8cc8，8 槽）
//   dispatch_sig 0x08201b32（报告 dispatch_sig.md 全文转写）
//   read 0x082335f8 / process 0x08233602（objdump 转写：恒定返回 0）
//   D1/D2 0x082431aa / D0 0x082431da（objdump 转写：vptr + 基类析构 + delete 守卫）
// ============================================================================
int DisPatcher_MakeWarRoomTemp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    if (user->get_state() != 3)
        return 0;
    PacketGuard pg;
    WarRoom* room = G_CGameManager()->GetWarRoom();
    int createRet;
    CParty* party;
    pg.put_header(1, 0x5c);
    if (room == 0)
    {
        pg.put_byte(0);
        pg.put_byte(4);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->CheckInParty())
    {
        party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_warroom((unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    createRet = room->Create(user, (char*)"임시", 0x1770);
    if (createRet > 0)
    {
        pg.put_byte(0);
        pg.put_byte(createRet);
        pg.finalize(true);
        user->Send(pg);
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__,
                              0x976d, "create_room fail: 쟁역 방 생성 실패 ID[%d]\n",
                              room->GetIndex());
        G_CGameManager()->PutWarRoom(room);
        return 0;
    }
    pg.clear();
    pg.put_header(0, 2);
    pg.put_byte(1);
    pg.put_short(1);
    user->make_basic_info((char*)&pg, 1);
    pg.finalize(true);
    user->Send(pg);
    pg.clear();
    pg.put_header(0, 0x57);
    pg.put_short(1);
    room->MakeRoomInfo((char*)&pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
    G_GameWorld()->goto_warroom(user);
    return 0;
}

int DisPatcher_MakeWarRoomTemp::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x082335f8：恒定返回 0
}

int DisPatcher_MakeWarRoomTemp::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                        ParamBase& /*param*/)
{
    return 0;    // ORIG 0x08233602：恒定返回 0
}

DisPatcher_MakeWarRoomTemp::~DisPatcher_MakeWarRoomTemp()
{
}

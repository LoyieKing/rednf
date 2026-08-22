// df_game_r 角色任务列表实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMissionList_Charac.md（Ghidra C）+ ORIG
// 反汇编（ctor 0x832af8a / dtor 0x85ec626 / 弱方法 0x85e68b6..0x85e7130）。
#include <bitset>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#include "CMissionList_Charac.h"
#include "CMission.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"

// PvP_Room 最小声明（PvP_Room.h 引入 PvP_deps.h 的重复 PVP_BATTLE_MODE，
// 本 TU 只需 get_pvp_battle_mode）
class PvP_Room
{
public:
    int get_pvp_battle_mode();
};

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号，形态按 mangled 名转录）
// ============================================================================

extern "C" int sub_CDataManager_GetPvPChannelType(void* self)
    asm("_ZN12CDataManager17GetPvPChannelTypeEv");
extern "C" void* sub_CUser_GetPVPRoom(void* self)
    asm("_ZN5CUser10GetPVPRoomEv");
extern "C" void sub_cUserHistoryLog_pvpMissionAdd(void* self, int kind,
                                                  int index, int count,
                                                  int streak)
    asm("_ZN15cUserHistoryLog13pvpMissionAddEiiii");
extern "C" void sub_cUserHistoryLog_pvpMissionDel(void* self, int kind,
                                                  int index)
    asm("_ZN15cUserHistoryLog13pvpMissionDelEii");

// 全局辅助（数据域 TU）：bitset <-> 存档字节串、每日排程判定
// ORIG 0x8a5a989：逐字节/位转换
void StrToBitset(unsigned char* data, std::bitset<256>& bitset)
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 8; ++j)
            bitset[i * 8 + j] = (data[i] >> j) & 1;
}
// ORIG 0x8a5a90a：bitset → 32 字节
void BitsetToStr(const std::bitset<256>& bitset, unsigned char* out)
{
    for (int i = 0; i < 32; ++i)
    {
        unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            if (bitset[i * 8 + j])
                byte |= (1 << j);
        out[i] = byte;
    }
}
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；与 CInventory.h
// 的占位版本不同，本 TU 使用真实形态）
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// PvP 任务存档结构（SIG_REQUEST_PVP_MISSION_LIST，0x14a 字节）
#pragma pack(push, 1)
struct SIG_REQUEST_PVP_MISSION_LIST
{
    int m_characNo;                      // +0x00
    MissionInfo m_missionList[36];       // +0x04（8*36 = 0x120）
    unsigned char m_cleared[32];         // +0x124
    short m_field144;                    // +0x144
    short m_field146;                    // +0x146
    short m_field148;                    // +0x148
    char m_pad14a;                       // +0x14a（memset 0x14a）
};
#pragma pack(pop)

// CEnvironment +0x37c 的每日任务排程时间（ORIG 布局）
static int GetDailyScheduleTime(CEnvironment* env)
{
    return *(int*)((char*)env + 0x37c);
}

// 任务槽清空（等价 ORIG MissionInfo::clear，避免跨 TU 依赖）
static void ClearMissionInfoSlot(MissionInfo& info)
{
    std::memset(&info, 0, sizeof(info));
}

// ============================================================================
// 构造 / 析构 / 虚接口
// ============================================================================

CMissionList_Charac::CMissionList_Charac()
{
    for (int i = 0; i < 36; ++i)
    {
        m_missionList[i].m_kind = 0;
        m_missionList[i].m_index = 0;
        m_missionList[i].m_count = 0;
        m_missionList[i].m_streak = 0;
    }
    m_clearedFlags.reset();
}

CMissionList_Charac::~CMissionList_Charac()
{
}

bool CMissionList_Charac::loadData(CUser* pUser, char* pData)
{
    _reset();
    m_user = pUser;
    char* p = pData;

    for (int kind = 0; kind < 36; ++kind)
    {
        m_missionList[kind].m_index = *(short*)(pData + kind * 8 + 6);
        m_missionList[kind].m_kind = *(short*)(pData + kind * 8 + 4);
        m_missionList[kind].m_count = *(short*)(pData + kind * 8 + 8);
        m_missionList[kind].m_streak = *(short*)(pData + kind * 8 + 10);
    }
    StrToBitset((unsigned char*)(pData + 0x124), m_clearedFlags);
    m_field148 = *(short*)(p + 0x144);
    m_field14a = *(short*)(p + 0x146);
    m_field14c = *(short*)(p + 0x148);

    CDataManager* mgr = G_CDataManager();
    std::list<CMission*>* list =
        (std::list<CMission*>*)(void*)mgr->get_kind_mission_list(6);
    for (std::list<CMission*>::const_iterator it = list->begin();
         it != list->end(); ++it)
    {
        if ((*it) && isClearMission((*it)->GetMissionIndex()))
        {
            m_field14e = 1;
            break;
        }
    }

    if (isUserNewbie())
    {
        MakeMissionList_forNewbies(*pUser);
        Reset_DailyMission();
    }
    else
    {
        MakeMissionList_forOldUser(*pUser);
    }

    if (CheckDailyScheduleTime(GetDailyScheduleTime(G_CEnvironment()),
                               pUser->getCurCharacLastPlayTick(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        Reset_DailyMission();
    }
    return 1;
}

void CMissionList_Charac::getData(char* pData) const
{
    std::memset(pData, 0, 0x14a);
    std::memcpy(pData, &m_pad14f[1], 4);
    for (int kind = 0; kind < 36; ++kind)
    {
        *(short*)(pData + kind * 8 + 6) = m_missionList[kind].m_index;
        *(short*)(pData + kind * 8 + 4) = m_missionList[kind].m_kind;
        *(short*)(pData + kind * 8 + 8) = m_missionList[kind].m_count;
        *(short*)(pData + kind * 8 + 10) = m_missionList[kind].m_streak;
    }
    BitsetToStr(m_clearedFlags, (unsigned char*)(pData + 0x124));
    *(short*)(pData + 0x144) = m_field148;
    *(short*)(pData + 0x146) = m_field14a;
    *(short*)(pData + 0x148) = m_field14c;
}

bool CMissionList_Charac::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvP_MissionSystem.cpp", 0x4a5),
        true);
    **guard << 0x263;
    **guard << (int)pUser->GetUID();
    SIG_REQUEST_PVP_MISSION_LIST* pData =
        guard->GetInBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    if (!pData)
    {
        return 0;
    }
    std::memset(pData, 0, 0x14a);
    pData->m_characNo = pUser->get_charac_no(-1);
    for (int kind = 0; kind < 36; ++kind)
    {
        pData->m_missionList[kind].m_index =
            m_missionList[kind].m_index;
        pData->m_missionList[kind].m_kind =
            m_missionList[kind].m_kind;
        pData->m_missionList[kind].m_count =
            m_missionList[kind].m_count;
        pData->m_missionList[kind].m_streak =
            m_missionList[kind].m_streak;
    }
    BitsetToStr(m_clearedFlags, pData->m_cleared);
    pData->m_field144 = m_field148;
    pData->m_field146 = m_field14a;
    pData->m_field148 = m_field14c;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

void CMissionList_Charac::_reset()
{
    std::memset(m_missionList, 0, sizeof(m_missionList));
    m_clearedFlags.reset();
    m_field148 = 0;
    m_field14a = 0;
    m_field14c = 0;
    m_field14e = 0;
    m_user = 0;
}

// ============================================================================
// 事件入口
// ============================================================================

void CMissionList_Charac::Update_EnterWorld_event(CUser& user)
{
    int type = G_GameWorld()->GetChannelType();
    if ((type == 0xf) || (type == 0x10))
    {
        alter();
        std::vector<int> out;
        MissionClearCondition_Parameter param(PVP_BATTLE_MODE_2);
        check_mission_kind(7, user, param, out);
        check_mission_kind(0x1e, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
}

void CMissionList_Charac::Update_Kill_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        alter();
        std::vector<int> out;
        check_mission_kind(5, user, param, out);
        check_mission_kind(0xf, user, param, out);
        check_mission_kind(0x19, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
}

void CMissionList_Charac::Update_Win_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        Update_Within_Mission_event(user, param, outClear);
        alter();
        if ((outClear == false) && (getIndex_byKind(0x1b) == 0))
        {
            std::vector<int> out;
            check_mission_kind(2, user, param, out);
            check_mission_kind(4, user, param, out);
            check_mission_kind(9, user, param, out);
            check_mission_kind(10, user, param, out);
            check_mission_kind(0xc, user, param, out);
            check_mission_kind(0xe, user, param, out);
            check_mission_kind(0x11, user, param, out);
            check_mission_kind(0x13, user, param, out);
            check_mission_kind(0x14, user, param, out);
            check_mission_kind(0x16, user, param, out);
            check_mission_kind(0x18, user, param, out);
            check_mission_kind(0x1a, user, param, out);
            Send_Cleared_Mission_List(user, out);
        }
    }
}

void CMissionList_Charac::Update_Revenge_event(CUser& user,
                                               PVP_BATTLE_MODE mode,
                                               bool win)
{
    if ((G_GameWorld()->GetChannelType() == 0xf) &&
        (getIndex_byKind(0x1b) == 0))
    {
        if (win == false)
        {
            m_missionList[0xc].m_streak = 0;
            m_missionList[0x16].m_streak = 0;
        }
        else
        {
            alter();
            std::vector<int> out;
            MissionClearCondition_Parameter param(mode);
            check_mission_kind(0xc, user, param, out);
            check_mission_kind(0x16, user, param, out);
            Send_Cleared_Mission_List(user, out);
        }
    }
}

void CMissionList_Charac::Update_GameOver_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    if ((G_GameWorld()->GetChannelType() == 0xf) &&
        (getIndex_byKind(0x1b) == 0))
    {
        alter();
        std::vector<int> out;
        check_mission_kind(1, user, param, out);
        check_mission_kind(3, user, param, out);
        check_mission_kind(8, user, param, out);
        check_mission_kind(0xd, user, param, out);
        check_mission_kind(0x10, user, param, out);
        check_mission_kind(0x12, user, param, out);
        check_mission_kind(0x17, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
}

void CMissionList_Charac::Update_RecvPacket_event(
    CUser& user, std::vector<short>& kinds, std::vector<int>& values)
{
    if ((G_GameWorld()->GetChannelType() == 0xf) &&
        (getIndex_byKind(0x1b) == 0))
    {
        alter();
        PvP_Room* room = (PvP_Room*)sub_CUser_GetPVPRoom(&user);
        if (room)
        {
            MissionClearCondition_Parameter param(
                (PVP_BATTLE_MODE)room->get_pvp_battle_mode());
            for (unsigned int i = 0; i < kinds.size(); ++i)
            {
                CMission* mission =
                    (CMission*)G_CDataManager()->find_mission(kinds[i]);
                if (mission)
                {
                    int kind = mission->GetMissionKind();
                    if ((kind != 6) && (kind != 0x15))
                    {
                        check_mission_kind(kind, user, param, values);
                    }
                }
            }
        }
    }
}

void CMissionList_Charac::Update_ComboMission_Event(CUser& user,
                                                    int comboIdx,
                                                    short count)
{
    short c = count;
    if ((count >= 0) && (count < 9) &&
        ((comboIdx == 0x1f) || (comboIdx == 0x20)))
    {
        short value = m_missionList[comboIdx].m_count;
        if (count > 3)
        {
            c = (short)(count - 4);
            value = m_missionList[comboIdx].m_streak;
        }
        int shift = (3 - (int)c) * 4 & 0x1f;
        unsigned short nib =
            (unsigned short)((int)value >> shift) & 0xf;
        short e = (short)(nib - 1);
        if (e < 0)
        {
            e = 0;
        }
        e = (short)((int)e << shift);
        value = (short)(value - (short)((int)(short)nib << shift)) + e;
        if (count < 4)
        {
            m_missionList[comboIdx].m_count = value;
        }
        else
        {
            m_missionList[comboIdx].m_streak = value;
        }

        MissionClearCondition_Parameter param(PVP_BATTLE_MODE_1);
        param.m_mode = 1;
        std::vector<int> out;
        check_mission_kind(comboIdx, user, param, out);
        if (!out.empty())
        {
            Send_Cleared_Mission_List(user, out);
        }
        alter();
    }
}

void CMissionList_Charac::Update_Within_Mission_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    std::vector<int> out;
    check_mission_kind(0x1b, user, param, out);
    if (!out.empty())
    {
        outClear = true;
    }
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::check_mission_kind(
    unsigned int kind, CUser& user,
    const MissionClearCondition_Parameter& param,
    std::vector<int>& outClearList)
{
    bool cleared = false;
    if (m_missionList[kind].m_index == 0)
    {
        return;
    }
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission(m_missionList[kind].m_index);
    if (!mission || !mission->canUpdate(param))
    {
        return;
    }
    mission->Update(m_missionList[kind], param);
    if (mission->isClearable(m_missionList[kind], cleared))
    {
        int index = (int)m_missionList[kind].m_index;
        outClearList.push_back(index);
        if (kind == 0x1b)
        {
            ClearMissionInfoSlot(m_missionList[7]);
            ClearMissionInfoSlot(m_missionList[0x1e]);
        }
        ClearMission(user, (short)index);
    }
    if (cleared)
    {
        Send_Fail_Mission(user, m_missionList[kind].m_index);
        cleared = false;
    }
}

// ============================================================================
// 列表管理
// ============================================================================

void CMissionList_Charac::MakeMissionList_forNewbies(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        std::list<CMission*>* list =
            (std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        if (list->front()->GetMissionGrade() == 2)
        {
            continue;
        }
        MissionInfo info;
        for (std::list<CMission*>::const_iterator it = list->begin();
             it != list->end(); ++it)
        {
            if ((*it)->isAcceptCondition(user))
            {
                (*it)->makeMissionInfo(info, user);
                addNewMission(info);
                alter();
                break;
            }
        }
    }
}

void CMissionList_Charac::MakeMissionList_forOldUser(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        std::list<CMission*>* list =
            (std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        if (getIndex_byKind(list->front()->GetMissionKind()) != 0)
        {
            continue;
        }
        if (list->front()->GetMissionGrade() == 2)
        {
            continue;
        }
        MissionInfo info;
        for (std::list<CMission*>::const_iterator it = list->begin();
             it != list->end(); ++it)
        {
            if ((*it)->isAcceptCondition(user))
            {
                (*it)->makeMissionInfo(info, user);
                addNewMission(info);
                alter();
                break;
            }
        }
    }
}

bool CMissionList_Charac::MakeMissionList_JustKind(const CUser& user,
                                                   unsigned int kind)
{
    bool result = false;
    std::list<CMission*>* list =
        (std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
    if (!list->empty())
    {
        MissionInfo info;
        if (getIndex_byKind(list->front()->GetMissionKind()) == 0)
        {
            for (std::list<CMission*>::const_iterator it = list->begin();
                 it != list->end(); ++it)
            {
                if ((*it)->isAcceptCondition(user))
                {
                    (*it)->makeMissionInfo(info, user);
                    addNewMission(info);
                    alter();
                    return true;
                }
            }
        }
    }
    return result;
}

void CMissionList_Charac::addNewMission(const MissionInfo& info)
{
    if (G_CDataManager()->find_mission(info.m_kind))
    {
        m_missionList[info.m_kind].m_kind = info.m_kind;
        m_missionList[info.m_kind].m_index = info.m_index;
        m_missionList[info.m_kind].m_count = info.m_count;
        m_missionList[info.m_kind].m_streak = info.m_streak;
        if (m_user)
        {
            sub_cUserHistoryLog_pvpMissionAdd(
                &m_user->m_historyLog, (int)info.m_kind, (int)info.m_index,
                (int)info.m_count, (int)info.m_streak);
        }
    }
}

void CMissionList_Charac::Remove_Mission(int kind)
{
    if ((kind > 0) && (kind < 0x24))
    {
        if (m_user)
        {
            sub_cUserHistoryLog_pvpMissionDel(
                &m_user->m_historyLog,
                (int)m_missionList[kind].m_kind,
                (int)m_missionList[kind].m_index);
        }
        ClearMissionInfoSlot(m_missionList[kind]);
        alter();
    }
}

void CMissionList_Charac::ClearMission(CUser& user, short index)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission((int)index);
    if (!mission)
    {
        return;
    }
    int kind = mission->GetMissionKind();
    if (kind == 6)
    {
        m_field14e = 1;
    }
    if (m_missionList[kind].m_index == index)
    {
        alter();
        m_clearedFlags[(int)index] = true;
        ClearMissionInfoSlot(m_missionList[kind]);
        mission->Reward(user);
        if (mission->GetMissionGrade() != 2)
        {
            MakeMissionList_forOldUser(user);
        }
    }
}

void CMissionList_Charac::ClearMissionKind(CUser& user, short kind)
{
    if ((kind > 0) && (kind < 0x24))
    {
        ClearMission(user, m_missionList[kind].m_index);
    }
}

void CMissionList_Charac::ResetMissionList(CUser& user)
{
    _reset();
    MakeMissionList_forNewbies(user);
}

void CMissionList_Charac::ResetDaily()
{
    Reset_DailyMission();
}

void CMissionList_Charac::Reset_DailyMission()
{
    std::bitset<32> daily = G_CDataManager()->get_daily_mission_kind();
    for (int i = 0; i < 0x20; ++i)
    {
        if (daily[i])
        {
            ClearMissionInfoSlot(m_missionList[i]);
            m_missionList[i].m_kind = (short)i;
        }
    }

    std::vector<std::pair<unsigned int, unsigned int> > newList;
    G_CDataManager()->get_New_DailyMission(newList);
    if (newList.empty())
    {
        // ORIG 此处仅打日志
    }
    for (unsigned int i = 0; i < newList.size(); ++i)
    {
        unsigned int kind = newList[i].first;
        if (daily[kind])
        {
            MakeMissionList_JustKind(*m_user, kind);
        }
    }
    alter();
}

void CMissionList_Charac::GM_SetMissionValue(int kind, int value)
{
    if ((kind > 0) && (kind < 0x24))
    {
        m_missionList[kind].m_count = (short)value;
        alter();
    }
}

int CMissionList_Charac::getNewMission(CUser& user, int kind)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission(kind);
    if (mission)
    {
        ClearMissionKind(user, (short)mission->GetMissionKind());
    }
    return 0;
}

// ============================================================================
// 发送
// ============================================================================

void CMissionList_Charac::Send_MissionList(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x140);
    int index = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    for (int kind = 1; kind < 0x24; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            packet.put_short((int)m_missionList[kind].m_index);
            if (kind == 0x1b)
            {
                short combo = 0;
                for (int i = 0; i < 5; ++i)
                {
                    if (((int)m_missionList[kind].m_count >> i) & 1)
                    {
                        ++combo;
                    }
                }
                packet.put_short((int)combo);
            }
            else
            {
                packet.put_short((int)m_missionList[kind].m_count);
            }
            int missionIdx = (int)m_missionList[kind].m_index;
            CMission* mission =
                (CMission*)G_CDataManager()->find_mission(missionIdx);
            if (!mission)
            {
                ClearMissionInfoSlot(m_missionList[kind]);
            }
            else
            {
                if ((kind == 0x1b) || (kind == 0x1f) || (kind == 0x20))
                {
                    packet.put_short((int)m_missionList[kind].m_streak);
                }
                else
                {
                    packet.put_short(mission->getClearCondition());
                }
                ++count;
            }
        }
    }
    packet.put_byte(index, count);
    unsigned char buf[32];
    BitsetToStr(m_clearedFlags, buf);
    for (int i = 0; i < 0x20; ++i)
    {
        packet.put_byte((int)buf[i]);
    }
    packet.finalize(true);
    user.Send(packet);
}

void CMissionList_Charac::Send_Cleared_Mission_List(
    CUser& user, const std::vector<int>& clearList)
{
    if (clearList.empty())
    {
        return;
    }
    PacketGuard packet;
    packet.put_header(0, 0x141);
    packet.put_byte((int)clearList.size());
    for (unsigned int i = 0; i < clearList.size(); ++i)
    {
        packet.put_short(clearList[i]);
    }
    packet.finalize(true);
    user.Send(packet);
}

void CMissionList_Charac::Send_Fail_Mission(CUser& user, short index)
{
    PacketGuard packet;
    packet.put_header(0, 0x157);
    packet.put_short((int)index);
    packet.finalize(true);
    user.Send(packet);
}

void CMissionList_Charac::setIssue(bool isNewbie, bool isDaily)
{
    if (isDaily)
    {
        m_field148 = (short)(m_field148 * 2);
        m_field148 = (short)((unsigned short)m_field148 +
                             (unsigned short)isNewbie);
    }
    else
    {
        m_field14a = (short)(m_field14a * 2);
        m_field14a = (short)((unsigned short)m_field14a +
                             (unsigned short)isNewbie);
    }
    m_field14c = (short)(m_field14c * 2);
    m_field14c = (short)((unsigned short)m_field14c +
                         (unsigned short)isNewbie);
}

// ============================================================================
// 查询
// ============================================================================

bool CMissionList_Charac::isClearMission(int index) const
{
    if ((index >= 0) && (index < 0x100) &&
        (m_clearedFlags[(unsigned int)index] != 0))
    {
        return 1;
    }
    return 0;
}

short CMissionList_Charac::getIndex_byKind(int kind) const
{
    if ((kind < 1) || (kind > 0x23))
    {
        return 0;
    }
    return m_missionList[kind].m_index;
}

int CMissionList_Charac::getIssue_mode(bool isDaily) const
{
    if (isDaily)
    {
        return m_field148;
    }
    return m_field14a;
}

int CMissionList_Charac::getIssue_all() const
{
    return m_field14c;
}

int CMissionList_Charac::getWinningRate() const
{
    unsigned short rate = (unsigned short)m_field14c;
    int count = 0;
    for (int i = 0; i < 10; ++i)
    {
        if (rate & 1)
        {
            ++count;
        }
        rate = (unsigned short)((int)(unsigned int)rate >> 1);
    }
    return count * 10;
}

bool CMissionList_Charac::empty_progress() const
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            return 0;
        }
    }
    return 1;
}

bool CMissionList_Charac::isUserNewbie() const
{
    static std::bitset<256> zeroBit;
    if ((empty_progress() == 0) ||
        (zeroBit == m_clearedFlags) == 0)
    {
        return 0;
    }
    return 1;
}

bool CMissionList_Charac::isClearStealingSkillMission() const
{
    return m_field14e != 0;
}

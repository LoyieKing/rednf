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
#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_REQUEST_PVP_MISSION_LIST（0x14a）
#include "CDataManager.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "PacketGuard.h"

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
// CUser::GetPVPRoom / cUserHistoryLog::pvpMissionAdd/pvpMissionDel 均已在
// CUser.h 权威声明，直接真实调用（原 extern "C" asm 桥已删除）。


// 全局辅助（数据域 TU）：bitset <-> 存档字节串、每日排程判定
// ORIG 0x8a5a989：逐字节/位转换
void StrToBitset(unsigned char* data, std::bitset<256>& bitset)
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 8; ++j)
            bitset[i * 8 + j] = (data[i] >> j) & 1;
}
// ORIG 0x8a5a90a：bitset → 32 字节（ORIG mangled 第二参为 std::string&）
void BitsetToStr(const std::bitset<256>& bitset, std::string& out)
{
    out.resize(0x20);
    for (int i = 0; i < 32; ++i)
    {
        unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            byte += (unsigned char)(bitset[i * 8 + j] << j);
        out[i] = (char)byte;
    }
}

// ORIG 0x8a5a90a（另一重载 _Z11BitsetToStrRKSt6bitsetILj256EEPh，输出 unsigned char*）
void BitsetToStr(const std::bitset<256>& bitset, unsigned char* out)
{
    for (int i = 0; i < 32; ++i)
    {
        // ORIG 槽序 j(-0x10)、byte(-0x11)、i(-0xc)：j 先于 byte 初始化（P28/P191
        // 双 init 形），内层循环槽往返 movzbl/lea 形。
        int j = 0;
        unsigned char byte = 0;
        for (j = 0; j < 8; ++j)
            byte += (unsigned char)(bitset[i * 8 + j] << j);
        out[i] = byte;
    }
}
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；与 CInventory.h
// 的占位版本不同，本 TU 使用真实形态）


// CEnvironment +0x37c 的每日任务排程时间（ORIG 布局）
static int GetDailyScheduleTime(CEnvironment* env)
{
    // env+0x37c = Server_Envir(+0x198)::m_fatigueTime(+0x1e4)（CEnvironment.h 权威
    // 布局；与本仓 CBoosterGage.cpp / BingoEventLink4.cpp / CGameManager.cpp 同款
    // 具名口径一致）。ORIG loadData 0x085e49d0 `mov 0x37c(%eax),%eax` 4 字节读。
    return env->m_serverEnvir.m_fatigueTime;
}

// ============================================================================
// 构造 / 析构 / 虚接口
// ============================================================================

CMissionList_Charac::CMissionList_Charac()
{
    // ORIG C1 @0x832af90：仅隐式成员构造（MissionInfo[36] 逐个清零 +
    // bitset<256> 默认构造，不额外 reset；显式清零循环与 reset 均为
    // NEW 冗余，移除以对齐 ORIG 字节）。
}

CMissionList_Charac::~CMissionList_Charac()
{
}

bool CMissionList_Charac::loadData(CUser* pUser, char* pData)
{
    _reset();
    m_user = pUser;
    // pData 实为 SIG_REQUEST_PVP_MISSION_LIST（0x14a 字节 DB/存档 blob，
    // _saveData 侧同一类型），定基类型后全部具名字段访问。
    // ORIG 0x085e47ee `mov %eax,-0x14(%ebp)` 亦保留一份本地副本。
    SIG_REQUEST_PVP_MISSION_LIST* pSig =
        reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);

    for (int kind = 0; kind < 36; ++kind)
    {
        // ORIG 0x085e481c..0x085e4866：movzwl blob+6/+4/+8/+0xa → this+7/+5/+9/+0xb
        // 即 blob m_missionList[kind]（+0x04 起、步长 8）的 index/kind/count/streak。
        m_missionList[kind].m_index = pSig->m_missionList[kind].m_index;
        m_missionList[kind].m_kind = pSig->m_missionList[kind].m_kind;
        m_missionList[kind].m_count = pSig->m_missionList[kind].m_count;
        m_missionList[kind].m_streak = pSig->m_missionList[kind].m_streak;
    }
    StrToBitset(pSig->m_cleared, m_clearedFlags);   // +0x124（32 字节位图）
    m_field148 = pSig->m_field144;                  // +0x144
    m_field14a = pSig->m_field146;                  // +0x146
    m_field14c = pSig->m_field148;                  // +0x148

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
    // pData = SIG_REQUEST_PVP_MISSION_LIST blob（与 _saveData/loadData 同一布局；
    // ORIG 0x085e4acd `mov %eax,-0x10(%ebp)` 保留一份本地副本）。
    SIG_REQUEST_PVP_MISSION_LIST* pSig =
        reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);
    std::memset(pSig, 0, sizeof(SIG_REQUEST_PVP_MISSION_LIST));   // 0x14a
    pSig->m_characNo = m_tail14f.m_characNo;    // this+0x150 → blob+0x00
    for (int kind = 0; kind < 36; ++kind)
    {
        pSig->m_missionList[kind].m_index = m_missionList[kind].m_index;
        pSig->m_missionList[kind].m_kind = m_missionList[kind].m_kind;
        pSig->m_missionList[kind].m_count = m_missionList[kind].m_count;
        pSig->m_missionList[kind].m_streak = m_missionList[kind].m_streak;
    }
    {
        // ORIG 直接调用 BitsetToStr(Ph) 重载写 0x20 字节到 pData+0x124（无 string 中间层）
        BitsetToStr(m_clearedFlags, pSig->m_cleared);
    }
    pSig->m_field144 = m_field148;
    pSig->m_field146 = m_field14a;
    pSig->m_field148 = m_field14c;
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
    // ORIG +0x14f：BitsetToStr(m_clearedFlags, m_cleared) 直接写 unsigned char*
    //（Ph 重载），无 std::string 中间层（原实现错误引出了 string ctor/dtor/data）。
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
void CMissionList_Charac::Update_EnterWorld_event(CUser& user)
{
    int type = G_GameWorld()->GetChannelType();
    if ((type == 0xf) || (type == 0x10))
    {
        alter();
        std::vector<int> out;
        MissionClearCondition_Parameter param(PVP_BATTLE_MODE_2);
        int pvpType = G_CDataManager()->GetPvPChannelType();  // ORIG 死代码调用（结果未使用）
        int chanType = G_GameWorld()->GetChannelType();       // ORIG 死代码调用（结果未使用）
        check_mission_kind(7, user, param, out);
        check_mission_kind(0x1e, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
}

void CMissionList_Charac::Update_Kill_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    alter();
    std::vector<int> out;
    check_mission_kind(5, user, param, out);
    check_mission_kind(0xf, user, param, out);
    check_mission_kind(0x19, user, param, out);
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::Update_Win_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    Update_Within_Mission_event(user, param, outClear);
    alter();
    if (outClear == true)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
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
            short issueMode = getIssue_mode(mode == PVP_BATTLE_MODE_2);  // ORIG 死代码调用（结果未使用）
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
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
    alter();
    PvP_Room* room = (PvP_Room*)user.GetPVPRoom();
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
                    bool hackUser;
                    // ORIG 0x85e5caa：kind 6/0x15 执行 check_mission_kind，
                    // 其它 kind 记 HackUser 日志；ORIG 短路二次 GetMissionKind
                    // + 两分支内各重取（P167 定 4 调用位点，复现需逐位点对齐）。
                    if ((kind == 6) ||
                        ((kind = mission->GetMissionKind()) == 0x15))
                        hackUser = false;
                    else
                        hackUser = true;
                    if (hackUser)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x704, 5);
                        trace("[@missionSystem] HackUser! send [%d kind]\n",
                              mission->GetMissionKind());
                    }
                    else
                    {
                        check_mission_kind(mission->GetMissionKind(), user,
                                           param, values);
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
            m_missionList[7].clear();
            m_missionList[0x1e].clear();
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
    // ORIG 0x85e4c6c: find_mission 以 MissionInfo.index（+2）判定存在性，slot 以 kind（+0）索引
    if (G_CDataManager()->find_mission(info.m_index))
    {
        m_missionList[info.m_kind].m_kind = info.m_kind;
        m_missionList[info.m_kind].m_index = info.m_index;
        m_missionList[info.m_kind].m_count = info.m_count;
        m_missionList[info.m_kind].m_streak = info.m_streak;
        if (m_user)
        {
            m_user->m_historyLog.pvpMissionAdd(
                (int)info.m_kind, (int)info.m_index,
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
            m_user->m_historyLog.pvpMissionDel(
                (int)m_missionList[kind].m_kind,
                (int)m_missionList[kind].m_index);
        }
        m_missionList[kind].clear();
        alter();
    }
}

void CMissionList_Charac::ClearMission(CUser& user, short index)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission((int)index);
    if (!mission)
    {
        // ORIG 0x85e6441：mission 缺失记日志（cMyTrace 5/0x797）后返回。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x797, 5);
        trace("[@missionSystem] InvalidIndex: <index : %d>", (int)index);
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
        m_missionList[kind].clear();
        mission->Reward(user);
        if (mission->GetMissionGrade() != 2)
        {
            MakeMissionList_forOldUser(user);
        }
    }
    else
    {
        // ORIG 0x85e64e1：m_index 不匹配记日志（cMyTrace 5/0x7a5）后返回。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7a5, 5);
        trace("[@missionSystem] NotMatchIndex: <valid index:%d invalid index:%d>",
              (int)m_missionList[kind].m_index, (int)index);
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
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
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
            m_missionList[i].clear();
            m_missionList[i].m_kind = (short)i;
        }
    }

    std::vector<std::pair<unsigned int, unsigned int> > newList;
    G_CDataManager()->get_New_DailyMission(newList);
    if (newList.empty())
    {
        // ORIG 0x85e5607：cMyTrace 5/0x669 记 "[@missionSystem] Daily Mission List is Empty!!"
        cMyTrace trace(__PRETTY_FUNCTION__, 0x669, 5);
        trace("[@missionSystem] Daily Mission List is Empty!!");
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
        // ORIG kind 落槽 -0xc（P0 具名局部；槽序 mission=-0x10 先、kind=-0xc 后）。
        int missionKind = mission->GetMissionKind();
        ClearMissionKind(user, (short)missionKind);
    }
    // ORIG 尾无 mov $0x0（call ClearMissionKind 后直接 leave;ret）：源码无 return，
    // 返回值自然沿 ClearMissionKind 的 eax 掉出（4.1 实证）。
}

// ============================================================================
// 发送
// ============================================================================

    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
int CMissionList_Charac::Send_MissionList(CUser& user)
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
            CMission* mission = (CMission*)G_CDataManager()->find_mission(
                (int)m_missionList[kind].m_index);
            if (!mission)
            {
                m_missionList[kind].clear();
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
    unsigned char buf[0x20];
    BitsetToStr(m_clearedFlags, buf);
    for (int i = 0; i < 0x20; ++i)
    {
        packet.put_byte((int)buf[i]);
    }
    packet.finalize(true);
    user.Send(packet);
    return 1;
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
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    else
    {
        m_field14a = (short)(m_field14a * 2);
        m_field14a = (short)((unsigned short)m_field14a +
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    m_field14c = (short)(m_field14c * 2);
    m_field14c = (short)((unsigned short)m_field14c +
                         (unsigned short)(isNewbie ? 1 : 0));
}

// ============================================================================
// 查询
// ============================================================================

bool CMissionList_Charac::isClearMission(int index) const
{
    // bitset::operator[] 已返回 bool，`!= 0` 触发隐式规范化岛（4.4 多 5 行）；
    // ORIG test %al 直测（P32 bool 直写）。
    if ((index >= 0) && (index < 0x100) && m_clearedFlags[(unsigned int)index])
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

unsigned short CMissionList_Charac::getIssue_mode(bool isDaily) const
{
    if (isDaily)
    {
        return m_field148;
    }
    return m_field14a;
}

unsigned short CMissionList_Charac::getIssue_all() const
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
    static std::bitset<256> zeroBit(0);   // ORIG bitset<256>::bitset(unsigned long)（C1Em），非默认构造
    if (empty_progress() && (zeroBit == m_clearedFlags))
        return true;
    else
        return false;
}

bool CMissionList_Charac::isClearStealingSkillMission() const
{
    return m_field14e;
}

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
// ORIG 0x8a5a86c：bitset → 32 字节（第二参 std::string&，resize(0x20) 后逐字节写）
// [b28d-58] DIFF N（P183 折叠 + N98 槽序）：ORIG 0x08a5a8d2 段为读-加-写三段
//   `mov %eax,%edx; movzbl byte,%eax; lea (%edx,%eax,1),%eax; mov %al,byte`；
//   4.4-O0 对 `x+byte` / `byte+x` / `+=` / `+(int)byte` 四形恒折 `add %al,byte`
//   内存直加（P183 折叠铁律）；i 槽 ORIG -0x10 vs 4.4 -0x14（N98 首次使用序，
//   声明序不可翻）。语义等价（uchar 回绕 + out[i] 截断同）。
void BitsetToStr(const std::bitset<256>& bitset, std::string& out)
{
    out.resize(0x20);
    for (int i = 0; i < 32; ++i)
    {
        unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            // ORIG 0x08a5a8d2 段：`mov %eax,%edx; movzbl byte,%eax;
            // lea (%edx,%eax,1),%eax; mov %al,byte` ⇒ 源形是 `bit + byte`
            // 顺序（x 在 edx、byte 在 eax）；`byte + x`/`+=` 均被 4.4 折成
            // `add %al,mem` 内存直加。
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = (char)byte;
    }
}

// ORIG 0x8a5a90a（另一重载 _Z11BitsetToStrRKSt6bitsetILj256EEPh，输出 unsigned char*）
void BitsetToStr(const std::bitset<256>& bitset, unsigned char* out)
{
    for (int i = 0; i < 32; ++i)
    {
        // [b28d-58] DIFF N（同 Ss 重载）：ORIG 槽 i(-0x10)/byte(-0x11)/j(-0xc)，
        // 4.4 为 i(-0x14)/j(-0x10)/byte(-0x9)（N98 首次使用序不可翻）；加法段
        // ORIG 读-加-写三段 vs 4.4 `add %al,byte`（P183 折叠）——语义等价。
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
    // b28c-L5：ORIG 0x085e5fe4 两守卫均为「call 结果寄存器直比 + bool 物化岛」：
    //   `call GetChannelType; cmp $0xf,%eax; setne %al; test %al,%al; jne <尾岛>`、
    //   `call getIndex_byKind; test %ax,%ax; setne %al; test %al,%al; jne <尾岛>`，
    // 且两失败出口落**不同** nop 中继（+0x1b5 / +0x1b8），body 后另有 `jmp`。
    // 4.4 对正极性 `if (a==K && b==0) {body}` 恒产 je/jne 直跳 + 结果汇聚岛
    // （少 setne、多 mov$1/jmp/mov$0），探针 /tmp/l5_probe_a.cpp hA
    // （`if (g1(x) != 0xf) return;`）实证否定早返直产 ORIG setne 三段物化；
    // 双岛由 goto 尾标签串复现（P223/BM8）。std::vector 局部包内层块，
    // 避开 4.4「goto 跨同层非平凡局部声明」拒编译约束。
    if (G_GameWorld()->GetChannelType() != 0xf)
        goto L_notchannel;
    if (getIndex_byKind(0x1b) != 0)
        goto L_hasmision;
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
L_hasmision:;
L_notchannel:;
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
    // [b28d-64] P0：ORIG 0x085e4c8f 具名指针局部（-0xc 槽往返 + cmpl）⇒ 声明局部；
    // 原码 `if (call())` 4.4 产 setne 物化（+4 条）。
    // [b28d-64] N84/P83：ORIG 0x085e4ca7..b0 为 8 字节结构整体赋值（读 +4 先/+0 后、
    // 写 +0/+4），逐 short 四写会膨胀 20 条 ⇒ 改 `m_missionList[kind] = info;`。
    void* mission = G_CDataManager()->find_mission(info.m_index);
    if (mission != 0)
    {
        m_missionList[info.m_kind] = info;
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
    // b28c-L5：ORIG 0x085e4bec 两条范围检查各落**独立** nop 中继岛
    // （+0x79 `nop; jmp +0x7d` / +0x7c `nop;` fall 到 +0x7d leave），body 尾另有
    // `jmp +0x7d`。4.4 对 `if ((kind>0)&&(kind<0x24)) {body}` 恒 jump-thread 合并两
    // 失败出口（少 4 条）。探针 /tmp/l5_probe_a.cpp fD 实证：`goto` 双守卫 + 尾标签串
    // 精确产 ORIG 三岛形（P223 多岛扩展 BM8 同法）。语义与原守卫完全一致。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    if (m_user)
    {
        m_user->m_historyLog.pvpMissionDel(
            (int)m_missionList[kind].m_kind,
            (int)m_missionList[kind].m_index);
    }
    m_missionList[kind].clear();
    alter();
L_overrange:;
L_nonrange:;
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
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e63d6 两失败出口各落独立 nop 中继岛
    // （+0x40 nop;jmp / +0x43 nop fall），探针 l5_probe_a fD 形复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    ClearMission(user, m_missionList[kind].m_index);
L_overrange:;
L_nonrange:;
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
    // [b28d-58] DIFF N104（EH 尾块相对序）：槽位集与 ORIG 全等——daily -0x38、
    //   bitset::reference -0x34、newList -0x44、trace -0x2c（P221 全表达式临时链
    //   `cMyTrace(...)(...)` 翻正 trace 槽方向，具名对象被 4.4 放最深 -0x44）。
    //   残差仅 ORIG 正常 epilogue 在前 / unwind 块在函数最末，4.4 相反（difflib
    //   rotate 放大 ~30 行），gap=1，语义等价。
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
        // [b28d-58] 试 P221 全表达式临时链：具名 trace 被 4.4 放最深槽 -0x44，
        // ORIG 为 -0x2c（块内对象槽方向相反）。
        cMyTrace(__PRETTY_FUNCTION__, 0x669, 5)(
            "[@missionSystem] Daily Mission List is Empty!!");
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
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e56ee 三岛形（+0x2f nop;jmp /
    // +0x32 nop fall），探针 l5_probe_a fD 复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    m_missionList[kind].m_count = (short)value;
    alter();
L_overrange:;
L_nonrange:;
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
    // b28c-L5：ORIG 0x085e50fa 尾形 `mov $0x1,%eax / jmp / mov $0,%eax / leave`（20 条）。
    // 4.4 对「if(cond) return 1; return 0;」恒额外物化一段 bool 归一岛（+5 条：
    // `mov $0,%eax; test %al; je; mov $1; jmp; mov $0`），探针 /tmp/l5_probe_isclear.cpp
    // fA(异形 25)/fB(三目 20 全等)/fD(嵌套 if 20 但条件树异形) 定案：三目返回形
    // 与 ORIG 逐条全等（P47 三目 vs if-return 块排布镜像的本例应用）。
    return ((index >= 0) && (index < 0x100) && m_clearedFlags[(unsigned int)index])
               ? 1
               : 0;
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

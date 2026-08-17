// df_game_r 任务类族实现（G3-1 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMission*.md + ORIG 弱访问器反汇编。
#include <bitset>

#include "CMission.h"
#include "CMissionList_Charac.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "GlobalData.h"

// RefPvpGrade 完整定义属数据域 TU；此处补本类需要的方法（ORIG 0x85d4832/0x85d4860）
class RefPvpGrade
{
public:
    int GetPvpPoint(int grade) const;
    int GetPvpNextRankPoint(int grade) const;
};

// cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};

// ============================================================================
// CMission
// ============================================================================

CMission::CMission(const MissionScript& script)
    : m_rewardItems(script.m_rewardItems)
{
    m_conditionType = script.m_kind;              // script+0x78（byte）
    m_kind = script.m_field08;
    m_missionIndex = script.m_field0c;
    m_field10 = script.m_field00;
    m_prevMissionIndex = script.m_prevMissionIndex;
    m_gradeMin = script.m_gradeMin;
    m_gradeMax = script.m_gradeMax;
    m_rateMin = script.m_rateMin;
    m_rateMax = script.m_rateMax;
    m_grade = script.m_field20;
    m_expRate = script.m_expRate;
    m_job = script.m_job;
}

CMission::~CMission() {}

bool CMission::_canUpdate(const MissionClearCondition_Parameter&) const
{
    return 1;
}

bool CMission::_isAcceptCondition(const CUser&) const
{
    return 1;
}

bool CMission::makeMissionInfo(MissionInfo& info, const CUser& user) const
{
    info.m_kind = (short)GetMissionKind();
    info.m_index = (short)GetMissionIndex();
    info.m_count = 0;
    info.m_streak = 0;
    _makeTotalMissionInfo(info, user);
    return 1;
}

bool CMission::canUpdate(const MissionClearCondition_Parameter& param) const
{
    if (m_conditionType == 0xff)
    {
        if ((param.m_mode != 1) && (param.m_mode != 2)) return 0;
    }
    else if (m_conditionType != param.m_mode)
    {
        return 0;
    }
    return _canUpdate(param);
}

int CMission::isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->getIndex_byKind(GetMissionKind()) != 0) return 0;

    int grade = user.get_pvp_grade();
    bool gradeRejected;
    if ((m_gradeMin == -1) ||
        ((m_gradeMin <= grade) && (grade < m_gradeMax)))
    {
        gradeRejected = false;
    }
    else
    {
        gradeRejected = true;
    }
    if (gradeRejected) return 0;

    int winRate = user.get_pvp_WinningRate_relateMission();
    if ((m_rateMin == -1) ||
        ((m_rateMin <= winRate) && (winRate < m_rateMax)))
    {
        return _isAcceptCondition(user);
    }
    return 0;
}

int CMission::Reward(CUser& user) const
{
    int exp = (int)(m_expRate *
                    (float)G_CDataManager()->get_BaseMissionExp_byRank(
                        (unsigned short)m_gradeMin));

    user.m_historyLog.pvpMissionClearReward(
        user.getPVPResultRefW()->m_expPoint, exp, GetMissionKind(),
        GetMissionIndex());
    user.getPVPResultRefW()->m_expPoint += exp;

    std::vector<int> outSlots;
    int ret = ((CInventory*)user.getCurCharacInvenW())
                  ->insert_event_items(m_rewardItems, outSlots,
                                       (eMoneyAddReason)0x1e,
                                       (eItemAddReason)0x33, true,
                                       "game_server_msg_173",
                                       "game_server_msg_174");
    if ((ret > 0) && (outSlots.size() > 2))
    {
        user.SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                            outSlots[0]);
    }
    _Reward(user);
    return 1;
}

bool CMission::operator<(const CMission& other)
{
    if (m_kind == other.GetMissionKind())
    {
        if (m_missionIndex == other.GetMissionIndex()) return false;
        return m_missionIndex < other.GetMissionIndex();
    }
    return m_kind < other.GetMissionKind();
}

bool CMission::_isAcceptCondition_Series(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (GetPrevMissionIndex() == -1) return 1;
    if (!list->isClearMission(GetPrevMissionIndex())) return 0;
    if (list->getIndex_byKind(GetMissionKind()) != 0) return 0;
    return 1;
}

// ============================================================================
// CMission_rank（抽象，无 ctor；verifyMission 保持纯虚）
// ============================================================================

CMission_rank::~CMission_rank() {}

bool CMission_rank::isClearable(MissionInfo& info, bool&) const
{
    return m_clearCount <= (int)info.m_count;
}

bool CMission_rank::isSeriesMission() const
{
    return 0;
}

int CMission_rank::Update(MissionInfo&, const MissionClearCondition_Parameter&) const
{
    return 1;
}

int CMission_rank::getClearCondition() const
{
    return m_clearCount;
}

// ============================================================================
// CMission_winning_count
// ============================================================================

CMission_winning_count::CMission_winning_count(const MissionScript& script,
                                               int count)
    : CMission(script)
{
    m_winCount = count;
}

CMission_winning_count::~CMission_winning_count() {}

void CMission_winning_count::_makeTotalMissionInfo(MissionInfo& info,
                                                   const CUser& user) const
{
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
}

bool CMission_winning_count::_canUpdate(
    const MissionClearCondition_Parameter& param) const
{
    std::bitset<32> bits(param.m_victory);
    return is_Victory_in_a_row(bits, 1) != 0;
}

bool CMission_winning_count::_isAcceptCondition(const CUser& user) const
{
    return _isAcceptCondition_Series(user);
}

bool CMission_winning_count::isClearable(MissionInfo& info, bool&) const
{
    return m_winCount <= (int)info.m_count;
}

bool CMission_winning_count::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_winCount + 10;
}

bool CMission_winning_count::isSeriesMission() const
{
    return 1;
}

int CMission_winning_count::Update(MissionInfo& info,
                                   const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_winning_count::getClearCondition() const
{
    return m_winCount;
}

// ============================================================================
// CMission_winning_streak
// ============================================================================

CMission_winning_streak::CMission_winning_streak(const MissionScript& script,
                                                 int streak, int clear)
    : CMission(script)
{
    m_streak = streak;
    m_clear = clear;
}

CMission_winning_streak::~CMission_winning_streak() {}

void CMission_winning_streak::_makeTotalMissionInfo(MissionInfo& info,
                                                    const CUser&) const
{
    info.m_streak = (short)m_streak;
}

bool CMission_winning_streak::_canUpdate(
    const MissionClearCondition_Parameter&) const
{
    return 1;
}

bool CMission_winning_streak::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}

bool CMission_winning_streak::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_clear + 10;
}

bool CMission_winning_streak::isSeriesMission() const
{
    return 0;
}

int CMission_winning_streak::Update(
    MissionInfo& info, const MissionClearCondition_Parameter& param) const
{
    if (param.m_victory & 1)
    {
        info.m_streak -= 1;
        if (info.m_streak < 1)
        {
            info.m_count += 1;
            info.m_streak = (short)m_streak;
        }
    }
    else
    {
        info.m_streak = (short)m_streak;
    }
    return 1;
}

int CMission_winning_streak::getClearCondition() const
{
    return m_clear;
}

// ============================================================================
// CMission_join
// ============================================================================

CMission_join::CMission_join(const MissionScript& script, int count)
    : CMission(script)
{
    m_joinCount = count;
}

CMission_join::~CMission_join() {}

void CMission_join::_makeTotalMissionInfo(MissionInfo& info,
                                          const CUser&) const
{
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
}

bool CMission_join::_isAcceptCondition(const CUser& user) const
{
    return _isAcceptCondition_Series(user);
}

bool CMission_join::isClearable(MissionInfo& info, bool&) const
{
    return m_joinCount <= (int)info.m_count;
}

bool CMission_join::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_joinCount + 10;
}

bool CMission_join::isSeriesMission() const
{
    return 1;
}

int CMission_join::Update(MissionInfo& info,
                          const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_join::getClearCondition() const
{
    return m_joinCount;
}

// ============================================================================
// CMission_kill_count
// ============================================================================

CMission_kill_count::CMission_kill_count(const MissionScript& script, int count)
    : CMission(script)
{
    m_killCount = count;
}

CMission_kill_count::~CMission_kill_count() {}

void CMission_kill_count::_makeTotalMissionInfo(MissionInfo& info,
                                                const CUser&) const
{
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
}

bool CMission_kill_count::_isAcceptCondition(const CUser& user) const
{
    return _isAcceptCondition_Series(user);
}

bool CMission_kill_count::isClearable(MissionInfo& info, bool&) const
{
    return m_killCount <= (int)info.m_count;
}

bool CMission_kill_count::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_killCount + 10;
}

bool CMission_kill_count::isSeriesMission() const
{
    return 1;
}

int CMission_kill_count::Update(MissionInfo& info,
                                const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_kill_count::getClearCondition() const
{
    return m_killCount;
}

// ============================================================================
// CMission_hereafter_join
// ============================================================================

CMission_hereafter_join::CMission_hereafter_join(const MissionScript& script,
                                                 int count)
    : CMission(script)
{
    m_joinCount = count;
}

CMission_hereafter_join::~CMission_hereafter_join() {}

bool CMission_hereafter_join::isClearable(MissionInfo& info, bool&) const
{
    return m_joinCount <= (int)info.m_count;
}

bool CMission_hereafter_join::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_joinCount + 10;
}

bool CMission_hereafter_join::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_join::Update(MissionInfo& info,
                                    const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_hereafter_join::getClearCondition() const
{
    return m_joinCount;
}

// ============================================================================
// CMission_hereafter_winning_count
// ============================================================================

CMission_hereafter_winning_count::CMission_hereafter_winning_count(
    const MissionScript& script, int count)
    : CMission(script)
{
    m_winCount = count;
}

CMission_hereafter_winning_count::~CMission_hereafter_winning_count() {}

bool CMission_hereafter_winning_count::_canUpdate(
    const MissionClearCondition_Parameter& param) const
{
    std::bitset<32> bits(param.m_victory);
    return is_Victory_in_a_row(bits, 1) != 0;
}

bool CMission_hereafter_winning_count::isClearable(MissionInfo& info,
                                                   bool&) const
{
    return m_winCount <= (int)info.m_count;
}

bool CMission_hereafter_winning_count::verifyMission(
    const MissionInfo& info) const
{
    return (int)info.m_count < m_winCount + 10;
}

bool CMission_hereafter_winning_count::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_winning_count::Update(
    MissionInfo& info, const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_hereafter_winning_count::getClearCondition() const
{
    return m_winCount;
}

// ============================================================================
// CMission_remain_hp
// ============================================================================

CMission_remain_hp::CMission_remain_hp(const MissionScript& script, int hp,
                                       int clear)
    : CMission(script)
{
    m_remainHp = hp;
    m_clear = clear;
}

CMission_remain_hp::~CMission_remain_hp() {}

bool CMission_remain_hp::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}

bool CMission_remain_hp::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_clear + 10;
}

bool CMission_remain_hp::isSeriesMission() const
{
    return 0;
}

int CMission_remain_hp::Update(MissionInfo& info,
                               const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_remain_hp::getClearCondition() const
{
    return m_clear;
}

// ============================================================================
// CMission_revenge
// ============================================================================

CMission_revenge::CMission_revenge(const MissionScript& script, int count)
    : CMission(script)
{
    m_revengeCount = count;
}

CMission_revenge::~CMission_revenge() {}

bool CMission_revenge::isClearable(MissionInfo& info, bool&) const
{
    return m_revengeCount <= (int)info.m_count;
}

bool CMission_revenge::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_revengeCount + 10;
}

bool CMission_revenge::isSeriesMission() const
{
    return 0;
}

int CMission_revenge::Update(MissionInfo& info,
                             const MissionClearCondition_Parameter& param) const
{
    switch ((int)info.m_streak)
    {
    case 0:
        if ((param.m_victory & 1) == 0) info.m_streak = 1;
        return 0;
    case 1:
        info.m_streak = 2;
        return 0;
    case 2:
        if ((param.m_victory & 1) != 0)
        {
            info.m_streak = 0;
            info.m_count += 1;
            return 1;
        }
        return 0;
    default:
        cMyTraceNoop log;
        log("[@missionSystem] : revenge ??? : %d", (int)info.m_streak);
        return 0;
    }
}

int CMission_revenge::getClearCondition() const
{
    return m_revengeCount;
}

// ============================================================================
// CMission_hereafter_kill_count
// ============================================================================

CMission_hereafter_kill_count::CMission_hereafter_kill_count(
    const MissionScript& script, int count)
    : CMission(script)
{
    m_killCount = count;
}

CMission_hereafter_kill_count::~CMission_hereafter_kill_count() {}

bool CMission_hereafter_kill_count::isClearable(MissionInfo& info,
                                                bool&) const
{
    return m_killCount <= (int)info.m_count;
}

bool CMission_hereafter_kill_count::verifyMission(
    const MissionInfo& info) const
{
    return (int)info.m_count < m_killCount + 10;
}

bool CMission_hereafter_kill_count::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_kill_count::Update(
    MissionInfo& info, const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_hereafter_kill_count::getClearCondition() const
{
    return m_killCount;
}

// ============================================================================
// CMission_kill
// ============================================================================

CMission_kill::CMission_kill(const MissionScript& script, int minKill,
                             int clear)
    : CMission(script)
{
    m_minKill = minKill;
    m_clear = clear;
}

CMission_kill::~CMission_kill() {}

bool CMission_kill::_canUpdate(const MissionClearCondition_Parameter& param) const
{
    return m_minKill <= param.m_killCount;
}

bool CMission_kill::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}

bool CMission_kill::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_clear + 10;
}

bool CMission_kill::isSeriesMission() const
{
    return 0;
}

int CMission_kill::Update(MissionInfo& info,
                          const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_kill::getClearCondition() const
{
    return m_clear;
}

// ============================================================================
// CMission_within
// ============================================================================

CMission_within::CMission_within(const MissionScript& script, int round,
                                 int clear, int a)
    : CMission(script)
{
    m_round = round;
    m_clear = clear;
    m_field48 = a;
}

CMission_within::~CMission_within() {}

bool CMission_within::_isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (user.getPVPResultRefR()->m_pvpGrade != m_gradeMin) return 0;

    int curPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(m_gradeMin);
    int nextPoint =
        GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(m_gradeMin);
    if (user.getPVPResultRefR()->m_expPoint >= nextPoint)
    {
        const_cast<CUser&>(user).getPVPResultRefW()->m_expPoint = nextPoint;
        return 1;
    }
    return 0;
}

bool CMission_within::isClearable(MissionInfo& info, bool& clear) const
{
    int count = 0;
    for (int i = 0; i < m_round; i++)
    {
        if (((int)info.m_count >> i) & 1) count++;
    }
    if (count < m_clear)
    {
        if ((m_round - (int)info.m_streak) + count < m_clear)
        {
            info.m_count = 0;
            info.m_streak = 0;
            clear = true;
        }
        return 0;
    }
    return 1;
}

bool CMission_within::verifyMission(const MissionInfo&) const
{
    return 1;
}

bool CMission_within::isSeriesMission() const
{
    return 0;
}

int CMission_within::Update(MissionInfo& info,
                            const MissionClearCondition_Parameter& param) const
{
    info.m_count = (short)((int)info.m_count * 2);
    info.m_count = (short)(info.m_count +
                           ((param.m_victory & 1) ? 1 : 0));
    info.m_streak += 1;
    return 1;
}

int CMission_within::getClearCondition() const
{
    return m_clear;
}

// ============================================================================
// CMission_remains_hp
// ============================================================================

CMission_remains_hp::CMission_remains_hp(const MissionScript& script, int hp,
                                         int clear)
    : CMission(script)
{
    m_remainHp = hp;
    m_clear = clear;
}

CMission_remains_hp::~CMission_remains_hp() {}

bool CMission_remains_hp::_isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    return user.get_charac_job() == m_job;
}

bool CMission_remains_hp::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}

bool CMission_remains_hp::verifyMission(const MissionInfo& info) const
{
    return (int)info.m_count < m_clear + 10;
}

bool CMission_remains_hp::isSeriesMission() const
{
    return 0;
}

int CMission_remains_hp::Update(MissionInfo& info,
                                const MissionClearCondition_Parameter&) const
{
    info.m_count += 1;
    return 1;
}

int CMission_remains_hp::getClearCondition() const
{
    return m_clear;
}

// ============================================================================
// CMission_move_channel
// ============================================================================

CMission_move_channel::CMission_move_channel(const MissionScript& script,
                                             int target)
    : CMission(script)
{
    m_targetChannel = target;
}

CMission_move_channel::~CMission_move_channel() {}

bool CMission_move_channel::_isAcceptCondition(const CUser& user) const
{
    return _isAcceptCondition_Series(user);
}

bool CMission_move_channel::isClearable(MissionInfo& info, bool&) const
{
    if (info.m_streak == 0x10) return 1;
    if (m_targetChannel == (int)info.m_count) return 1;
    if ((m_targetChannel == 10) && (info.m_count == 3)) return 1;
    return 0;
}

bool CMission_move_channel::verifyMission(const MissionInfo&) const
{
    return 1;
}

bool CMission_move_channel::isSeriesMission() const
{
    return 1;
}

int CMission_move_channel::Update(
    MissionInfo& info, const MissionClearCondition_Parameter& param) const
{
    info.m_count = (short)param.m_channelTo;
    info.m_streak = (short)param.m_channelFrom;
    return 1;
}

int CMission_move_channel::getClearCondition() const
{
    return 1;
}

// ============================================================================
// CMission_move_channel_total
// ============================================================================

CMission_move_channel_total::CMission_move_channel_total(
    const MissionScript& script, int target)
    : CMission(script)
{
    m_targetChannel = target;
}

CMission_move_channel_total::~CMission_move_channel_total() {}

bool CMission_move_channel_total::_isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->getIndex_byKind(0x1b)) return 0;
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (GetPrevMissionIndex() == -1) return 0;
    if (list->isClearMission(GetPrevMissionIndex())) return 0;
    if (list->getIndex_byKind(7)) return 0;
    return 1;
}

bool CMission_move_channel_total::isClearable(MissionInfo& info, bool&) const
{
    if (info.m_streak == 0x10) return 1;
    if (m_targetChannel == (int)info.m_count) return 1;
    if ((m_targetChannel == 10) && (info.m_count == 3)) return 1;
    return 0;
}

bool CMission_move_channel_total::verifyMission(const MissionInfo&) const
{
    return 1;
}

bool CMission_move_channel_total::isSeriesMission() const
{
    return 1;
}

int CMission_move_channel_total::Update(
    MissionInfo& info, const MissionClearCondition_Parameter& param) const
{
    info.m_count = (short)param.m_channelTo;
    info.m_streak = (short)param.m_channelFrom;
    return 1;
}

int CMission_move_channel_total::getClearCondition() const
{
    return 1;
}

// ============================================================================
// CMission_combo_clear
// ============================================================================

CMission_combo_clear::CMission_combo_clear(
    const MissionScript& script,
    const std::map<combo_script_key, combo_script_data>& comboMap)
    : CMission(script), m_comboMap(comboMap)
{
}

CMission_combo_clear::~CMission_combo_clear() {}

void CMission_combo_clear::_makeTotalMissionInfo(MissionInfo& info,
                                                 const CUser& user) const
{
    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();

    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    if (it == m_comboMap.end()) return;

    int total = (int)it->second.m_rewardList.size();
    int i = 0;
    while ((i < 4) && (i < total))
    {
        info.m_count = (short)((int)info.m_count << 4);
        info.m_count = (short)(info.m_count +
                               (it->second.m_rewardList[i].second & 0xf));
        i++;
    }
    if (total > 4)
    {
        for (i = 4; i < 8; i++)
        {
            info.m_streak = (short)((int)info.m_streak << 4);
            if (i < total)
            {
                info.m_streak = (short)(
                    info.m_streak +
                    (it->second.m_rewardList[i].second & 0xf));
            }
        }
    }
}

bool CMission_combo_clear::_isAcceptCondition(const CUser& user) const
{
    if (!_isAcceptCondition_Series(user)) return 0;

    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();
    return m_comboMap.find(key) != m_comboMap.end();
}

bool CMission_combo_clear::isClearable(MissionInfo& info, bool&) const
{
    return (info.m_count == 0) && (info.m_streak == 0);
}

bool CMission_combo_clear::verifyMission(const MissionInfo&) const
{
    return 1;
}

bool CMission_combo_clear::isSeriesMission() const
{
    return 1;
}

int CMission_combo_clear::Update(MissionInfo&,
                                 const MissionClearCondition_Parameter&) const
{
    return 1;
}

int CMission_combo_clear::getClearCondition() const
{
    return 0;
}

int CMission_combo_clear::_Reward(CUser& user) const
{
    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();

    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    if (it == m_comboMap.end()) return 1;

    std::vector<std::pair<int, int> > items;
    items.push_back(it->second.m_bonus);

    std::vector<int> outSlots;
    int ret = ((CInventory*)user.getCurCharacInvenW())
                  ->insert_event_items(items, outSlots,
                                       (eMoneyAddReason)0x1e,
                                       (eItemAddReason)0x33, true,
                                       "game_server_msg_173",
                                       "game_server_msg_174");
    if ((ret > 0) && (outSlots.size() > 2))
    {
        user.SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                            outSlots[0]);
    }
    return 1;
}

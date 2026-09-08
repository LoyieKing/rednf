// df_game_r 任务类族实现（G3-1 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMission*.md + ORIG 弱访问器反汇编。
#include <bitset>

#include "CMission.h"
#include "CMissionList_Charac.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "GlobalData.h"
#include "RefPvpGrade.h"   // RefPvpGrade 权威定义（GetPvpPoint/GetPvpNextRankPoint）

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

// b28d-50：ORIG 0x085e2ffc 发射序 = 声明序（标量 +4..+14 → +0x18/+0x20 两处
// 8 字节**整体拷贝** → +0x28/+0x2c → vector +0x30 → +0x3c）；旧「vector 只写
// meminit、标量留函数体」使 4.4 先发 vector C1（P239）。全部移入初始化列表且
// 按声明序书写，+0x18/+0x20 经 union 的 pair 视图整体拷贝（P246/P247）。
CMission::CMission(const MissionScript& script)
    : m_conditionType(script.m_kind)              // script+0x78（byte）
    , m_kind(script.m_field08)
    , m_missionIndex(script.m_field0c)
    , m_field10(script.m_field00)
    , m_prevMissionIndex(script.m_prevMissionIndex)
    // [b28d-70] pair 成员化后 C1 仍 DIFF 61/59（残余 2×add-vs-lea，N18a，零回归）。
    // ORIG C1 0x085e304b/0x085e305f 两处 8 字节逐字段内联拷贝：成员与 MissionScript
    // +0x10/+0x18 同为 std::pair<int,int>（pair 拷贝构造对 <int,int> trivial ⇒ 内联，
    // 无 call；get_WithinMissionIndex 0x085e2f00 的 pair 消费实证该真实类型，b28d-70）。
    , m_rankRange(script.m_grade)
    , m_rateRange(script.m_rate)
    , m_grade(script.m_field20)
    , m_expRate(script.m_expRate)
    , m_rewardItems(script.m_rewardItems)
    , m_job(script.m_job)
{
}

CMission::~CMission() {}

// ORIG 0x85e3188：基类空实现（非纯虚；未覆写该方法的派生类保持具体）。
void CMission::_makeTotalMissionInfo(MissionInfo& info,
                                     const CUser& user) const
{
    (void)info; (void)user;
}

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
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG %edx vs 4.4 %eax（N7，探针实证），
    // 跳转/其余助记符全等，ndiff 12 封顶。
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

bool CMission::isAcceptCondition(const CUser& user) const
{
    // [b28d-70] 判词：AE 78=78（m_rankRange.first/.second、m_rateRange.first/.second
    // 偏移 +0x18/+0x1c/+0x20/+0x24 与原字段名完全同址，pair 化零指令差）。
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->getIndex_byKind(GetMissionKind()) != 0) return 0;

    // b28d-50：ORIG 0x085e31d9 段无 grade 槽（两次独立 call get_pvp_grade，
    // 0x085e31f0/0x085e31ff）且布尔**留在 %al**（mov $1/$0 + test，无槽）⇒ 原码是
    // 单一 `if (A && (B || C)) return 0;`（&&/|| 跳转物化），非具名 bool + else-if；
    // m_rankRange.first 作左操作数先入 %ebx 跨 call 保活（P23/P66）。
    if ((m_rankRange.first != -1) &&
        ((m_rankRange.first > user.get_pvp_grade()) ||
         (user.get_pvp_grade() >= m_rankRange.second)))
        return 0;

    int winRate = user.get_pvp_WinningRate_relateMission();
    // b28d-50：ORIG 0x085e3233 段块序为 REJECT（mov $0）在前、PASS（虚调）在后
    // （je PASS; jg REJECT; jg PASS; REJECT; PASS）⇒ 原码为否定式 `if(拒绝) return 0;
    // return _isAcceptCondition(user);`，而非正向 PASS 先行（4.4 反产 jle REJECT）。
    if ((m_rateRange.first != -1) &&
        ((m_rateRange.first > winRate) || (winRate >= m_rateRange.second)))
        return 0;
    return _isAcceptCondition(user);
}

int CMission::Reward(CUser& user) const
{
    // [b28d-70] 判词：AE 124=124（m_rankRange.first 读 +0x18；P167 CLEAR 16=16）。
    // b28d-50：ORIG 0x085e32xx 对 get_BaseMissionExp_byRank 的结果先
    // `mov $0,%edx; mov %eax,-0x30; mov %edx,-0x2c; fildll` ⇒ 原码为
    // `(float)(unsigned long long)` 零扩展 64 位（int 恒 fildl）。
    // b28d-50：ORIG 先 fildll（call 结果）后 flds m_expRate（0x085e3352 段，
    // 无临时槽）⇒ 书写序为「call 结果 * m_expRate」；写成 m_expRate * call() 会
    // 把 m_expRate 先 fstps 到临时槽再 fmuls（P243 x87 序）。
    int exp = (int)((float)(unsigned long long)(unsigned int)
                        G_CDataManager()->get_BaseMissionExp_byRank(
                            (unsigned short)m_rankRange.first) * m_expRate);

    user.m_historyLog.pvpMissionClearReward(
        user.getPVPResultRefW()->m_expPoint, exp, GetMissionKind(),
        GetMissionIndex());
    user.getPVPResultRefW()->m_expPoint += exp;

    std::vector<int> outSlots;
    // b28d-50：ORIG 0x085e3393 在 insert_event_items call **前**发
    // `movl $0x0,-0xc(%ebp)`（ret 预清）⇒ 声明与赋值必须两句式（P198①）。
    int ret = 0;
    ret = ((CInventory*)user.getCurCharacInvenW())
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

int CMission::_Reward(CUser& user) const
{
    // 基类默认实现：ORIG 弱符号 0x85e6950，仅返回 1。
    // 派生类可覆写以执行具体奖励逻辑（见 CMission_combo_clear::_Reward）。
    (void)user;
    return 1;
}

bool CMission::operator<(const CMission& other)
{
    // ORIG W 0x085e30ae：!= 早退形（cmp;setne;test;je 续判，落 setl 返回），
    // 相等分支内联在 fall-through（P222 正向包裹；== 形产生镜像块序）。
    if (m_kind != other.GetMissionKind())
        return m_kind < other.GetMissionKind();
    if (m_missionIndex != other.GetMissionIndex())
        return m_missionIndex < other.GetMissionIndex();
    return false;
}

bool CMission::_isAcceptCondition_Series(const CUser& user) const
{
    // [b28d-70] 判词：AE 56/57（pair 化后逐条同形）。
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
// ORIG 0x85e6adc（W）：mov 0x40(%eax),%eax; ret —— 直返 m_clearCount。
int CMission_rank::getClearCondition() const
{
    return m_clearCount;
}

// ORIG 0x85e372c：段位区间判定；两次独立 get_pvp_grade（P48 无 CSE），
// && 短路产生 jg/jge 直跳失败块 + 布尔返回归一。
bool CMission_rank::isAcceptCondition(const CUser& user) const
{
    // [b28d-70] 判词：AE 30/31（m_rankRange.first/.second 同址）。
    // [b28c-35] ORIG 0x085e372c：第一比较先装载 m_rankRange.first 到 %ebx（跨 call
    // 保活）再 call get_pvp_grade ⇒ 左操作数是成员（交换操作数序，P23/P66）；第二比较
    // 仍左 call 右成员。原 `grade >= m_rankRange.first` 形 4.4 恒先 call 后读成员
    // （22 vs 30 insn）。
    // 尾段 ORIG 为两层物化（&& 结果 al → test %al,%al; je; mov $1/$0）⇒ P56
    // `if(cond)return true; return false;` 形保留第二层。
    if (m_rankRange.first <= user.get_pvp_grade() &&
        user.get_pvp_grade() < m_rankRange.second)
        return true;
    return false;
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
    // [b28c-35] NEAR/N7：ORIG 0x085e3548 虚调用段 `mov (%eax),%edx; …; call *%edx`
    // （间接 call 目标 %edx），4.4.7 恒 %eax；其余 28 条逐条全等。P230 补：N7 属
    // 4.4.6 后端敏感族，本波勿自切。
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
    // ORIG：`call; test %al,%al; je; mov$1; jmp; mov$0` 分支形态（P56），
    // 直 return 表达式被 4.4 折叠删分支。改 if/return true;false 保留 ORIG CFG。
    if (is_Victory_in_a_row(bits, 1))
        return true;
    return false;
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
    if (m_winCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_winning_count::isSeriesMission() const
{
    return 1;
}

int CMission_winning_count::Update(MissionInfo& info,
                                   const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    if (m_clear + 10 <= (int)info.m_count)
        return false;
    return true;
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
        --info.m_streak;
        if (info.m_streak < 1)
        {
            ++info.m_count;
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
    // [b28c-35] NEAR/N7：ORIG 0x085e378a 虚调用 `mov (%eax),%edx; mov -0xc,%eax;
    // call *%edx`（目标 %edx），4.4.7 恒 %eax；其余 28 条全等（含 P0 具名局部
    // prev 的 -0xc 槽往返）。4.4.6 后端可翻，本波勿自切。
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
    if (m_joinCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_join::isSeriesMission() const
{
    return 1;
}

int CMission_join::Update(MissionInfo& info,
                          const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    // [b28c-35] NEAR/N7：同 join（ORIG 0x085e38xx 虚调用目标 %edx vs 4.4.7 %eax）。
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
    if (m_killCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_kill_count::isSeriesMission() const
{
    return 1;
}

int CMission_kill_count::Update(MissionInfo& info,
                                const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    if (m_joinCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_hereafter_join::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_join::Update(MissionInfo& info,
                                    const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    if (is_Victory_in_a_row(bits, 1))
        return true;
    return false;
}

bool CMission_hereafter_winning_count::isClearable(MissionInfo& info,
                                                   bool&) const
{
    return m_winCount <= (int)info.m_count;
}

bool CMission_hereafter_winning_count::verifyMission(
    const MissionInfo& info) const
{
    if (m_winCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_hereafter_winning_count::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_winning_count::Update(
    MissionInfo& info, const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    if (m_clear + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_remain_hp::isSeriesMission() const
{
    return 0;
}

int CMission_remain_hp::Update(MissionInfo& info,
                               const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    if (m_revengeCount + 10 <= (int)info.m_count)
        return false;
    return true;
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
        // [b28c-35] ORIG 0x085e3ac0 段：case2 的 else 与 default 共享函数尾
        // `mov $0,%eax`（ORIG `je <nop>; nop; mov $0`），独立 `return 0` 会多
        // `mov $0;jmp` 一对（61 vs 61 但块数 128→126）。
        break;
    default:
    {
        // b28c-lane12 P0 清绑：删 asm("ebx") 绑定与 register 关键字（C28L20A
        // 反例：裸 register 强制 push/pop + 帧破形）。ORIG 形为「变参在 ctor
        // call 前求值并跨 call 保活」（P87）⇒ 改 P227 全表达式临时链承载。
        cMyTraceNoop()("[@missionSystem] : revenge ??? : %d",
                       (int)info.m_streak);
        break;
    }
    }
    return 0;
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
    if (m_killCount + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_hereafter_kill_count::isSeriesMission() const
{
    return 0;
}

int CMission_hereafter_kill_count::Update(
    MissionInfo& info, const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    // ORIG W 0x085e3bc4：cmp m_minKill, m_killCount; jl <false-block> → mov $1（killCount>=minKill 返回 true）。
    // 修正：先前 source 写了 killCount < minKill（方向反了），按 ORIG 改为 >=。
    if (param.m_killCount >= m_minKill)
        return true;
    return false;
}

bool CMission_kill::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}

bool CMission_kill::verifyMission(const MissionInfo& info) const
{
    if (m_clear + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_kill::isSeriesMission() const
{
    return 0;
}

int CMission_kill::Update(MissionInfo& info,
                          const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    // [b28d-70] 判词：AE 86=86（GetRankRange().first 三处 sret 形不变）。
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    // b28d-50：ORIG 0x085e3c30 三处均**调 GetRankRange() 取 .gradeMin**（0x085e692a
    // sret 落 -0x2c/-0x24/-0x1c 三个临时槽），不是直读 m_gradeMin；且尾比较为
    // `curPoint + (nextPoint - curPoint)` 两步冗余式（0x085e3d1a sub/add 段，P171
    // 照抄）。按 ORIG 改后 86/63 gap 闭合。
    if (user.getPVPResultRefR()->m_pvpGrade != GetRankRange().first) return 0;

    int curPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(GetRankRange().first);
    int nextPoint =
        GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(GetRankRange().first);
    if (user.getPVPResultRefR()->m_expPoint >= curPoint + (nextPoint - curPoint))
    {
        const_cast<CUser&>(user).getPVPResultRefW()->m_expPoint = nextPoint;
        return 1;
    }
    return 0;
}

bool CMission_within::isClearable(MissionInfo& info, bool& clear) const
{
    short count = 0;
    for (int i = 0; i < m_round; i++)
    {
        if (((int)info.m_count >> i) & 1) count++;
    }
    if (count >= m_clear) return 1;
    if ((m_round - (int)info.m_streak) + count < m_clear)
    {
        info.m_count = 0;
        info.m_streak = 0;
        clear = true;
    }
    return 0;
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
    // [b28c-35] N：ORIG 0x085e3d58 首段 `movzwl 0x4(%eax),%eax; cwtl; add %eax,%eax`
    // （short 提升两段），4.4 恒折 `movzwl; add` 无 cwtl。探针 /tmp/l35_probe_short.cpp
    // f1..f6 六形（(int)/(short)/(unsigned short)/复合/op= 等）全部无 cwtl ⇒ 跨版本
    // zext/sext 折叠，源形不可造。语义等价（仅低 16 位存回，负值结果一致）。
    info.m_count = (short)((int)info.m_count * 2);
    info.m_count = (short)(info.m_count +
                           ((param.m_victory & 1) ? 1 : 0));
    ++info.m_streak;
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
    if (user.get_charac_job() == m_job) return true;
    return false;
}

bool CMission_remains_hp::isClearable(MissionInfo& info, bool&) const
{
    if (m_clear <= (int)info.m_count)
        return true;
    return false;
}

bool CMission_remains_hp::verifyMission(const MissionInfo& info) const
{
    if (m_clear + 10 <= (int)info.m_count)
        return false;
    return true;
}

bool CMission_remains_hp::isSeriesMission() const
{
    return 0;
}

int CMission_remains_hp::Update(MissionInfo& info,
                                const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
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
    // [b28c-35] ORIG 0x085e441b 为 `test %al,%al; je <函数尾>`（正向包裹：it!=end
    // 时落 body，假则直跳尾）；早返 `if (it==end) return;` 产反向 `jne` 布局（P59）。
    // [b28c-35] ORIG 0x085e4414 的 operator!= 实参序为 `(&it, &end)`（0x4(%esp)=&it）；
    // 写 `it != end` 时 4.4 先求右操作数 ⇒ `(&end, &it)` 反序。交换操作数即对齐。
    if (m_comboMap.end() != it)
    {
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
}

bool CMission_combo_clear::_isAcceptCondition(const CUser& user) const
{
    if (!_isAcceptCondition_Series(user)) return 0;

    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();
    // b28d-50：①ORIG 0x085e438c 尾为 test %al,%al; je <mov $0>; mov $1; jmp;
    // mov $0（P56 分支形态）；②ORIG 求值序为 **find 先、end 后**（find sret -0x14、
    // end sret -0xc），单表达式 `find()!=end()` 4.4 反产 end 先（P293 具名 it 形）。
    // 具名 const_iterator + 分支 return + 反序实参（end() 在左）即三处同复现
    // （ORIG 0x085e4387 实参序 (%esp)=end 槽、0x4(%esp)=it 槽，P74补②/P220）。
    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    if (m_comboMap.end() != it)
        return true;
    return false;
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
    // b28d-50：ORIG 0x085e43xx 实参序 (%esp)=end、0x4(%esp)=it（同 _isAcceptCondition，
    // P74补②/P220 反序）——`end() == it` 复现。
    if (m_comboMap.end() == it) return 1;

    std::vector<std::pair<int, int> > items;
    items.push_back(it->second.m_bonus);

    std::vector<int> outSlots;
    // b28d-50：ORIG 预清 `movl $0x0,-0xc(%ebp)` 在 call 前（P198①两句式）。
    int ret = 0;
    ret = ((CInventory*)user.getCurCharacInvenW())
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

// ORIG W 0x085e692a：sret 返回 std::pair<int,int>（8 字节）。ORIG 为逐字段内联拷贝
// （`mov 0x1c(%eax),%edx; mov 0x18(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx);
// mov %ecx,%eax; ret $0x4`）——pair<int,int> 拷贝构造 trivial ⇒ 无 call，与 C1
// 0x085e304b 同族。成员已按 ORIG 建模为 std::pair<int,int> m_rankRange（b28d-70）。
// [b28d-70] 判词：IDENTICAL 11=11。
MissionRankRange CMission::GetRankRange() const
{
    return m_rankRange;
}

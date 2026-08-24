// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 CGameManager.h（m_pad[0xc0]；ctor/dtor 由 CGameManager.cpp 提供）。
// getIndex/setIndex 依 ORIG 访问 +0 的 index；leaveUser/onTimerStageTick/reset
// 依赖 ProcStage/CharacData 完整布局（当前 CGameManager.h 仅为 pad 占位），
// 按任务约定保留为签名匹配的空体桩（符号 T），后续批次补全语义。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（getProcStage/selectStage）

namespace advancealtar
{

// ORIG 0x81348ba：(const) return *(int*)this;
int StageControl::getIndex() const
{
    return *(const int*)(const void*)&m_pad[0];
}

// ORIG 0x82a669c：*(int*)this = idx;
void StageControl::setIndex(int idx)
{
    *(int*)(void*)&m_pad[0] = idx;
}

// ORIG 0x812fc4e：leaveUser —— 用户离开 advancealtar 舞台。
//   布局：+0x04 ProcStage；+0x98 CharacData（+0x04 m_user，setUser/getUser W thunk 实证）。
//   user 为空/无当前角色 → 0；
//   HistoryLog::leaveDungeon（未建模，推断跳过）→
//   GameWorld::out_from_dungeon(user) → 广播 basic_info(0,2) →
//   send_itemspace(0) → SendItemLockListInven() → setAdvanceAltarIndex(-1)
//   （CUser+0x8d012）→ StageControl::destroy（未建模，推断跳过）→ 1。
char StageControl::leaveUser()
{
    char* cd = (char*)this + 0x98;
    CUser* user = *(CUser**)(cd + 4);  // CharacData+0x04 m_user（setUser/getUser thunk 实证）
    if (user == 0)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // [推断/未建模批次] ORIG 调 HistoryLog::leaveDungeon(user, getProcStage(this))；
    // StageControl::destroy 已建模（本 TU）但 ORIG leaveUser 是否收尾销毁需
    // leaveUser.md 复核，维持前批跳过决策不变。
    G_GameWorld()->out_from_dungeon(user);
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_itemspace(0);
    user->getCurCharacInvenW()->SendItemLockListInven();
    user->m_field8d012 = -1;  // ORIG CUser::setAdvanceAltarIndex(user,-1)
    return 1;  // ORIG uVar4 = 1
}

// ORIG 0x812fb98：onTimerStageTick —— ProcStage+0x04 state 为 3/4 时推进
//   CharacData::upGage（+0x98 处）与 ProcStage::procTimeLine（+0x04 处）。
//   [推断] sendTestCheckGage（upGage 尾部发包）与 procTimeLine 依赖
//   ProcSummonObjectMs 内部（未建模），暂不执行；state 检查与 upGage 真实。
char StageControl::onTimerStageTick()
{
    char* procStage = (char*)this + 0x04;
    int state = *(int*)(procStage + 0x04);
    if (state == 3 || state == 4)
    {
        // ORIG CharacData::upGage @ 0x812d0b6（真实偏移实现）
        char* cd = (char*)this + 0x98;
        CDataManager* dm = G_CDataManager();
        int gageLevel = *(int*)(cd + 0x08);
        int gage = *(int*)(cd + 0x0c);
        char* cfg = (char*)dm + 0x6364;
        int base = **(int**)(cfg + 0x18);
        int maxGage = base * *(int*)(cfg + 0x0c + (gageLevel + 4) * 4);
        if (gage < maxGage)
        {
            float inc = (float)*(int*)(cfg + 0x08) *
                        ((float)*(int*)(cfg + 0x08 + gageLevel * 4) / (float)base);
            *(int*)(cd + 0x0c) = gage + (int)(inc + 0.5f);
        }
        else
        {
            *(int*)(cd + 0x0c) = maxGage;
        }
        // [推断] ORIG 此处调 CharacData::sendTestCheckGage() 与
        // ProcStage::procTimeLine()（均依赖未建模内部），暂不执行。
    }
    return 1;  // ORIG 恒返回 1
}

void StageControl::reset()
{
}

// ORIG T 0x081300ce（docs/class_func_reports/advancealtar__StageControl/destroy.md
// 全文，Prod_GlobalAdv 2026-08-24）：reset(this) → G_CGameManager()->returnAdvanceAltar(this)。
void StageControl::destroy()
{
    reset();
    G_CGameManager()->returnAdvanceAltar(this);
}

// ORIG W 0x081348ae：return (ProcStage*)(this + 0x04)。
ProcStage* StageControl::getProcStage()
{
    return reinterpret_cast<ProcStage*>(reinterpret_cast<char*>(this) + 0x04);
}

// ORIG W 0x081348a0：return (CharacData*)(this + 0x98)。
CharacData* StageControl::getCharacData()
{
    return reinterpret_cast<CharacData*>(reinterpret_cast<char*>(this) + 0x98);
}

// ----------------------------------------------------------------------------
// ORIG T 0x081300f6（docs/class_func_reports/advancealtar__StageControl/
// sendStageInfo.md 全文）：getUser 判空直接返回；put_header(0,0x209) +
// put_binary({stageIndex, stageDifficulty}, 8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
void StageControl::sendStageInfo()
{
    CUser* user = getCharacData()->getUser();
    if (user == 0)
    {
        return;
    }

    char body[8];
    int stageIndex = getProcStage()->getStageIndex();
    int difficulty = getProcStage()->getStageDifficulty();
    std::memcpy(body, &stageIndex, 4);
    std::memcpy(body + 4, &difficulty, 4);

    PacketGuard guard;
    guard.put_header(0, 0x209);
    guard.put_binary(body, 8);
    guard.finalize(true);
    user->Send(guard);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa7a（docs/class_func_reports/advancealtar__StageControl/
// selectStage.md 全文，Prod_GlobalAdv 批次 2026-08-24）：
//   user 判空 → 3；!isAvailableTciket → 0xf5；!isEnterStage(stageId) → 8；
//   CharacData(+0x98)：m_field0 = getCurrentRidableId()、setUser(user)、
//     setSlotItem()；ProcStage::resetAll → startStage(stageId, diff)；
//   失败透传错误码；成功 → sendStageInfo() → sendMapInfo()【未建模批次跳过】
//     → setNotSurvivalModeFirstRound() → set_state(0xd) → 0。
// ----------------------------------------------------------------------------
int StageControl::selectStage(CUser* user, int stageId,
                              advancealtar::StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    CharacAdvanceAltarManager& cam = user->m_advanceAltar;
    if (!cam.isAvailableTciket())
    {
        return 0xf5;
    }
    if (!cam.isEnterStage(stageId))
    {
        return 8;
    }

    CharacData* cd = getCharacData();
    cd->m_field0 = cam.getCurrentRidableId();
    cd->setUser(user);
    cd->setSlotItem();

    ProcStage* procStage = getProcStage();
    procStage->resetAll();
    const int err = procStage->startStage(stageId, diff);
    if (err != 0)
    {
        return err;
    }

    sendStageInfo();
    // [未建模批次] ORIG sendMapInfo(0x081301e0)：依赖 getTimeLine 段循环 +
    // CharacAdvanceAltarManager::getTowerInfo + addCreatedMapMonster +
    // _MapMonsterSummary/_PhaseSummary 包体（约 0.6k 行报告子树），本批跳过。
    procStage->setNotSurvivalModeFirstRound();
    user->set_state((ch_state)0xd);
    return 0;
}

// ---- TimeLine / Phase / TimeLineSummary（ORIG advancealtar__TimeLine 报告）----
Phase::Phase()
    : m_field0(0), m_field4(0), m_assigns()
{
}

void Phase::reset()
{
    // ORIG T 0x08899cf8：两标量清零 + vector clear。
    m_field0 = 0;
    m_field4 = 0;
    m_assigns.clear();
}

TimeLineSummary::TimeLineSummary()
    : m_field0(0), m_phaseSummarys()
{
}

TimeLineSummary::~TimeLineSummary()
{
}

TimeLineSummary::TimeLineSummary(const TimeLineSummary& other)
    : m_field0(other.m_field0), m_phaseSummarys(other.m_phaseSummarys)
{
}

TimeLineSummary& TimeLineSummary::operator=(const TimeLineSummary& other)
{
    if (this != &other)
    {
        m_field0 = other.m_field0;
        m_phaseSummarys = other.m_phaseSummarys;
    }
    return *this;
}

void TimeLineSummary::reset()
{
    // ORIG T 0x08899d4e：int 清零 + vector clear。
    m_field0 = 0;
    m_phaseSummarys.clear();
}

// TimeLine ctor（ORIG T 0x08899d6e）：vector<Phase> 默认构造 + summary@+0xc 构造。
TimeLine::TimeLine()
    : m_phases(), m_summary()
{
}

// TimeLine dtor（ORIG T 0x08899db4）：summary 先、vector 后自动析构。
TimeLine::~TimeLine()
{
}

// 拷贝构造（ORIG W 0x0837e642）：vector/summary 成员拷贝。
TimeLine::TimeLine(const TimeLine& other)
    : m_phases(other.m_phases), m_summary(other.m_summary)
{
}

TimeLine& TimeLine::operator=(const TimeLine& other)
{
    if (this != &other)
    {
        m_phases = other.m_phases;
        m_summary = other.m_summary;
    }
    return *this;
}

void TimeLine::reset()
{
    // ORIG T 0x08899dfa：vector<Phase>::clear + TimeLineSummary::reset。
    m_phases.clear();
    m_summary.reset();
}

}  // namespace advancealtar
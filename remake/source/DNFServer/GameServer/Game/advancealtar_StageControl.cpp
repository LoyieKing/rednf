// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 声明唯一归口 advancealtar.h（具名布局：m_index@+0x00 / ProcStage 槽@+0x04 /
// CharacData 槽@+0x98，sizeof 0xc0）；本 TU 全部函数依据
// docs/class_func_reports/advancealtar__StageControl/ 报告翻译。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy
#include <new>            // placement new（StageControl 子对象构造）
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（getProcStage/selectStage）

namespace advancealtar
{
// ----------------------------------------------------------------------------
// ORIG T 0x0812f9a2（StageControl.md 全文）：m_index=-1 →
//   ProcStage::ProcStage(this+4, characData@this+0x98)（T 0x812dc18）+
//   CharacData::CharacData(this+0x98)（T 0x812d024）。
// 子对象以字节槽持有（advancealtar.h 布局注释），此处 placement 构造；
// ORIG 先 ProcStage 后 CharacData 的顺序保留（ProcStage ctor 仅存指针，
// 无观察差异）。
// ----------------------------------------------------------------------------
StageControl::StageControl()
{
    m_index = -1;
    new (getCharacData()) CharacData;
    new (reinterpret_cast<void*>(m_procStage)) ProcStage(*getCharacData());
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：CharacData::~CharacData →
//   ProcStage::~ProcStage（vector@+0x80、map@+0x50、ProcSummonObjectMs@+0x18
//   随成员析构；堆元素由 reset() delete，与 ORIG 一致）。
// ----------------------------------------------------------------------------
StageControl::~StageControl()
{
    reinterpret_cast<ProcStage*>(m_procStage)->~ProcStage();
    getCharacData()->~CharacData();
}

// ORIG W 0x81348ba：(const) return *(int*)this;
int StageControl::getIndex() const
{
    return m_index;
}

// ORIG W 0x82a669c：*(int*)this = idx;
void StageControl::setIndex(int idx)
{
    m_index = idx;
}

// ORIG T 0x0812fc4e（leaveUser.md 全文）：getUser/getCurCharacR 判空 → 0；
//   [blocker] HistoryLog::leaveDungeon(user, procStage)（报告 0x8133abc 在，
//   源码声明/定义缺，随 ProcStage 域批次）→ out_from_dungeon → 广播
//   basic_info(0,2) → send_itemspace(0) → getCurCharacInvenR().
//   SendItemLockListInven() → setAdvanceAltarIndex(-1)（CUser+0x8d012）→
//   destroy(this) → 1。
char StageControl::leaveUser()
{
    CUser* user = getCharacData()->getUser();  // CharacData::getUser W thunk 实证
    if (user == 0)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // [blocker] ORIG 此处调 HistoryLog::leaveDungeon(user, *getProcStage())。
    G_GameWorld()->out_from_dungeon(user);
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_itemspace(0);
    user->getCurCharacInvenR()->SendItemLockListInven();
    user->m_advanceAltarIdx = -1;  // ORIG CUser::setAdvanceAltarIndex(user,-1)
    destroy();                // ORIG 尾部真实调用 destroy(this)（leaveUser.md 实证）
    return 1;                 // ORIG uVar4 = 1
}

// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
char StageControl::onTimerStageTick()
{
    ProcStage* procStage = getProcStage();
    int state = procStage->m_state;  // ProcStage::getState W 0x08134838（+0x04）
    if (state == ProcStage::State::S_N3 || state == ProcStage::State::S_N4)
    {
        getCharacData()->upGage();
        procStage->procTimeLine();
    }
    return 1;  // ORIG 恒返回 1
}

// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
void StageControl::reset()
{
    getProcStage()->resetAll();
    getCharacData()->reset();
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
    return reinterpret_cast<ProcStage*>(m_procStage);
}

// ORIG W 0x081348a0：return (CharacData*)(this + 0x98)。
CharacData* StageControl::getCharacData()
{
    return reinterpret_cast<CharacData*>(m_characData);
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

// ----------------------------------------------------------------------------
// ORIG T 0x081306be（sendEplp.md 全文）：getUser 判空直接返回；
//   available = user->m_advanceAltar.isAvailableTciket()（CUser+0x8df60）；
//   NOTIPacketStruct::_NotiAdvanceAltarEplpState{int eplpType;
//   char isAvailableTicket; pad[3]}（8B，权威定义 advancealtar.h）；
//   put_header(0,0x20e) + put_binary(8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
void StageControl::sendEplp(StageEndType::T endType)
{
    CUser* user = getCharacData()->getUser();
    if (user == 0)
    {
        return;
    }

    NOTIPacketStruct::_NotiAdvanceAltarEplpState body;
    body.m_eplpType = endType;
    body.m_isAvailableTicket =
        user->m_advanceAltar.isAvailableTciket() ? 1 : 0;

    PacketGuard guard;
    guard.put_header(0, 0x20e);
    guard.put_binary((char*)&body, 8);
    guard.finalize(true);
    user->Send(guard);
}

// ORIG T 0x08130644（onEplp.md 全文）：sendEplp(endType) +
//   Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex())
//   （ORIG T 0x0812f956，定义 advancealtar_ProcStage.cpp）。
// ----------------------------------------------------------------------------
void StageControl::onEplp(StageEndType::T endType)
{
    sendEplp(endType);
    Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex());
}

// ORIG T 0x08130692（onEplpReturnVillage.md 全文）：ProcStage::getState()==6 → leaveUser()。
void StageControl::onEplpReturnVillage()
{
    if (getProcStage()->m_state == ProcStage::State::S_N6)
    {
        leaveUser();
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x081305f6（onClearReward.md 全文）：(-1 < e && 2 < e) && e == 3 时
//   ProcStage::setState(6) + Timer_StageControl::RegistTimerEPLP(getIndex(), 3)
//   （第二参为字面 3，非透传）。
// ----------------------------------------------------------------------------
void StageControl::onClearReward(StageEndType::T endType)
{
    if (!(endType < 0) && endType > 2 && endType == 3)
    {
        getProcStage()->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(getIndex(), (StageEndType::T)3);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fbfa（onTimer.md 全文）：msg 0xa9 → onEplp(endType)；
//   0xaa → onEplpReturnVillage()；0xa8 → onClearReward(endType)；其余无操作。
//   TIMER_MESSAGE 0xa8-0xaa 枚举器名待 TimerDispatcher 批次定名，暂用字面值。
// ----------------------------------------------------------------------------
void StageControl::onTimer(TIMER_MESSAGE msg, StageEndType::T endType)
{
    if (msg == (TIMER_MESSAGE)0xa9)
    {
        onEplp(endType);
    }
    else if (msg == (TIMER_MESSAGE)0xaa)
    {
        onEplpReturnVillage();
    }
    else if (msg == (TIMER_MESSAGE)0xa8)
    {
        onClearReward(endType);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fdd4（gotoStage.md 全文；静态语义，asm 不使用 this）：
//   getUserVillage 判空 → cMyTrace(0x585 行, vill) 返回；getArea(get_area(false))
//   判空 → cMyTrace(0x58c 行, area) 返回；否则 erase_user →
//   包1 0x17{short uid, byte vill, byte 0xff, short posX, short posY,
//   byte direction, byte visible_values} Send → clear →
//   包2 3{byte 1, short uid, byte 1} Send + send_all(guard, user) → set_state(0xd)。
// ----------------------------------------------------------------------------
void StageControl::gotoStage(CUser* user)
{
    Village* vill = G_GameWorld()->getUserVillage(user);
    if (vill == 0)
    {
        cMyTrace trace("static void advancealtar::StageControl::gotoStage(CUser*)",
                       0x585, 5);
        trace("%d", (int)user->getCurCharacVill());
        return;
    }
    int areaId = user->get_area(false);
    Area* area = vill->getArea(areaId);
    if (area == 0)
    {
        cMyTrace trace("static void advancealtar::StageControl::gotoStage(CUser*)",
                       0x58c, 5);
        trace("%d", areaId);
        return;
    }

    area->erase_user(user);
    PacketGuard guard;
    guard.put_header(0, 0x17);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(user->getCurCharacVill());
    guard.put_byte(0xff);
    guard.put_short(user->get_posX() & 0xffff);
    guard.put_short(user->get_posY() & 0xffff);
    guard.put_byte(user->get_direction());
    guard.put_byte(user->get_charac_visible_values());
    guard.finalize(true);
    user->Send(guard);

    guard.clear();
    guard.put_header(0, 3);
    guard.put_byte(1);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    G_GameWorld()->send_all(guard, user);
    user->set_state((ch_state)0xd);
}

// ----------------------------------------------------------------------------
// [blocker] getCenterCount（ORIG T 0x081307a6）/ sendMapInfo（ORIG T 0x081301e0）
// 依赖未建模符号，暂不落地定义（声明已入 advancealtar.h）：
//   - CharacAdvanceAltarManager::getTowerInfo(int,int&,int&) const：CUser.h/CUser.cpp
//     缺声明与定义（owner TranslateCUserNext 已硬中止，待 Main 重派）
//   - ProcStage::{getMap,getTimeLine,getCurrentSurvivalRoundNo,getMapBasisLevel}：
//     ORIG W thunk（0x08134860/0x08134852/0x08134878/0x08134894）头内未声明、
//     cpp 未定义（ProcStage 域，本批已获授权但属 sendMapInfo 直译配套，随
//     getTowerInfo 一并落地）
//   - ProcSummonObjectMs 子树（阻塞 ProcStage ctor/dtor/procTimeLine，进而
//     阻塞 StageControl 完整 ctor/dtor 与 onTimerStageTick 尾调）
//   已就绪：_NotiAdvanceAltarMapInfo/_NotiAdvanceAltarEplpState（shared/packet
//   权威）、_PhaseSummary/TimeLine（advancealtar.h）、CMap::get_index/
//   get_monster_list/_mapMonster、_MapMonsterSummary/m_createdMapMonsters/
//   addCreatedMapMonster（advancealtar_ProcStage.h/cpp 本批落地）。
// ----------------------------------------------------------------------------

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
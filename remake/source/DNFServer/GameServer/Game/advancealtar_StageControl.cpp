// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 声明唯一归口 advancealtar.h（具名布局：m_index@+0x00 / ProcStage 槽@+0x04 /
// CharacData 槽@+0x98，sizeof 0xc0）；本 TU 全部函数依据
// docs/class_func_reports/advancealtar__StageControl/ 报告翻译。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "CMap.h"  // CMap::get_index/get_monster_list、_mapMonster（sendMapInfo）
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
StageControl::StageControl()
{
    m_index = -1;
    CharacData* characData = reinterpret_cast<CharacData*>(
        reinterpret_cast<char*>(this) + 0x98);
    new (reinterpret_cast<void*>(m_procStage)) ProcStage(*characData);
    new (characData) CharacData;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：先 CharacData::~CharacData(this+0x98)
//   再 ProcStage::~ProcStage(this+0x04)（成员逆序析构）。两处子对象指针均直算
//   `mov this,%eax; add $0x98/0x4,%eax; mov %eax,(%esp)`，ORIG 无具名局部（P0：
//   指针全程寄存器/出参槽直传）→ 此处亦内联 reinterpret_cast，不落 -0xc 槽。
//
// [b22判词·N族(P67 EH 析构清理落点)] ORIG（gcc4.1.2）把 ProcStage(+0x04) 与
//   CharacData(+0x98) 作为真实数据成员，编译器自动生成的 D1 为二者析构建 try 区：
//   CharacData::~CharacData 若抛，落点 `mov %edx,%ebx; mov %eax,%esi; ProcStage~D1;
//   _Unwind_Resume` 仍销毁 ProcStage 再续抛 ⇒ 多出 `push %esi/%ebx; sub $0x10` 帧、
//   一处 jmp 中继、整段 landing pad（orig 29 vs 语义体 13，差 16=EH 机械）。本重建
//   StageControl 因 advancealtar.h↔advancealtar_ProcStage.h 双向 include 环
//   （StageControl 定义处 ProcStage/CharacData 仅前向声明、类型不完整）以
//   `unsigned char m_procStage[0x94]/m_characData[0x28]` 字节槽持子对象，用户显式
//   析构体内 GCC4.4.7 不为「非成员自动析构」建 EH 清理（实测：即便改真实成员，4.4.7
//   空体 D1 亦不出此 pad，见探针），故无法复现该落点；且 sizeof(ProcStage)/
//   sizeof(CharacData) 恰等槽宽（0x94/0x28），成员化需拆 include 环（越界改动、
//   波及全体 advancealtar TU）。可观察语义（逆序销毁 CharacData→ProcStage，参数
//   指针/调用序/出参槽装载逐条同 ORIG）完整一致，byte 层 N 封顶。
StageControl::~StageControl()
{
    reinterpret_cast<CharacData*>(reinterpret_cast<char*>(this) + 0x98)->~CharacData();
    reinterpret_cast<ProcStage*>(m_procStage)->~ProcStage();
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
    CUser* user =
        reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    HistoryLog::leaveDungeon(*user, *getProcStage());
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
    user->setAdvanceAltarIndex(-1);  // ORIG CUser::setAdvanceAltarIndex(user,-1)
    destroy();                // ORIG 尾部真实调用 destroy(this)（leaveUser.md 实证）
    return 1;                 // ORIG uVar4 = 1
}

// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
bool StageControl::onTimerStageTick()   // 返回类型 bool（advancealtar.h:513 同源，StrC13 会签）
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N3 ||
        reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N4)
    {
        reinterpret_cast<CharacData*>(m_characData)->upGage();
        reinterpret_cast<ProcStage*>(m_procStage)->procTimeLine();
    }
    return true;  // ORIG 恒返回 1
}

// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
void StageControl::reset()
{
    // ORIG 直访 this+0x04 / this+0x98（不进 getProcStage/getCharacData thunk 函数；P-O0 内联访问器直访）。
    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    reinterpret_cast<CharacData*>(m_characData)->reset();
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
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }

    char body[8];
    int stageIndex =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageIndex();
    int difficulty =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageDifficulty();
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
    sendMapInfo();
    procStage->setNotSurvivalModeFirstRound();
    user->set_state((ch_state)0xd);
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081306be（sendEplp.md 全文）：getUser 判空直接返回；
//   available = user->m_advanceAltar.isAvailableTciket()（CUser+0x8df60）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 NOTIPacketStruct.h）；
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
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() ==
        ProcStage::State::S_N6)
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
// 头声明 static（ORIG 无 this）；类外定义不带 static。
void StageControl::gotoStage(CUser* user)
{
    Village* vill = G_GameWorld()->getUserVillage(user);
    if (vill == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x585, 5);
        trace("%d", (int)user->getCurCharacVill());
        return;
    }
    // [b23S3C] P48：ORIG 两次独立 get_area(false)——+0x80 结果 eax 直落
    // getArea 实参槽（无 areaId 槽）、判空分支 +0xab 再 call 一次喂 trace。
    Area* area = vill->getArea(user->get_area(false));
    if (area == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x58c, 5);
        trace("%d", user->get_area(false));
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
// ORIG W 0x081307a6（getCenterCount.md 全文）：map/user 判空 → 0；
 // 遍历 CMap::get_monster_list()（按值返回 list<_mapMonster>），对每只怪
 // getTowerInfo(monster.m_orderIdx, out, out)（+0x2c），命中即计数。
// ----------------------------------------------------------------------------
int StageControl::getCenterCount() const
{
    ProcStage* procStage = const_cast<ProcStage*>(
        reinterpret_cast<const ProcStage*>(m_procStage));
    CMap* map = procStage->getMap();                    // W thunk 0x08134860
    if (map == 0)
    {
        return 0;
    }
    CharacData* characData = const_cast<CharacData*>(
        reinterpret_cast<const CharacData*>(m_characData));
    CUser* user = characData->getUser();
    if (user == 0)
    {
        return 0;
    }

    int count = 0;                                      // -0x14
    std::list<_mapMonster> monsters = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = monsters.begin();
         it != monsters.end(); ++it)
    {
        int valueA = 0;                                 // local_88[1]
        int valueB = 0;                                 // local_88[0]
        CharacAdvanceAltarManager& cam = user->m_advanceAltar;
        if (cam.getTowerInfo(it->m_mobId, valueA, valueB))
        {
            ++count;
        }
    }
    return count;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081301e0（sendMapInfo.md 全文，0416B）：user/map/timeLine 三重判空；
 // _NotiAdvanceAltarMapInfo(0x14 packed){stageType, clearSurvivalRoundNo,
 // mapIndex=CMap::get_index, currentPhaseIndex=timeline+0xc, mapBasisLevel}；
 // put_header(0,0x20a)+put_binary(0x14)；put_int(phaseSummarys.size()) 后逐
 // _PhaseSummary put_binary(0xc)；生存模式且非首轮：put_int(getCenterCount())
 // 并遍历怪物表逐只构造 _MapMonsterSummary（仅 getTowerInfo 命中者
 // addCreatedMapMonster + put_binary(0xd)）；否则 put_int(0)；finalize→Send。
// ----------------------------------------------------------------------------
void StageControl::sendMapInfo()
{
    CUser* user = getCharacData()->getUser();
    if (user == 0)
    {
        return;
    }
    ProcStage* procStage = getProcStage();
    CMap* map = procStage->getMap();
    if (map == 0)
    {
        return;
    }
    TimeLine* timeLine = procStage->getTimeLine();
    if (timeLine == 0)
    {
        return;
    }

    NOTIPacketStruct::_NotiAdvanceAltarMapInfo body;
    body.m_stageType = procStage->getStageType();
    body.m_currentSurvivalRoundNo = procStage->getCurrentSurvivalRoundNo();
    body.m_mapIndex = map->get_index();
    // TimeLine+0x0c = m_summary（TimeLineSummary）+0x00 = m_field0（int，`mov 0xc(%eax)`）。
    body.m_currentPhaseIndex = timeLine->m_summary.m_field0;
    body.m_mapBasisLevel = procStage->getMapBasisLevel();

    PacketGuard guard;
    guard.put_header(0, 0x20a);
    guard.put_binary(reinterpret_cast<char*>(&body), 0x14);
    guard.put_int(static_cast<int>(timeLine->m_summary.m_phaseSummarys.size()));
    for (std::vector<_PhaseSummary>::const_iterator it =
             timeLine->m_summary.m_phaseSummarys.begin();
         it != timeLine->m_summary.m_phaseSummarys.end(); ++it)
    {
        guard.put_binary(reinterpret_cast<const char*>(&*it), 0xc);
    }

    const bool sendMonsters = procStage->getStageType() != 1 ||
                              !procStage->isSurvivalModeFirstRound();
    if (sendMonsters)
    {
        std::list<_mapMonster> monsters = map->get_monster_list();
        guard.put_int(getCenterCount());
        CharacAdvanceAltarManager& cam = user->m_advanceAltar;
        for (std::list<_mapMonster>::const_iterator it = monsters.begin();
             it != monsters.end(); ++it)
        {
            advancealtar::_MapMonsterSummary summary;
            summary.m_orderIdx = it->m_orderIdx;          // _mapMonster+0x2c
            summary.m_seq = 0;                            // addCreatedMapMonster 覆写
            int towerValue = 0;                           // getTowerInfo 出参 1
            int towerCount = 0;                           // getTowerInfo 出参 2
            if (cam.getTowerInfo(it->m_orderIdx, towerValue, towerCount))
            {
                summary.m_towerRefA =
                    static_cast<short>(towerValue);       // u16 截断 [+0x06]
                summary.m_towerRefB =
                    static_cast<unsigned char>(towerCount);  // 低字节 [+0x08]
                summary.m_roleType = static_cast<unsigned char>(
                    it->m_roleType);                      // +0x18 低字节
                summary.m_posX = static_cast<unsigned char>(
                    it->m_posX);                          // +0x20 低字节
                summary.m_champion = it->m_champion;      // +0x1c
                procStage->addCreatedMapMonster(summary);
                guard.put_binary(reinterpret_cast<char*>(&summary), 0xd);
            }
        }
    }
    else
    {
        guard.put_int(0);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ---- TimeLine / Phase / TimeLineSummary（ORIG advancealtar__TimeLine 报告）----
Phase::Phase()
    : m_field0(0), m_field4(0), m_assigns()
{
}

void Phase::reset()
{
    // ORIG T 0x08899cf8：仅 m_field0 清零 + m_assigns.clear()（+0x04 不清）。
    m_field0 = 0;
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
    m_field0 = other.m_field0;
    m_phaseSummarys = other.m_phaseSummarys;
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
    m_phases = other.m_phases;
    m_summary = other.m_summary;
    return *this;
}

void TimeLine::reset()
{
    // ORIG T 0x08899dfa：vector<Phase>::clear + TimeLineSummary::reset。
    m_phases.clear();
    m_summary.reset();
}

}  // namespace advancealtar
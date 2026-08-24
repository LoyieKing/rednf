// ============================================================================
// advancealtar_ProcStage.cpp —— advancealtar::{ProcStage, CharacData,
// Timer_StageControl, HistoryLog} + getMemberNames 真实实现。
//
// 证据（ORIG 汇编+反编译 C 全文已读）：
//   docs/class_func_reports/advancealtar__ProcStage/{isRewardState,getStageType,
//     reset,resetAll,startStage,setNotSurvivalModeFirstRound,isSurvivalModeFirstRound,
//     getStageIndex}.md
//   docs/class_func_reports/advancealtar__CharacData/setSlotItem.md
//   docs/class_func_reports/advancealtar__Timer_StageControl/{RegistTimerEPLP,
//     RegistTimerClearReward}.md
//   docs/class_func_reports/advancealtar__HistoryLog/{dungeonClearInfo,enterDungeon}.md
//   docs/class_func_reports/advancealtar/getMemberNames.md
//   报告缺失、按门禁以 objdump 转写并标注的 W thunk：
//     CharacData::setUser 0x08134806 / getUser 0x08134814；
//     ProcStage::setState 0x08134844 / getStageDifficulty 0x0813486c /
//     getStartStageTime 0x08134886。
//
// 未建模子树（调用点保留 [未建模批次] 注释，不做伪造）：
//   ProcSummonObjectMs(+0x18)、map<short,_MapMonsterSummary>(+0x50) 内部、
//   CharacAdvanceAltarManager::getSlotItem(0x08132638)（setSlotItem 填表）、
//   CAdvanceAltarLog::incrementEntranceData / advanceAltarstatistics_（统计日志）。
// ============================================================================

#include "advancealtar_ProcStage.h"

#include <algorithm>  // std::random_shuffle
#include <cstdio>     // sprintf

#include "QuickParty.h"       // 先于 CDataManager/CDungeon 链：QUICKPARTY_H 守卫去重
#include "CDataManager.h"     // G_CDataManager/find_dungeon/find_map
#include "CDungeon.h"         // CDungeon::get_index
#include "CSystemTime.h"      // CSystemTime::getCurSec
#include "GlobalData.h"       // s_systemTime_
#include "CGameManager.h"     // StageControl 完整声明（dungeonClearInfo 引用）
#include "TimerQueue.h"       // G_TimerQueue / TimerEntry::OBJ_TYPE

namespace advancealtar
{

// ==== CharacData ============================================================

// ORIG W 0x08134806：*(this+4) = user。
void CharacData::setUser(CUser* user)
{
    m_user = user;
}

// ORIG W 0x08134814：return *(this+4)。
CUser* CharacData::getUser() const
{
    return m_user;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d246（setSlotItem.md 全文）：m_user 非空时经
// CharacAdvanceAltarManager::getSlotItem(0x08132638) 以用户祭坛管理器数据填充
// 本对象 +0x10 的 std::map<unsigned char,_SlotItem>。
// [未建模批次] getSlotItem 及 _SlotItem/BuyUpgradeData 子树未建模；本批仅保留
// 判空守卫，槽位表填充留待该批次。
// ----------------------------------------------------------------------------
void CharacData::setSlotItem()
{
    if (m_user != 0)
    {
        // ORIG: getSlotItem(m_user->m_advanceAltar, /*map&*/ *this + 0x10);
    }
}

// ==== ProcStage =============================================================

// ORIG W 0x08134844：*(this+4) = state。
void ProcStage::setState(State::T state)
{
    m_state = static_cast<int>(state);
}

// ORIG T 0x0812f770：state==3 || state==4。
bool ProcStage::isRewardState() const
{
    return m_state == 3 || m_state == 4;
}

// ORIG T 0x0812eb04：dungeon 非空 ? *(int*)(dungeon+0x804) : 0。
int ProcStage::getStageType() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    // STDungeonScript 舞台类型字段（getStageType 报告偏移实证）
    return *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(m_dungeon) + 0x804);
}

// ORIG W 0x0813486c：return *(int*)(this+0x68)。
int ProcStage::getStageDifficulty() const
{
    return m_difficulty;
}

// ORIG W 0x08134886：return *(int*)(this+0x8c)。
int ProcStage::getStartStageTime() const
{
    return m_startStageTime;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812dde0（reset.md 全文）：+0x08/+0x0c/+0x10 清零、difficulty=-1、
// survivalRound=0、ProcSummonObjectMs::reset(+0x18)；type!=1 时另做：
// state=7、+0x14=0、map(+0x50) clear、survivalFirst=1、field7c=0、vector clear。
// [未建模批次] ProcSummonObjectMs::reset(0x0812d43c) 与 map<short,...>::clear
// 内部不落地（pad 区占位，无符号依赖）。
// ----------------------------------------------------------------------------
void ProcStage::reset(StageType::T type)
{
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_difficulty = -1;
    m_survivalRound = 0;

    if (type != StageType::STAGE_SURVIVAL)
    {
        m_state = 7;
        m_field14 = 0;
        m_survivalFirst = 1;
        m_roundMaps.clear();
    }
}

// ORIG T 0x0812dd9a：dungeon=0、startStageTime=0、reset(0)、reset(1)。
void ProcStage::resetAll()
{
    m_dungeon = 0;
    m_startStageTime = 0;
    reset(StageType::STAGE_NORMAL);
    reset(StageType::STAGE_SURVIVAL);
}

// ORIG T 0x0812f70c：stageType==1 && survivalFirst!=0。
bool ProcStage::isSurvivalModeFirstRound() const
{
    return getStageType() == 1 && m_survivalFirst != 0;
}

// ORIG T 0x0812f74c：stageType==1 时 survivalFirst=0。
void ProcStage::setNotSurvivalModeFirstRound()
{
    if (getStageType() == 1)
    {
        m_survivalFirst = 0;
    }
}

// ORIG T 0x0812eb26：dungeon 非空 ? dungeon->get_index() : 0。
int ProcStage::getStageIndex() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    return m_dungeon->get_index();
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812de70（startStage.md 全文）：
//   getUser 判空 → 3；find_dungeon(stageId) 判空 → 0x15；
//   stageType==1（生存轮次推进）:
//     reset(STAGE_SURVIVAL)；非首轮 → ++survivalRound；首轮 →
//       roundMaps = dungeon+0x814 vector 拷贝 + random_shuffle、state=1、
//       startStageTime=getCurSec、HistoryLog::enterDungeon；
//     survivalRound >= roundMaps.size() → state=7、返回 0x15；
//     否则 mapBasisLevel = survivalRound、mapIdx = roundMaps[survivalRound]；
//   其它（普通关卡选择）:
//     reset(STAGE_NORMAL)、difficulty=diff、state=1、
//     mapIdx = *(int*)(dungeon + (stageId+0x200)*4)、
//     mapBasisLevel = *(int*)(dungeon + (stageId+0x208)*4)、
//     startStageTime=getCurSec、HistoryLog::enterDungeon；
//   公共尾部: find_map(mapIdx) 判空 → 0x15；timeLine = map+0xf4；返回 0。
// ----------------------------------------------------------------------------
int ProcStage::startStage(int stageId, StageDifficulty::T diff)
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return 3;
    }

    m_dungeon = G_CDataManager()->find_dungeon(stageId);
    if (m_dungeon == 0)
    {
        return 0x15;
    }

    int mapIdx = 0;
    if (getStageType() == 1)
    {
        reset(StageType::STAGE_SURVIVAL);
        if (!isSurvivalModeFirstRound())
        {
            ++m_survivalRound;
        }
        else
        {
            // ORIG: roundMaps = *(std::vector<int>*)(dungeon+0x814); random_shuffle。
            const std::vector<int>& src = *reinterpret_cast<const std::vector<int>*>(
                reinterpret_cast<const char*>(m_dungeon) + 0x814);
            m_roundMaps = src;
            std::random_shuffle(m_roundMaps.begin(), m_roundMaps.end());
        }

        if (static_cast<unsigned int>(m_survivalRound) >= m_roundMaps.size())
        {
            m_state = 7;
            return 0x15;
        }
        mapIdx = m_roundMaps[static_cast<unsigned int>(m_survivalRound)];
        m_mapBasisLevel = m_survivalRound;
    }
    else
    {
        reset(StageType::STAGE_NORMAL);
        m_difficulty = static_cast<int>(diff);
        m_state = 1;
        mapIdx = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(m_dungeon) +
            static_cast<unsigned int>(stageId + 0x200) * 4);
        m_mapBasisLevel = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(m_dungeon) +
            static_cast<unsigned int>(stageId + 0x208) * 4);
        m_startStageTime = GlobalData::s_systemTime_.getCurSec();
        HistoryLog::enterDungeon(*user, *this);
    }

    m_map = static_cast<CMap*>(G_CDataManager()->find_map(mapIdx));
    if (m_map == 0)
    {
        return 0x15;
    }
    m_timeLine = reinterpret_cast<TimeLine*>(reinterpret_cast<char*>(m_map) + 0xf4);
    return 0;
}

// ==== Timer_StageControl ====================================================

// ----------------------------------------------------------------------------
// ORIG T 0x0812f89c（RegistTimerClearReward.md 全文）：
// InsertTimerInMilisecond(OBJ_TYPE 8, index, msg 0xa8, 3000ms, key 0xa8, endType)。
// ----------------------------------------------------------------------------
void Timer_StageControl::RegistTimerClearReward(int index, StageEndType::T endType)
{
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)8, index,
                                            (TIMER_MESSAGE)0xa8, 3000LL,
                                            0xa8, (unsigned int)endType);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812f8ec（RegistTimerEPLP.md 全文）：interval 恒 1000ms（param_2==3
// 分支同值，ORIG 即冗余）；InsertTimerInMilisecond(OBJ_TYPE 8, index, msg 0xa9,
// 1000ms, key 0xa9, endType)。
// ----------------------------------------------------------------------------
void Timer_StageControl::RegistTimerEPLP(int index, StageEndType::T endType)
{
    int intervalMs = 1000;                       // ebp-0x1c 初值 0x3e8
    if (static_cast<int>(endType) == 3)
    {
        intervalMs = 1000;                       // ORIG 同值分支，忠实保留
    }
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)8, index,
                                            (TIMER_MESSAGE)0xa9, intervalMs,
                                            0xa9, (unsigned int)endType);
}

// ==== HistoryLog ============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x08133bd4（dungeonClearInfo.md 全文）：
//   playSec = startStageTime>=1 ? getCurSec()-startStageTime : 0；
//   names[0xff] 清零后 getMemberNames(user, names)；
//   cUserHistoryLog(user+0x79700).DungeonClearInfo(isLast, playSec, stageIndex,
//     0, 1, ENUM_DUNGEON_MODE 0, names, characData->m_field0)；
//   isLast 时 incrementEntranceData(advanceAltarstatistics_, idx, 0,1,0)
//     [CAdvanceAltarLog 未建模，跳过]。
// ----------------------------------------------------------------------------
void HistoryLog::dungeonClearInfo(CUser& user, StageControl& control, bool isLast)
{
    ProcStage* procStage = control.getProcStage();
    const int startStageTime = procStage->getStartStageTime();

    long playSec = 0;
    if (startStageTime >= 1)
    {
        playSec = static_cast<long>(
            GlobalData::s_systemTime_.getCurSec() - startStageTime);
    }

    char memberNames[255];                    // ebp-0x11f 起 0xff 字节清零
    for (int i = 0; i < static_cast<int>(sizeof(memberNames)); ++i)
    {
        memberNames[i] = 0;
    }
    getMemberNames(&user, memberNames);

    const int stageIndex = procStage->getStageIndex();
    const int ridableData = control.getCharacData()->m_field0;

    cUserHistoryLog* hist =
        reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(&user) + 0x79700);
    hist->DungeonClearInfo(static_cast<int>(isLast), playSec, stageIndex,
                           0, 1, ENUM_DUNGEON_MODE_0, memberNames, ridableData);

    if (isLast)
    {
        // [未建模批次] ORIG CAdvanceAltarLog::incrementEntranceData(
        // advanceAltarstatistics_, stageIndex, 0, 1, 0)（统计日志类未建模）。
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x081339b0（enterDungeon.md 全文）：
//   names[0xff] 清零后 getMemberNames；EnterDungeon(stageIndex, difficulty, 1,
//   names, 2, 0)；incrementEntranceData(..., idx, 1,0,0) [未建模跳过]。
// ----------------------------------------------------------------------------
void HistoryLog::enterDungeon(CUser& user, const ProcStage& procStage)
{
    char memberNames[255];                    // ebp-0x107 起 0xff 字节清零
    for (int i = 0; i < static_cast<int>(sizeof(memberNames)); ++i)
    {
        memberNames[i] = 0;
    }
    getMemberNames(&user, memberNames);

    const int difficulty = procStage.getStageDifficulty();
    const int stageIndex = procStage.getStageIndex();

    cUserHistoryLog* hist =
        reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(&user) + 0x79700);
    hist->EnterDungeon(stageIndex, difficulty, 1, memberNames, 2, 0);

    // [未建模批次] ORIG CAdvanceAltarLog::incrementEntranceData(
    // advanceAltarstatistics_, stageIndex, 1, 0, 0)。
}

}  // namespace advancealtar

// ----------------------------------------------------------------------------
// ORIG T 0x0812cf54（docs/class_func_reports/advancealtar/getMemberNames.md 全文）：
// 双参非空时 sprintf(buf, "\"%s\",\"%s\",\"N/A\",\"N/A\",\"N/A\"", name, name)。
// ----------------------------------------------------------------------------
namespace advancealtar
{
void getMemberNames(CUser* user, char* buf)
{
    if (user == 0 || buf == 0)
    {
        return;
    }
    const char* name = user->getCurCharacName();
    sprintf(buf, "\"%s\",\"%s\",\"N/A\",\"N/A\",\"N/A\"", name, name);
}
}  // namespace advancealtar

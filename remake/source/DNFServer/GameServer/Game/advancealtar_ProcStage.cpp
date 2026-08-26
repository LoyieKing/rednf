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
//   CharacAdvanceAltarManager::getSlotItem(0x08132638)（setSlotItem 填表；
//   其余 getSlotItem 依赖 BuyUpgradeData::getFieldDataPoint/CDataManager::find_item
//   已在 CDataManager 域建模）、
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
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整布局（HistoryLog::killMonster）
#include "TimerQueue.h"       // G_TimerQueue / TimerEntry::OBJ_TYPE

namespace advancealtar
{

// ==== CharacData ============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x0812d024（CharacData.md 全文）：4 标量清零 +
//   map<unsigned char,_SlotItem>@+0x10 默认构造（m_slotItems 成员初始化即一致）。
// ----------------------------------------------------------------------------
CharacData::CharacData()
    : m_field0(0), m_user(0), m_gageLevel(0), m_gage(0), m_slotItems()
{
}

// ORIG T 0x0812d062（~CharacData.md 全文）：map@+0x10 析构（成员自动析构一致）。
CharacData::~CharacData()
{
}

// ORIG T 0x0812d078（reset.md 全文）：m_gageLevel/m_user/m_gage/m_field0 清零 +
//   m_slotItems clear。
void CharacData::reset()
{
    m_gageLevel = 0;
    m_user = 0;
    m_gage = 0;
    m_field0 = 0;
    m_slotItems.clear();
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d0b6（upGage.md 全文）：maxGage =
//   CDataManager+0x637c 解引 * (+0x6370 + (m_gageLevel+4)*4)；未达上限时
//   m_gage += ROUND(系数(+0x636c) * level 表(+0x636c+level*4) / base)；
//   两分支尾均调 sendTestCheckGage()（ORIG 空函数）。
// ----------------------------------------------------------------------------
void CharacData::upGage()
{
    CDataManager* dm = G_CDataManager();
    char* cfg = (char*)dm + 0x6364;  // CDataManager 配置区（CData 域偏移）
    int base = **(int**)(cfg + 0x18);
    int maxGage = base * *(int*)(cfg + 0x0c + (m_gageLevel + 4) * 4);
    if (m_gage < maxGage)
    {
        float inc = (float)*(int*)(cfg + 0x08) *
                    ((float)*(int*)(cfg + 0x08 + m_gageLevel * 4) / (float)base);
        m_gage = m_gage + (int)(inc + 0.5f);
    }
    else
    {
        m_gage = maxGage;
    }
    sendTestCheckGage();
}

// ORIG T 0x0812d372（sendTestCheckGage.md 全文）：空函数。
void CharacData::sendTestCheckGage()
{
}

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

// ----------------------------------------------------------------------------
// ORIG W 0x08134674：_SummonObject ctor(this+0x14) 后 memset(this,0,0x38)。
// 整体清零使子对象前置 memset 冗余，行为一致。
// ----------------------------------------------------------------------------
_SummonObjectMs::_SummonObjectMs()
{
    std::memset(this, 0, sizeof(*this));
}

int Manager::gmTimeSpeed_ = 0;   // ORIG gmTimeSpeed_E（全局静态，初值 0）

// ==== ProcStage::ProcSummonObjectMs =========================================

// ORIG T 0x0812d378（ProcSummonObjectMs.md 全文）：m_currentMs=0、vector/map
// 构造、m_deadCount=m_aliveDelta=0、存参 characData/&currentMs。
ProcStage::ProcSummonObjectMs::ProcSummonObjectMs(CharacData& characData,
                                                  int& currentMs)
    : m_currentMs(0), m_summons(), m_summonMap(),
      m_deadCount(0), m_aliveDelta(0),
      m_characData(&characData), m_pCurrentMs(&currentMs)
{
}

// ORIG T 0x0812d3f0（~.md 全文）：map/vector 析构（成员自动析构一致；
// 堆元素由 reset() delete，dtor 不释放与 ORIG 一致）。
ProcStage::ProcSummonObjectMs::~ProcSummonObjectMs()
{
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d43c（reset.md 全文）：m_currentMs/m_deadCount/m_aliveDelta 清零；
//   遍历 vector operator delete 各元素；map clear；vector clear。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::reset()
{
    m_currentMs = 0;
    m_deadCount = 0;
    m_aliveDelta = 0;
    for (std::vector<_SummonObjectMs*>::iterator it = m_summons.begin();
         it != m_summons.end(); ++it)
    {
        delete *it;
    }
    m_summonMap.clear();
    m_summons.clear();
}

// ORIG T 0x0812d50a（isEndSummon.md 全文）：return vector.size() <= m_currentMs。
bool ProcStage::ProcSummonObjectMs::isEndSummon() const
{
    return m_summons.size() <= static_cast<unsigned int>(m_currentMs);
}

// ORIG W 0x08134820（objdump）：return m_field2c - m_field28。
int ProcStage::ProcSummonObjectMs::getAliveMonsterCount() const
{
    return m_aliveDelta - m_deadCount;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d540（insertSummonObject.md 全文）：push_back(&so) +
//   map.insert(make_pair(so.m_seq, &so))。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::insertSummonObject(_SummonObjectMs& so)
{
    m_summons.push_back(&so);
    m_summonMap.insert(std::make_pair(so.m_seq, &so));
}

// ORIG T 0x0812d5ba（getLastInsertDataIndex.md 全文）：return vector.size()。
int ProcStage::ProcSummonObjectMs::getLastInsertDataIndex()
{
    return static_cast<int>(m_summons.size());
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d5d0（completePushSummonObject.md 全文）：
//   firstIndex>0 且 size()>firstIndex 时对 [firstIndex, end) 按 compare 升序 sort。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::completePushSummonObject(int firstIndex)
{
    if (firstIndex > 0 &&
        static_cast<int>(m_summons.size()) > firstIndex)
    {
        std::sort(m_summons.begin() + firstIndex, m_summons.end(),
                  ProcStage::ProcSummonObjectMs::compare);
    }
}

// ORIG T 0x0812daec（compare.md 全文）：return lhs->m_ms < rhs->m_ms。
bool ProcStage::ProcSummonObjectMs::compare(const _SummonObjectMs* lhs,
                                            const _SummonObjectMs* rhs)
{
    return lhs->m_ms < rhs->m_ms;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d658（summonObjectAndSendPacket.md 全文）：自 m_currentMs 起遍历
//   m_summons：元素空跳过；*m_pCurrentMs < so->m_ms → 结束返回；
//   到时且未发送（m_sentFlag==0）→ m_aliveDelta++、置位、组
//   _NotiAdvanceAltarSpawnUnit{spawnIndex=seq, memcpy _SummonObject} 包
//   （header 0x20b, payload 0x28）发当前用户。每轮 m_currentMs++。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::summonObjectAndSendPacket()
{
    while (static_cast<unsigned int>(m_summons.size()) >
           static_cast<unsigned int>(m_currentMs))
    {
        _SummonObjectMs* so = m_summons[m_currentMs];
        if (so != 0)
        {
            if (*m_pCurrentMs < so->m_ms)
            {
                return;
            }
            if (so->m_ms <= *m_pCurrentMs && so->m_sentFlag == 0)
            {
                ++m_aliveDelta;
                so->m_sentFlag = 1;

                NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit body;
                body.m_spawnIndex = static_cast<unsigned short>(so->m_seq);
                std::memcpy(body.m_summon, so->m_summon, 0x24);

                PacketGuard guard;
                guard.put_header(0, 0x20b);
                guard.put_binary((char*)&body, 0x28);
                guard.finalize(true);
                CUser* user = m_characData->getUser();
                user->Send(guard);
            }
        }
        ++m_currentMs;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812d804（debugLog.md 全文）：vector/map 两轮 cMyTrace 遍历输出
//   （格式串含 ORIG 原始 "Vecotr" 拼写，忠实保留；行号 0x140/0x14d）。
//   ObjectId/ObjectType/ObjectIndex 取 m_summon 槽内 _SummonObject
//   +0x00 m_id / +0x08 m_type / +0x0c m_field0c。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::debugLog() const
{
    {
        cMyTrace trace(
            "void advancealtar::ProcStage::ProcSummonObjectMs::debugLog() const",
            0x140, 0);
        for (std::vector<_SummonObjectMs*>::const_iterator it = m_summons.begin();
             it != m_summons.end(); ++it)
        {
            const _SummonObjectMs* so = *it;
            trace("\tVecotr Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                  so->m_ms, so->m_actionDefineId,
                  *reinterpret_cast<const int*>(so->m_summon + 0x00),
                  static_cast<int>(so->m_seq), static_cast<int>(so->m_deadFlag),
                  *reinterpret_cast<const int*>(so->m_summon + 0x08),
                  *reinterpret_cast<const int*>(so->m_summon + 0x0c));
        }
    }
    {
        cMyTrace trace(
            "void advancealtar::ProcStage::ProcSummonObjectMs::debugLog() const",
            0x14d, 0);
        for (std::map<short, _SummonObjectMs*>::const_iterator it = m_summonMap.begin();
             it != m_summonMap.end(); ++it)
        {
            const _SummonObjectMs* so = it->second;
            trace("\tMap Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                  so->m_ms, so->m_actionDefineId,
                  *reinterpret_cast<const int*>(so->m_summon + 0x00),
                  static_cast<int>(so->m_seq), static_cast<int>(so->m_deadFlag),
                  *reinterpret_cast<const int*>(so->m_summon + 0x08),
                  *reinterpret_cast<const int*>(so->m_summon + 0x0c));
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812db00（dieMob.md 全文）：m_summonMap.find((short)mobSeq)；
//   未命中 → 0；命中且 so->m_sentFlag==1 且 m_deadFlag!=1 →
//   m_deadFlag=1、m_deadCount++、getUser 非空调 HistoryLog::killMonster(
//   user, _SummonObject::m_id（m_summon 槽 +0x00）, msg) 并返回 1；否则 0。
//   （第二 ushort 参数 ORIG 未引用，忠实保留。）
// ----------------------------------------------------------------------------
int ProcStage::ProcSummonObjectMs::dieMob(unsigned short mobSeq, unsigned short param,
                                          const MSG_MONSTER_DIE& msg)
{
    (void)param;  // ORIG 未使用
    std::map<short, _SummonObjectMs*>::iterator it = m_summonMap.find((short)mobSeq);
    if (it == m_summonMap.end())
    {
        return 0;
    }
    _SummonObjectMs* so = it->second;
    if (so->m_sentFlag == 1 && so->m_deadFlag != 1)
    {
        so->m_deadFlag = 1;
        ++m_deadCount;
        CUser* user = m_characData->getUser();
        if (user != 0)
        {
            const int monsterId = *reinterpret_cast<const int*>(so->m_summon);
            HistoryLog::killMonster(*user, monsterId, msg);
        }
        return 1;
    }
    return 0;
}

// ==== ProcStage =============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x0812dc18（ProcStage.md 全文）：存参 characData；state=7；
//   field8/fieldc/field10=0；field14=0（short）；ProcSummonObjectMs ctor
//   (this+0x18, *characData, &field8)；map(+0x50) 构造；difficulty=-1；
//   mapBasisLevel/dungeon/map=0；survivalFirst=1；survivalRound=0；
//   vector(+0x80) 构造；startStageTime=0；timeLine=0。
// ----------------------------------------------------------------------------
ProcStage::ProcStage(CharacData& characData)
    : m_characData(&characData),
      m_state(State::S_N7),
      m_field8(0),
      m_fieldc(0),
      m_field10(0),
      m_field14(0),
      m_summonObjectMs(*m_characData, m_field8),
      m_createdMapMonsters(),
      m_difficulty(-1),
      m_mapBasisLevel(0),
      m_dungeon(0),
      m_map(0),
      m_survivalFirst(1),
      m_survivalRound(0),
      m_roundMaps(),
      m_startStageTime(0),
      m_timeLine(0)
{
}

// ORIG T 0x0812dd26（~ProcStage.md 全文）：vector/map/子对象析构——成员自动
// 析构与 ORIG 一致（堆元素由 reset() delete）。
ProcStage::~ProcStage()
{
}

// ORIG T 0x0812e146（incCurrentMs.md 全文）：m_field8 += gmTimeSpeed_ * 1000。
void ProcStage::incCurrentMs()
{
    m_field8 += Manager::gmTimeSpeed_ * 1000;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eb5a（isEndAllPhase.md 全文）：timeLine 非空且
//   (unsigned)m_fieldc >= phases.size()。
// ----------------------------------------------------------------------------
bool ProcStage::isEndAllPhase() const
{
    if (m_timeLine == 0)
    {
        return false;
    }
    return static_cast<unsigned int>(m_fieldc) >= m_timeLine->m_phases.size();
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812eba8（makeCurrentPhaseSummonObject.md 全文）：
//   timeLine 空 → 0；全部 phase 完成 → 0；取 phases[m_fieldc] 并后移下标；
//   alive=getAliveMonsterCount()；phaseEnd=m_field10+phase.m_field0；
//   按 assigns 的 [minAlive,maxAlive]（-1=不限）选 ActionDefine：
//   无命中 → 用不限档；仍无 → 0。define 空则跳过生成但保留尾部流程。
//   对 define->m_actions：spawnMs=m_field10+firstDelayMs，未达 phaseEnd 且
//   （flag==0 或 flag 计数 < flagValue）时 new _SummonObjectMs 填充并插入，
//   首个插入记 lastInsertIndex；否则 spawnMs+=nextDelayMs。
//   尾部 completePushSummonObject(lastInsertIndex)、m_field10+=时长、debugLog、返回 1。
// ----------------------------------------------------------------------------
bool ProcStage::makeCurrentPhaseSummonObject()
{
    if (m_timeLine == 0)
    {
        return false;
    }
    if (isEndAllPhase())
    {
        return false;
    }

    Phase& phase = m_timeLine->m_phases[m_fieldc];
    ++m_fieldc;
    const int alive = m_summonObjectMs.getAliveMonsterCount();
    const int phaseLength = phase.m_field0;
    const int phaseEnd = phaseLength + m_field10;

    _ActionAssign* unlimitedAssign = 0;
    _ActionAssign* rangedAssign = 0;
    for (std::vector<_ActionAssign>::iterator it = phase.m_assigns.begin();
         it != phase.m_assigns.end(); ++it)
    {
        if (it->m_minAliveCount == -1 && it->m_maxAliveCount == -1)
        {
            unlimitedAssign = &*it;      // 不限档
        }
        else if (alive >= it->m_minAliveCount && it->m_maxAliveCount >= alive)
        {
            rangedAssign = &*it;         // 存活数区间档（循环不覆盖已命中者）
        }
    }
    if (rangedAssign == 0)
    {
        rangedAssign = unlimitedAssign;
    }
    if (rangedAssign == 0)
    {
        return false;
    }

    std::map<int, int> spawnCounts;
    int firstIndex = -1;
    ActionDefine* define = rangedAssign->m_define;
    if (define != 0)
    {
        for (std::vector<_Action>::iterator it = define->m_actions.begin();
             it != define->m_actions.end(); ++it)
        {
            int spawnMs = m_field10 + it->m_firstDelayMs;
            while (spawnMs < phaseEnd)
            {
                bool create = true;
                if (it->m_flag != 0)
                {
                    const int key = it->m_pSummon->m_field0c;
                    std::map<int, int>::iterator found = spawnCounts.find(key);
                    if (found == spawnCounts.end())
                    {
                        spawnCounts.insert(std::make_pair(key, 1));
                    }
                    else if (found->second < static_cast<int>(it->m_flagValue))
                    {
                        ++found->second;
                    }
                    else
                    {
                        create = false;   // 该对象本轮配额用尽
                    }
                }
                if (create)
                {
                    _SummonObjectMs* so = new _SummonObjectMs;
                    so->m_ms = spawnMs;
                    so->m_seq = m_field14;
                    m_field14 = static_cast<short>(m_field14 + 1);
                    so->m_actionDefineId = define->m_id;
                    std::memcpy(so->m_summon, it->m_pSummon, 0x24);
                    m_summonObjectMs.insertSummonObject(*so);
                    spawnMs += it->m_nextDelayMs;
                    if (firstIndex == -1)
                    {
                        firstIndex = m_summonObjectMs.getLastInsertDataIndex();
                    }
                }
                else
                {
                    spawnMs += it->m_nextDelayMs;
                }
            }
        }
    }
    m_summonObjectMs.completePushSummonObject(firstIndex);
    m_field10 += phaseLength;
    m_summonObjectMs.debugLog();
    return true;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812e0d6（procTimeLine.md 全文）：state==3 时 isEndSummon →
//   makeCurrentPhaseSummonObject 失败置 state=4；incCurrentMs 后仍为 3 则
//   summonObjectAndSendPacket()。
// ----------------------------------------------------------------------------
void ProcStage::procTimeLine()
{
    if (m_state == State::S_N3)
    {
        if (m_summonObjectMs.isEndSummon())
        {
            if (!makeCurrentPhaseSummonObject())
            {
                m_state = State::S_N4;
            }
        }
        incCurrentMs();
        if (m_state == State::S_N3)
        {
            m_summonObjectMs.summonObjectAndSendPacket();
        }
    }
}

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

// ORIG W 0x08134860：return *(int*)(this+0x74)。
CMap* ProcStage::getMap() const
{
    return m_map;
}

// ORIG W 0x08134852：return *(int*)(this+0x90)。
TimeLine* ProcStage::getTimeLine() const
{
    return m_timeLine;
}

// ORIG W 0x08134878：return *(int*)(this+0x7c) + 1（objdump 实证 add $0x1）。
int ProcStage::getCurrentSurvivalRoundNo() const
{
    return m_survivalRound + 1;
}

// ORIG W 0x08134894：return *(int*)(this+0x6c)。
int ProcStage::getMapBasisLevel() const
{
    return m_mapBasisLevel;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0812dde0（reset.md 全文）：+0x08/+0x0c/+0x10 清零、difficulty=-1、
// survivalRound=0、ProcSummonObjectMs::reset(+0x18)；type!=1 时另做：
// state=7、+0x14=0、map(+0x50) clear、survivalFirst=1、field7c=0、vector clear。
// ----------------------------------------------------------------------------
void ProcStage::reset(StageType::T type)
{
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_difficulty = -1;
    m_survivalRound = 0;
    m_summonObjectMs.reset();

    if (type != StageType::STAGE_SURVIVAL)
    {
        m_state = 7;
        m_field14 = 0;
        m_createdMapMonsters.clear();
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

// ----------------------------------------------------------------------------
// ORIG T 0x0812e16e（addCreatedMapMonster.md 全文）：seq = m_field14；
//   summary.m_seq(+0x04) = seq；m_field14 = seq+1；
//   m_createdMapMonsters.insert(make_pair(seq, summary))。
// ----------------------------------------------------------------------------
void ProcStage::addCreatedMapMonster(_MapMonsterSummary& summary)
{
    const short seq = m_field14;
    summary.m_seq = seq;
    m_field14 = static_cast<short>(seq + 1);
    m_createdMapMonsters.insert(std::make_pair(summary.m_seq, summary));
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

// ----------------------------------------------------------------------------
// ORIG T 0x0812f956（RegistTimerEPLPReturnVillage.md 全文）：
// InsertTimerInMilisecond(OBJ_TYPE 8, index, msg 0xaa, 50000ms, param2 0, param3 0xaa)。
// ----------------------------------------------------------------------------
void Timer_StageControl::RegistTimerEPLPReturnVillage(int index)
{
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)8, index,
                                            (TIMER_MESSAGE)0xaa, 50000LL,
                                            0, (unsigned int)0xaa);
}

// ==== HistoryLog ============================================================

// ----------------------------------------------------------------------------
// ORIG T 0x08133b40（killMonster.md 全文）：
//   cUserHistoryLog(user+0x79700).KillMonster(monsterId,
//     msg.m_killValue1(+0xa1e), msg.m_killValue2(+0xa1a),
//     msg.m_killPairA24(+0xa24), msg.m_killPairA26(+0xa26),
//     msg, roleType=0, isBoss=false, curExp=0, exp=0, eExpAddReason=1)。
// ----------------------------------------------------------------------------
void HistoryLog::killMonster(CUser& user, int monsterId, const MSG_MONSTER_DIE& msg)
{
    user.m_historyLog.KillMonster(monsterId,
                                  msg.m_killValue1,
                                  msg.m_killValue2,
                                  msg.m_killPairA24,
                                  msg.m_killPairA26,
                                  msg, (char)0, false, 0, 0,
                                  (eExpAddReason)1);
}

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

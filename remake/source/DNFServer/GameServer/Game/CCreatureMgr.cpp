// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureMgr / CCreature（S5 语义对齐）。
// 布局：CInventory.h 中 user_creature::CCreatureMgr（ORIG 尺寸 0x4c）、
//       CGameManager.h 中 user_creature::CCreature（镜像尺寸 0x100）。
// 报告依据 docs/class_func_reports/user_creature__CCreatureMgr/ 与
// user_creature__CCreature/。符号 T/W。
// 本批（SendCreatureItemList）：新建 user_creature::CPacketRespondent
// （CPacketRespondent.h/.cpp，PacketGuard 派生，ORIG W 全套）并接入：
//   SendCreatureItemList / CCreature::SetState 复活包链 / CCreature::UseFeed +
//   CCreatureMgr::UseFeed(CDBMsgSender::UseFeedCreature) / GetCreatureItemId /
//   GetCreatureName / isMaxLevel / vaildItemExp（CCreatureScript 已建模）。
// 本批（CreatureExperience）：实现 CCreature::GainExp 全链——GainExpCreature/
//   GainItemExpCreature/CalculateExp/GetExpLevel/GetAddExp/IsAvailableArtifact/
//   SetItemId/GetCreatureId(int)/Evolute/SelectEvolute，及 CCreatureMgr::GetArtifact、
//   CDBMsgSender::EvoluteCreature（ORIG 地址见各函数注释；字段全部具名访问）。
// CDBMsgSender（ORIG user_creature 域）由本 TU 提供最小真实实现。
// ============================================================================
#include <string.h>

#include <map>
#include <algorithm>   // std::swap<Inven_Item>（ORIG _ZSt4swapI10Inven_ItemEvRT_S2_）

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CCreatureScriptMgr.h"  // user_creature::GetInstanceCreatureScriptMgr / FindCreatureScript
#include "CPacketRespondent.h"
#include "CCreatureScript.h"   // user_creature::CCreatureScript 访问器（isMaxLevel/vaildItemExp）
#include "TimerQueue.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "GameTypes.h"

// SetEggItem 依赖的随机数（定义于 DNFFunctionLib.cpp，全局 T _Z11get_rand_inti）。
int get_rand_int(int range);

namespace user_creature
{

// ============================================================================
// CCreature（ORIG 0x833705e ctor / 0x83370b0 Reset）
// ============================================================================

// ORIG _ZN13user_creature8CStomach15SetStomachValueEf W 0x833eef6：
// 仅写 +4（m_value），float 位拷贝。
void CStomach::SetStomachValue(float value)
{
    m_value = value;
}

// ORIG _ZN13user_creature8CStomach15GetStomachValueENS_14CREATURE_STATEEb T 0x8336cfc。
// 非成长宠恒 100；state==1（战斗）按 -60/秒消耗，state 2..3 按 +360/秒恢复，
// 上限 100；每次调用后刷新 m_lastTime。常量取自 .rodata 0x8c2f450/54/58。
float CStomach::GetStomachValue(CREATURE_STATE state, bool growFlag)
{
    if (!growFlag)
        return 100.0f;

    int curTime = GlobalData::s_systemTime_.getCurSec();
    float delta = (float)(curTime - m_lastTime);
    int s = (int)state;
    if (s == 1)
    {
        m_value += delta / -60.0f;
        if (!(m_value > 0.0f))
            m_value = 0.0f;
    }
    else if (s > 1 && s <= 3)
    {
        m_value += delta / 360.0f;
        if (m_value > 100.0f)
            m_value = 100.0f;
    }
    m_lastTime = curTime;
    return m_value;
}

// ORIG _ZN13user_creature8CStomach14IncrementValueEf T 0x08336e66：
// m_value += amount；超过 100.0f 钳回 100.0f（0x42c80000）；返回新值。
float CStomach::IncrementValue(float amount)
{
    m_value += amount;
    if (100.0f < m_value)
        m_value = 100.0f;
    return m_value;
}

// ORIG Reset @ 0x83370b0（逐字段转写）。
void CCreature::Reset()
{
    m_creatureItemType = 1;            // +0x08 = 1（列表包要求 type==1）
    m_uid = 0;                         // +0x0c
    m_slotNo = 0;                      // +0x10
    m_state = 2;                       // +0x3c = 2
    m_saveFlag = 0;                    // +0x40 byte
    m_script = 0;                      // +0x44
    m_creatureId = 0;                  // +0x48
    m_level = 1;                       // +0x2c = 1
    m_exp = 0;                         // +0x30
    m_noCharge = 0;                    // +0x14 byte
    m_isGrow = 1;                      // +0x4c byte
    m_field4d = 5;                     // +0x4d byte
    memset(m_name, 0, 0xd);            // +0x1c memset 13B
    m_stomach.SetStomachValue(0.0f);   // CStomach::SetStomachValue(this+0x34, 0.0)
}

// ORIG CCreature @ 0x833705e：基类构造 → vptr → CStomach/COverSkillTime → Reset()。
// 推断：本镜像无虚表；基类与子对象按零初始化后统一 Reset。
CCreature::CCreature()
{
    memset(this, 0, sizeof(*this));
    m_overSkill.m_time = 0;
    Reset();
}

CCreature::~CCreature()
{
}

// ORIG _ZN13user_creature9CCreature8GetStateEv W 0x833f064：返回 +0x3c。
int CCreature::GetState()
{
    return m_state;
}

// ORIG _ZN13user_creature9CCreature13IsDieCreatureEv T 0x8338478：
// GetStomachValue(m_state, m_isGrow) <= 1.0f。
bool CCreature::IsDieCreature()
{
    return m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow != 0) <= 1.0f;
}

// ORIG _ZN13user_creature9CCreature8SetStateEP5CUserNS_14CREATURE_STATEE T 0x0833715a
// （docs/class_func_reports/user_creature__CCreature/SetState.md）。
void CCreature::SetState(CUser* user, CREATURE_STATE state)
{
    float stomachValue = m_stomach.GetStomachValue(state, m_isGrow != 0);
    if (m_state == (int)state || (int)state != 1 || m_isGrow == 0)
    {
        // 进入 state 3 且非同值切换：死亡宠复活流程（ORIG 0x08337269 起）。
        if (m_state != (int)state && (int)state == 3 && m_isGrow != 0)
        {
            if (IsDieCreature())
            {
                CPacketRespondent pkt(user, 0, 0);
                pkt.MakeNotipacketRevivalCreature();
                pkt.SendAll();
                m_stomach.SetStomachValue(1.0f);
                pkt.clear();  // InterfacePacketBuf::clear（复用包对象重填状态包）
                pkt.MakeNotipacketCreatureState(m_uid, 1);
                pkt.Send();
            }
        }
    }
    else
    {
        // 切入战斗态（state 1）：按 60 秒 × 饱食度系数插入复活定时器
        // （常量 .rodata 0x8c2f460 = double 60.0；TIMER_MESSAGE 0x5c）。
        int sec = (int)(60.0 * stomachValue);
        int characNo = user->getCurCharacNo();
        int uniqueId = user->get_unique_id();
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                    (TIMER_MESSAGE)0x5c, sec, uniqueId,
                                    (unsigned int)characNo);
    }
    m_state = (int)state;
    m_saveFlag = 1;
}

// ORIG isMaxLevel @ 0x833756e：m_script==0 → false；
// 否则 GetMaxLevel(m_script) <= m_level（setge）。
bool CCreature::isMaxLevel()
{
    if (m_script == 0)
        return false;
    return m_level >= m_script->GetMaxLevel();
}

// ORIG vaildItemExp @ 0x83378aa：state==2 拒绝；m_isGrow!=0 拒绝；
// 否则委托 CCreatureScript::validItemExp(m_script, itemIdx)
// （ORIG 不判 m_script 空，忠实转写）。
bool CCreature::vaildItemExp(int itemIdx)
{
    if (m_state == 2)
        return false;
    if (m_isGrow != 0)
        return false;
    return m_script->validItemExp(itemIdx);
}
// ORIG GainExp @ 0x83378fc：路由入口（reports/user_creature__CCreature/GainExp.md）。
// m_script==0 → 0；选择进化等级有效（!=-1）且 m_level>=它 → 0；
// m_level<GetMaxLevel 时按 m_isGrow 分流：==1 走 GainExpCreature，否则 GainItemExpCreature；
// 达到 MaxLevel → 0。
bool CCreature::GainExp(int exp, CUser* user)
{
    if (m_script == 0)
        return false;
    int selectEvolutionLevel = m_script->GetTypeSelectEvolutionLevel();
    if (selectEvolutionLevel != -1 && m_level >= selectEvolutionLevel)
        return false;
    if (m_level < m_script->GetMaxLevel())
    {
        if (m_isGrow == 1)
            return GainExpCreature(exp, user);
        return GainItemExpCreature(exp, user);
    }
    return false;
}

// ============================================================================
// GainExp 依赖链（CreatureExperience 批次；逐函数按 reports/user_creature__CCreature
// 汇编转写。字段全部具名访问，无裸偏移；exp 打包语义：低字节=m_field4d（A/B 型
// 平衡值），高 16 位=成长进度计数。）
// ============================================================================

// ORIG _ZN13user_creature9CCreature13GetCreatureIdEi T 0x833769c：
// find_item(itemIdx) 空/堆叠 → -1；否则 m_creatureId = CEquipItem::get_creature_species()
// 并返回它（GetCreatureId.md）。
int CCreature::GetCreatureId(int itemId)
{
    CItem* found = G_CDataManager()->find_item(itemId);
    if (found == 0)
        return -1;
    if (found->is_stackable())
        return -1;
    m_creatureId = static_cast<CEquipItem*>(found)->get_creature_species();
    return m_creatureId;
}

// ORIG _ZN13user_creature9CCreature9SetItemIdEi T 0x83375ae（SetItemId.md）：
// 写 m_itemId → GetCreatureId(itemIdx) 解析种族 → FindCreatureScript 刷新 m_script；
// 无脚本打 trace 返回；IsItemGrowCreature(script) 真→m_isGrow=0、假→m_isGrow=1
// （注意与字段名直觉相反）；非成长宠且 exp==0 时 exp 低字节与 m_field4d 同置 5。
void CCreature::SetItemId(int itemId)
{
    m_itemId = itemId;                       // CCreatureItem::SetItemId（+0x04）
    int creatureId = GetCreatureId(m_itemId);
    m_script = GetInstanceCreatureScriptMgr()->FindCreatureScript(creatureId);
    if (m_script == 0)
    {
        cMyTrace trace("void user_creature::CCreature::SetItemId(int)", 0x56c, 5);
        trace("cannot find  creature script memory\n");
        return;
    }
    if (m_script->IsItemGrowCreature())
        m_isGrow = 0;
    else
        m_isGrow = 1;
    if (m_isGrow != 1 && m_exp == 0)
    {
        m_exp = (m_exp & ~0xFF) | 0x05;      // ORIG movb $0x5 到 +0x30 低字节
        m_field4d = 5;
    }
}

// ORIG _ZN13user_creature9CCreature19IsAvailableArtifactEi T 0x83376f6：
// m_script==0 → false；否则委托 CCreatureScript::IsAvailableArtifactSlot(slot)。
bool CCreature::IsAvailableArtifact(int slot)
{
    if (m_script == 0)
        return false;
    return m_script->IsAvailableArtifactSlot(slot) != 0;
}

// ORIG _ZN13user_creature9CCreature11GetExpLevelEi T 0x83374d2（GetExpLevel.md）：
// 成长宠（m_isGrow==1）：G_CDataManager()->get_creature_exp_level(exp)；
// 非成长宠：exp / GetGrowItemCountforLevelUp()（0 视作 25）+ 1；
// 有脚本且超 GetMaxLevel 则钳到 GetMaxLevel。
int CCreature::GetExpLevel(int exp)
{
    int level;
    if ((m_isGrow ^ 1) == 0)                 // m_isGrow==1 走表查询
        level = G_CDataManager()->get_creature_exp_level(exp);
    else
    {
        int count = m_script->GetGrowItemCountforLevelUp();
        if (count == 0)
            count = 0x19;
        level = exp / count + 1;
    }
    if (m_script != 0)
    {
        int maxLevel = m_script->GetMaxLevel();
        if (maxLevel < level)
            level = m_script->GetMaxLevel();
    }
    return level;
}

// ORIG _ZN13user_creature9CCreature9GetAddExpEiP5CUser T 0x8337724（GetAddExp.md）：
// 槽位可用时取当前角色背包内 CCreatureMgr（ORIG invenW+0x6e8，镜像具名成员）
// 的 GetArtifact(slot)；神器存在且（最低等级为 0 或 <=m_level）返回其经验量，
// 否则 0.0f。
float CCreature::GetAddExp(int artifactSlot, CUser* user)
{
    float result = 0.0f;
    if (IsAvailableArtifact(artifactSlot))
    {
        CInventory* inven = user->getCurCharacInvenW();
        CArtifact* artifact =
            inven->GetCreatureMgrW()->GetArtifact(artifactSlot);
        if (artifact != 0)
        {
            int minLevel = artifact->GetCreatureMinimumLevel();
            if (minLevel == 0 || minLevel <= m_level)
                result = artifact->GetExperienceAmount();
        }
    }
    return result;
}

// ORIG _ZN13user_creature9CCreature12CalculateExpEiRbP5CUser T 0x83377b6
// （CalculateExp.md；除数 .rodata 0x8c2f450 = 100.0f 实证）：
// gained = exp*(GetAddExp(0x17)+GetAddExp(0x18)+GetAddExp(0x19))/100 截断取整；
// m_exp += gained + exp；新等级 = GetExpLevel(m_exp)，高于当前则置 levelUp。
void CCreature::CalculateExp(int exp, bool& levelUp, CUser* user)
{
    float addRate = 0.0f;
    addRate += GetAddExp(0x17, user);
    addRate += GetAddExp(0x18, user);
    addRate += GetAddExp(0x19, user);
    int gain = (int)((float)exp * addRate / 100.0f);   // fistpl 截断
    gain += exp;
    m_exp += gain;
    int newLevel = GetExpLevel(m_exp);
    if (m_level < newLevel)
        levelUp = true;
    m_level = newLevel;
}

// ORIG _ZN13user_creature9CCreature7EvoluteEP5CUser T 0x83381ea（Evolute.md）：
// 无脚本/无进化目标/找不到目标脚本/0x16 槽空 → false；
// SetItemId(目标脚本 itemIdx)、槽物写新 itemIdx，CDBMsgSender(0x8a).EvoluteCreature
// 落库失败打 trace 返回 false；成功后 Send + 进化通知包（SendAccordingToPlace）
// + SendUpdateItemList(target=1, space=3, slot=0x16)。
bool CCreature::Evolute(CUser* user)
{
    if (m_script == 0)
    {
        cMyTrace trace("bool user_creature::CCreature::Evolute(CUser*)", 0x743, 5);
        trace("cannot evolute because it haven't creature script uid(%d), "
              "creature id(%d) \n", user->get_unique_id() & 0xffff, m_itemId);
        return false;
    }
    int evolutionCreatureId = m_script->GetEvolutionCreatureId();
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    SetItemId(nextScript->GetItemId());
    Inven_Item* slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = m_itemId;          // *(int*)(ref+2) = 新宠物物品 idx
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(m_uid, m_itemId))   // ORIG CCreatureItem::GetUid/GetItemId（镜像同址具名字段）
    {
        cMyTrace trace("bool user_creature::CCreature::Evolute(CUser*)", 0x763, 5);
        trace("cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);       // 析构按 finalize 标志自动补发
    pkt.MakeNotipacketEvoluteCreature(evolutionCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

// ORIG _ZN13user_creature9CCreature13SelectEvoluteEP5CUser T 0x8337e48
// （SelectEvolute.md）：与 Evolute 同构，差异仅进化目标选取——
// m_field4d<=4 取 GET_BTypeCreature，否则 GET_ATypeCreature（==-1 拒绝）。
bool CCreature::SelectEvolute(CUser* user)
{
    if (m_script == 0)
    {
        cMyTrace trace("bool user_creature::CCreature::SelectEvolute(CUser*)",
                       0x6a2, 5);
        trace("cannot selectevolute because it haven't creature script uid(%d), "
              "creature id(%d) \n", user->get_unique_id() & 0xffff, m_itemId);
        return false;
    }
    int targetCreatureId;
    if (m_field4d < 5)
        targetCreatureId = m_script->GET_BTypeCreature();
    else
        targetCreatureId = m_script->GET_ATypeCreature();
    if (targetCreatureId == -1)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(targetCreatureId);
    if (nextScript == 0)
        return false;
    SetItemId(nextScript->GetItemId());
    Inven_Item* slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = m_itemId;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(m_uid, m_itemId))   // ORIG CCreatureItem::GetUid/GetItemId（镜像同址具名字段）
    {
        cMyTrace trace("bool user_creature::CCreature::SelectEvolute(CUser*)",
                       0x6c9, 5);
        trace("cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(targetCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

// ORIG _ZN13user_creature9CCreature15GainExpCreatureEiP5CUser T 0x8337bbc
// （GainExpCreature.md）：仅成长宠（m_isGrow==1）；饱食度截断 >0 才继续；
// CalculateExp 结算经验；未满级或有升级 → GainExp 通知包（析构补发）；
// 升级且可普通进化则 Evolute（失败整体返 0）；最后置 m_saveFlag=1 返回 1。
bool CCreature::GainExpCreature(int exp, CUser* user)
{
    bool levelUp = false;
    if (m_isGrow != 1)
        return false;
    int stomachValue = (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                                      m_isGrow != 0);
    if (!(stomachValue > 0))
        return false;
    CalculateExp(exp, levelUp, user);
    bool notify = (m_level < m_script->GetMaxLevel()) || levelUp;
    if (notify)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    bool canEvolute = levelUp && IsAbleEvolute(user) != 0;
    if (canEvolute && !Evolute(user))
        return false;
    m_saveFlag = 1;
    return true;
}

// ORIG _ZN13user_creature9CCreature19GainItemExpCreatureEiP5CUser T 0x83379c0
// （GainItemExpCreature.md）：仅非成长宠（m_isGrow==0）。A 型经验物品使
// m_field4d+1（钳 10）、B 型使其 -1（钳 0）；随后把 m_exp 低字节改写为
// m_field4d 并令高 16 位进度 +1（16 位回绕），以新进度查 GetExpLevel 定级；
// 通知包条件同成长宠；升级且可选择性进化则 SelectEvolute（失败返 0）；
// 最后置 m_saveFlag=1 返回 1。
bool CCreature::GainItemExpCreature(int exp, CUser* user)
{
    bool levelUp = false;
    if (m_isGrow != 0)
        return false;
    if (m_script->validItem_AType(exp))
    {
        int value = (int)m_field4d + 1;
        if (value > 10)
            value = 10;
        m_field4d = (unsigned char)value;
    }
    if (m_script->validItem_BType(exp))
    {
        int value = (int)m_field4d - 1;
        if (value < 0)
            value = 0;
        m_field4d = (unsigned char)value;
    }
    unsigned int packed = (unsigned int)m_exp;
    packed = (packed & 0xFFFFFF00u) | (unsigned int)m_field4d;   // 低字节 ← m_field4d
    unsigned short progress = (unsigned short)(packed >> 16) + 1; // 高 16 位进度 +1
    packed = (packed & 0xFFFFu) | ((unsigned int)progress << 16);
    m_exp = (int)packed;
    int newLevel = GetExpLevel((int)(short)progress);             // cwtl 符号扩展
    if (m_level < newLevel)
        levelUp = true;
    m_level = newLevel;
    bool notify = (m_level < m_script->GetMaxLevel()) || levelUp;
    if (notify)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    bool canEvolute = levelUp && IsAbleSelectEvolute(user) != 0;
    if (canEvolute && !SelectEvolute(user))
        return false;
    m_saveFlag = 1;
    return true;
}

// ============================================================================
// ORIG W 访问器（0x833f04c..0x833f0e6；本 TU 提供强定义）
// ============================================================================

// ORIG IsSave @ 0x833f04c：movzbl +0x40。
bool CCreature::IsSave()
{
    return m_saveFlag != 0;
}

// ORIG GetName @ 0x833f058：返回 this+0x1c。
const char* CCreature::GetName()
{
    return m_name;
}

// ORIG GetStomach @ 0x083384ee：(int)CStomach::GetStomachValue(+0x3c,+0x4c)
// （fnstcw/fldcw RC=11 截断，即 C++ float→int 截断转换）。
int CCreature::GetStomach()
{
    return (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                          m_isGrow != 0);
}

// ORIG GetExp @ 0x833f08e：+0x30。
int CCreature::GetExp()
{
    return m_exp;
}

// ORIG GetLevel @ 0x833f09a：+0x2c。
int CCreature::GetLevel()
{
    return m_level;
}

// ORIG IsNoCharge @ 0x833f0a6：movzbl +0x14。
bool CCreature::IsNoCharge()
{
    return m_noCharge != 0;
}

// ORIG SetNoCharge @ 0x833f0b2：写 +0x14 字节。
void CCreature::SetNoCharge(bool flag)
{
    m_noCharge = flag ? 1 : 0;
}

// ORIG IsGrowCreature @ 0x833f0ca：movzbl +0x4c 后 xor $1 —— 即 +0x4c==0 返回
// true、==1 返回 false（与 Reset 初值相反；GainExp/UseFeed/vaildItemExp 用裸字段判断，
// 本访问器供包序列化/查询路径使用，语义以汇编为准）。
bool CCreature::IsGrowCreature()
{
    return (m_isGrow ^ 1) != 0;
}

// ORIG GetCreatureId @ 0x833f0da：+0x48。
int CCreature::GetCreatureId()
{
    return m_creatureId;
}

// ORIG GetOverSkillTime @ 0x833f0e6：返回 this+0x50。
COverSkillTime* CCreature::GetOverSkillTime()
{
    return &m_overSkill;
}

// ORIG UseFeed(CUser*, int&) T @ 0x083373d4：
// 仅 +0x4c==1 可喂；死亡则 MakeNotipacketRevivalCreature + SendAll；
// IncrementValue(30.0f) 截断写入 out；MakeNotipacketCreatureState(uid,out)
// 后 Send()，返回其值。
bool CCreature::UseFeed(CUser* user, int& stomachValue)
{
    if (m_isGrow != 1)
        return false;
    CPacketRespondent pkt(user, 0, 0);
    if (IsDieCreature())
    {
        pkt.MakeNotipacketRevivalCreature();
        pkt.SendAll();
    }
    stomachValue = (int)m_stomach.IncrementValue(30.0f);
    int value = stomachValue;
    pkt.MakeNotipacketCreatureState(m_uid, value);
    return pkt.Send() != 0;
}

// ============================================================================
// CCreatureMgr
// ============================================================================

// ORIG Reset @ 0x833a45a。
void CCreatureMgr::Reset()
{
    m_equippedCreature = 0;            // +0x18
    m_user = 0;                        // +0x1c
    m_tempExpireDelta = -1;            // +0x2c = -1
    if (!m_field30.empty())            // +0x30 map clear
        m_field30.clear();
    if (!m_creatureItems.empty())      // this+0x00 map<int,CCreatorItem*>
    {
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); ++it)
        {
            CCreatureItem* creatureItem = it->second;
            if (creatureItem != 0)
            {
                // BLOCKED: GetInstanceCreatureItemFactory()/FreeCreatureItem 未建模
                //（原实现失败时 LogManager::logFormat(1,"Creature.cpp",
                // "...Reset()",0xb50,"FreeCreatureItem() error")）。
            }
        }
        m_creatureItems.clear();
    }
    for (int i = 0; i < 3; ++i)        // CArtifact::Reset(this+(i+8)*4)
        m_artifacts[i].Reset();
    m_isMaxLevelGrowCreature = 1;      // +0x48 byte = 1
}

void CCreatureMgr::SetUser(CUser* user)
{
    m_user = user;
}

// ORIG UseItem @ 0x833a5bc：feed 判定用 Inven_Item+2（int，即 m_addInfo/item idx）。
int CCreatureMgr::UseItem(Inven_Item* item, int slot)
{
    (void)slot;
    if (item->m_addInfo == 0x18 && !UseFeed())
        return 0x85;
    if (m_equippedCreature != 0 &&
        m_equippedCreature->vaildItemExp(item->m_addInfo) &&
        !GainExp(item->m_addInfo))
        return 0x11;
    return 0;
}

// ORIG CCreatureMgr::UseFeed T @ 0x08339dba（UseFeed.md）：
// 装备宠为空 → 0；CCreature::UseFeed(m_user, out) 失败 → 0；
// 成功后 CDBMsgSender(0x8a, m_user) 发 UseFeedCreature(uid, out)：
// 写库失败打 cMyTrace("...UseFeed()",0xab5,5) 返回 0；成功 sender.Send() 返回 1。
bool CCreatureMgr::UseFeed()
{
    if (m_equippedCreature == 0)
        return false;
    int stomachValue = 0;
    if (!m_equippedCreature->UseFeed(m_user, stomachValue))
        return false;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // ORIG 以 CCreatureItem::GetUid(equipped) 取 uid（镜像同址转换）。
    int uid = reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid();
    if (!sender.UseFeedCreature(uid, stomachValue))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::UseFeed()", 0xab5, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    return true;
}

// ORIG GainExp @ 0x8339b06。
bool CCreatureMgr::GainExp(int exp)
{
    if (m_equippedCreature == 0)
        return false;
    if (!m_equippedCreature->GainExp(exp, m_user))
        return false;
    if (m_equippedCreature->IsGrowCreature())
        m_isMaxLevelGrowCreature = m_equippedCreature->isMaxLevel() ? 1 : 0;
    return true;
}

bool CCreatureMgr::IsEquippedCreature() const
{
    return m_equippedCreature != 0;
}

// ORIG IsGrowCreature_Equipped_Creature @ 0x833debc。
bool CCreatureMgr::IsGrowCreature_Equipped_Creature() const
{
    if (m_equippedCreature == 0)
        return false;
    return m_equippedCreature->IsGrowCreature();
}

// ORIG setTempExpireTime W 0x833f10c：m_tempExpireDelta(+0x2c) = time。
void CCreatureMgr::setTempExpireTime(int time)
{
    m_tempExpireDelta = time;
}
// ORIG CCreatureMgr::InsertCreatureItem @ 0x833b204（InsertCreatureItem.md）：
// 槽位/类型过滤后 CDBMsgSender(0x87, m_user)，size=m_creatureItems.size()，
// sender.InsertCreatureItem(m_user,item,slot,reason,size,days,expireTick)：
// 成功 → sender.Send() + 到期时间（days==0 且 expireTick!=0 取
// expireTick-GetDateTimeTick()，否则 days*0x15180）写 m_tempExpireDelta 返回 1；
// 失败 → cMyTrace("...InsertCreatureItem(...)",0xcaf,5) 返回 0。
bool CCreatureMgr::InsertCreatureItem(Inven_Item* item, int slotNo, int reason,
                                      int days, int expireTick)
{
    if (!IsCreatureEquipmentScope(slotNo))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x87, m_user);
    int mapSize = (int)m_creatureItems.size();
    if (!sender.InsertCreatureItem(m_user, item, slotNo, reason, mapSize,
                                   days, expireTick))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::InsertCreatureItem("
            "Inven_Item*, int, int, int, int)", 0xcaf, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    int tempTime = 0;
    if (days == 0 && expireTick != 0)
    {
        tempTime = OS_API::GetDateTimeTick();
        tempTime = expireTick - tempTime;
    }
    else
    {
        tempTime = days * 0x15180;
    }
    setTempExpireTime(tempTime);
    return true;
}

// ORIG SendCreatureItemList @ 0x833bdd2（41 指令，SendCreatureItemList.md）：
// CPacketRespondent(m_user,0,0) → MakeNotipacketCreatureItemList(&m_creatureItems,-1)
// → Send()（透传 CUser::Send 返回值）→ 析构。
int CCreatureMgr::SendCreatureItemList() const
{
    CPacketRespondent pkt(m_user, 0, 0);
    pkt.MakeNotipacketCreatureItemList(&m_creatureItems, -1);
    return pkt.Send();
}

}  // namespace user_creature

namespace user_creature
{

// ---- 简单访问器（ORIG 证据见各注释；宠物侧字段未建模部分显式标注）----
// ORIG GetCreatureItemId @ 0x833a638：m_equippedCreature 为空返回 0，
// 否则 CCreatureItem::GetItemId（W 0x833efa4，镜像同址转换调用）。
int CCreatureMgr::GetCreatureItemId() const
{
    if (m_equippedCreature == 0)
        return 0;
    return reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId();
}

// ORIG T 0x833b80c：把宠物区（+0x12880 memset 0x1ed8，计数 +0x1287c）与蛋区
// （m_field30 map，计数 +0x15340）填入 SIG_LOAD_INVENTORY。条目 = 0x38 字节；
// 首字段 m_itemId、+0x28 为 expireTime。宠物序列化方法未建模，写入 id+expire。
int CCreatureMgr::GetCreatureItem(SIG_LOAD_INVENTORY* sig) const
{
    memset((char*)sig + 0x12880, 0, 0x1ed8);
    int count = 0;
    for (std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.begin();
         it != m_creatureItems.end() && count <= 0x8c; ++it, ++count)
    {
        CCreatureItem* item = it->second;
        char* dst = (char*)sig + count * 0x38 + 0x12880;
        *(int*)(dst + 0x00) = item->m_itemId;
        *(int*)(dst + 0x28) = item->m_expireTime;
    }
    *(int*)((char*)sig + 0x1287c) = count;

    int eggCount = 0;
    for (std::map<int, int>::const_iterator it = m_field30.begin();
         it != m_field30.end() && eggCount <= 0x13; ++it, ++eggCount)
    {
        char* dst = (char*)sig + (eggCount + 0x2a68) * 8;
        *(int*)(dst + 4) = it->first;
        *(int*)(dst + 8) = it->second;
    }
    *(int*)((char*)sig + 0x15340) = eggCount;
    return 1;
}

// ORIG GetCreatureName @ 0x833a688：空返回静态空串（DAT_08c2d760），
// 否则 CCreature::GetName()。
const char* CCreatureMgr::GetCreatureName() const
{
    if (m_equippedCreature == 0)
        return "";  // ORIG &DAT_08c2d760 空串
    return m_equippedCreature->GetName();
}

// ORIG IsDieCreature @ 0x833a660：空返回 1，否则委托 CCreature::IsDieCreature。
bool CCreatureMgr::IsDieCreature() const
{
    if (m_equippedCreature == 0)
        return true;
    return m_equippedCreature->IsDieCreature();
}

// ORIG GetEquipedCreatureLevel @ 0x833a6b0。
int CCreatureMgr::GetEquipedCreatureLevel() const
{
    if (m_equippedCreature == 0)
        return 0;
    // ORIG 委托 CCreature::GetLevel（读取 +0x2c 等级域，同 isMaxLevel 用途）。
    return m_equippedCreature->m_level;
}

// ORIG _ZN13user_creature12CCreatureMgr11GetArtifactEi T 0x833be56（GetArtifact.md）：
// idx=slot-0x17，仅上界检查（>3 → 0）；返回 &m_artifacts[idx]
// （ORIG this+(idx+8)*4 与镜像 m_artifacts 基址 +0x20 一致）。
CArtifact* CCreatureMgr::GetArtifact(int slot)
{
    int idx = slot - 0x17;
    if (idx > 3)
        return 0;
    return &m_artifacts[idx];
}


// ============================================================================
// 本批补齐（U3）：IsCreatureEquipmentScope / RecvMail / RegistTempPeriod
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr24IsCreatureEquipmentScopeEi T 0x0833926c。
bool CCreatureMgr::IsCreatureEquipmentScope(int slot)
{
    // slot ∈ [-∞,-1]∪[0x8c,∞) 且 slot != 0x16 → 非法；其余合法。
    if ((slot < 0 || 0x8b < slot) && slot != 0x16)
        return false;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16RegistTempPeriodEiii T 0x0833d7de。
// m_field30（map<int,int>）查 key=b；不存在则插入 (b,c) 并返回 true。
bool CCreatureMgr::RegistTempPeriod(int a, int b, int c)
{
    std::map<int, int>::const_iterator it = m_field30.find(b);
    if (it != m_field30.end())
        return false;
    m_field30.insert(std::make_pair(b, c));
    return true;
}

// ============================================================================
// GetRemainDate —— ORIG _ZNK13user_creature13CCreatureItem13GetRemainDateEi
//   W 0x0833f016（CCreatureItem）与
//   _ZNK13user_creature12CCreatureMgr13GetRemainDateEii T 0x0833d78e（CCreatureMgr）。
// 支持 CInventory::MakeItemPacket CREATURE/INVEN 分支
//   buf.put_int(m_creatureMgr.GetRemainDate(item->m_addInfo2, nowTick))。
// ============================================================================

// ORIG CCreatureItem::GetRemainDate(int) @0x833f016：
//   v = *(int*)(this+0x18)（m_expireTime）；
//   v <= 0 → 返回 v；
//   否则 r = v - nowSec；r == 0 → 返回 1；否则返回 r。
int CCreatureItem::GetRemainDate(int nowSec) const
{
    int v = m_expireTime;               // +0x18
    if (v <= 0)
        return v;
    int r = v - nowSec;
    if (r == 0)
        return 1;
    return r;
}

// ORIG CCreatureMgr::GetRemainDate(int, int) const @0x833d78e：
//   m_tempExpireDelta（+0x2c）!= -1 → 直接返回它；
//   否则 FindCreatureItem(uid)：空 → -1；否则 → item->GetRemainDate(nowSec)。
int CCreatureMgr::GetRemainDate(int uid, int nowSec) const
{
    if (m_tempExpireDelta != -1)        // +0x2c
        return m_tempExpireDelta;
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nowSec);
}

// ORIG _ZN13user_creature12CCreatureMgr8RecvMailEP10Inven_Itemii T 0x0833ab9a
// （docs/class_func_reports/user_creature__CCreatureMgr/RecvMail.md）。
bool CCreatureMgr::RecvMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    int characNo = m_user->getCurCharacNo();
    if (!sender.RecvMail(item, characNo, slot, reason, mapSize))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::RecvMail(Inven_Item*, int, int)",
            0xc09, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // WriteCreatureLog(INVEN_TYPE 3, item+0x02, item+0x07, 1, item+0x01, reason)
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        item->m_field1, (eItemAddReason)reason);
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr8SendMailEP10Inven_Itemii T 0x0833aa32
// （docs/class_func_reports/user_creature__CCreatureMgr/SendMail.md）。
// 逻辑：非宠物装备槽 → 1；item 类型非 5（CREATURE_EQUIP）→ 1；
// CDBMsgSender sender(0x8a,m_user) → mapSize=m_creatureItems.size()，
// characNo=m_user->getCurCharacNo()；SendMail 写库失败 → cMyTrace 并返回 0；
// 成功 → sender.Send() 后 DeleteCreatureItem(item->m_addInfo2, reason)。
bool CCreatureMgr::SendMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    int characNo = m_user->getCurCharacNo();
    if (!sender.SendMail(item, characNo, slot, reason, mapSize))
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::SendMail(Inven_Item*, int, int)",
            0xbed, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // ORIG：call DeleteCreatureItem(this, item->m_addInfo2, reason)，返回值作为本函数结果。
    // DeleteCreatureItem 内部经 UnregisterCreatureItem(reason) 注销宠物项。
    return DeleteCreatureItem(item->m_addInfo2, reason);
}

// ORIG _ZN13user_creature12CCreatureMgr18DeleteCreatureItemEii T 0x0833b606。
// 仅委托 UnregisterCreatureItem(param_2)（param_1 调用方传 item->m_addInfo2，本函数未使用）。
// 返回 void 的 ORIG 反编译，但 eax 继承 UnregisterCreatureItem 的返回值，
// 故以 bool 返回其值（匹配 SendMail 调用处的分支判断）。
bool CCreatureMgr::DeleteCreatureItem(int param_1, int param_2)
{
    return UnregisterCreatureItem(param_2);
}

// ORIG _ZN13user_creature12CCreatureMgr22UnregisterCreatureItemEi T 0x0833a854。
// 在 m_creatureItems 中查 uid：命中则经 CCreatureItemFactory 释放并 erase，返回 true；
// 未命中 → cMyTrace("UnregisterCreatureItem failed. uid(%d), key(%d)\n") 并返回 false。
// 注：CCreatureItemFactory::FreeCreatureItem 未建模（CCreature.cpp 未入 CMake），
// 与 Reset() 一致采用 BLOCKED 标注；erase 照常执行。
bool CCreatureMgr::UnregisterCreatureItem(int uid)
{
    std::map<int, CCreatureItem*>::iterator it = m_creatureItems.find(uid);
    if (it != m_creatureItems.end())
    {
        // BLOCKED: GetInstanceCreatureItemFactory()/FreeCreatureItem 未建模
        //（原实现 FreeCreatureItem(it->second) 释放宠物项后再 erase）。
        m_creatureItems.erase(it);
        return true;
    }
    cMyTrace trace(
        "bool user_creature::CCreatureMgr::UnregisterCreatureItem(int)",
        0xbc1, 5);
    trace("UnregisterCreatureItem failed. uid(%d), key(%d)\n",
          m_user->get_unique_id() & 0xffff, uid);
    return false;
}

// ============================================================================
// user_creature::CDBMsgSender（ORIG 域内类，本 TU 提供最小真实实现）
// ============================================================================

// ORIG C1 @ 0x833f8ae：清 m_stream 后委托 Init。
CDBMsgSender::CDBMsgSender(ENUM_INTERNALPACKET type, CUser* user)
    : m_stream(0), m_user(0)
{
    Init(type, user);
}

// ORIG Init @ 0x833f7a8：0x89 走 BigStreamPool，其余走 StreamPool；随后
// CStreamGuard(stream,false) 依次 << type、<< GetUID()。文件名/行号为池记录用。
bool CDBMsgSender::Init(ENUM_INTERNALPACKET type, CUser* user)
{
    if ((int)type == ENUM_INTERNALPACKET_0x89)
        m_stream = GlobalData::s_big_stream_pool->Acquire("creature_db_msg", 0xa7);
    else
        m_stream = GlobalData::s_stream_pool->Acquire("creature_db_msg", 0xa9);
    if (m_stream == 0)
        return false;
    m_user = user;
    {
        CStreamGuard guard(m_stream, false);
        // ORIG 经 operator*()（返回 this）后调用成员 operator<<。
        guard << (int)type;
        guard << (int)m_user->GetUID();
    }
    return true;
}

// ORIG D1/D2 @ 0x833f8d8：按 stream 的 poolType 归还对象池后置空。
CDBMsgSender::~CDBMsgSender()
{
    if (m_stream == 0)
        return;
    switch (m_stream->m_poolType)
    {
        case 1: GlobalData::s_small_stream_pool->Free(m_stream); break;
        case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
        default: GlobalData::s_stream_pool->Free(m_stream); break;
    }
    m_stream = 0;
}

// ORIG Send @ 0x833f950：投递 DB_Q 后释放持有并返回 true。
bool CDBMsgSender::Send()
{
    if (m_stream == 0)
        return false;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, m_stream);
    m_stream = 0;
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender8RecvMailEP10Inven_Itemiiii W 0x833ff34：
// GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 → setCopy → 逐字段填充。
bool CDBMsgSender::RecvMail(Inven_Item* item, int characNo, int slot,
                            int reason, int mapSize)
{
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x130;
    buf->m_field53 = 0;
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 4;
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender8SendMailEP10Inven_Itemiiii W 0x833fe8c：
// 与 RecvMail 同构，差异：m_hdr |0x100（or $0x1,%dh）、m_field53=1、m_field64=3、
// 不写 slot（+0x2c）。slot 形参在汇编中未使用。
bool CDBMsgSender::SendMail(Inven_Item* item, int characNo, int slot, int reason,
                            int mapSize)
{
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x100;                              // or $0x1,%dh
    buf->m_field53 = 1;                               // +0x53
    buf->m_field64 = 3;                               // +0x64
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender15UseFeedCreatureEii W 0x0833fc5a：
// m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
// m_hdr|=2；+0x30(m_addInfo2)=uid；+0x44(m_stomachValue)=饱食度；返回 1。
bool CDBMsgSender::UseFeedCreature(int uid, int stomachValue)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x2;
    buf->m_addInfo2 = uid;             // +0x30：UseFeedCreature 消息承载宠物 uid
    buf->m_stomachValue = stomachValue;  // +0x44
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender12UseKeepRightEii W 0x083401ec
// （objdump 逐条转写，SetKeepRightItem 落库用）：m_stream 空 → 0；
// GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 → m_hdr|=0x80；
// +0x30(m_addInfo2)=uid；+0x52(m_keepRight)=flag 低字节；返回 1。
bool CDBMsgSender::UseKeepRight(int uid, int flag)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x80;
    buf->m_addInfo2 = uid;               // +0x30：宠物 uid
    buf->m_keepRight = (unsigned char)flag;  // +0x52
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender15EvoluteCreatureEii W 0x833fd74
// （objdump 逐条转写）：m_stream 空 → 0；buf 清 0x74 后 m_hdr|=0x40；
// compat.m_itemIdx(+0x08)=新宠物物品 idx；m_addInfo2(+0x30)=uid；返回 1。
bool CDBMsgSender::EvoluteCreature(int uid, int itemIdx)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x40;
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->m_itemIdx = itemIdx;                      // buf+0x08（ORIG mov %edx,0x8(%eax)）
    buf->m_addInfo2 = uid;                            // +0x30
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender18InsertCreatureItemEP5CUserP10Inven_Itemiiiii
// W 0x0833faaa（objdump 逐条转写）：
//   m_stream 空 → 0；buf 清 0x74 后 setCopy(item) 到 +0x04；
//   m_slot(+0x2c)=slot；m_characNo(+0x48)=user->getCurCharacNo()；
//   m_stomachValue(+0x44)=100；m_reason(+0x51 byte)=reason；
//   m_mapSize(+0x68)=mapSize；m_days(+0x54)=days；m_expireTick(+0x58)=expireTick；
//   m_ceraOrderNo(+0x60)=user->get_cera_item_order_no()；
//   itemIdx=*(int*)(item+2)：find_item 为空或 is_stackable → 返回 0；
//   CEquipItem::get_sub_type()==0 → m_flag50(+0x50)=1 且 m_field64(+0x64)=6，
//   否则 m_flag50=0；返回 1。
bool CDBMsgSender::InsertCreatureItem(CUser* user, Inven_Item* item, int slot,
                                      int reason, int mapSize, int days,
                                      int expireTick)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // buf+0x04
    compat->setCopy(*item);
    buf->m_slot = slot;                               // +0x2c
    buf->m_characNo = user->getCurCharacNo();         // +0x48
    buf->m_stomachValue = 100;                        // +0x44（固定 100）
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_days = days;                               // +0x54
    buf->m_expireTick = expireTick;                   // +0x58
    buf->m_ceraOrderNo = (int)user->get_cera_item_order_no();  // +0x60

    int itemIdx = *(int*)((char*)item + 2);           // item+0x02（ORIG mov 0x2(%eax)）
    CItem* found = G_CDataManager()->find_item(itemIdx);
    if (found == 0)
        return false;
    if (found->is_stackable())
        return false;

    CEquipItem* equip = static_cast<CEquipItem*>(found);
    if (equip->get_sub_type() == 0)
    {
        buf->m_flag50 = 1;                            // +0x50
        buf->m_field64 = 6;                           // +0x64
    }
    else
    {
        buf->m_flag50 = 0;
    }
    return true;
}

// ============================================================================
// CCreatureItemConverter::SetEggItem —— ORIG 0x08336eae (T)
// _ZN13user_creature22CCreatureItemConverter10SetEggItemEP10Inven_Item
// 语义（反编译）：槽物 item 的 itemIdx = *(int*)(item+2)；
//   find_item(itemIdx) 为 0 → 返回 0；
//   物品 is_stackable → 返回 0；
//   否则按 CEquipItem::get_sub_type()：
//     ==1 → 仅置 item->m_field0 = 1；
//     ==2 → get_rand_int(m_randomTable.m_field0) 得随机权重，
//           GetRandomItemIndex(table, idx) 得目标 itemIdx，
//           写回 item->m_addInfo 并置 item->m_field0 = 1；
//   均返回 1。
// ============================================================================
int CCreatureItemConverter::SetEggItem(Inven_Item* item)
{
    if (item == 0)
        return 0;

    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return 0;
    if (found->is_stackable())
        return 0;

    CEquipItem* equip = static_cast<CEquipItem*>(found);
    int subType = equip->get_sub_type();
    if (subType == 1)
    {
        item->m_field0 = 1;
    }
    else if (subType == 2)
    {
        int range = equip->m_randomTable.m_field0;
        int randIdx = get_rand_int(range);
        int target = equip->m_randomTable.GetRandomItemIndex(randIdx);
        item->m_addInfo = target;
        item->m_field0 = 1;
    }
    return 1;
}


// ============================================================================
// 进化资格校验链（RestoreUserQuestStubs：Dispatcher_VerifyCreatureQuest 依赖）
// ============================================================================

// ORIG _ZNK13user_creature9CCreature13IsAbleEvoluteEP5CUser T 0x083380fc：
// script==0 / GetEvolutionLevel()==0 / m_level<进化等级 → 0；
// 无进化任务 → 1；有进化任务 → 给 user 补发任务列表包（get_quest_info）后 0。
int CCreature::IsAbleEvolute(CUser* user) const
{
    if (m_script == 0)
        return 0;
    int evolutionLevel = m_script->GetEvolutionLevel();
    if (evolutionLevel == 0)
        return 0;
    if (m_level < evolutionLevel)
        return 0;
    if (!m_script->IsExistEvolutionQuest())
        return 1;
    if (user != 0)
    {
        PacketGuard packet;
        user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&packet));
        user->Send(packet);
    }
    return 0;
}

// ORIG _ZN13user_creature9CCreature19IsAbleSelectEvoluteEP5CUser T 0x08337d58：
// 同 IsAbleEvolute，但等级门槛取 GetTypeSelectEvolutionLevel()（-1 视为不可）。
int CCreature::IsAbleSelectEvolute(CUser* user)
{
    if (m_script == 0)
        return 0;
    int selectEvolutionLevel = m_script->GetTypeSelectEvolutionLevel();
    if (selectEvolutionLevel == -1)
        return 0;
    if (m_level < selectEvolutionLevel)
        return 0;
    if (!m_script->IsExistEvolutionQuest())
        return 1;
    if (user != 0)
    {
        PacketGuard packet;
        user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&packet));
        user->Send(packet);
    }
    return 0;
}

// ORIG _ZNK13user_creature12CCreatureMgr26VerifyEquipedCreatureQuestEv
// T 0x0833d6fa：无装备宠物 → 0；成长宠走 IsAbleSelectEvolute，否则 IsAbleEvolute。
int CCreatureMgr::VerifyEquipedCreatureQuest() const
{
    if (m_equippedCreature == 0)
        return 0;
    if (m_equippedCreature->IsGrowCreature())
        return m_equippedCreature->IsAbleSelectEvolute(m_user);
    return m_equippedCreature->IsAbleEvolute(m_user);
}

// ============================================================================
// SwapCreatureItem 调用链（Prod_DB_Misc 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreatureMgr/ 与 user_creature__CCreature/
// 的 ## 汇编 + ## 反编译 C 转写，符号与 ORIG mangled 一致）
// ============================================================================

// ORIG _ZN13user_creature13CCreatureItem9SetSlotNoEi T 0x08337050：写 +0x10 槽号。
void CCreatureItem::SetSlotNo(int slotNo)
{
    m_slotNo = slotNo;
}

// ORIG _ZN13user_creature13CCreatureItem18GetCreatureItemTypeEv W 0x0833ef98
//（本批改为外联定义：SwapCreature 链按 ORIG 外联调用形态取类型）。
int CCreatureItem::GetCreatureItemType()
{
    return m_creatureItemType;
}

// ORIG _ZN13user_creature14COverSkillTime9checkTimeEv T 0x0833d65e：
// (float)(getCurSec()-m_time) ∈ (0.0, 20.0)（0x8c2f468 处常量 = 20.0f）→ 1。
bool COverSkillTime::checkTime()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    float diff = static_cast<float>(curSec - m_time);
    return diff > 0.0f && diff < 20.0f;
}

// ORIG _ZN13user_creature9CCreature5EquipEP5CUser T 0x08337330：
// 战斗中 → SetState(1)，否则 SetState(3)；恒返回 1。
bool CCreature::Equip(CUser* user)
{
    if (user->is_fighting())
        SetState(user, CREATURE_STATE_1);
    else
        SetState(user, CREATURE_STATE_3);
    return true;
}

// ORIG _ZN13user_creature9CCreature9DismantleEP5CUser T 0x08337382：
// SetState(2) 后恒返回 1。
bool CCreature::Dismantle(CUser* user)
{
    SetState(user, CREATURE_STATE_2);
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender16SwapCreatureItemEP5CUserP10Inven_ItemiS4_i
// W 0x0833fa0c（objdump 逐条转写）：m_stream 空 → 0；
// GetInBuffer<SIG_SWAP_CREATURE_ITEM> → memset 0x74；
// +0x04=slot1、+0x00=item1->get_add_info()、item1 空(+0x02==0) → +0x08=1；
// +0x10=slot2、+0x0c=item2->get_add_info()、item2 空 → +0x14=1；返回 1。
bool CDBMsgSender::SwapCreatureItem(CUser* user, Inven_Item* item1, int slot1,
                                    Inven_Item* item2, int slot2)
{
    (void)user;   // ORIG 未读该实参
    if (m_stream == 0)
        return false;
    SIG_SWAP_CREATURE_ITEM* buf = GetInBuffer<SIG_SWAP_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_slot1 = slot1;
    buf->m_itemIdx1 = item1->get_add_info();
    // ORIG 直接读 item+0x02（== isEmpty 的判据）判空
    if (item1->m_addInfo == 0)
        buf->m_empty1 = 1;
    buf->m_slot2 = slot2;
    buf->m_itemIdx2 = item2->get_add_info();
    if (item2->m_addInfo == 0)
        buf->m_empty2 = 1;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr19IsArtifactEquipSlotEi T 0x083393fc：
// 槽 0x17/0x18/0x19 为神器装备槽。
bool CCreatureMgr::IsArtifactEquipSlot(int slot)
{
    return slot == 0x17 || slot == 0x18 || slot == 0x19;
}

// ORIG _ZN13user_creature12CCreatureMgr11EquipArtifactEiP10Inven_Item T 0x083396c8：
// GetArtifact 为空 → 0；否则委托 CArtifact::EquipArtifact 并按其结果返回。
bool CCreatureMgr::EquipArtifact(int slot, Inven_Item* item)
{
    CArtifact* artifact = GetArtifact(slot);
    if (artifact == 0)
        return false;
    if (!artifact->EquipArtifact(item))
        return false;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr17DismantleArtifactEiP10Inven_Item T 0x08339718：
// GetArtifact 为空 → 0；否则 CArtifact::Reset 后返回 1。
bool CCreatureMgr::DismantleArtifact(int slot, Inven_Item* item)
{
    CArtifact* artifact = GetArtifact(slot);
    if (artifact == 0)
        return false;
    artifact->Reset();
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr5EquipEPNS_9CCreatureE T 0x083392f6：
// 仅种族类型 1 可装备；CCreature::Equip 成功后记录为已装备宠。
bool CCreatureMgr::Equip(CCreature* creature)
{
    // ORIG 经外联 GetCreatureItemType(W 0x833ef98) 取类型后 !=1 判定（无中间局部）
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Equip(m_user))
        return false;
    m_equippedCreature = creature;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr9DismantleEPNS_9CCreatureE T 0x0833934c：
// 种族类型 1 才可卸下；成功后清空已装备记录。
bool CCreatureMgr::Dismantle(CCreature* creature)
{
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Dismantle(m_user))
        return false;
    m_equippedCreature = 0;
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr17EquipCreatureItemEPNS_9CCreatureES2_
// T 0x083393a2：非空旧宠先 Dismantle，非空新宠再 Equip，任一失败返回 0。
bool CCreatureMgr::EquipCreatureItem(CCreature* dismantleItem,
                                     CCreature* equipItem)
{
    if (dismantleItem != 0)
    {
        if (!Dismantle(dismantleItem))
            return false;
    }
    if (equipItem != 0)
    {
        if (!Equip(equipItem))
            return false;
    }
    return true;
}

// ============================================================================
// CheckItem / SetKeepRightItem（Cerashop creature 商品购买预检链）
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr16SetKeepRightItemEP5CUserP10Inven_Item
// T 0x0833c00c（SetKeepRightItem.md）：遍历 m_creatureItems，对
// GetCreatureItemType()==1 且 IsNoCharge() 的宠物清免充标志并落库——
// CDBMsgSender((ENUM_INTERNALPACKET)0x8a, user).UseKeepRight(uid, flag)
// （flag 为清标志后重读的 IsNoCharge，恒 0）；失败打 cMyTrace
// ("...SetKeepRightItem(CUser*, Inven_Item*)",0xdf0,5) 返 0；成功 Send +
// CPacketRespondent(m_user,0,0).MakeNotipacketCreatureItemList(
// &m_creatureItems, slotNo)。循环尽 item->reset() 返 1。
bool CCreatureMgr::SetKeepRightItem(CUser* user, Inven_Item* item)
{
    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); ++it)
    {
        CCreatureItem* creatureItem = it->second;
        if (creatureItem == 0 || creatureItem->GetCreatureItemType() != 1)
            continue;

        // 同一对象按 CCreature 语义访问（ORIG 直呼 CCreature 访问器，
        // 单继承布局同址；镜像类型间 reinterpret_cast 保持同一符号）。
        CCreature* creature = reinterpret_cast<CCreature*>(creatureItem);
        if (creature->IsNoCharge())
        {

        creature->SetNoCharge(false);
        CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
        int noCharge = (int)(unsigned char)creature->IsNoCharge();
        int uid = creatureItem->GetUid();
        if (!sender.UseKeepRight(uid, noCharge))
        {
            cMyTrace trace(
                "bool user_creature::CCreatureMgr::SetKeepRightItem(CUser*, Inven_Item*)",
                0xdf0, 5);
            trace("cannot write db uid(%d)\n",
                  user->get_unique_id() & 0xffff);
            return false;
        }
        sender.Send();
        CPacketRespondent pkt(m_user, 0, 0);
        pkt.MakeNotipacketCreatureItemList(&m_creatureItems,
                                           creatureItem->GetSlotNo());
        }
    }
    item->reset();
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr9CheckItemEP5CUserP10Inven_ItemRb
// T 0x0833c240（CheckItem.md）：默认 outFlag=true；仅当物品为堆叠宠物蛋类
// （m_field1==7 ITEM_TYPE_CREATURE_STACK 且 m_addInfo==0x1a）时置 outFlag=
// false 并委托 SetKeepRightItem 清占用（其返回值即函数返回）；否则返 true。
bool CCreatureMgr::CheckItem(CUser* user, Inven_Item* item, bool& b)
{
    b = true;
    if (item->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_STACK &&
        item->m_addInfo == 0x1a)
    {
        b = false;
        return SetKeepRightItem(user, item);
    }
    return true;
}

// ORIG _ZNK13user_creature12CCreatureMgr12GetExtraGoldEi T 0x0833c31a：
// 无装备宠/已死亡 → 0；按宠物种族 id 给额外金币比例（*5 或 *3 再 /100）。
int CCreatureMgr::GetExtraGold(int gold) const
{
    if (m_equippedCreature == 0 || m_equippedCreature->IsDieCreature())
        return 0;
    switch (m_equippedCreature->GetCreatureId())
    {
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x40:
            return gold * 5 / 100;
        case 0x2a:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x3f:
            return gold * 3 / 100;
        default:
            return 0;
    }
}

// ORIG _ZNK13user_creature12CCreatureMgr21GetOverSkillExtraGoldEi T 0x0833c6f6：
// 装备宠存活且 itemId==0xf68a 且超技能计时有效时给 *5/100，否则 0。
int CCreatureMgr::GetOverSkillExtraGold(int gold) const
{
    if (m_equippedCreature != 0 && !m_equippedCreature->IsDieCreature() &&
        m_equippedCreature->m_itemId == 0xf68a &&
        m_equippedCreature->GetOverSkillTime()->checkTime())
    {
        return gold * 5 / 100;
    }
    return 0;
}

// ORIG _ZN13user_creature12CCreatureMgr12SwapCreatureEP10Inven_ItemiiS2_ii
// T 0x08339420：双物品含宠物装备(类型 5)时校验槽位/换装，随后交换槽号并落库。
bool CCreatureMgr::SwapCreature(Inven_Item* item1, int slot1, int type1,
                                Inven_Item* item2, int slot2, int type2)
{
    CCreatureItem* left = 0;
    CCreatureItem* right = 0;

    if (item1->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
        item2->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
    {
        left = FindCreatureItem(item1->get_add_info());
        right = FindCreatureItem(item2->get_add_info());

        if (!IsCreatureEquipmentScope(slot1))
        {
            unsigned short uidLow =
                static_cast<unsigned short>(m_user->get_unique_id());
            cMyTrace trace("bool user_creature::CCreatureMgr::SwapCreature("
                           "Inven_Item*, int, int, Inven_Item*, int, int)",
                           0x9a5, 5);
            trace("IsCreatureEquipmentScope() uid(%d) slot_left(%d)\n",
                  uidLow, slot1);
            return false;
        }
        if (!IsCreatureEquipmentScope(slot2))
        {
            unsigned short uidLow =
                static_cast<unsigned short>(m_user->get_unique_id());
            cMyTrace trace("bool user_creature::CCreatureMgr::SwapCreature("
                           "Inven_Item*, int, int, Inven_Item*, int, int)",
                           0x9ab, 5);
            trace("IsCreatureEquipmentScope() uid(%d) slot_right(%d)\n",
                  uidLow, slot2);
            return false;
        }

        // 目标侧为 0x16 号槽且来源计数 0 → 视作穿戴/脱下链
        // （FindCreatureItem 返回 CCreatureItem*；ORIG 形参为 CCreature*，镜像同址转换）
        if (slot1 == 0x16 && type1 == 0)
        {
            if (!EquipCreatureItem(reinterpret_cast<CCreature*>(left),
                                   reinterpret_cast<CCreature*>(right)))
                return false;
        }
        else if (slot2 == 0x16 && type2 == 0)
        {
            if (!EquipCreatureItem(reinterpret_cast<CCreature*>(right),
                                   reinterpret_cast<CCreature*>(left)))
                return false;
        }
    }

    // 来源侧计数为 0 时槽号 +0xd8（ORIG add $0xd8 实证）
    if (type2 == 0)
        slot2 += 0xd8;
    if (type1 == 0)
        slot1 += 0xd8;

    if (left != 0)
        left->SetSlotNo(slot2);
    if (right != 0)
        right->SetSlotNo(slot1);

    CDBMsgSender sender(static_cast<ENUM_INTERNALPACKET>(0x8c), m_user);
    bool ok = sender.SwapCreatureItem(m_user, item1, slot1, item2, slot2);
    if (ok)
        sender.Send();
    return ok;
}

// ORIG _ZN13user_creature12CCreatureMgr12SwapArtifactEP10Inven_ItemiiS2_ii
// T 0x08339752：任一侧计数为 0 时按神器槽位拆解/装配，随后清空源物品首字节。
bool CCreatureMgr::SwapArtifact(Inven_Item* item1, int slot1, int type1,
                                Inven_Item* item2, int slot2, int type2)
{
    if (type1 == 0 || type2 == 0)
    {
        int equipSlot = -1;              // 待装配（槽号，物品指针）
        Inven_Item* equipItem = 0;
        int dismantleSlot = -1;          // 待拆解（槽号，物品指针）
        Inven_Item* dismantleItem = 0;

        if (IsArtifactEquipSlot(slot1))
        {
            if (!item2->isEmpty())
            {
                equipSlot = slot1;
                equipItem = item2;
            }
            if (!item1->isEmpty())
            {
                dismantleSlot = slot1;
                dismantleItem = item1;
            }
        }
        if (IsArtifactEquipSlot(slot2))
        {
            if (!item1->isEmpty())
            {
                equipSlot = slot2;
                equipItem = item1;
            }
            if (!item2->isEmpty())
            {
                dismantleSlot = slot2;
                dismantleItem = item2;
            }
        }

        if (dismantleSlot != -1 && dismantleItem != 0)
        {
            if (!DismantleArtifact(dismantleSlot, dismantleItem))
                return false;
        }
        if (equipSlot != -1 && equipItem != 0)
        {
            if (!EquipArtifact(equipSlot, equipItem))
                return false;
            equipItem->m_field0 = 0;     // ORIG movb $0,(源物品)：仅清首字节
        }
    }
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16SwapCreatureItemEP10Inven_ItemiiS2_ii
// T 0x083399fa：按物品类型分流到 SwapCreature/SwapArtifact，最后交换两物品内容。
bool CCreatureMgr::SwapCreatureItem(Inven_Item* item1, int slot1, int type1,
                                    Inven_Item* item2, int slot2, int type2)
{
    if (item1->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
        item2->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
    {
        if (!SwapCreature(item1, slot1, type1, item2, slot2, type2))
            return false;
    }
    else if (item1->m_field1 == Inven_Item::ITEM_TYPE_ARTIFACT ||
             item2->m_field1 == Inven_Item::ITEM_TYPE_ARTIFACT)
    {
        if (!SwapArtifact(item1, slot1, type1, item2, slot2, type2))
            return false;
    }

    std::swap(*item1, *item2);           // ORIG _ZSt4swapI10Inven_ItemEvRT_S2_
    return true;
}

// ORIG _ZN13user_creature12CCreatureMgr16ResponseCreatureEv T 0x0833a6d8：
// 构造 CPacketRespondent(user,0,0) → 填充 ResponseCreature 通知包 →
// 以 (村, 区域) 广播（门区由 Broadcast 内部直发本人），透传其返回值。
int CCreatureMgr::ResponseCreature()
{
    CPacketRespondent respondent(m_user, 0, 0);
    respondent.MakeNotipacketResponseCreature();
    int area = m_user->get_area(false);
    int vill = (int)(char)m_user->getCurCharacVill();  // ORIG movsbl 符号扩展
    return (int)respondent.Broadcast(vill, area);
}

// ORIG _ZN13user_creature12CCreatureMgr20RegisterCreatureItemEPNS_13CCreatureItemEi
// T 0x0833a77e：uid 查重后插入 m_creatureItems；重复则 cMyTrace 报错返回 false。
bool CCreatureMgr::RegisterCreatureItem(CCreatureItem* item, int uid)
{
    if (FindCreatureItem(uid) != 0)
    {
        cMyTrace trace(
            "bool user_creature::CCreatureMgr::RegisterCreatureItem"
            "(user_creature::CCreatureItem*, int)", 0xbb4, 5);
        trace("RegisterCreatureItem failed. uid(%d), key(%d)\n",
              m_user->get_unique_id() & 0xffff, uid);
        return false;
    }
    m_creatureItems.insert(std::make_pair(uid, item));
    return true;
}

// ============================================================================
// CDBMsgSender::RenameCreatureItem @ 0x0833fbde（W，objdump 逐条转写）
//   m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
//   m_hdr|=0x200；+0x30(m_addInfo2)=uid；name 拷入 +0x04 m_item 兼容区（截 0x27）；
//   返回 1。hdr 位/name 位置依同族（UseFeedCreature|0x2 等）推断，无独立报告。
// ============================================================================
bool CDBMsgSender::RenameCreatureItem(int uid, const char* name)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x200;
    buf->m_addInfo2 = uid;                            // +0x30
    char* dst = reinterpret_cast<char*>(buf->m_item); // +0x04
    strncpy(dst, name != 0 ? name : "", 0x27);
    return true;
}

// ============================================================================
// CDBMsgSender::HatchCreature @ 0x0833fcc2（W，objdump 逐条转写）
//   m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
//   m_hdr|=0x400；+0x30(m_addInfo2)=uid；+0x50(m_flag50)=flag 低字节；
//   +0x08(m_item 兼容区 m_itemIdx)=itemIdx；返回 1。hdr 位推断同族。
// ============================================================================
bool CDBMsgSender::HatchCreature(int uid, int flag, int itemIdx)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    if (buf == 0)
        return false;
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x400;
    buf->m_addInfo2 = uid;                            // +0x30
    buf->m_flag50 = (unsigned char)flag;              // +0x50
    Inven_Item_Compatible* compat =
        (Inven_Item_Compatible*)(void*)buf->m_item;   // +0x04
    compat->m_itemIdx = itemIdx;                      // +0x08
    return true;
}

// ============================================================================
// RenameCreature @ 0x08339b6e（RenameCreature.md）
//   装备宠物非空 → use_item 消耗改名书（creatureIdx×1，slot 空间）→ 宠物改名 +
//   改名通知广播 → CDBMsgSender::RenameCreatureItem 落库 → 成功 Send+回执包，
//   失败 trace+SetError(2)。use_item 失败按返回错误码 SetError。
// ============================================================================
void CCreatureMgr::RenameCreature(const char* name, int creatureIdx, int slot)
{
    CPacketRespondent respondent(m_user, 1, 0x67);
    if (m_equippedCreature == 0)
    {
        respondent.SetError(4);
        return;
    }
    CInventory* inven = m_user->getCurCharacInvenW();
    int useRet = inven->use_item(creatureIdx, 1, slot);
    if (useRet != 0)
    {
        respondent.SetError(useRet);
        return;
    }
    m_equippedCreature->RenameCreature(name);
    int len = (int)strlen(name);
    respondent.MakeNotipacketRenameCreature(name, len);
    int area = m_user->get_area(false);
    int vill = (int)(char)m_user->getCurCharacVill();
    respondent.Broadcast(vill, area);
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int uid = reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid();
    bool ok = sender.RenameCreatureItem(uid, name);
    if (ok)
    {
        sender.Send();
        respondent.MakeCmdpacketRenameCreature(creatureIdx, slot);
    }
    else
    {
        cMyTrace trace(
            "void user_creature::CCreatureMgr::RenameCreature(const char*, int, int)",
            0xaa0, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
    }
}

}  // namespace user_creature

// ============================================================================
// Inven_Item_Compatible（全局域，ORIG 0x833f38e/0x833f418；U3Creature 批次实现）
// ============================================================================

// ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e。
void Inven_Item_Compatible::setCopy(const Inven_Item& item)
{
    m_field0 = item.m_field0;
    m_type = item.m_field1;
    m_itemIdx = item.m_addInfo;
    m_attr = item.GetItemAttr();
    m_addInfo2 = item.m_addInfo2;
    m_endurance = item.m_fieldb;
    m_fieldd = item.m_fieldd;
    const unsigned char* src = (const unsigned char*)&item.m_amp;
    memcpy(&m_amp0, src, 4);                    // ← item+0x11（4B）
    memcpy(&m_rand0, src + 4, 8);               // ← item+0x15/+0x19
    unsigned short tail;
    memcpy(&tail, src + 12, 2);                 // ← item+0x1d
    m_rand8 = tail;
}

// ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418。
void Inven_Item_Compatible::makeInvenItem(Inven_Item& item)
{
    item.m_field0 = m_field0;
    item.m_field1 = m_type;
    item.m_addInfo = m_itemIdx;
    item.m_addInfo2 = m_addInfo2;
    item.m_fieldb = m_endurance;
    item.SetItemAttr(m_attr);
    item.m_fieldd = m_fieldd;
    unsigned char* dst = (unsigned char*)&item.m_amp;
    memcpy(dst, &m_amp0, 4);
    memcpy(dst + 4, &m_rand0, 8);
    memcpy(dst + 12, &m_rand8, 2);
}

// ============================================================================
// CCreatureItemFactory（ORIG ctor 0x0833f67a / dtor 0x0833f6d0 / guard 单例）
//   两 DynamicPool：+0x00 CEgg 池、+0x5c CCreature 池，各 0x3e8（1000）基数。
//   AcquireCreatureItem(type)：type==1 → CCreature 池，type==0 → CEgg 池；
//   Acquire 后经 vtable+0 调 C1 构造。失败 cMyTrace 报错并返回 0。
//   FreeCreatureItem(item)：按池归还。
// ============================================================================
namespace
{
    const char* kCreativeFactoryTrace = 
        "user_creature::CCreatureItemFactory::AcquireCreatureItem(user_creature::CREATURE_ITME_TYPE)";
}

user_creature::CCreatureItemFactory::CCreatureItemFactory()
    : m_eggPool(0x3e8), m_creaturePool(0x3e8)
{
}

user_creature::CCreatureItemFactory::~CCreatureItemFactory()
{
}

user_creature::CCreatureItem* user_creature::CCreatureItemFactory::AcquireCreatureItem(
    user_creature::CREATURE_ITME_TYPE type)
{
    user_creature::CCreatureItem* item = 0;
    if (type == 1)
        item = reinterpret_cast<user_creature::CCreatureItem*>(m_creaturePool.Acquire());
    else if (type == 0)
        item = reinterpret_cast<user_creature::CCreatureItem*>(m_eggPool.Acquire());
    if (item != 0)
    {
        return item;
    }
    cMyTrace trace("bool user_creature::CCreatureItemFactory::AcquireCreatureItem("
                   "user_creature::CREATURE_ITME_TYPE)", 0x68, 5);
    trace("AcquireCreatureItem failed\n");
    return 0;
}

bool user_creature::CCreatureItemFactory::FreeCreatureItem(user_creature::CCreatureItem* item)
{
    if (item == 0)
        return false;
    return m_creaturePool.Free(reinterpret_cast<user_creature::CCreature*>(item));
}

// ORIG guard 单例（guard var 0x943c070 / instance 0x943c080）。
user_creature::CCreatureItemFactory* user_creature::GetInstanceCreatureItemFactory()
{
    static user_creature::CCreatureItemFactory* s_instance;
    if (s_instance == 0)
        s_instance = new user_creature::CCreatureItemFactory();
    return s_instance;
}

// ============================================================================
// CEgg::Hatch @ 0x0833912a（objdump 逐条转写）：蛋物品 find_item 非空且
//   非 stackable → *out = CEquipItem::get_hatch_item_id() 返回 1；否则 0。
// ============================================================================
bool user_creature::CEgg::Hatch(int* out)
{
    CItem* item = G_CDataManager()->find_item(
        reinterpret_cast<user_creature::CCreatureItem*>(this)->GetItemId());
    if (item == 0)
        return false;
    if (item->is_stackable())
        return false;
    // ORIG CEquipItem::get_hatch_item_id() 返回 +0x164（无具名，按字节偏移直访）。
    *out = *(int*)((char*)item + 0x164);
    return true;
}

// ============================================================================
// CCreature::SetStomach @ 0x0833f070：写 +0x38 m_stomach.m_value = stomachValue。
// ============================================================================
void user_creature::CCreature::SetStomach(int stomachValue)
{
    m_stomach.m_value = (float)stomachValue;
}

// ============================================================================
// CCreatureItem::Set @ 0x0833ef20：深拷贝 src 各字段（蛋 → 宠物项复制）。
// ============================================================================
void user_creature::CCreatureItem::Set(user_creature::CCreatureItem* src)
{
    if (src == 0)
        return;
    m_itemId = src->m_itemId;
    m_creatureItemType = src->m_creatureItemType;
    m_uid = src->m_uid;
    m_slotNo = src->m_slotNo;
    m_noCharge = src->m_noCharge;
    m_stat = src->m_stat;
    m_expireTime = src->m_expireTime;
}

// ============================================================================
// CCreatureMgr::Hatch @ 0x08339fa0（Hatch.md）
//   蛋孵化：m_creatureItems 按 addInfo 查蛋（不存在 → SetError(0x15)）；
//   蛋类型非 0 → SetError(0x17)；CEgg::Hatch 失败 → SetError(0x11)；
//   工厂取新宠物项失败 → SetError(0x16)；Set(蛋副本) + SetItemId(孵化结果) +
//   SetCreatureItemType(1) + SetStomach(100)；
//   计算到期（getUsablePeriod==0 且 getExpirationDate!=0 → 过期日-当前秒，
//   否则 当前秒+usablePeriod*0x15180；两者皆 0 → 0）；
//   Unregister 旧蛋 + Register 新宠；槽位写 m_addInfo/m_itemId；
//   CDBMsgSender::HatchCreature 落库 → 成功 LogHistory+Send+SetOk+列表包+Send
//   +SendUpdateItemList；失败 trace+SetError(2)。
// ============================================================================
bool user_creature::CCreatureMgr::Hatch(int addInfo)
{
    CPacketRespondent respondent(m_user, 1, 0x69);
    std::map<int, CCreatureItem*>::iterator it = m_creatureItems.find(addInfo);
    if (it == m_creatureItems.end())
    {
        respondent.SetError(0x15);
        return false;
    }
    CEgg* egg = reinterpret_cast<CEgg*>(it->second);
    if (reinterpret_cast<CCreatureItem*>(egg)->GetCreatureItemType() != 0)
    {
        respondent.SetError(0x17);
        return false;
    }
    int creatureItemId = 0;
    if (!egg->Hatch(&creatureItemId))
    {
        respondent.SetError(0x11);
        return false;
    }
    CCreatureItem* newItem =
        GetInstanceCreatureItemFactory()->AcquireCreatureItem(
            user_creature::CREATURE_ITME_TYPE_CREATURE);
    if (newItem == 0)
    {
        respondent.SetError(0x16);
        return false;
    }
    newItem->Set(it->second);
    newItem->SetItemId(creatureItemId);
    newItem->SetCreatureItemType(user_creature::CREATURE_ITME_TYPE_CREATURE);
    reinterpret_cast<CCreature*>(newItem)->SetStomach(100);

    CItem* itemScript = G_CDataManager()->find_item(creatureItemId);
    int expireTime = 0;
    if (itemScript != 0)
    {
        int usablePeriod = itemScript->getUsablePeriod();
        int expirationDate = itemScript->getExpirationDate();
        if (usablePeriod == 0 && expirationDate != 0)
        {
            expireTime = expirationDate - OS_API::GetDateTimeTick();
        }
        else if (usablePeriod == 0 && expirationDate == 0)
        {
            expireTime = 0;
        }
        else
        {
            expireTime = OS_API::GetDateTimeTick() + usablePeriod * 0x15180;
        }
    }
    newItem->SetExpireTime(expireTime);

    int eggUid = reinterpret_cast<CCreatureItem*>(egg)->GetUid();
    UnregisterCreatureItem(eggUid);
    int newUid = newItem->GetUid();
    RegisterCreatureItem(newItem, newUid);

    Inven_Item* slotItem = m_user->getCurCharacInvenW()->GetInvenRef(3, newItem->GetSlotNo());
    if (slotItem != 0)
    {
        slotItem->m_addInfo = 0;
        slotItem->m_addInfo2 = newItem->GetItemId();
    }

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int capturedUid = newItem->GetUid();
    int capturedItemId = newItem->GetItemId();
    bool ok = sender.HatchCreature(capturedUid, 1, capturedItemId);
    if (ok)
    {
        m_user->LogHistory("Hatch,%d,%d", capturedItemId, capturedUid);
        sender.Send();
        respondent.SetOk();
        respondent.Send();
        int slotNo = newItem->GetSlotNo();
        respondent.MakeNotipacketCreatureItemList(&m_creatureItems, slotNo);
        respondent.Send();
        m_user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7, slotNo);
    }
    else
    {
        cMyTrace trace("bool user_creature::CCreatureMgr::Hatch(int)", 0xb1a, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
    }
    return ok;
}

// ORIG _ZN13user_creature20CCreatureItemFactory7GetSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f546：type==1 → m_creaturePool.Size()；type==0 → m_eggPool.Size()；
// 其余返回 0。
int user_creature::CCreatureItemFactory::GetSize(user_creature::CREATURE_ITME_TYPE type)
{
    if (type == user_creature::CREATURE_ITME_TYPE_CREATURE)
        return m_creaturePool.Size();
    if (type == user_creature::CREATURE_ITME_TYPE_EGG)
        return m_eggPool.Size();
    return 0;
}

// ORIG _ZN13user_creature20CCreatureItemFactory13GetRemainSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f510：同 GetSize 分派，调 DynamicPool::Remain()。
int user_creature::CCreatureItemFactory::GetRemainSize(
    user_creature::CREATURE_ITME_TYPE type)
{
    if (type == user_creature::CREATURE_ITME_TYPE_CREATURE)
        return m_creaturePool.Remain();
    if (type == user_creature::CREATURE_ITME_TYPE_EGG)
        return m_eggPool.Remain();
    return 0;
}

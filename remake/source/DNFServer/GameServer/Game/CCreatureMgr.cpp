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
#include "CEquipItem.h"
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
// itemGloballyUniqueIdentifier_t（Inven_Item +0x15 打包全局唯一标识区，
// Inven_Item_Compatible::setCopy/makeInvenItem 的具名访问依据）。
#include "ItemGloballyUniqueIdentifierGenerator.h"

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
    // ORIG 0x8336d1b：无 `int s` 槽；三分支二分下探树 `cmp $1;je B1 /
    // cmp $1;jl DEF / cmp $3;jg DEF`（技巧 34，switch 直写枚举形参）。
    switch (state)
    {
    case CREATURE_STATE_1:
    {
        m_value += delta / -60.0f;
        if (!(m_value > 0.0f))
            m_value = 0.0f;
        break;
    }
    case CREATURE_STATE_2:
    case CREATURE_STATE_3:
    {
        m_value += delta / 360.0f;
        if (m_value > 100.0f)
            m_value = 100.0f;
        break;
    }
    default:
        break;
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

// ORIG CCreature @ 0x833705e：基类构造（+0 ← 0x8c2f4d8）→ vptr 覆盖
// （+0 ← 0x8c2f4c0）→ CStomachC1（+0x34=当前秒、+0x38=100.0f）→
// COverSkillTimeC1（+0x48=0）→ Reset()。镜像无虚表，m_vptr 常量模拟
// vtable 指针；memset 在成员自动构造之后执行会覆盖子对象构造结果，
// 故补回 CStomach 初始值（Reset 随后把 m_value 覆盖为 0.0f，与 ORIG 一致）。
// 镜像无虚表：以 placement new 显式调用 CCreatureItemC1 模拟 ORIG 基类构造
// （写 +0x00=0x8c2f4d8，随后被 vptr 覆盖），与 ORIG call 序列一致；
// 余下字段不初始化（同 ORIG，由 Reset 覆盖）。
CCreature::CCreature()
{
    new (this) CCreatureItem;            // ORIG call CCreatureItemC1（写 +0x00）
    m_vptr = 0x08c2f4c0;                 // ORIG movl $0x8c2f4c0,(%eax)
    m_stomach.m_lastTime = GlobalData::s_systemTime_.getCurSec();  // ORIG CStomachC1：+0x34
    m_stomach.m_value = 100.0f;          // ORIG CStomachC1：+0x38
    m_overSkill.m_time = 0;
    Reset();
}

CCreature::~CCreature()
{
    m_vptr = 0x08c2f4c0;  // ORIG 0x83370a8：movl $0x8c2f4c0,(%eax)
}

// ORIG _ZN13user_creature9CCreature8GetStateEv W 0x833f064：返回 +0x3c。
int CCreature::GetState()
{
    return m_state;
}

// ORIG _ZN13user_creature9CCreature13IsDieCreatureEv T 0x8338478：
// GetStomachValue(m_state, m_isGrow) < 1.0f（ORIG fucompp+test0x45 sete=value<1）。
bool CCreature::IsDieCreature()
{
    return m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow) < 1.0f;
}

// ORIG _ZN13user_creature9CCreature8SetStateEP5CUserNS_14CREATURE_STATEE T 0x0833715a
// （docs/class_func_reports/user_creature__CCreature/SetState.md）。
void CCreature::SetState(CUser* user, CREATURE_STATE state)
{
    float stomachValue = m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow);
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
    // ORIG 0x833742x：je→尾mov0；call 体 fall；setge;test;je→中mov0；fall mov1。
    if (m_script != 0)
    {
        if (m_level >= m_script->GetMaxLevel())
            return true;
    }
    return false;
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
    // ORIG 83378e9：call; test al; je→mov0 尾块 = if/return true 形。
    if (m_script->validItemExp(itemIdx))
        return true;
    return false;
}
// ORIG GainExp @ 0x83378fc：路由入口（reports/user_creature__CCreature/GainExp.md）。
// m_script==0 → 0；选择进化等级有效（!=-1）且 m_level>=它 → 0；
// m_level<GetMaxLevel 时按 m_isGrow 分流：==1 走 GainExpCreature，否则 GainItemExpCreature；
// 达到 MaxLevel → 0。
bool CCreature::GainExp(int exp, CUser* user)
{
    // ORIG 0x8337923..：m_level 缓存 %ebx 跨 call 存活（callee-saved，0x8337936
    // `mov 0x2c(%eax),%ebx`）；两比较 sete/setne 物化于 %al（技巧 22），
    // GetTypeSelectEvolutionLevel 二次独立 call（无 level 槽）。
    register int lvlBx asm("ebx");
    register bool v asm("al");
    if (m_script == 0)
        return false;
    v = (m_script->GetTypeSelectEvolutionLevel() != -1);
    if (v)
    {
        lvlBx = m_level;
        v = (lvlBx >= m_script->GetTypeSelectEvolutionLevel());
        if (v)
            return false;
    }
    lvlBx = m_level;
    v = (lvlBx >= m_script->GetMaxLevel());
    // ORIG 0x83379a0+：setge true → fall `mov0;jmp end`（return false 早退），
    // 假路径 je→isGrow 分流区（`if(!v)` 反产 xor）。
    if (v)
        return false;
    if (m_isGrow == 1)
        return GainExpCreature(exp, user);
    return GainItemExpCreature(exp, user);
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
    // ORIG 0x83375c1 call CCreatureItem::SetItemId W 0x833efb0；0x83375cc call
    // GetItemId W 0x833efa4 后 mov %eax,0x4(%esp) → GetCreatureId(int) 0x833769c。
    reinterpret_cast<CCreatureItem*>(this)->SetItemId(itemId);
    int creatureId = GetCreatureId(
        reinterpret_cast<CCreatureItem*>(this)->GetItemId());
    m_script = GetInstanceCreatureScriptMgr()->FindCreatureScript(creatureId);
    if (m_script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x56c, 5);
        trace("cannot find  creature script memory\n");
        return;
    }
    if (m_script->IsItemGrowCreature())
        m_isGrow = 0;
    else
        m_isGrow = 1;
    if (m_isGrow != 1 && m_exp == 0)
    {
        // ORIG 0x833767f：栈 temp -0x20 先整字存、再 movb $5 覆写低字节（无
        // lea）、回读 int 与 movzbl 同槽 → union 别名形（4.4 对 *(&int) 取
        // 地址会物化 lea 反产两指令）。
        union ExpAlias { int w; unsigned char b; };
        ExpAlias expVal;
        expVal.w = m_exp;
        expVal.b = 5;
        m_exp = expVal.w;
        m_field4d = expVal.b;
    }
}

// ORIG _ZN13user_creature9CCreature19IsAvailableArtifactEi T 0x83376f6：
// m_script==0 → false；否则委托 CCreatureScript::IsAvailableArtifactSlot(slot)。
bool CCreature::IsAvailableArtifact(int slot)
{
    if (m_script == 0)
        return false;
    // ORIG 0x833771d：call 后无 test/setne，直落 %eax。
    return m_script->IsAvailableArtifactSlot(slot);
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
namespace
{
    // ORIG 0x833775a `lea 0x6e8(%eax),%edx`：GetCreatureMgrW 为 4.1 头内联的
    // 成员地址计算（无 call、无 extra 槽）。本 TU 以 pad 镜像视图复现单级 lea。
    struct CInventoryCreatureMgrView
    {
        char m_prefix[0x6e8];
        user_creature::CCreatureMgr m_creatureMgr;  // CInventory+0x6e8
    };
}
float CCreature::GetAddExp(int artifactSlot, CUser* user)
{
    float result = 0.0f;
    // ORIG 0x8337732：entry `movl $0,-0x14`（inven 声明初始化 dead store，
    // 4.1 copy-prop 后 call 结果不再回写该槽）。
    CInventory* inven = 0;
    if (IsAvailableArtifact(artifactSlot))
    {
        (void)inven;
        CArtifact* artifact = reinterpret_cast<CInventoryCreatureMgrView*>(
                                  user->getCurCharacInvenW())
                                  ->m_creatureMgr.GetArtifact(artifactSlot);
        if (artifact != 0)
        {
            int minLevel = artifact->GetCreatureMinimumLevel();
            // ORIG 0x8337795：残差为 4.1 phi 物化（两路各 `mov -0x18,%eax` +
            // jmp 值岛）；4.4 CFG merge 折成单一尾 load + jl。三元/自赋值形
            // 均反产 fld/fsts 或 store，维持 if 形（4.1-PHI 天花板）。
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
    // ORIG 0x83377bc/0x83377c4：两个 float 槽 entry 各自两段物化清零
    // （-0x18 先声明=截断缓存、-0x14 后声明=累加器，技巧 43/21）。
    float scaled = 0.0f;
    float addRate = 0.0f;
    addRate += GetAddExp(0x17, user);
    addRate += GetAddExp(0x18, user);
    addRate += GetAddExp(0x19, user);
    // ORIG 0x8337840/0x8337843：`fstps -0x18; flds -0x18`——乘除结果先落具名
    // float 变量、再从其 reload 做截断（4.4 表达式内联会省掉这对）。
    scaled = (float)exp * addRate / 100.0f;
    int gain = (int)scaled + exp;                       // ORIG fistpl 截断+加 exp
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x743, 5);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x763, 5);
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
        cMyTrace trace(__PRETTY_FUNCTION__,
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
        cMyTrace trace(__PRETTY_FUNCTION__,
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
    return m_saveFlag;
}

// ORIG GetStomach @ 0x083384ee：(int)CStomach::GetStomachValue(+0x3c,+0x4c)
// （fnstcw/fldcw RC=11 截断，即 C++ float→int 截断转换）。
int CCreature::GetStomach()
{
    return (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                          m_isGrow);
}

// ORIG GetExp @ 0x833f08e：+0x30。
int CCreature::GetExp()
{
    return m_exp;
}

// ORIG IsNoCharge @ 0x833f0a6：movzbl +0x14。
bool CCreature::IsNoCharge()
{
    return m_noCharge;
}

// ORIG SetNoCharge @ 0x833f0b2：bool 直存（mov %dl,0x14），无三目规范化。
void CCreature::SetNoCharge(bool flag)
{
    m_noCharge = flag;
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

// ORIG ~CCreatureMgr @ 0x08339226（~CCreatureMgr.md）：
// 成员逆序析构（先 m_field30 map<int,int>，再 m_creatureItems
// map<int,CCreatureItem*>）；指针/标量成员无析构。用户定义使 D1/D2 为 T。
CCreatureMgr::~CCreatureMgr()
{
}

// ORIG operator= @ 0x869d614【objdump 逐条转写】：
// map aSE → +0x18/+0x1c 指针 → +0x20/+0x24/+0x28 CArtifact 三条平铺 store →
// +0x2c → map<int,int> aSE(+0x30) → movzbl +0x48 → 返回 this。
CCreatureMgr& CCreatureMgr::operator=(const CCreatureMgr& src)
{
    m_creatureItems = src.m_creatureItems;
    m_equippedCreature = src.m_equippedCreature;
    m_user = src.m_user;
    // ORIG 0x869d644 三条 artifact store 共享 %eax/%edx（4.1 跨语句 CSE，value
    // 走 %ecx）；4.4 -O0 每条平铺 reload 且 value 覆盖 %edx，register 钉基址
    // 反产 %ebx spill+push/pop。probe 均劣化 → 4.1-CSE 天花板（N）。
    m_artifacts[0] = src.m_artifacts[0];
    m_artifacts[1] = src.m_artifacts[1];
    m_artifacts[2] = src.m_artifacts[2];
    m_tempExpireDelta = src.m_tempExpireDelta;
    m_field30 = src.m_field30;
    m_isMaxLevelGrowCreature = src.m_isMaxLevelGrowCreature;
    return *this;
}

// ORIG Reset @ 0x833a45a。
void CCreatureMgr::Reset()
{
    m_equippedCreature = 0;            // +0x18
    m_user = 0;                        // +0x1c
    m_tempExpireDelta = -1;            // +0x2c = -1
    if (m_field30.size() != 0)         // +0x30 map clear
        m_field30.clear();
    if (m_creatureItems.size() != 0)   // this+0x00 map<int,CCreatorItem*>
    {
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); ++it)
        {
            CCreatureItem* creatureItem = it->second;
            if (creatureItem != 0)
            {
                if (!GetInstanceCreatureItemFactory()->FreeCreatureItem(creatureItem))
                {
                    LogManager::logFormat(1, "Creature.cpp",
                                          "void user_creature::CCreatureMgr::Reset()",
                                          0xb50, "FreeCreatureItem() error");
                }
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
    // ORIG 0x833a5cb/0x833a5dd：短路用直接分支（jne / xor;test;je），无 4.4
    // && 物化岛（mov1;jmp;mov0;test）⇒ 展开为嵌套 if，false 路径 fall-through。
    if (item->m_addInfo == 0x18)
    {
        if (!UseFeed())
            return 0x85;
    }
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->vaildItemExp(item->m_addInfo))
        {
            if (!GainExp(item->m_addInfo))
                return 0x11;
        }
    }
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
            __PRETTY_FUNCTION__, 0xab5, 5);
        trace("cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    return true;
}

// ORIG GainExp @ 0x8339b06。
bool CCreatureMgr::GainExp(int exp)
{
    // ORIG 0x8339b14/0x8339b37：两条短路均 je 直达尾 mov0 岛（无 xor 取反）；
    // 两条 false 路径共用单一尾 return（ORIG 两 je 同指 0x8339b66）。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->GainExp(exp, m_user))
        {
            if (m_equippedCreature->IsGrowCreature())
                m_isMaxLevelGrowCreature =
                    m_equippedCreature->isMaxLevel() ? 1 : 0;
            return true;
        }
    }
    return false;
}

bool CCreatureMgr::IsEquippedCreature() const
{
    return m_equippedCreature != 0;
}

// ORIG IsGrowCreature_Equipped_Creature @ 0x833debc。
bool CCreatureMgr::IsGrowCreature_Equipped_Creature() const
{
    // ORIG 0x833debc：je→尾mov0；call;test al;je→中mov0;jmp；fall mov1。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsGrowCreature())
            return true;
        return false;
    }
    return false;
}

// ORIG IsGrowCreature @ 0x833f0ca：movzbl +0x4c 后 xor $1 —— 即 +0x4c==0 返回
// true、==1 返回 false（与 Reset 初值相反；GainExp/UseFeed/vaildItemExp 用裸字段判断，
// 本访问器供包序列化/查询路径使用，语义以汇编为准）。
// 定义点挪至 CCreatureMgr::IsGrowCreature_Equipped_Creature 之后：4.4 对同 TU 可见
// callee 收缩 caller 帧（sub $0x4），ORIG 0x833debf 为 sub $0x18（4.1 宽松帧）。
bool CCreature::IsGrowCreature()
{
    return (m_isGrow ^ 1) != 0;
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
    // ORIG 0x833a638：test;je→mov0 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0
               ? reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId()
               : 0;
}

// ORIG T 0x833b80c（docs/class_func_reports/user_creature__CCreatureMgr/
// GetCreatureItem.md，汇编+反编译 C 双段已读）：把宠物区（m_creature @
// SIG_LOAD_INVENTORY+0x12880，memset 0x1ed8；计数 m_creatureCount @+0x1287c）与
// 蛋区（m_field30 map → m_tempPeriods[]；计数 m_creatureCount2 @+0x15340）填入
// SIG_LOAD_INVENTORY。ORIG 宠物条目 = 0x38 字节（SIG_CREATURE_ITEM_DETAIL_INFO），
// 整条目由 (**(%CCreatureItem vtable + 0xc))(item, Inven_Item&, dst, this+0x1c)
// 虚调用序列化填充（该方法未建模），本重建按 ORIG 汇编只补条目首 dword 与
// m_expireTime（+0x28，ORIG 另有 mov %eax,(%edx) 由 getExpireTime 返回值写入）。
int CCreatureMgr::GetCreatureItem(SIG_LOAD_INVENTORY* sig) const
{
    memset(sig->m_creature, 0, sizeof(sig->m_creature));   // ORIG 0x1ed8
    int count = 0;
    for (std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.begin();
         it != m_creatureItems.end() && count <= 0x8c; ++it, ++count)
    {
        CCreatureItem* item = it->second;
        SIG_CREATURE_ITEM_DETAIL_INFO* dst =
            reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(sig->m_creature) + count;
        dst->m_slotNo = item->m_itemId;        // 条目 +0x00（序列化未建模的占位写）
        dst->m_expireTime = item->m_expireTime;  // 条目 +0x28
    }
    sig->m_creatureCount = count;

    int eggCount = 0;
    for (std::map<int, int>::const_iterator it = m_field30.begin();
         it != m_field30.end() && eggCount <= 0x13; ++it, ++eggCount)
    {
        // ORIG：mov %edx,0x4(%eax,%ecx,8) / mov %edx,0x8(%eax,%ecx,8)，
        // %ecx = eggCount+0x2a68 → SIG_POSTAL_CREATURE_TEMP_PERIOD[eggCount]
        // （m_uid @+0x15344 / m_tempPeriod @+0x15348，均 int）。
        sig->m_tempPeriods[eggCount].m_uid = it->first;
        sig->m_tempPeriods[eggCount].m_tempPeriod = it->second;
    }
    sig->m_creatureCount2 = eggCount;
    return 1;
}

// ORIG GetCreatureName @ 0x833a688：空返回静态空串（DAT_08c2d760），
// 否则 CCreature::GetName()。
const char* CCreatureMgr::GetCreatureName() const
{
    // ORIG 0x833a693：test;je→mov$<A> 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->GetName()
                                   : "";  // ORIG &DAT_08c2d760 空串
}

// ORIG GetName @ 0x833f058：返回 this+0x1c。
// 定义点挪至 CCreatureMgr::GetCreatureName 之后：同 TU 可见 callee 使 4.4 caller
// 帧塌缩为 sub $0x4，ORIG 0x833a68b sub $0x18 要求 callee 在调用点不可见。
const char* CCreature::GetName()
{
    return m_name;
}

// ORIG IsDieCreature @ 0x833a660：空返回 1，否则委托 CCreature::IsDieCreature。
bool CCreatureMgr::IsDieCreature() const
{
    // ORIG 0x833a66e：test;je→mov1 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->IsDieCreature() : true;
}

// ORIG GetEquipedCreatureLevel @ 0x833a6b0。
int CCreatureMgr::GetEquipedCreatureLevel() const
{
    // ORIG 0x833a6be：test;je→mov0 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->GetLevel() : 0;
}

// ORIG GetLevel @ 0x833f09a：+0x2c。
// 定义点挪至 CCreatureMgr::GetEquipedCreatureLevel 之后（同 GetName 帧塌缩原因）。
int CCreature::GetLevel()
{
    return m_level;
}


// ORIG _ZN13user_creature12CCreatureMgr11GetArtifactEi T 0x833be56（GetArtifact.md）：
// idx=slot-0x17，仅上界检查（>3 → 0）；返回 &m_artifacts[idx]
// （ORIG this+(idx+8)*4 与镜像 m_artifacts 基址 +0x20 一致）。
CArtifact* CCreatureMgr::GetArtifact(int slot)
{
    // ORIG 0x833be56：slot-0x17 两次独立现算（无局部槽、无帧 pop 收尾）。
    if (slot - 0x17 > 3)
        return 0;
    return &m_artifacts[slot - 0x17];
}


// ============================================================================
// 本批补齐（U3）：IsCreatureEquipmentScope / RecvMail / RegistTempPeriod
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr24IsCreatureEquipmentScopeEi T 0x0833926c。
bool CCreatureMgr::IsCreatureEquipmentScope(int slot)
{
    // ORIG 833926c：js→cmp16(7e) / jle→mov1(84) / jne→mov0(8b)。
    // && 失败与 || 右操作数共享 cmp16 块；then 块在条件链之后。
    if ((slot >= 0 && slot <= 0x8b) || slot == 0x16)
        return true;
    return false;
}

// ORIG _ZN13user_creature12CCreatureMgr16RegistTempPeriodEiii T 0x0833d7de。
// 汇编实证 key=a(0xc)、value=b(0x10)（0x833d7ed find(&a)、0x833d83b/0x833d842
// make_pair(&a,&b)）；形参 c(0x14) 未引用。查中 → false；插入后 true。
bool CCreatureMgr::RegistTempPeriod(int a, int b, int c)
{
    (void)c;
    std::map<int, int>::iterator it = m_field30.find(a);
    if (it != m_field30.end())
        return false;
    m_field30.insert(std::make_pair(a, b));
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
    // ORIG 0x833f024：jle→尾岛 reload m；内层 cmpl0;jne→03f(return r)，
    // fall→038(mov$1) = return 1 在前、return r 尾岛形。
    if (m_expireTime > 0)
    {
        int r = m_expireTime - nowSec;
        if (r == 0)
            return 1;
        return r;
    }
    return m_expireTime;
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
            __PRETTY_FUNCTION__,
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
            __PRETTY_FUNCTION__,
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
    return UnregisterCreatureItem(param_1);   // ORIG 读 param_1@+0xc（我们误用 param_2@+0x10）
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
        GetInstanceCreatureItemFactory()->FreeCreatureItem(it->second);
        m_creatureItems.erase(it);
        return true;
    }
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xbc1, 5);
    trace("UnregisterCreatureItem failed. uid(%d), key(%d)\n",
          m_user->get_unique_id() & 0xffff, uid);
    return false;
}

// ============================================================================
// user_creature::CDBMsgSender（ORIG 域内类，本 TU 提供最小真实实现）
// ============================================================================

// ORIG C1 @ 0x833f8ae：仅清 m_stream（offset 0）后委托 Init；m_user 由 Init 写入。
CDBMsgSender::CDBMsgSender(ENUM_INTERNALPACKET type, CUser* user)
    : m_stream(0)
{
    Init(type, user);
}

// Init 定义已迁移至 CCreature.cpp（ORIG W 0x833f7a8 逐条转写；本 TU 仅保留
// C1 委托调用，避免两 TU 强符号冲突；全局 ::ENUM_INTERNALPACKET 见 CacheCharacterType.h）。

// ORIG D1/D2 @ 0x833f8d8：按 stream 的 poolType 归还对象池后置空。
CDBMsgSender::~CDBMsgSender()
{
    // ORIG 0x833f8e5：je 直达尾 leave（正向 if 块，body fall-through 收尾，
    // 避免 4.4 early-return 在 leave 前产 jmp+nop 岛）。
    if (m_stream != 0)
    {
        // ORIG 0x833f8ef 下探树 cmp1/cmp2/test0；块布局 1@0x8ff、0@0x917、2@0x92f；
        // default 无动作（0x8fd jmp 尾）。case0 必须显式（否则 default 吞 0 消 test）。
        switch (m_stream->m_poolType)
        {
            case 1: GlobalData::s_small_stream_pool->Free(m_stream); break;
            case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
            case 0: GlobalData::s_stream_pool->Free(m_stream); break;
            default: break;
        }
        m_stream = 0;
    }
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
    // ORIG 0x833ff3d：头部 m_stream 判空（mov (%eax);test;jne），buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833ff71：lea 0x4(%eax),%edx 临时 this；arg2 经 %eax（0xc 形参晚装）。
    // register 绑 %eax 挤 item 到 %eax，令 4.4 复现 lea→%edx 装填序。
    register Inven_Item* itemAx asm("eax") = item;
    reinterpret_cast<Inven_Item_Compatible*>(&buf->m_item)->setCopy(*itemAx);
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
    // ORIG 0x833fe95：头部 m_stream 判空，buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833febb：lea 临时 this（无 compat 槽）。同 RecvMail 的 %eax 钉法。
    register Inven_Item* itemAx asm("eax") = item;
    reinterpret_cast<Inven_Item_Compatible*>(&buf->m_item)->setCopy(*itemAx);
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
    // ORIG 0x833fc82：GetInBuffer 结果无判空，直接 memset（buf 槽 -0xc）。
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
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
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x40;
    // ORIG 0x833fdc2：先 +0x30=uid，后 mov 0x10(%ebp),%edx; mov -0xc(%ebp),%eax;
    // mov %edx,0x8(%eax)（值先基后、单段 disp8）。4.4 store 恒基先+add 折叠
    // （N76 口径），probe register 钉 %edx/%eax 均反产多余 mov，维持最简形。
    buf->m_addInfo2 = uid;                            // +0x30
    *reinterpret_cast<int*>(&buf->m_item[4]) = itemIdx;  // buf+0x08（ORIG 折叠）
    return true;
}

// ORIG _ZN13user_creature12CDBMsgSender18InsertCreatureItemEP5CUserP10Inven_Itemiiiii
// W 0x0833faaa（objdump 逐条转写）：
//   m_stream 空 → 0；buf 清 0x74 后 setCopy(item) 到 +0x04；
//   m_slot(+0x2c)=slot；m_characNo(+0x48)=user->getCurCharacNo()；
//   m_stomachValue(+0x44)=100；m_reason(+0x51 byte)=reason；
//   m_mapSize(+0x68)=mapSize；m_days(+0x54)=days；m_expireTick(+0x58)=expireTick；
//   m_ceraOrderNo(+0x60)=user->get_cera_item_order_no()；
//   itemIdx=item->m_addInfo（ORIG mov 0x2(%eax) 即 +0x02 int）：find_item 为空或
//   is_stackable → 返回 0；
//   CEquipItem::get_sub_type()==0 → m_flag50(+0x50)=1 且 m_field64(+0x64)=6，
//   否则 m_flag50=0；返回 1。
bool CDBMsgSender::InsertCreatureItem(CUser* user, Inven_Item* item, int slot,
                                      int reason, int mapSize, int days,
                                      int expireTick)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG：`lea 0x4(%eax),%edx` 临时 this（无 compat 槽，技巧 36）；同族
    // RecvMail 0x833ff71 的 register %eax 钉 item 装填序。
    register Inven_Item* itemAx asm("eax") = item;
    reinterpret_cast<Inven_Item_Compatible*>(&buf->m_item)->setCopy(*itemAx);
    buf->m_slot = slot;                               // +0x2c
    buf->m_characNo = user->getCurCharacNo();         // +0x48
    buf->m_stomachValue = 100;                        // +0x44（固定 100）
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_days = days;                               // +0x54
    buf->m_expireTick = expireTick;                   // +0x58
    buf->m_ceraOrderNo = (int)user->get_cera_item_order_no();  // +0x60

    // ORIG：itemIdx 跨 find_item call 存活于 %ebx（`mov %eax,%ebx` …
    // `mov %ebx,0x4(%esp)`），prologue push %ebx + frame 0x24。
    register int itemIdx asm("ebx") = item->m_addInfo;  // +0x02（ORIG mov 0x2(%eax)）
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
    // ORIG 0x08336eae：入口无 null 检查（直 `mov 0x2(%eax),%eax`），删臆造判空。
    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return 0;
    if (found->is_stackable())
        return 0;

    // ORIG：`mov -0x14,%eax; mov %eax,-0x10(%ebp); mov -0x10(%ebp),%eax; call`
    // equip 具名槽 -0x10；subType 无槽，switch 三级下探 cmp1;je;cmp2;je;test
    // （技巧 34 直写 switch）。
    CEquipItem* equip = static_cast<CEquipItem*>(found);
    switch (equip->get_sub_type())
    {
    case 1:
    {
        item->m_field0 = 1;
        break;
    }
    case 2:
    {
        // ORIG 0x8336f1b..0x8336f52：仅 range 具名槽(-0xc)；randIdx/target 均
        // 内联（call 结果 %eax 直落 arg，无槽）。
        int range = equip->m_randomTable.m_field0;
        item->m_addInfo = equip->m_randomTable.GetRandomItemIndex(get_rand_int(range));
        item->m_field0 = 1;
        break;
    }
    default:
        break;
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
    // ORIG 0x8338121/0x8338142：两次独立 GetEvolutionLevel call，==0 用
    // test;sete;test;je（0x8338126），> 用 cmp %edx,%eax;setg;test;je（0x833814d）。
    if ((bool)(m_script->GetEvolutionLevel() == 0))
        return 0;
    if ((bool)(m_script->GetEvolutionLevel() > m_level))
        return 0;
    // ORIG 0x8337dcb：`test;je e3b(mov1 尾岛)`——IsExist true 才 fall user 检查，
    // return 1 为尾岛（`if(!call())return 1` 反产 xor;mov1;jmp 物化岛）。
    if (m_script->IsExistEvolutionQuest())
    {
        if (user != 0)
        {
            PacketGuard packet;
            user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&packet));
            user->Send(packet);
        }
        return 0;
    }
    return 1;
}

// ORIG _ZN13user_creature9CCreature19IsAbleSelectEvoluteEP5CUser T 0x08337d58：
// 同 IsAbleEvolute，但等级门槛取 GetTypeSelectEvolutionLevel()（-1 视为不可）。
int CCreature::IsAbleSelectEvolute(CUser* user)
{
    if (m_script == 0)
        return 0;
    // ORIG 0x8337d7d/0x8337d9f：两次独立 call（无 level 缓存变量）；比较结果
    // sete/setg 物化 + test;je（技巧 9/22），非直接 jne/jle 分支。
    if ((bool)(m_script->GetTypeSelectEvolutionLevel() == -1))
        return 0;
    if ((bool)(m_script->GetTypeSelectEvolutionLevel() > m_level))
        return 0;
    // ORIG 0x8337dcb：`test;je e3b(mov1 尾岛)`——IsExist true 才 fall user 检查，
    // return 1 为尾岛（`if(!call())return 1` 反产 xor;mov1;jmp 物化岛）。
    if (m_script->IsExistEvolutionQuest())
    {
        if (user != 0)
        {
            PacketGuard packet;
            user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&packet));
            user->Send(packet);
        }
        return 0;
    }
    return 1;
}

// ORIG _ZNK13user_creature12CCreatureMgr26VerifyEquipedCreatureQuestEv
// T 0x0833d6fa：无装备宠物 → 0；成长宠走 IsAbleSelectEvolute，否则 IsAbleEvolute。
int CCreatureMgr::VerifyEquipedCreatureQuest() const
{
    // ORIG 0x833d708：je→尾mov0；then(IsAbleSelectEvolute)=fall，else=je 目标。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsGrowCreature())
            return m_equippedCreature->IsAbleSelectEvolute(m_user);
        else
            return m_equippedCreature->IsAbleEvolute(m_user);
    }
    return 0;
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
                __PRETTY_FUNCTION__,
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
// 两个独立 if（非 ||）：ORIG 字节序两条独立 0 返回路径（je +0xfc / jmp +0x101）。
int CCreatureMgr::GetExtraGold(int gold) const
{
    if (m_equippedCreature == 0)
        return 0;
    if (m_equippedCreature->IsDieCreature())
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
    // ORIG 0x833c704：四步分断——creature==0/ItemId 不符/checkTime 假三条
    // je→共享尾 mov0 岛(0x833c777)；IsDie 真值走中段 `mov0;jmp end` 物化岛
    // （技巧 40 &&-materialize 形）；0xf68a 比较为 sete;test;je（技巧 9/22，
    // bool 取反物化而非直接 jne）。
    // ORIG 0x833c704 je→尾共享 mov0(0x833c777)：goto 钉块序（技巧 23）；
    // IsDie true → 中置 `mov0;jmp end` 物化（技巧 40）；match 用 sete;test;je
    // 直判（技巧 22，`return 0` 形反产 xor+物化岛）；checkTime 正向 fall=gold。
    register bool match asm("al");  // 声明先于 goto（label 跨初始化限制）
    if (m_equippedCreature == 0)
        goto RET0;
    if (m_equippedCreature->IsDieCreature())
        return 0;
    match =
        (reinterpret_cast<user_creature::CCreatureItem*>(m_equippedCreature)
             ->GetItemId() == 0xf68a);
    // 正向 if(match)（技巧 19/22）：`if(!match)` 反产 sete;xor;jne 翻转形，
    // 正向落 test %al,%al; je RET0（ORIG 0x833c735）。
    if (match)
    {
        if (m_equippedCreature->GetOverSkillTime()->checkTime())
            return gold * 5 / 100;
    }
RET0:
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
    // ORIG 0x833a726/0x833a74c：area 与 Broadcast 返回值两段生命周期复用 %ebx
    // （无栈槽，frame 0x30、respondent@-0x28）。register 绑 ebx 复现。
    register int bx asm("ebx");
    bx = m_user->get_area(false);
    bx = respondent.Broadcast((int)(char)m_user->getCurCharacVill(), bx);
    return bx;
}

// ORIG _ZN13user_creature12CCreatureMgr20RegisterCreatureItemEPNS_13CCreatureItemEi
// T 0x0833a77e：uid 查重后插入 m_creatureItems；重复则 cMyTrace 报错返回 false。
bool CCreatureMgr::RegisterCreatureItem(CCreatureItem* item, int uid)
{
    // ORIG 0x833a798：`test;setne %al;test;je` 查重物化（技巧 22）；true 块
    // uid→%esi（0x833a7a1 `mov 0x10(%ebp),%esi`）、get_unique_id 结果
    // `movzwl %ax,%ebx`（0x833a7b2）跨 ctor call 存活，trace 变参寄存器直传
    // （0x833a7d8 `mov %esi,0xc(%esp); mov %ebx,0x8(%esp)`）。PF 串 @0x8c2f140。
    register int uidSx asm("esi");
    register unsigned int uniqBx asm("ebx");
    register bool dup asm("al");
    dup = (FindCreatureItem(uid) != 0);
    if (dup)
    {
        uidSx = uid;
        // ORIG 0x833a7b2 `movzwl %ax,%ebx`：赋值点即零扩展进 %ebx（int 载体），
        // ushort 载体使 4.4 把扩展推到 use 点（movzwl %dx 二段）。
        uniqBx = (unsigned short)m_user->get_unique_id();
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xbb4, 5);
        trace("RegisterCreatureItem failed. uid(%d), key(%d)\n", uniqBx, uidSx);
        return false;
    }
    m_creatureItems.insert(std::make_pair(uid, item));
    return true;
}

// ============================================================================
//   m_hdr|=0x200；+0x30(m_addInfo2)=uid；name 拷入 +0x34（m_pad34 名字区，截 0xc）；
//   返回 1。hdr 位/name 位置依同族（UseFeedCreature|0x2 等）推断，无独立报告。
//   （ORIG asm 833fbe9..833fc1a 无 buf 判空、无 name 判空。）
bool CDBMsgSender::RenameCreatureItem(int uid, const char* name)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x1;
    buf->m_addInfo2 = uid;                            // +0x30
    // ORIG asm 833fc1a：lea 0x34(%eax),%edx 直接传 dst（无中间变量），strncpy 0xc。
    strncpy(buf->m_name, name, 0xc);
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
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x244;   // ORIG asm 833fcf0：or $0x244（0x200|0x40|0x4）
    buf->m_addInfo2 = uid;                            // +0x30
    buf->m_flag50 = (unsigned char)flag;              // +0x50
    // ORIG 0x833fd0f：无 compat 槽，`mov %edx,0x8(%eax)` buf 基址直存。
    *reinterpret_cast<int*>(&buf->m_item[4]) = itemIdx;  // buf+0x08
    CItem* item = G_CDataManager()->find_item(itemIdx);
    buf->m_days = item->getUsablePeriod();            // +0x54
    buf->m_expireTick = item->getExpirationDate();    // +0x58
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
    // ORIG（objdump 0x833f3ef..0x833f412）：mov 0x11/0x15/0x19(%edx) → 三个 int、
    // movzwl 0x1d(%edx) → ushort。+0x11 为 stAmplifyOption_t 头部 4 字节打包
    // （m_abilityType/m_abilityValue/m_lock，无独立 4 字节成员）；+0x15..0x1e 为
    // 打包 itemGloballyUniqueIdentifier_t（CEquipItem::make_item 同一区域实证）。
    m_amp0 = *reinterpret_cast<const int*>(&item.m_amp);            // item+0x11（4B）
    const itemGloballyUniqueIdentifier_t* guid =
        reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(&item.m_amp.m_pad4[0]);
    m_rand0 = guid->m_serverId;                                     // item+0x15
    m_rand4 = guid->m_datetime;                                     // item+0x19
    m_rand8 = guid->m_sequenceNo;                                   // item+0x1d
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
    // ORIG 0x833f476..0x833f497：guid 三写（0x15/0x19/0x1d）以单一 %eax 基址
    // 连续（共享装载），m_amp0(→+0x11) 独立重载置后；中间变量 guid 反产 -0xc 槽。
    // ORIG 0x833f476：三级 disp 共享 %eax=item 基址——具名嵌套成员路径折叠
    // （cast+数组下标形反产每写一次 add 重算）。
    item.m_amp.m_guid.m_serverId = m_rand0;                          // → item+0x15
    item.m_amp.m_guid.m_datetime = m_rand4;                          // → item+0x19
    item.m_amp.m_guid.m_sequenceNo = m_rand8;                        // → item+0x1d
    *reinterpret_cast<int*>(&item.m_amp) = m_amp0;                   // → item+0x11（ORIG 置后独立重载）
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
    // ORIG 0x833f5b6：非空 → 0x833f5bc mov (%eax),%eax; mov (%eax),%edx;
    // mov -0xc,%eax; mov %eax,(%esp); call *%edx —— 经 vtable[0] 对池内存
    // placement 构造 C1；成功/失败统一尾 mov -0xc,%eax 返回（0x833f613）。
    if (item != 0)
    {
        typedef void (*CtorCall)(void*);
        // ORIG 0x833f5bc：fnptr 双级解引用（call *%edx）+ arg 经 %eax 装
        // (%esp)。4.4 恒 fnptr→%eax/arg→%edx，角色互换属 N7（虚调用 callee
        // 寄存器互换）；register 绑 %edx 反产 spill 对拷，维持最简内联形。
        (reinterpret_cast<CtorCall>(*reinterpret_cast<CtorCall**>(item)))(item);
        return item;
    }
    // ORIG 0x833f5cd：ctor cMyTrace(__PRETTY_FUNCTION__@0x8c2eda0, 0x68, 5)；
    // 0x833f5f0：operator()("%d]"@0x8c2d761, PF, 0x68)。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x68, 5);
    trace("%d]", __PRETTY_FUNCTION__, 0x68);
    return item;
}

// ORIG 0x833f618：GetCreatureItemType()==1 → 归还 m_creaturePool(+0x5c)；
// ==0 → 归还 m_eggPool(+0x00)；否则返回 0（ORIG 无 null 检查，忠实转写）。
bool user_creature::CCreatureItemFactory::FreeCreatureItem(user_creature::CCreatureItem* item)
{
    if (item->GetCreatureItemType() == 1)
        return m_creaturePool.Free(reinterpret_cast<user_creature::CCreature*>(item));
    if (item->GetCreatureItemType() == 0)
        return m_eggPool.Free(reinterpret_cast<user_creature::CEgg*>(item));
    return false;
}

// ORIG guard 单例（guard var 0x943c070 / instance 0x943c080）：__cxa_guard
// 双检锁 + atexit 注册。
user_creature::CCreatureItemFactory* user_creature::GetInstanceCreatureItemFactory()
{
    static user_creature::CCreatureItemFactory s_instance;
    return &s_instance;
}

// ============================================================================
// CEgg::Hatch @ 0x0833912a（objdump 逐条转写）：蛋物品 find_item 非空且
//   非 stackable → *out = CEquipItem::get_hatch_item_id() 返回 1；否则 0
//   （ORIG 0x8339181 为外联 call 0x833eeda；nm -S 核验
//   _ZNK10CEquipItem18get_hatch_item_idEv W 0x0833eeda size 0xe）。
// ============================================================================
bool user_creature::CEgg::Hatch(int* out)
{
    CItem* item = G_CDataManager()->find_item(
        reinterpret_cast<user_creature::CCreatureItem*>(this)->GetItemId());
    if (item == 0)
        return false;
    if (item->is_stackable())
        return false;
    // ORIG 0x8339175..0x8339181：先物化 CEquipItem* 局部（mov -0x10(%ebp),%eax;
    // mov %eax,-0xc(%ebp)），再以该局部为 this 外联调用 get_hatch_item_id。
    CEquipItem* equip = static_cast<CEquipItem*>(item);
    *out = equip->get_hatch_item_id();
    return true;
}

// ============================================================================
// CCreature::SetStomach @ 0x0833f070：调 CStomach::SetStomachValue(this+0x34, (float)stomachValue)，
// ORIG 即 call SetStomachValue(写入 +0x4 m_value)。
// ============================================================================
void user_creature::CCreature::SetStomach(int stomachValue)
{
    char _pad[0x10];  // P31 帧对齐：ORIG sub $0x18，默认帧 sub $0x8，补 16B 达一致
    m_stomach.SetStomachValue((float)stomachValue);
}

// ============================================================================
// CCreatureItem::Set @ 0x0833ef20：深拷贝 src 各字段（蛋 → 宠物项复制）。
// ============================================================================
void user_creature::CCreatureItem::Set(user_creature::CCreatureItem* src)
{
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
    reinterpret_cast<CCreature*>(newItem)->SetItemId(creatureItemId);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0xb1a, 5);
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

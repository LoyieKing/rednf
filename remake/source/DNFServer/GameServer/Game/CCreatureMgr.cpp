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
// [b28d-solo-CCreatureMgr 单文件泳道收口判词·检查点14 后终版] 清单 11 件（TU 三数
// identical=180 ae=413 合计 593 / near=1 / diff=10；基线 592/near1/diff11）：
//   AE/identical：RenameCreature（P225 while+return 复现 4.1 `setne %al;test;je`，
//     162=162）、CCreatureC1（21=21，Main 头改动后 `call CCreatureItem::C2Ev` 对
//     ORIG C1Ev = P93 同址别名豁免）、GetOverSkillExtraGold、GetExtraGold（N112
//     4.4 对齐 nop，compare_tu 口径已 AE）。
//   NEAR：SwapCreature（189=189，4 处实参/this 装载序 = P75/N-G；P277 试形零效）。
//   N：SetKeepRightItem（156=156，P75/N-G；P277 试形 156→158 劣化已回滚）、
//     AcquireCreatureItem（N7 间接 call %eax vs ORIG %edx；**4.4.6 路由两组合实测
//     净负且本件不归零 ⇒ 已记负例，勿再靠换后端翻**）、SetEggItem（N111 死 test）、
//     SetState（N18b/P209 形参 mem 直比 + N7；S1 交换操作数已试）、Hatch（§6.2：
//     p167 CLEAR 60=60 + gap 1）、GetCreatureItem（§6.2：p167 CLEAR 15=15，
//     154 vs 147，残余槽布局 N82/N93 + 尾部 epilogue）。
//   头改动后回归（已报 Main 请落）：CCreatureItem::Set 37 vs 39 —— CInventory.h
//     m_noCharge 已 bool 而 GetNoCharge() 仍返 unsigned char ⇒ 4.4 插 setne；
//     直证 P315/P136：ORIG 0x833ef86 直存 + getter `movzbl;ret` ⇒ 请求改 bool。
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
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 已消 x87 比较 lowering；
// 唯一残差 = 阈值比较的 C3 掩码：ORIG `test $0x45,%ah`（严格 `m_value<0.0f`）
// vs 原 `<=` 的 `test $0x5` ⇒ 改严格小于后逐条全等。ORIG 0x08336cfc
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
        // [b28d-solo-CCreatureMgr AE] ORIG 0x8336d80 `test $0x45,%ah; sete`：
        // fucompp 比较 st0=0.0 / st1=m_value，掩码 0x45 含 C3 ⇒ 命中条件为
        // 0.0>m_value（严格 `<`）；此前写 `<=` 得 `test $0x5`（C3 位差）⇒ 改
        // `m_value < 0.0f` 即逐条全等。
        if (m_value < 0.0f)
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
// [b28-BM7 判词] 残余 = N85：ORIG 比较形 `fucompp; fnstsw %ax; sahf`（4.1）vs
// 4.4 `fucomip %st(1),%st; fstp %st(0)`（x87 比较两向跨版本固有差，patterns N85）；
// 常量地址 0x8c2f450 引用为 AE 归一项。语义（>100 钳位+返回）全等。
// [b28d-23 N·N85] x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp）⇒ N。ORIG 0x08336e66
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering（fucompp/fnstsw/sahf）；ORIG 0x8336e66
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
    m_creatureItemType = CREATURE_ITME_TYPE_CREATURE;  // +0x08 = 1（列表包要求 type==1）
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
// [b28d-23 N·N83] 镜像无虚表：ORIG 21 条（直 call CCreatureItemC1）vs 4.4 55 条（placement new _ZnwjPv + EH cleanup）⇒ N。ORIG 0x0833705e
// [b28d-solo-CCreatureMgr N] N83：镜像类 placement new（_ZnwjPv + EH cleanup，55 条）vs ORIG 直 call 基类 C1（21 条）；ORIG 0x833705e
// [b28d-solo-CCreatureMgr R2上报] 唯一出路 = 头改动（Main 代落）：CCreatureItem 多态化 +
//   CCreature : public CCreatureItem。ORIG 0x833705e 21 条 = call CCreatureItem::C1 →
//   编译器自动写 vptr（_ZTV+8）→ CStomach::C1(+0x34) → COverSkillTime::C1(+0x50) → Reset；
//   我方 p167 EXTRA _ZnwjPv/_ZdlPvS_/_Unwind_Resume/_ZN11CSystemTime9getCurSecEv。
// [b28d-solo-CCreatureMgr AE·检查点14 已落] Main 落头改动后 21=21（compare_tu AE）：
//   我方 `call CCreatureItem::C2Ev` 对 ORIG `C1Ev` = P93 同址别名（0x8340c82）豁免；
//   vptr 写为 `movl $8,(%eax)+R_386_32 _ZTV13CCreature`（AE 伪代码化）。
CCreature::CCreature()
{
    // [b28d-Main·检查点14 真多态化] 基类 ctor 由编译器隐式调用（ORIG 0x833705e
    //   `call CCreatureItem::C1Ev` + `movl $0x8c2f4c0,(%eax)` 自动复现）；
    //   原 `new (this) CCreatureItem` + 手工 m_vptr 写已删（消 N83 placement-new）。
    // [b28d-Main·检查点14] 删除冗余成员初始化：ORIG ctor 体仅 Reset()，
    //   +0x34/+0x38/+0x50 的初始化由 CStomach::C1Ev/COverSkillTime::C1Ev 完成
    //   （本仓库两成员 ctor 已同形）⇒ 复现 ORIG 21 条。
    Reset();
}

// ~CCreature 定义迁至 CGameManager.cpp（CNode 特化之前）：P210 callee 定义顺序
// 定律——DynamicPool<CCreature>::CNode::~CNode 需先可见 dtor 定义才能继承
// ORIG 的 sub $0x4 小帧（仅声明时 4.4 保守撑至 0x18；探针 /tmp/bm3/）。

// ORIG _ZN13user_creature9CCreature8GetStateEv W 0x833f064：返回 +0x3c。
int CCreature::GetState()
{
    return m_state;
}

// ORIG _ZN13user_creature9CCreature13IsDieCreatureEv T 0x8338478：
// GetStomachValue(m_state, m_isGrow) < 1.0f（ORIG fucompp+test0x45 sete=value<1）。
// [b28d-23 N·N85/P230] x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp/seta）；-march=i486 可消但本 TU 未路由且禁改路由表 ⇒ N。ORIG 0x08338478
// [b28d-solo-CCreatureMgr AE] P252 具名 float 局部复现 fstps/flds 槽往返 + TU 路由 -march=i486 消 x87 lowering；ORIG 0x8338478
bool CCreature::IsDieCreature()
{
    // ORIG 0x833a660 sub $0x28：float 比较物化 spill 槽 -0xc(fstps/flds)+slack。
    // 4.4 栈顶保留不占槽 → 帧差 0x10；int[4] 16B 对齐恰好补齐（dead pad）。
    // [b28-BM7 判词] 残余 = N85：ORIG `fstps/flds; fucompp; fnstsw %ax;
    // test $0x45,%ah; sete %al` vs 4.4 `fucomip %st(1),%st; fstp %st(0); seta`
    // （x87 比较两向+槽往返跨版本固有差）。语义（value<1 ⇒ die）全等。
    // [b28d-solo-CCreatureMgr P252] ORIG `fstps -0xc(%ebp); flds -0xc(%ebp)`
    // ⇒ 原码有具名 float 局部（N85 突破口）；直返表达式会省掉该槽往返对。
    float stomachValue =
        m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow);
    return stomachValue < 1.0f;
}

// ORIG _ZN13user_creature9CCreature8SetStateEP5CUserNS_14CREATURE_STATEE T 0x0833715a
// （docs/class_func_reports/user_creature__CCreature/SetState.md）。
// [b28d-solo-CCreatureMgr N] ndiff=10：①三处 `(int)state != m_state` ORIG 为
// `mov 0x3c(%eax),%eax; cmp 0x10(%ebp),%eax`（形参 mem 直比），4.4 恒
// `mov 0x3c,%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx`——8 形探针（f1..f8 含
// `m_state != (int)state` / `(CREATURE_STATE)m_state != state` / unsigned 双向）
// 均不可产（N18b/P209 族）；②InsertTimer 三实参首值 callee-saved 角色互换
// （ORIG esi=第 6 实参 getCurCharacNo / ebx=GetUID，4.4 相反，N82附/P78）。
// [b28d-solo-CCreatureMgr S1 已试] 交换左右操作数（m_state != (int)state）后 4.4 仍产
// `mov 0x3c,%edx; mov state,%eax; cmp %eax,%edx`（两寄存器比较，135 条不变）⇒ 恢复直证形。
// 语义全等（p167 CLEAR）。ORIG 0x0833715a
void CCreature::SetState(CUser* user, CREATURE_STATE state)
{
    float stomachValue = m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow);
    // [b23S3C] P191/P194 极性：ORIG 0x8337192/0x195、0x833719b/0x19f、
    // 0x83371ac/0x1ae 三条判定全部「假则前跳」到 0x0d4 复活测试块，定时器分支走
    // fall-through ⇒ 原码先判 timer 条件（&& 短路正向形），else 内再判复活条件；
    // 旧 || 负形使 4.4 产出 xor $0x1（bool==0 折叠）+ 块序倒置。语义等价。
    // [P321-S1 已试] 交换左右操作数后 4.4 仍产 `mov 0x3c,%edx; mov state,%eax;
    // cmp %eax,%edx`（两寄存器比较）⇒ 恢复直证形；残余 = P209/N18b 形参 mem 直比
    // 不可造（4.4 恒载入寄存器）。ORIG 0x8337192 `cmp 0x10(%ebp),%eax`。
    // [C01/F2 复查·已试无效] Main 转发 C05 F2（int 局部 ⇒ 比较内存直比）在本件
    // **不适用**：ORIG 0x8337192 `cmp 0x10(%ebp),%eax` 的 mem 直比对象是**形参槽**
    // （enum CREATURE_STATE，mangled 固定不可改型），非局部；试具名 int 局部形
    // `const int st=(int)state;` 后本件 135→**141**（多 3 条 store/load 且预载
    // `mov 0x10(%ebp),%edx` 仍在）⇒ 已回退，维持 P209/N18b 封顶。
    if ((int)state != m_state && (int)state == 1 && m_isGrow != 0)
    {
        // 切入战斗态（state 1）：按 60 秒 × 饱食度系数插入复活定时器
        // （常量 .rodata 0x8c2f460 = double 60.0；TIMER_MESSAGE 0x5c）。
        // [b23S3C] P0 律/P48 去缓存：ORIG 0x83371d7/0x8a/0x98 三个 call 结果只进
        // %esi/%edi/%ebx，无 -off 槽 ⇒ 原码无 characNo/uniqueId 具名局部，
        // 直落 InsertTimer 实参（求值序右起：characNo→uid→GetUID→G_TimerQueue，
        // 与 ORIG 一致）；sec 走 -0x1c 槽（fistpl 实证）保留具名。
        int sec = (int)(60.0 * stomachValue);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                    (TIMER_MESSAGE)0x5c, sec,
                                    user->get_unique_id(),
                                    user->getCurCharacNo());
    }
    else
    {
        // 进入 state 3 且非同值切换：死亡宠复活流程（ORIG 0x08337269 起）。
        if ((int)state != m_state && (int)state == 3 && m_isGrow != 0)
        {
            if (IsDieCreature())
            {
                CPacketRespondent pkt(user, 0, 0);
                pkt.MakeNotipacketRevivalCreature();
                pkt.SendAll();
                m_stomach.SetStomachValue(1.0f);
                pkt.clear();  // InterfacePacketBuf::clear（复用包对象重填状态包）
                // [b23S3C] ORIG 0x8337269→0x83372c9：状态包 uid 实参来自
                // CCreatureItem::GetUid(this) 调用（非 m_uid 直读）⇒ 补真调用。
                pkt.MakeNotipacketCreatureState(
                    reinterpret_cast<CCreatureItem*>(this)->GetUid(), 1);
                pkt.Send();
            }
        }
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
    if (m_script == 0)
        return false;
    if (m_script->GetTypeSelectEvolutionLevel() != -1)
    {
        if (m_level >= m_script->GetTypeSelectEvolutionLevel())
            return false;
    }
    if (m_level >= m_script->GetMaxLevel())
        return false;
    // ORIG 0x833797e：movzbl;xor$1;test;je <GainExpCreature>，fall=GainItemExp
    // （P194 X1 块序：xor 物化 (m_isGrow!=1)，je 是 ==1 成功分支）。
    if (m_isGrow != 1)
        return GainItemExpCreature(exp, user);
    return GainExpCreature(exp, user);
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
// [b28d-solo-CCreatureMgr AE] P197③ 预清 + P194 xor 物化 + P59 非成长 fall + P48 两次独立 GetMaxLevel；ORIG 0x83374d2
int CCreature::GetExpLevel(int exp)
{
    // [b28d-solo-CCreatureMgr P197③/P194/P59] 逐条照 ORIG 0x83374d8..0x833756b：
    // ①level 预清 `movl $0,-0x10`；②`xor $1;test;je <成长>` ⇒ 非成长 fall；
    // ③count 槽 -0xc（浅于 level）；④尾部两次独立 GetMaxLevel call（无具名
    // maxLevel 局部，P48）。
    int level = 0;
    if (!(m_isGrow == 1))
    {
        int count = m_script->GetGrowItemCountforLevelUp();
        if (count == 0)
            count = 0x19;
        level = exp / count + 1;
    }
    else
    {
        level = G_CDataManager()->get_creature_exp_level(exp);
    }
    if (m_script != 0)
    {
        if (m_script->GetMaxLevel() < level)
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
// [b28d-23 N·P133/P169] 分支极性/块序（ORIG jge 落 -0x18 岛 vs 4.4 jl 落共享尾）⇒ N。ORIG 0x08337724
// [b28d-solo-CCreatureMgr AE] P169/P59 早退形：`if(minLevel!=0){if(m_level<minLevel)return result;}` 复现双跳同落+早退；ORIG 0x8337724
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
            // [b28d-solo-CCreatureMgr P169/P59] ORIG 0x833778a/0x8337795 两条
            // 独立跳转同落赋值块（je/jge 833779c），fall 为早退
            // `mov -0x18(%ebp),%eax; jmp 83377ad` ⇒ 原码是「minLevel!=0 且
            // m_level<minLevel 早退」形；|| 合并形反产 jl 落共享尾（旧判词）。
            if (minLevel != 0)
            {
                if (m_level < minLevel)
                    return result;
            }
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
        // [b23S3C] ORIG 0x8338250：operator() 实参 0x8(%esp)=%esi(get_unique_id)、
        // 0xc(%esp)=%ebx(GetItemId)，即 vararg 序为 (uid, itemId)；交换实参只改
        // 求值次序、日志字段错位，属语义倒退 ⇒ 恢复 (uid, itemId)。
        cMyTrace(__PRETTY_FUNCTION__, 0x743, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n",
            user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int evolutionCreatureId = m_script->GetEvolutionCreatureId();
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // [b23S3C] P0 律：ORIG -0x10(%ebp) 存 CCreatureScript::GetItemId 结果后回读
    // 入参 ⇒ 原码为具名中间局部（缺它整帧上移 4，全函数槽号错位）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // [b23S3C] P197③：ORIG 0x83382c7 先 `movl $0x0,-0xc(%ebp)` 再取 InvenRef
    // ⇒ 声明置 0 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x763, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
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
        // [b23S3C] ORIG 0x8337e60/0x6d：vararg（GetItemId→%ebx、uid→%esi）在 ctor
        // call 之前求值、trace 对象占 -0x38 匿名槽 ⇒ 临时对象直调形（同 Evolute，
        // 具名 trace 局部会挪槽位并改求值次序）。
        cMyTrace(__PRETTY_FUNCTION__, 0x6a2, 5)(
            "cannot selectevolute because it haven't creature script uid(%d), "
            "creature id(%d) \n",
            user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    // [b23S3C] P197③：ORIG 0x8337ebd 分支前先 `movl $0x0,-0x18(%ebp)` ⇒ 声明置 0
    // + 各分支独立赋值（合并初始化式会丢预清行）。
    int targetCreatureId = 0;
    // [b23S3C] P191 极性：ORIG 0x8337ecd `cmp $0x4,%al; jbe +0x9a`(BType 为跳转目标)，
    // AType 走 fall-through ⇒ 先写 `> 4` 分支（等价 A/B 选择，块序对齐）。
    if (m_field4d > 4)
        targetCreatureId = m_script->GET_ATypeCreature();
    else
        targetCreatureId = m_script->GET_BTypeCreature();
    if (targetCreatureId == -1)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(targetCreatureId);
    if (nextScript == 0)
        return false;
    // [b23S3C] 同 Evolute：P0 律 -0x10 具名中间局部 + P197③ -0xc 预清两句式。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        // [b23S3C] 同上：ORIG 0x8338012 ctor 前求 uid→%ebx，trace 落 -0x28 匿名槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x6c9, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
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
// [b28d-solo-CCreatureMgr AE] 翻正三旋钮：①notify/canEvolute 去具名 bool（P0 表达式内联）；
// ②`IsAbleEvolute` int→bool（P248 档①，ORIG 调用点 0x8337d11 `test %al,%al`）；
// ③`({ levelUp && IsAbleEvolute(user); })` 语句表达式 + 嵌套 if 复现 ORIG 物化岛 + 直跳（P226）。
// 127=127 逐条全等。ORIG 0x8337bbc
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
    // P0：ORIG 0x8337c2x 的 notify 判定为直 `test %al,%al`（无槽往返）⇒ 表达式
    // 内联，禁具名 bool 局部（槽化形 cmpb 反劣）。
    if (m_level < m_script->GetMaxLevel() || levelUp)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    // 同上：canEvolute 无具名局部。
    // P226 条件向：ORIG 0x8337cfb..0x8337d23 对 `levelUp && IsAbleEvolute(user)`
    // 出 mov$1/mov$0 物化岛 + test;je（无槽）⇒ 语句表达式承载短路结果。
    if (({ levelUp && IsAbleEvolute(user); }))
    {
        if (!Evolute(user))
            return false;
    }
    m_saveFlag = 1;
    return true;
}

// ORIG _ZN13user_creature9CCreature19GainItemExpCreatureEiP5CUser T 0x83379c0
// （GainItemExpCreature.md）：仅非成长宠（m_isGrow==0）。A 型经验物品使
// m_field4d+1（钳 10）、B 型使其 -1（钳 0）；随后把 m_exp 低字节改写为
// m_field4d 并令高 16 位进度 +1（16 位回绕），以新进度查 GetExpLevel 定级；
// 通知包条件同成长宠；升级且可选择性进化则 SelectEvolute（失败返 0）；
// 最后置 m_saveFlag=1 返回 1。
// [b28d-solo-CCreatureMgr AE] 翻正四旋钮：①`char levelUp` + 函数头 `int newLevel=0`
// 预清（P198①）；②A/B 型钳位改 char 局部 + `++value/--value` 就地字节自增（P320 字节版）；
// ③packed 改 union 字节/半字视图（P197① 拆半）；④notify 内联 + P226 语句表达式嵌套 if。
// 157=157 逐条全等。ORIG 0x83379c0
bool CCreature::GainItemExpCreature(int exp, CUser* user)
{
    // ORIG 0x83379c4..0x83379cc：char levelUp(-0x11) 与 int newLevel(-0x10) 均函数头
    // 声明并预清（P198①），newLevel 为具名 int 局部（非表达式直传）。
    char levelUp = 0;
    int newLevel = 0;
    if (m_isGrow != 0)
        return false;
    if (m_script->validItem_AType(exp))
    {
        // ORIG 0x8337a0x：`mov %al,-0xa; addb $1,-0xa; cmpb $0xa,-0xa` ⇒ char
        // 局部就地自增（P320 字节版；int 局部会拆 movzbl/add/mov 三段）。
        char value = (char)m_field4d;
        ++value;
        if (value > 10)
            value = 10;
        m_field4d = (unsigned char)value;
    }
    if (m_script->validItem_BType(exp))
    {
        char value = (char)m_field4d;
        --value;
        if (value < 0)
            value = 0;
        m_field4d = (unsigned char)value;
    }
    // ORIG 0x8337a5x：低字节写 m_field4d + 高半字 +1 均为 union 字节/半字视图
    // （P197① 拆半；显式移位/掩码链恒多 5 条）。
    union ExpPack { int all; unsigned short h[2]; unsigned char b[4]; };
    ExpPack packed;
    packed.all = m_exp;
    packed.b[0] = m_field4d;                       // 低字节 ← m_field4d
    packed.h[1] = (unsigned short)(packed.h[1] + 1); // 高 16 位进度 +1（16 位回绕）
    m_exp = packed.all;
    newLevel = GetExpLevel((int)(short)packed.h[1]);   // cwtl 符号扩展
    if (m_level < newLevel)
        levelUp = 1;
    m_level = newLevel;
    // P0：ORIG 0x8337b0x 的 notify 判定无槽往返 ⇒ 表达式内联。
    if (m_level < m_script->GetMaxLevel() || levelUp != 0)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    // P226 条件向 + 嵌套 if：ORIG 0x8337b2x 对 `levelUp && IsAbleSelectEvolute`
    // 出物化岛（无槽），外层 `canEvolute && !SelectEvolute` 为直跳。
    if (({ levelUp != 0 && IsAbleSelectEvolute(user); }))
    {
        if (!SelectEvolute(user))
            return false;
    }
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
// [b28d-23 N·N82] 帧/槽差（79/81）⇒ N。ORIG 0x083373d4
// [b28d-solo-CCreatureMgr N] bool 规范化：`return pkt.Send()`（int→bool）4.1 直透 `mov %eax,%ebx` vs 4.4 test/setne；P183 去 value 槽与 GetUid 真调用已落；ORIG 0x83373d4
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
    // [b28d-solo-CCreatureMgr P0/P183] ORIG 0x8337470 `mov 0x10(%ebp),%eax;
    // mov (%eax),%ebx`（直读 out 形参入 callee-saved，无具名 value 槽）+ 
    // 0x833747b 外联 `call CCreatureItem::GetUid`（非 m_uid 直读）⇒ 删缓存
    // 局部、uid 改真调用（P167 调用点差集口径）。
    pkt.MakeNotipacketCreatureState(
        reinterpret_cast<CCreatureItem*>(this)->GetUid(), stomachValue);
    // [b28d-solo-CCreatureMgr P32] ORIG 0x8337499 `call Send; mov %eax,%ebx`
    // （bool 直透，无 test/setne）⇒ 去掉 `!= 0` 物化。
    return pkt.Send();
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
// [b28d-23 N·4.1-CSE] 三 artifact store 共享 %eax/%edx 基址（ORIG 跨语句 CSE）；P206 8B 视图形/register 钉基址均反劣 ⇒ N。ORIG 0x0869d614
// [b28d-solo-CCreatureMgr AE] P247 ArtifactTailView 12B 结构赋值复现共享基址+%ecx 缓存块拷贝；ORIG 0x869d614
CCreatureMgr& CCreatureMgr::operator=(const CCreatureMgr& src)
{
    m_creatureItems = src.m_creatureItems;
    m_equippedCreature = src.m_equippedCreature;
    m_user = src.m_user;
    // [b28d-solo-CCreatureMgr P247/P206] ORIG 0x869d644 三条 artifact store 共享
    // %eax(dest)/%edx(src)、value 走 %ecx（12B 块拷贝形）；逐元素赋值 4.4 恒
    // 每条重载双基址。改用 12B 视图结构整体赋值复现块拷贝（布局/mangled 不变）。
    struct ArtifactTriple { int m_w0, m_w1, m_w2; };
    struct ArtifactTailView { char m_pre[0x20]; ArtifactTriple m_art; };
    reinterpret_cast<ArtifactTailView*>(this)->m_art =
        reinterpret_cast<const ArtifactTailView*>(&src)->m_art;
    m_tempExpireDelta = src.m_tempExpireDelta;
    m_field30 = src.m_field30;
    m_isMaxLevelGrowCreature = src.m_isMaxLevelGrowCreature;
    return *this;
}

// ORIG Reset @ 0x833a45a。
// [b28d-solo-CCreatureMgr AE] 翻正钥匙 = 循环增量改后置 `it++`（P167：ORIG 增量段
// ppEi，前置 ppEv 会多/少一条并把后续槽整体上移）；96=96 逐条全等。ORIG 0x0833a45a
void CCreatureMgr::Reset()
{
    // [b28-BM7 判词] 残余 = N82/N89 复合：①ORIG 帧 0x48（迭代器槽 -0x1c/
    // -0x18 深排），4.4 紧凑 0x38 浅排（double framePad×2 实测反产 0x58/
    // -0x2c 更漂，P190 附注边界）；②begin/end/++/clear 调用 4.1 发射
    // out-of-line 模板实例（N89），4.4 内联。cMyTrace arg 已按 P118 改
    // __PRETTY_FUNCTION__（0x833a514 _ZZ 符号实证）。
    m_equippedCreature = 0;            // +0x18
    m_user = 0;                        // +0x1c
    m_tempExpireDelta = -1;            // +0x2c = -1
    if (m_field30.size() != 0)         // +0x30 map clear
        m_field30.clear();
    if (m_creatureItems.size() != 0)   // this+0x00 map<int,CCreatorItem*>
    {
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); it++)   // P167：ORIG 增量段为后置 ppEi
        {
            CCreatureItem* creatureItem = it->second;
            if (creatureItem != 0)
            {
                if (!GetInstanceCreatureItemFactory()->FreeCreatureItem(creatureItem))
                {
                    // P118/P139：ORIG 0x833a514 arg2 = `_ZZN13user_creature12
                    // CCreatureMgr5ResetEvE19__PRETTY_FUNCTION__` 局部静态。
                    LogManager::logFormat(1, "Creature.cpp",
                                          __PRETTY_FUNCTION__,
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
// [b28d-solo-CCreatureMgr AE] 翻正三旋钮：①`if (!sender.UseFeedCreature(...))` 内联判定
// + 早返（禁具名 ok，4.4 槽化 cmpb 反劣）；②cMyTrace 全表达式临时链（P227 vararg 跨 ctor
// 保活 %ebx）；③GetUid 内联实参（P183）。84=84 逐条全等。ORIG 0x08339dba
bool CCreatureMgr::UseFeed()
{
    // ORIG 0x8339dca/0x8339df8/0x8339e98 三失败路径共享尾 + 返回值 %ebx 汇聚：
    // 源码形为单个具名 char ok 复用（Ghidra cVar2 两次赋值 + `== 1` 比较），
    // 尾部 `return ok == 1;`（4.1 出 `xor $1;test;je` 与 `mov %ebx,%eax` 汇聚）。
    if (m_equippedCreature != 0)
    {
        int stomachValue = 0;
        if (m_equippedCreature->UseFeed(m_user, stomachValue))
        {
            CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
            // [b28d-solo P0/P183] ORIG 0x8339e1b `mov -0x1c(%ebp),%ebx` 后
            // 0x8339e27 外联 call GetUid，uid 无具名槽 ⇒ 内联实参。
            // ORIG 0x8339e3f `xor $1;test;je <成功>`：失败块 fall-through、
            // 判定整体内联（无具名 ok 局部，4.4 会槽化 cmpb 反劣）。
            if (!sender.UseFeedCreature(
                    reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(),
                    stomachValue))
            {
                // P227：ORIG 0x8339e54 vararg 在 ctor 前求值并跨 ctor 保活 %ebx。
                cMyTrace(__PRETTY_FUNCTION__, 0xab5, 5)(
                    "cannot write db uid(%d)\n",
                    m_user->get_unique_id() & 0xffff);
                return false;
            }
            sender.Send();
            return true;
        }
    }
    return false;
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
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833b204
// [b28d-solo-CCreatureMgr AE] P248 裸 register tick=expireTick 令值升格 callee-saved %ebx 跨 GetDateTimeTick call 保活（非绑定，P0 合规）；ORIG 0x833b204
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
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x20）
        cMyTrace("bool user_creature::CCreatureMgr::InsertCreatureItem("
                 "Inven_Item*, int, int, int, int)", 0xcaf, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    int tempTime = 0;
    if (days == 0 && expireTick != 0)
    {
        // [b28d-solo-CCreatureMgr P243/P0] ORIG 0x833b325 `mov 0x1c(%ebp),%ebx;
        // call GetDateTimeTick; mov %ebx,%edx; sub %eax,%edx` = 单句
        // `expireTick - GetDateTimeTick()`（两句式会多一次槽回读）。
        register int tick = expireTick;   // P248 裸 register（非绑定，P0 合规）
        tempTime = tick - OS_API::GetDateTimeTick();
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
bool CCreatureMgr::SendCreatureItemList() const
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
// [b28d-solo-CCreatureMgr N] ndiff=100：已翻正 ①两循环 begin/end 循环外缓存
// （P13）+ while 前置条件形（P25）；②每轮两次 operator->（P167 ptEv 差集清零）；
// ③第二循环 `it2++` 后置（P167 ppEi；第一循环 ORIG 为前置 ppEv，两者相反已分别对齐）。
// 残余 = N89（4.1 外联 map/迭代器模板实例 vs 4.4 内联）+ 未建模依赖：
// ORIG 循环体虚调用 `(**(code**)(*item+0xc))(item,&invenItem,dst,this+0x1c)` =
// CCreatureItem vtable 槽3（CEgg::SaveCreatureItem 0x83390c8 / CCreature::SaveCreatureItem
// 0x83387f6），并需 `Inven_Item invenItem;` 局部（p167 仍缺 Inven_ItemC1Ev）。
// 待专项批次：CCreatureItem 多态化（_ZTV 0x08c2f4d0，4 槽 Reset/IsSave/Load/Save，
// 需 CGameManager.h/CInventory.h/CreatureOnlySimple.cpp 多 TU 协同 + 移除 m_vptr 常量写）。
// 154 vs 134。ORIG 0x0833b80c
// [b28d-solo-CCreatureMgr R2上报] p167 MISSING -1 _ZN10Inven_ItemC1Ev：ORIG 有
//   `Inven_Item invenItem;` 局部 + 循环体 vtable 槽3 虚调用（CEgg::SaveCreatureItem
//   0x83390c8 / CCreature::SaveCreatureItem 0x83387f6）；需 CCreatureItem 多态化
//   （与 CCreatureC1 同一组头改动，已报 Main）。
// [b28d-solo-CCreatureMgr §6.2 封顶·检查点14 已落] Main 落多态化后 p167 CLEAR
//   （15=15：Inven_ItemC1Ev + 虚调用 SaveCreatureItem 到位）；154 vs 147（gap 7）。
//   已对齐：invenItem 函数体作用域（ORIG -0x61）、dst char* 算术形（P214①）、
//   第二 map end() 每轮求值（P13 反例，ORIG 0x833b9f6）。残余 = N82/N93 槽布局
//   （count -0x10 vs -0x18、it/end -0x1c/-0x20 vs -0x2c/-0x30、第二循环 -0x14 组）
//   + 尾部 epilogue（ORIG `lea -0x8(%ebp),%esp; pop esi` vs 4.4 `mov -0x4,%ebx; leave`）。
//   试形失败记录：内联 dst 表达式（去具名槽）帧 0x70→0x7c + push edi/esi，147→160 ⇒ 已回滚。
bool CCreatureMgr::GetCreatureItem(SIG_LOAD_INVENTORY* sig) const
{
    memset(sig->m_creature, 0, sizeof(sig->m_creature));   // ORIG 0x1ed8
    // [b28d-solo-CCreatureMgr] ORIG 0x833b8b4 `lea -0x61(%ebp); call Inven_ItemC1`
    // 在 memset 之后、begin 之前 ⇒ invenItem 是**函数体作用域**局部（非循环体内）。
    Inven_Item invenItem;
    // ORIG 0x833b8xx：begin/end 循环外各求值一次（P13）+ while 前置条件形
    // （P25/P164）；`it++` 后置（P28 增量侧：ORIG 增量段为 ppEi 三槽阶梯）。
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.begin();
    std::map<int, CCreatureItem*>::const_iterator itEnd = m_creatureItems.end();
    // [P191 双初始化配套] ORIG 0x833b86d/0x833b870 两条相邻 `movl $0x0,-0x10(%ebp)`
    //   ⇒ 原码 = `int count = 0;` 声明 + 循环前再清零一句，两处都要写。
    int count = 0;
    count = 0;
    while (it != itEnd && count <= 0x8c)
    {
        CCreatureItem* item = it->second;
        // [P183 地址重物化铁律] ORIG 0x833b89d..b8b2 在实参位重算 count*0x38 全链
        //   （`shl $3; lea (,%eax,8); sub; lea 0x12880(%edx); add 0xc(%ebp)`），
        //   **无 dst 具名局部**；第二处 store 亦重算（`lea 0x128a8(%edx)`）⇒
        //   [b28d-Main·检查点14 CCreatureItem 多态化] 虚调用 = vtable 槽3
        //   (SaveCreatureItem)，实参 `&invenItem([ebp-0x61]) / 重算 dst / m_user(+0x1c)`。
        //   [b28d-solo-CCreatureMgr 旧试形失败] 那次「内联」同时保留了 dst 的旧注释形，
        //   帧 0x70→0x7c ⇒ 本次按 ORIG 两处各自独立重算。
        item->SaveCreatureItem(
            &invenItem,
            &reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                sig->m_creature)[count],
            m_user);
        // ORIG 每轮两次 operator->（P167 差集：ptEv 少 1）——getExpireTime 经
        // 第二次 it->second 取，勿复用上面缓存的 item 指针。
        // [L23 本轮试形记录] 三形实测（fresh .o）：①dst 具名缓存=147 条（少算
        //   两处重算链，与 ORIG 结构不符）；②本形（两处内联元素指针）=161 条；
        //   ③`count*0x38 + sig->m_creature` 交换操作数=165 条（4.4 仍 base-first
        //   且多 %esi 中转）⇒ 取②，结构对齐 ORIG（无缓存 + 两处独立重算）。
        reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
            sig->m_creature)[count].m_expireTime =
            it->second->getExpireTime();  // 条目 +0x28
        // P167：第一个 map 的 ORIG 增量段是前置 ppEv（第二个 map 才是 ppEi）。
        ++it;
        ++count;
    }
    sig->m_creatureCount = count;

    int eggCount = 0;
    std::map<int, int>::const_iterator it2 = m_field30.begin();
    // [P13 反例] ORIG 0x833b9f6/0x833ba08 第二个 map 的 end() 在**循环内每轮**
    //   求值（`lea 0x30(%eax); call end(); sub $4` 位于条件块），非循环外缓存
    //   （与第一循环相反）；it2End 具名局部使静态少 5 条 ⇒ 改每轮调用。
    while (it2 != m_field30.end() && eggCount <= 0x13)
    {
        // ORIG：mov %edx,0x4(%eax,%ecx,8) / mov %edx,0x8(%eax,%ecx,8)，
        // %ecx = eggCount+0x2a68 → SIG_POSTAL_CREATURE_TEMP_PERIOD[eggCount]
        // （m_uid @+0x15344 / m_tempPeriod @+0x15348，均 int）。
        sig->m_tempPeriods[eggCount].m_uid = it2->first;
        sig->m_tempPeriods[eggCount].m_tempPeriod = it2->second;
        it2++;
        eggCount++;
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
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833ab9a
// [b28d-solo-CCreatureMgr AE] P183 去 characNo 具名槽（实参直传）+ P221 全表达式临时链（trace 槽 -0x2c）；ORIG 0x833ab9a
bool CCreatureMgr::RecvMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // [b28d-solo-CCreatureMgr P0/P183] ORIG 0x833ac0f `call getCurCharacNo` 结果
    // 经 %eax 直压 0x8(%esp)（无 characNo 具名槽）⇒ 删局部、内联实参。
    if (!sender.RecvMail(item, m_user->getCurCharacNo(), slot, reason, mapSize))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x2c 浅于 sender 邻域）
        cMyTrace(__PRETTY_FUNCTION__, 0xc09, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
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
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833aa32
// [b28d-solo-CCreatureMgr AE] P194 尾判 `if(!DeleteCreatureItem())return false;return true;` + P221 临时链 + P183 去 characNo 槽；ORIG 0x833aa32
bool CCreatureMgr::SendMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // [b28d-solo-CCreatureMgr P0/P183] 同 RecvMail：characNo 无具名槽。
    if (!sender.SendMail(item, m_user->getCurCharacNo(), slot, reason, mapSize))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x1c）
        cMyTrace(__PRETTY_FUNCTION__, 0xbed, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // [b28d-solo-CCreatureMgr P194] ORIG 0x833ab56 `xor $1;test;je <mov$0>;mov$1`
    // ⇒ 原码是 `if (!DeleteCreatureItem(...)) return false; return true;`
    // （直返透传形无此三段）。
    if (!DeleteCreatureItem(item->m_addInfo2, reason))
        return false;
    return true;
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
// [b28d-solo-CCreatureMgr AE] 翻正两旋钮：①块序改 `if (it == end) { fail; return false; }`
// 早返（失败块 fall-through、成功块跳转目标，与 ORIG 一致）；②`it->second` 存具名局部
// （P0 槽往返：ORIG `mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp)`）；68=68 逐条全等。
// ORIG 0x0833a854
bool CCreatureMgr::UnregisterCreatureItem(int uid)
{
    std::map<int, CCreatureItem*>::iterator it = m_creatureItems.find(uid);
    // ORIG 0x833a8xx 块序：失败块在前（fall-through）、成功块在后（跳转目标）
    // ⇒ 写 `if (it == end) { fail; return false; }` 早返形。
    if (it == m_creatureItems.end())
    {
        // [b28d-solo P227] 全表达式临时链（ORIG 变参跨 ctor 保活形）。
        cMyTrace(__PRETTY_FUNCTION__, 0xbc1, 5)(
            "UnregisterCreatureItem failed. uid(%d), key(%d)\n",
            m_user->get_unique_id() & 0xffff, uid);
        return false;
    }
    // ORIG 0x833a8xx：`mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp)` ⇒ it->second
    // 存具名局部（P0 槽往返），FreeCreatureItem 实参从该槽回读。
    CCreatureItem* creatureItem = it->second;
    GetInstanceCreatureItemFactory()->FreeCreatureItem(creatureItem);
    m_creatureItems.erase(it);
    return true;
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
            case 0: GlobalData::s_stream_pool->Free(m_stream); break;
            case 2: GlobalData::s_big_stream_pool->Free(m_stream); break;
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
// [b28d-23 N·P30/N4] 临时 this 取址：ORIG `lea 0x4(%eax),%edx` vs 4.4 `add $0x4,%eax`（+1 mov）⇒ N。ORIG 0x0833ff34
// [b28d-solo-CCreatureMgr AE] P235 m_itemView.setCopy 出 ORIG lea（N107 cast 形恒 add 的边界）；ORIG 0x833ff34
bool CDBMsgSender::RecvMail(Inven_Item* item, int characNo, int slot,
                            int reason, int mapSize)
{
    // ORIG 0x833ff3d：头部 m_stream 判空（mov (%eax);test;jne），buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833ff71：lea 0x4(%eax),%edx 临时 this；arg2 经 %eax（0xc 形参晚装）。
    // 【asm("eax") 绑定违规清零】batch-19 曾以 register asm 换 ndiff 8→4，
    // 硬约束禁 asm 绑定已移除；compat 局部形探针使 buf 槽 -0xc→-0x10 全帧
    // 错位（N82）反劣；裸形残差 = P30 lea/add 装载序 + N82 槽号 3 处。
    // [b28d-solo-CCreatureMgr P235] cast 形改具名视角成员调用（完整类型）：
    // ORIG `lea 0x4(%eax),%edx`（N107 add-vs-lea 六 cast 形恒 add）。
    buf->m_itemView.setCopy(*item);
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
// [b28d-23 N·P30/N4] 同 RecvMail：lea-vs-add 不可翻 ⇒ N。ORIG 0x0833fe8c
// [b28d-solo-CCreatureMgr AE] P235 m_itemView.setCopy 出 ORIG lea（同 RecvMail）；ORIG 0x833fe8c
bool CDBMsgSender::SendMail(Inven_Item* item, int characNo, int slot, int reason,
                            int mapSize)
{
    // ORIG 0x833fe95：头部 m_stream 判空，buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833febb：lea 临时 this（无 compat 槽）。
    // 【asm("eax") 绑定违规清零】同 RecvMail 回裸形（compat 局部形 N82 反劣）；
    // 另 +0x2b 处 ORIG `xor$1;test;je` 常量岛在 4.4 折成 bool^1 直落形。
    buf->m_itemView.setCopy(*item);   // [b28d-solo P235] 具名视角出 lea（N107 边界）
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
// [b28d-23 NEAR·N76] 同 HatchCreature：+0x8 store 装载序不可翻。ORIG 0x0833fd74
// [b28d-solo-CCreatureMgr AE] P235 具名视角（同 HatchCreature）；ORIG 0x833fd74
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
    // [b28d-solo-CCreatureMgr P235] cast 形改具名视角直访问（完整类型成员）：
    // ORIG `mov 0x10(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,0x8(%eax)`。
    buf->m_itemView.m_itemIdx = itemIdx;              // buf+0x08 disp8
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
// [b28d-23 N·N81/N76] 临时 this lea-vs-add + store 装载序 ⇒ N。ORIG 0x0833faaa
// [b28d-solo-CCreatureMgr AE] +0x48 m_characNo 在当前头集/路由下与 ORIG 逐条对齐（曾因 edx 中转差异判 N；字段 unsigned 化会破坏 RecvMail/SendMail 同字段，P248 两端冲突，勿改）；ORIG 0x833faaa
bool CDBMsgSender::InsertCreatureItem(CUser* user, Inven_Item* item, int slot,
                                      int reason, int mapSize, int days,
                                      int expireTick)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG：`lea 0x4(%eax),%edx` 临时 this（无 compat 槽）。【N96/P0 违例清零】
    // 历史 register asm("eax") 钉装填序已删，回与 RecvMail/SendMail 同构裸形；
    // 残差 lea-vs-add + store 序 = N81/N76。
    buf->m_itemView.setCopy(*item);   // [b28d-solo P235] 具名视角出 lea（N107 边界）
    buf->m_slot = slot;                               // +0x2c
    // [b28d-solo-CCreatureMgr N·N7/N18a 封顶] ORIG `call; mov %eax,%edx;
    // mov -0x14(%ebp),%eax; mov %edx,0x48(%eax)`（call 返回值经 edx 中转 + 基址后载）
    // vs 4.4 `mov -0x14(%ebp),%edx; mov %eax,0x48(%edx)`。已试形：裸 int（同型
    // LHS-first）、(unsigned int) cast、enum cast、语句表达式（反产 lea+mov 各 1 条）
    // 均不闭合；字段 unsigned 化可翻正本点但使 RecvMail/SendMail 同字段反向
    // （P248 两端冲突反例，实测回退）⇒ 跨版本 call 返回值寄存器分配差异封顶。
    buf->m_characNo = user->getCurCharacNo();  // +0x48
    buf->m_stomachValue = 100;                        // +0x44（固定 100）
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_days = days;                               // +0x54
    buf->m_expireTick = expireTick;                   // +0x58
    // [b28d-solo-CCreatureMgr P211] 去冗余 (int) cast：字段已 unsigned 同型化
    // （ORIG 0x833fb65 LHS-first 直存，无 edx 中转）。
    buf->m_ceraOrderNo = user->get_cera_item_order_no();       // +0x60

    // 【N96/P0 违例清零】register asm("ebx") 已删；P197② 裸语句表达式复现
    // ORIG `mov 0x2(%eax),%eax; mov %eax,%ebx`…`mov %ebx,0x4(%esp)` callee-saved 直传。
    CItem* found = G_CDataManager()->find_item(({ item->m_addInfo; }));  // +0x02
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
// [b28d-23 N·P133] 分支布局/极性（get_sub_type 两跳同落）⇒ N。ORIG 0x08336eae
// [b28d-solo-CCreatureMgr N] N111：ORIG switch 决策链残留死 `test %eax,%eax`（4.4 不产死指令，补 case 0 被折叠）；ORIG 0x8336eae
// [b28d-solo-CCreatureMgr 复测] 58 vs 57（差即该死 test）；case 0 已试折叠 ⇒ 维持 N111 封顶。
bool CCreatureItemConverter::SetEggItem(Inven_Item* item)
{
    // ORIG 0x08336eae：入口无 null 检查（直 `mov 0x2(%eax),%eax`），删臆造判空。
    // ORIG push %ebx：`mov 0x2(%eax),%eax; mov %eax,%ebx` 跨 call 直落寄存器
    // （P0 无槽往返）⇒ P197② 裸语句表达式实参形（CArtifact::EquipArtifact 同法实证）。
    CItem* found = G_CDataManager()->find_item(({ item->m_addInfo; }));
    if (found == 0)
        return 0;
    if (found->is_stackable())
        return 0;

    // ORIG：`mov -0x14,%eax; mov %eax,-0x10(%ebp); mov -0x10(%ebp),%eax; call`
    // equip 具名槽 -0x10；subType 无槽，switch 三级下探 cmp1;je;cmp2;je;test
    // （技巧 34 直写 switch）。
    // [batch-19 N 封顶 ndiff=1] ORIG +0x61 的 `test %eax,%eax` 是**死指令**：
    // 其后紧跟无条件 `jmp +0xa7`（= `mov $1,%eax` 出口），flags 无消费者 ⇒
    // 4.1 为 `case 0:`/`default:` 同体生成的合并比较残留。实测：补
    // `case 0: break;` 后 4.4 直接折叠该比较（仍 64 insn，ndiff 不变）⇒
    // 源码层无法保留死 test，封顶。
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
// [b28d-solo-CCreatureMgr P248档①] ORIG 调用点 0x8337d11 `test %al,%al` ⇒ bool。
bool CCreature::IsAbleEvolute(CUser* user) const
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
bool CCreature::IsAbleSelectEvolute(CUser* user)
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
bool CCreatureMgr::VerifyEquipedCreatureQuest() const
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
CREATURE_ITME_TYPE CCreatureItem::GetCreatureItemType()
{
    return m_creatureItemType;   // 同型直返（enum）
}

// ORIG _ZN13user_creature14COverSkillTime9checkTimeEv T 0x0833d65e：
// (float)(getCurSec()-m_time) ∈ (0.0, 20.0)（0x8c2f468 处常量 = 20.0f）→ 1。
// [b28d-23 N·N85] x87 比较 lowering（两处 fucompp/fnstsw）⇒ N。ORIG 0x0833d65e
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering；ORIG 0x833d65e
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
// [b28d-solo-CCreatureMgr N] ndiff=2：已翻正 ①`it++` 后置（P167）、②`a!=0 && b==1`
// 短路物化正向包裹（P214①）、③去具名 creature 局部（P183）、④UseKeepRight 双实参
// 内联（P0）、⑤cMyTrace 全生命周期临时链（P227）。唯一残余 = SetNoCharge(false) 的
// 立即数与 this 装载序（ORIG `movl $0,0x4(%esp)` 先 / 4.4 先备 this；P75/N-G 跨版本
// 装载序，1 条位置差）。156=156。ORIG 0x0833c00c
// [b28d-solo-CCreatureMgr P277 试形失败] 具名派生指针（CCreature* creature = ...）
// 156→158 条 + 槽基 -0x28→-0x2c（多 4B 局部）⇒ 已回滚，封 N（P75/N-G）。
bool CCreatureMgr::SetKeepRightItem(CUser* user, Inven_Item* item)
{
    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)   // P167：ORIG 增量段为后置 ppEi
    {
        CCreatureItem* creatureItem = it->second;
        // ORIG 0x833c03f..0x833c061：`creatureItem!=0 && type==1` 短路物化
        // （mov$1/mov$0 + test;je）正向包裹，勿写 `==0 || !=1` 的 continue 形。
        // 同一对象按 CCreature 语义访问（ORIG 直呼 CCreature 访问器，单继承布局
        // 同址；每次重发 reinterpret_cast 表达式，无具名 creature 局部）。
        if (creatureItem != 0 && creatureItem->GetCreatureItemType() == 1)
        {
        if (reinterpret_cast<CCreature*>(creatureItem)->IsNoCharge())
        {
            // [P277 试形失败] 具名派生指针 156→158 条 + 槽基 -0x28→-0x2c（多 4B 局部）
            // ⇒ 回滚；残余 = P75/N-G 跨版本实参装载序（ORIG `movl $0,0x4(%esp)` 先）。
            reinterpret_cast<CCreature*>(creatureItem)->SetNoCharge(false);
            CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
            // P0/P183：ORIG 0x833c0xx `movzbl %al,%ebx` + `call GetUid` 均无槽往返
            // ⇒ 两个实参表达式内联（右起求值：noCharge 先入 %ebx）。
            if (!sender.UseKeepRight(
                    creatureItem->GetUid(),
                    (int)(unsigned char)reinterpret_cast<CCreature*>(creatureItem)
                        ->IsNoCharge()))
            {
                // P227：ORIG vararg 在 ctor 前求值并跨 ctor 保活 %ebx ⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0xdf0, 5)(
                    "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
                return false;
            }
            sender.Send();
            CPacketRespondent pkt(m_user, 0, 0);
            pkt.MakeNotipacketCreatureItemList(&m_creatureItems,
                                               creatureItem->GetSlotNo());
        }
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
// [b28d-solo-CCreatureMgr AE] 既有（P0 具名 int 槽 + goto 块序）；compare_tu AE；ORIG 0x833c31a
// [b28d-solo-CCreatureMgr N112] game_func_report 口径多 1 条 4.4 对齐 nop（jmp 后
// mov$0 岛前）；compare_tu 已 AE（-falign-jumps 不可控，N112 剥离口径）。
int CCreatureMgr::GetExtraGold(int gold) const
{
    if (m_equippedCreature == 0)
        goto RET0;                          // ORIG je→+0xfc（default 岛）
    if (m_equippedCreature->IsDieCreature())
        return 0;
    // [b28d-23 翻正] ORIG 每个返回块为 `mov %eax,-N(%ebp); mov -N(%ebp),%eax;
    // jmp end`（具名 int 局部槽往返，P0）且 4 块各占独立槽
    // -0x18/-0x14/-0x10/-0xc（N82：4.1 不复用槽）。跳表实测（ORIG .rodata
    // 0x8c2d9f4，43 项）：块序 = {0x16,0x17,0x18}*5 → {0x2a,0x3f}*3 →
    // {0x40}*5 → {0x36,0x37,0x38}*3，故按块序书写 case 并各给具名局部。
    switch (m_equippedCreature->GetCreatureId())
    {
        case 0x16:
        case 0x17:
        case 0x18:
        {
            int extra = gold * 5 / 100;
            return extra;
        }
        case 0x2a:
        case 0x3f:
        {
            int extra = gold * 3 / 100;
            return extra;
        }
        case 0x40:
        {
            int extra = gold * 5 / 100;
            return extra;
        }
        case 0x36:
        case 0x37:
        case 0x38:
        {
            int extra = gold * 3 / 100;
            return extra;
        }
        default:
            goto RET0;
    }
RET0:
    return 0;
}

// ORIG _ZNK13user_creature12CCreatureMgr21GetOverSkillExtraGoldEi T 0x0833c6f6：
// 装备宠存活且 itemId==0xf68a 且超技能计时有效时给 *5/100，否则 0。
// [b28d-solo-CCreatureMgr AE] 既有（裸比较 + goto 钉块序）；compare_tu AE；ORIG 0x833c6f6
// [b28d-solo-CCreatureMgr N112] 同上：多 1 条 4.4 对齐 nop，compare_tu 口径已 AE。
int CCreatureMgr::GetOverSkillExtraGold(int gold) const
{
    // ORIG 0x833c704：四步分断——creature==0/ItemId 不符/checkTime 假三条
    // je→共享尾 mov0 岛(0x833c777)；IsDie 真值走中段 `mov0;jmp end` 物化岛
    // （技巧 40 &&-materialize 形）；0xf68a 比较为 sete;test;je（技巧 9/22，
    // bool 取反物化而非直接 jne）。
    // ORIG 0x833c704 je→尾共享 mov0(0x833c777)：goto 钉块序（技巧 23）；
    // IsDie true → 中置 `mov0;jmp end` 物化（技巧 40）；match 用 sete;test;je
    // 直判（技巧 22，`return 0` 形反产 xor+物化岛）；checkTime 正向 fall=gold。
    // [b28d-23] P0/N96 清绑：删 register asm("al")。裸比较 `f()==K` 在 4.4 即产
    // ORIG 的 `cmp $0xf68a,%eax; sete %al; test %al,%al; je`（探针 /tmp/p23_b.cpp
    // a 形，ORIG 0x833c732 段），零栈槽、无需任何绑定。
    if (m_equippedCreature == 0)
        goto RET0;
    if (m_equippedCreature->IsDieCreature())
        return 0;
    // 正向 if（技巧 19/22）：`if(!match)` 反产 sete;xor;jne 翻转形，
    // 正向落 test %al,%al; je RET0（ORIG 0x833c735）。
    if (reinterpret_cast<user_creature::CCreatureItem*>(m_equippedCreature)
            ->GetItemId() == 0xf68a)
    {
        if (m_equippedCreature->GetOverSkillTime()->checkTime())
            return gold * 5 / 100;
    }
RET0:
    return 0;
}

// ORIG _ZN13user_creature12CCreatureMgr12SwapCreatureEP10Inven_ItemiiS2_ii
// T 0x08339420：双物品含宠物装备(类型 5)时校验槽位/换装，随后交换槽号并落库。
// [b28d-solo-CCreatureMgr N] ndiff=14：已翻正 ①uidLow 具名 short 局部改内联实参
// （P200 `movzwl %ax,%ebx` 直落 callee-saved）、②两处 cMyTrace 全表达式临时链
// （P227 vararg 跨 ctor 保活）、③尾部 `if(!SwapCreatureItem(...))return false;`
// 内联判定+早返（P194，删具名 ok 槽）。残余 14 行 = 三处实参中转寄存器名
// （ORIG `mov X,%eax; mov %eax,disp` vs 4.4 `mov X,%edx; mov %edx,disp`；
// N7/P274 寄存器角色差，语义全等）。188=188。ORIG 0x08339420
// [b28d-solo-CCreatureMgr P277 试形失败] SetSlotNo receiver 加 static_cast 同类型形
// 不改变装载序（189 条不变，仍实参先压 vs ORIG this-first）⇒ 已回滚，封 N（P75/N-G）。
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
            // P200：ORIG 0x83394xx `movzwl %ax,%ebx` 直落 callee-saved（无槽往返）
            // ⇒ 短整表达式作普通实参直传，禁具名中间局部。
            // P227/P87：ORIG 在 ctor 前 `movzwl %ax,%ebx` 求值 uid 并跨 ctor
            // 保活 ⇒ 全表达式临时链。
            cMyTrace("bool user_creature::CCreatureMgr::SwapCreature("
                     "Inven_Item*, int, int, Inven_Item*, int, int)",
                     0x9a5, 5)(
                "IsCreatureEquipmentScope() uid(%d) slot_left(%d)\n",
                static_cast<unsigned short>(m_user->get_unique_id()), slot1);
            return false;
        }
        if (!IsCreatureEquipmentScope(slot2))
        {
            cMyTrace("bool user_creature::CCreatureMgr::SwapCreature("
                     "Inven_Item*, int, int, Inven_Item*, int, int)",
                     0x9ab, 5)(
                "IsCreatureEquipmentScope() uid(%d) slot_right(%d)\n",
                static_cast<unsigned short>(m_user->get_unique_id()), slot2);
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

    // [P277 试形失败] static_cast 同类型形不改变装载序（仍实参先压，189 条不变）⇒
    // 回滚；残余 = ORIG 0x8339613/0x833962b this-first 装载序（P75/N-G 跨版本）。
    if (left != 0)
        left->SetSlotNo(slot2);
    if (right != 0)
        right->SetSlotNo(slot1);

    CDBMsgSender sender(static_cast<ENUM_INTERNALPACKET>(0x8c), m_user);
    // ORIG 0x8339679 `xor $1;test;je <成功>` + ebx 汇聚（mov $0/$1 → mov %ebx,%eax）
    // ⇒ 内联判定 + 早返形（P63/P194），禁具名 ok 局部。
    if (!sender.SwapCreatureItem(m_user, item1, slot1, item2, slot2))
        return false;
    sender.Send();
    return true;
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
bool CCreatureMgr::ResponseCreature()
{
    CPacketRespondent respondent(m_user, 0, 0);
    respondent.MakeNotipacketResponseCreature();
    // ORIG 0x833a726/0x833a74c：area 与 Broadcast 返回值两段生命周期复用 %ebx
    // （无栈槽，frame 0x30、respondent@-0x28）。
    // [b28d-23] P0/N96 清绑：删 asm("ebx") 绑定；按 P231/P248 保留**纯 register**
    // 存储建议（非绑定形，P0 合规）：值跨 getCurCharacVill call 保活且 4.4 不自升格
    // （三条件齐备）。
    // [b28d-solo-CPacketRespondent 消费点同步] Broadcast 返回 bool（ORIG 0x8340404
    // gate 臂直透 `call CUser::Send` 无 movzbl）⇒ 直接 return 让 4.1/4.4 都走
    // `call Broadcast; mov %eax,%ebx; <dtor>; mov %ebx,%eax`（返回值跨析构保活，
    // bool 无需归一化）；area 单独用 register int 落 %ebx（get_area 返回 int）。
    register int area;
    area = m_user->get_area(false);
    return respondent.Broadcast((int)(char)m_user->getCurCharacVill(), area);
}

// ORIG _ZN13user_creature12CCreatureMgr20RegisterCreatureItemEPNS_13CCreatureItemEi
// T 0x0833a77e：uid 查重后插入 m_creatureItems；重复则 cMyTrace 报错返回 false。
// [b28d-23 N·N18a/N76] 查重物化后 uid/get_unique_id 装载序 ⇒ N。ORIG 0x0833a77e
// [b28d-solo-CCreatureMgr AE] P227 全表达式临时链令变参在 ctor 前求值并 callee-saved 保活（删具名 uniqBx）；ORIG 0x833a77e
bool CCreatureMgr::RegisterCreatureItem(CCreatureItem* item, int uid)
{
    // ORIG 0x833a798：`test;setne %al;test;je` 查重物化（技巧 22）；true 块
    // uid→%esi（0x833a7a1 `mov 0x10(%ebp),%esi`）、get_unique_id 结果
    // `movzwl %ax,%ebx`（0x833a7b2）跨 ctor call 存活，trace 变参寄存器直传
    // PF 串 @0x8c2f140。
    // 【N96/P0 违例清零】历史 register asm("esi"/"ebx"/"al") 绑定形已删；
    // 自然局部形下 4.4 的 uid 槽往返/movzwl 二段/spill 中转均不可钉寄存器，
    // 残余为 N18a/N76 跨版本装载序。
    // [b28d-solo-CCreatureMgr P227/P194] ORIG 0x833a7a1 `mov 0x10(%ebp),%esi`
    // （uid 跨 ctor 保活）+ 0x833a7b2 `movzwl %ax,%ebx`：全表达式临时链令
    // 4.4 在 ctor 前求实参并以 callee-saved 保活（具名 uniqBx 反产槽预清+往返）。
    if (FindCreatureItem(uid) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbb4, 5)(
            "RegisterCreatureItem failed. uid(%d), key(%d)\n",
            (unsigned short)m_user->get_unique_id(), uid);
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
// [b28d-23 NEAR·N76] +0x8 store 装载序：ORIG 0x833fd0f RHS-first vs 4.4 LHS-first（槽基址形）；探针 /tmp/p23_probe.cpp k2 证仅形参基址 RHS-first，槽基址恒 LHS-first ⇒ N76。ORIG 0x0833fcc2
// [b28d-solo-CCreatureMgr AE] P235 SIG_CREATURE_ITEM::m_item 匿名 union 具名视角 m_itemView 复现 +0x08 store RHS-first；ORIG 0x833fcc2
bool CDBMsgSender::HatchCreature(int uid, int flag, int itemIdx)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x244;   // ORIG asm 833fcf0：or $0x244（0x200|0x40|0x4）
    buf->m_addInfo2 = uid;                            // +0x30
    buf->m_flag50 = (unsigned char)flag;              // +0x50
    // ORIG 0x833fd0f：无 compat 槽，`mov %edx,0x8(%eax)` buf 基址直存（具名视图折叠）。
    // [b28d-solo-CCreatureMgr P235] cast 形（base 先装载）改具名视角完整类型直访问。
    buf->m_itemView.m_itemIdx = itemIdx;              // buf+0x08
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
// [b28c-30 SIGFIX] void→bool（P174 串定 + Ghidra C `return bVar5`）：失败路径
// 返 false、成功路径返 RenameCreatureItem 的 ok；mangled 不含返回类型。
// [b28d-solo-CCreatureMgr N] ndiff=2：已翻正 ①useRet 函数头预清（P198①）；
// ②len/area/vill/uid/inven 五具名局部全部内联（P0/P183，帧 0x70→0x50 与 ORIG 一致）；
// ③RenameCreatureItem 内联判定+早返（P194）、cMyTrace 临时链（P227）。
// 唯一残余 = ORIG 对槽比 `useRet != 0` 多 `setne %al; test %al,%al` 物化岛
// （4.4 恒直 je，setcc-fold 族 N；探针 `if(useRet)`/`(bool)useRet` 同形）。
// [b28d-solo-CCreatureMgr AE] P225 while+return 形复现 4.1 槽比零 bool 物化
// （ORIG 0x8339bf4 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`）；
// 162=162，compare_tu DIFF→AE。ORIG 0x08339b6e
bool CCreatureMgr::RenameCreature(const char* name, int creatureIdx, int slot)
{
    CPacketRespondent respondent(m_user, 1, 0x67);
    // ORIG 0x8339bb4 `movl $0x0,-0xc(%ebp)`：useRet 函数头预清（P198①）。
    int useRet = 0;
    if (m_equippedCreature == 0)
    {
        respondent.SetError(4);
        return false;
    }
    // ORIG 0x8339bc4 段 getCurCharacInvenW 结果直用（无具名 inven 槽）⇒ 内联链。
    useRet = m_user->getCurCharacInvenW()->use_item(creatureIdx, 1, slot);
    // [P225 试形] ORIG 0x8339bf4 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`：
    // 4.1 槽比零 bool 物化，4.4 对 if 恒折 cmp/jne 直跳 ⇒ while+return 形。
    while (useRet != 0)
    {
        respondent.SetError(useRet);
        return false;
    }
    m_equippedCreature->RenameCreature(name);
    // P0/P183：ORIG 的 len/area/vill/uid 均无槽往返（Ghidra sVar2/iVar3/cVar1
    // 复用寄存器）⇒ 表达式直传，禁具名中间局部。
    respondent.MakeNotipacketRenameCreature(name, (int)strlen(name));
    respondent.Broadcast((int)(char)m_user->getCurCharacVill(),
                         m_user->get_area(false));
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // ORIG 0x8339ccd 段 `xor $1;test;je <成功>`：失败块 fall-through、判定内联。
    if (!sender.RenameCreatureItem(
            reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(), name))
    {
        cMyTrace(
            "bool user_creature::CCreatureMgr::RenameCreature(const char*, int, int)",
            0xaa0, 5)("cannot write db uid(%d)\n",
                      m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
        return false;
    }
    sender.Send();
    respondent.MakeCmdpacketRenameCreature(creatureIdx, slot);
    return true;
}

}  // namespace user_creature

// ============================================================================
// Inven_Item_Compatible（全局域，ORIG 0x833f38e/0x833f418；U3Creature 批次实现）
// ============================================================================

// ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e。
// [b28d-23 NEAR·N76] 语句序照 ORIG 0x833f38e 重排 + m_amp0Int 具名视角（54→47 条）；残余 1 = +0x11 装载序 ⇒ N76。ORIG 0x0833f38e
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f38e
void Inven_Item_Compatible::setCopy(const Inven_Item& item)
{
    // [b28d-23 翻正] 语句序照 ORIG 0x833f38e 逐条发射序（-O0 书写序=机器码序）：
    // +0x0/+0x1/+0x4/+0xc/+0x10 → GetItemAttr(+0x8) → +0x14 → guid 结构赋值
    // (+0x1c/0x20/0x24) → +0x18（m_amp0 置后）。原按偏移升序书写致整段错位。
    m_field0 = item.m_field0;
    m_type = item.m_field1;
    m_itemIdx = item.m_addInfo;
    m_addInfo2 = item.m_addInfo2;
    m_endurance = item.m_fieldb;
    m_attr = item.GetItemAttr();
    m_fieldd = item.m_fieldd;
    // ORIG（objdump 0x833f3ef..0x833f412）：guid 三字段为单条结构赋值形
    // （双基址各载一次 + 三连续 store）；+0x11 的 m_amp0 独立重载置后。
    m_guid = item.m_amp.m_guid;                                     // item+0x15/0x19/0x1d
    // [b28d-solo-CCreatureMgr P247] 4B 结构赋值形（LHS-first）复现 ORIG 尾段
    // `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov 0x11(%edx),%edx;
    //  mov %edx,0x18(%eax)`；标量 int 形恒 RHS-first（残余 2 行根因）。
    m_amp0 = item.m_amp.m_amp0Head;                                 // item+0x11（4B 结构视角）
}

// ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418。
// [b28c-30] N76 封顶：末段 `*reinterpret_cast<int*>(&item.m_amp) = m_amp0;` ORIG
// 为 `mov %edx,0x11(%eax)` disp 直写；取址形反产 `add $0x11,%eax; mov %edx,(%eax)`
//（+1 条）。具名 int 视角可去 add 但装载序变 RHS-first（ORIG LHS-first），净差 1
// ⇒ 封顶（前两段 guid 结构赋值 + 两字段同型化已 AE）。
// [b28d-23 NEAR·N76] m_amp0Int 具名视角消 add $0x11（54→47 条）；残余 1 = +0x11 store 装载序（ORIG LHS-first vs union 视角 RHS-first，int/unsigned 同异型试尽）⇒ N76。ORIG 0x0833f418
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f418
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
    // ORIG 0x833f476：单条结构赋值（双基址各载一次 + 三连续 store）——具名
    // guid 视图成员直赋值（三独立字段赋值 4.4 恒逐条重载双基址）。
    item.m_amp.m_guid = m_guid;                                      // → item+0x15/0x19/0x1d
    // [b28d-solo-CCreatureMgr P247] 同 setCopy：4B 结构赋值形（LHS-first）。
    item.m_amp.m_amp0Head = m_amp0;                                  // → item+0x11（ORIG 置后独立重载）
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

// [b28d-23 N·N7] 虚调用目标寄存器 %eax/%edx 互换 + 立即数重定位（.o 显示差）⇒ N。ORIG 0x0833f57c
// [b28d-solo-CCreatureMgr N] N7：间接 call 目标 4.4.7 恒 %eax vs ORIG %edx（P230 补称 4.4.6 后端可修）；ORIG 0x833f57c
// [b28d-PoolSmall·Main 批准的 4.4.6 路由实验 → **负例，勿再试**] 本 TU 现路由
//   C6(4.4.7)+-march=i486（x87 族所需）。两种 4.4.6 组合均整 TU 实测（未动路由表）：
//     · **c6446r**（4.4.7 驱动 + `-B cc1plus446bin/`，头集仍 4.4.7）
//         ⇒ identical 180 / ae **410** / near 1 / diff **13**
//     · 4.4.6 后端 **+ 4.4.6 头集**（-isystem 三项原位替换）
//         ⇒ identical 180 / ae **410** / near 1 / diff **13**
//   基线 4.4.7 = 180 / 414 / 1 / 9 ⇒ 两种组合均 **ae −4、diff +4 净负**，且
//   AcquireCreatureItem 在两组合下**仍 DIFF（未归零）**；新劣化件 =
//   COverSkillTime::checkTime、CStomach::IncrementValue、CStomach::GetStomachValue、
//   CCreature::IsDieCreature（原均 AE）。⇒ 本 TU 对 P230 补的「N7 后端敏感」**不成立**
//   （本件 N7 实为虚调用 receiver/目标双寄存器对调，非仅目标），路由表维持 C6 不变。
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
// [b28d-solo-CCreatureMgr 检查点14 头改动回归·已报 Main] 37 vs 39：CInventory.h 把
//   m_noCharge 改 bool 而 GetNoCharge() 仍返 unsigned char ⇒ 本函数 `m_noCharge =
//   src->GetNoCharge();` 4.4 插 `test %al,%al; setne %dl`（+2 条）。ORIG 0x833ef86
//   `call GetNoCharge; mov 0x8(%ebp),%edx; mov %al,0x14(%edx)` 直存；getter 体
//   0x833efd6 `movzbl 0x14(%eax),%eax; ret`（P315/P136）⇒ GetNoCharge 应为 **bool**
//   （返回类型不入 mangled）。等待 Main 落头+定义同步；本 TU 不改头。
void user_creature::CCreatureItem::Set(user_creature::CCreatureItem* src)
{
    // ORIG 0x0833ef20：经 getter 逐字段读 src（-O0 产生 call），且
    // m_expireTime(+0x18) 置 0 而非拷贝（蛋→宠物项复制，新宠无到期时间）。
    m_itemId = src->GetItemId();
    m_creatureItemType = src->GetCreatureItemType();
    m_uid = src->GetUid();
    m_slotNo = src->GetSlotNo();
    m_noCharge = src->GetNoCharge();
    m_stat = src->GetStat();
    m_expireTime = 0;
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
// [b28d-solo-CCreatureMgr N] ndiff=63（原 245）：已修**真语义缺口**（p167 由 6 MISSING
// +1 EXTRA → CLEAR）——①去掉臆造的 `itemScript != 0` 判空（ORIG 0x833a153 直用
// find_item 结果，P171 照抄）；②getUsablePeriod/getExpirationDate 各独立调用 3 次
// （P48 -O0 无 CSE，删具名缓存）；③槽位写回改 `m_field0=0` + `m_addInfo=itemId`
// （ORIG 0x833a27a `movb $0,(%eax)` + 0x833a28d `mov %edx,0x2(%eax)`；原写的
// m_addInfo=0/m_addInfo2 是错字段）；④newItem 函数头预清（P198①）；⑤`register bool ok`
// （P231，ORIG 无槽 ebx 汇聚）；⑥Set(egg) 隐式派生→基类转换；⑦Set 实参复用 egg
//   （消第二次 operator->）。326 vs 325。ORIG 0x08339fa0
// [b28d-solo-CCreatureMgr §6.2 封顶] p167 CLEAR(60=60) + gap 1 + 逐块比对。
// [L23 本轮重核 2026-09-10] 旧判词「槽布局族 it/end -0x34/-0x30 vs -0x24/-0x20 +
//   sender 与 trace 槽互换」**作废**：fresh .o 槽表与 ORIG **逐值全等**
//   （respondent -0x60 / sender -0x40 / it-sret -0x34 / itEnd-sret -0x30 /
//    trace -0x2c / creatureItemId -0x38 / unwind -0x8）⇒ 判据⑩不适用、无槽问题。
//   本轮已修：ok 由「赋值即初始化」改 `register bool ok; if(!call()){…ok=false;}
//   else{…ok=true;}` ⇒ 复现 ORIG 0x0833a2e0 `call; xor $0x1,%eax; test %al,%al; je`
//   直判形 + %ebx 汇聚（ORIG 0x0833a345 `mov $0,%ebx` / 0x0833a3ff `mov $1,%ebx`），
//   原形多 `mov %eax,%ebx; mov %ebx,%eax` 两条。
//   残余（唯一族）：Set/SetItemId/SetExpireTime/RegisterCreatureItem 四处实参装载序
//   （ORIG 0x833a0eb `mov -0x18,%edx; mov -0x1c,%eax; mov %edx,0x4(%esp)` 双载 vs
//   我方逐条压栈；SetItemId 处两侧恰好相反）+ LogHistory 寄存器角色（N7）+ sender
//   析构块序（N104）= P75/N-G 跨版本装载序族。P277 不适用：ORIG 槽表无具名派生
//   指针槽（⇒ 原码无该局部）。
bool user_creature::CCreatureMgr::Hatch(int addInfo)
{
    // ORIG 0x8339fa4 `movl $0x0,-0x1c(%ebp)`：newItem 先预清再赋值
    // （P198① 声明=0 + 独立赋值两句式）。
    CCreatureItem* newItem = 0;
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
    int creatureItemId;
    if (!egg->Hatch(&creatureItemId))
    {
        respondent.SetError(0x11);
        return false;
    }
    newItem = GetInstanceCreatureItemFactory()->AcquireCreatureItem(
        user_creature::CREATURE_ITME_TYPE_CREATURE);
    if (newItem == 0)
    {
        respondent.SetError(0x16);
        return false;
    }
    // ORIG 仅一次 _Rb_tree_iterator::operator->（P167 差集：EXTRA ptEv 清零）——
    // Set 的实参复用已取出的 egg 指针，勿二次 it->second。
    // ORIG 0x833a043 段实参经 %edx 中转 ⇒ 原码是派生指针隐式转基类（P211）；
    // 显式 reinterpret_cast 会同型化掉该中转。
    newItem->Set(egg);
    reinterpret_cast<CCreature*>(newItem)->SetItemId(creatureItemId);
    newItem->SetCreatureItemType(user_creature::CREATURE_ITME_TYPE_CREATURE);
    reinterpret_cast<CCreature*>(newItem)->SetStomach(100);

    CItem* itemScript = G_CDataManager()->find_item(creatureItemId);
    int expireTime = 0;
    // ORIG 0x833a153 起无 itemScript 判空（find_item 结果直用；P171 照抄原码遗留）。
    // 三次 getUsablePeriod / 三次 getExpirationDate 为独立调用（P48 -O0 无 CSE），
    // 禁具名缓存局部（P183）。
    if (itemScript->getUsablePeriod() == 0 && itemScript->getExpirationDate() != 0)
        expireTime = itemScript->getExpirationDate() - OS_API::GetDateTimeTick();
    else
        expireTime = OS_API::GetDateTimeTick() + itemScript->getUsablePeriod() * 0x15180;
    if (itemScript->getUsablePeriod() == 0 && itemScript->getExpirationDate() == 0)
        expireTime = 0;
    newItem->SetExpireTime(expireTime);

    UnregisterCreatureItem(reinterpret_cast<CCreatureItem*>(egg)->GetUid());
    RegisterCreatureItem(newItem, newItem->GetUid());

    // ORIG 0x833a23e 预清 -0xc + 独立赋值（P197③/P198①）；0x833a27a/0x833a28d
    // 写 `movb $0,(%eax)`（+0 = m_field0）+ `mov %edx,0x2(%eax)`（+2 = m_addInfo
    // 存新宠 itemIdx），无判空（P171 照抄）。
    Inven_Item* slotItem = 0;
    slotItem = m_user->getCurCharacInvenW()->GetInvenRef(3, newItem->GetSlotNo());
    slotItem->m_field0 = 0;
    slotItem->m_addInfo = newItem->GetItemId();

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // ORIG 0x833a2db..a2e5：`call HatchCreature; xor $0x1,%eax; test %al,%al; je
    //   <成功块>` —— call 结果**直判**（无 eax→ebx 中转），ok 的值只在两条分支
    //   尾部写 callee-saved %ebx（0x833a345 `mov $0,%ebx` / 0x833a3ff `mov $1,%ebx`）
    //   ⇒ 原码 = `register bool ok;`（无初始化）+ `if (!call()) {失败; ok=false;} else
    //   {成功; ok=true;}`（P231/P248 裸 register 四条件全满足；禁 asm 绑定）。
    register bool ok;
    if (!sender.HatchCreature(newItem->GetUid(), 1, newItem->GetItemId()))
    {
        // P227/P221：ORIG ctor 前求值 vararg 并跨 ctor 保活 %ebx ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xb1a, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
        ok = false;
    }
    else
    {
        m_user->LogHistory("Hatch,%d,%d", newItem->GetItemId(), newItem->GetUid());
        sender.Send();
        respondent.SetOk();
        respondent.Send();
        respondent.MakeNotipacketCreatureItemList(&m_creatureItems, newItem->GetSlotNo());
        respondent.Send();
        m_user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7, newItem->GetSlotNo());
        ok = true;
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

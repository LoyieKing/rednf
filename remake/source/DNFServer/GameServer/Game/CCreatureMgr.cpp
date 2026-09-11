// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreatureMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CCreatureMgr/SetStomachValue.inc"

// ORIG _ZN13user_creature8CStomach15GetStomachValueENS_14CREATURE_STATEEb T 0x8336cfc。
// 非成长宠恒 100；state==1（战斗）按 -60/秒消耗，state 2..3 按 +360/秒恢复，
// 上限 100；每次调用后刷新 m_lastTime。常量取自 .rodata 0x8c2f450/54/58。
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 已消 x87 比较 lowering；
// 唯一残差 = 阈值比较的 C3 掩码：ORIG `test $0x45,%ah`（严格 `m_value<0.0f`）
// vs 原 `<=` 的 `test $0x5` ⇒ 改严格小于后逐条全等。ORIG 0x08336cfc
#include "CCreatureMgr/GetStomachValue.inc"

// ORIG _ZN13user_creature8CStomach14IncrementValueEf T 0x08336e66：
// m_value += amount；超过 100.0f 钳回 100.0f（0x42c80000）；返回新值。
// [b28-BM7 判词] 残余 = N85：ORIG 比较形 `fucompp; fnstsw %ax; sahf`（4.1）vs
// 4.4 `fucomip %st(1),%st; fstp %st(0)`（x87 比较两向跨版本固有差，patterns N85）；
// 常量地址 0x8c2f450 引用为 AE 归一项。语义（>100 钳位+返回）全等。
// [b28d-23 N·N85] x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp）⇒ N。ORIG 0x08336e66
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering（fucompp/fnstsw/sahf）；ORIG 0x8336e66
#include "CCreatureMgr/IncrementValue.inc"

// ORIG Reset @ 0x83370b0（逐字段转写）。
#include "CCreatureMgr/Reset.inc"

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
#include "CCreatureMgr/CCreature.inc"

// ~CCreature 定义迁至 CGameManager.cpp（CNode 特化之前）：P210 callee 定义顺序
// 定律——DynamicPool<CCreature>::CNode::~CNode 需先可见 dtor 定义才能继承
// ORIG 的 sub $0x4 小帧（仅声明时 4.4 保守撑至 0x18；探针 /tmp/bm3/）。

// ORIG _ZN13user_creature9CCreature8GetStateEv W 0x833f064：返回 +0x3c。
#include "CCreatureMgr/GetState.inc"

// ORIG _ZN13user_creature9CCreature13IsDieCreatureEv T 0x8338478：
// GetStomachValue(m_state, m_isGrow) < 1.0f（ORIG fucompp+test0x45 sete=value<1）。
// [b28d-23 N·N85/P230] x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp/seta）；-march=i486 可消但本 TU 未路由且禁改路由表 ⇒ N。ORIG 0x08338478
// [b28d-solo-CCreatureMgr AE] P252 具名 float 局部复现 fstps/flds 槽往返 + TU 路由 -march=i486 消 x87 lowering；ORIG 0x8338478
#include "CCreatureMgr/IsDieCreature.inc"

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
#include "CCreatureMgr/SetState.inc"

// ORIG isMaxLevel @ 0x833756e：m_script==0 → false；
// 否则 GetMaxLevel(m_script) <= m_level（setge）。
#include "CCreatureMgr/isMaxLevel.inc"

// ORIG vaildItemExp @ 0x83378aa：state==2 拒绝；m_isGrow!=0 拒绝；
// 否则委托 CCreatureScript::validItemExp(m_script, itemIdx)
// （ORIG 不判 m_script 空，忠实转写）。
#include "CCreatureMgr/vaildItemExp.inc"
// ORIG GainExp @ 0x83378fc：路由入口（reports/user_creature__CCreature/GainExp.md）。
// m_script==0 → 0；选择进化等级有效（!=-1）且 m_level>=它 → 0；
// m_level<GetMaxLevel 时按 m_isGrow 分流：==1 走 GainExpCreature，否则 GainItemExpCreature；
// 达到 MaxLevel → 0。
#include "CCreatureMgr/GainExp.inc"

// ============================================================================
// GainExp 依赖链（CreatureExperience 批次；逐函数按 reports/user_creature__CCreature
// 汇编转写。字段全部具名访问，无裸偏移；exp 打包语义：低字节=m_field4d（A/B 型
// 平衡值），高 16 位=成长进度计数。）
// ============================================================================

// ORIG _ZN13user_creature9CCreature13GetCreatureIdEi T 0x833769c：
// find_item(itemIdx) 空/堆叠 → -1；否则 m_creatureId = CEquipItem::get_creature_species()
// 并返回它（GetCreatureId.md）。
#include "CCreatureMgr/GetCreatureId.inc"

// ORIG _ZN13user_creature9CCreature9SetItemIdEi T 0x83375ae（SetItemId.md）：
// 写 m_itemId → GetCreatureId(itemIdx) 解析种族 → FindCreatureScript 刷新 m_script；
// 无脚本打 trace 返回；IsItemGrowCreature(script) 真→m_isGrow=0、假→m_isGrow=1
// （注意与字段名直觉相反）；非成长宠且 exp==0 时 exp 低字节与 m_field4d 同置 5。
#include "CCreatureMgr/SetItemId.inc"

// ORIG _ZN13user_creature9CCreature19IsAvailableArtifactEi T 0x83376f6：
// m_script==0 → false；否则委托 CCreatureScript::IsAvailableArtifactSlot(slot)。
#include "CCreatureMgr/IsAvailableArtifact.inc"

// ORIG _ZN13user_creature9CCreature11GetExpLevelEi T 0x83374d2（GetExpLevel.md）：
// 成长宠（m_isGrow==1）：G_CDataManager()->get_creature_exp_level(exp)；
// 非成长宠：exp / GetGrowItemCountforLevelUp()（0 视作 25）+ 1；
// 有脚本且超 GetMaxLevel 则钳到 GetMaxLevel。
// [b28d-solo-CCreatureMgr AE] P197③ 预清 + P194 xor 物化 + P59 非成长 fall + P48 两次独立 GetMaxLevel；ORIG 0x83374d2
#include "CCreatureMgr/GetExpLevel.inc"

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
#include "CCreatureMgr/GetAddExp.inc"

// ORIG _ZN13user_creature9CCreature12CalculateExpEiRbP5CUser T 0x83377b6
// （CalculateExp.md；除数 .rodata 0x8c2f450 = 100.0f 实证）：
// gained = exp*(GetAddExp(0x17)+GetAddExp(0x18)+GetAddExp(0x19))/100 截断取整；
// m_exp += gained + exp；新等级 = GetExpLevel(m_exp)，高于当前则置 levelUp。
#include "CCreatureMgr/CalculateExp.inc"

// ORIG _ZN13user_creature9CCreature7EvoluteEP5CUser T 0x83381ea（Evolute.md）：
// 无脚本/无进化目标/找不到目标脚本/0x16 槽空 → false；
// SetItemId(目标脚本 itemIdx)、槽物写新 itemIdx，CDBMsgSender(0x8a).EvoluteCreature
// 落库失败打 trace 返回 false；成功后 Send + 进化通知包（SendAccordingToPlace）
// + SendUpdateItemList(target=1, space=3, slot=0x16)。
#include "CCreatureMgr/Evolute.inc"

// ORIG _ZN13user_creature9CCreature13SelectEvoluteEP5CUser T 0x8337e48
// （SelectEvolute.md）：与 Evolute 同构，差异仅进化目标选取——
// m_field4d<=4 取 GET_BTypeCreature，否则 GET_ATypeCreature（==-1 拒绝）。
#include "CCreatureMgr/SelectEvolute.inc"

// ORIG _ZN13user_creature9CCreature15GainExpCreatureEiP5CUser T 0x8337bbc
// （GainExpCreature.md）：仅成长宠（m_isGrow==1）；饱食度截断 >0 才继续；
// CalculateExp 结算经验；未满级或有升级 → GainExp 通知包（析构补发）；
// 升级且可普通进化则 Evolute（失败整体返 0）；最后置 m_saveFlag=1 返回 1。
// [b28d-solo-CCreatureMgr AE] 翻正三旋钮：①notify/canEvolute 去具名 bool（P0 表达式内联）；
// ②`IsAbleEvolute` int→bool（P248 档①，ORIG 调用点 0x8337d11 `test %al,%al`）；
// ③`({ levelUp && IsAbleEvolute(user); })` 语句表达式 + 嵌套 if 复现 ORIG 物化岛 + 直跳（P226）。
// 127=127 逐条全等。ORIG 0x8337bbc
#include "CCreatureMgr/GainExpCreature.inc"

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
#include "CCreatureMgr/GainItemExpCreature.inc"

// ============================================================================
// ORIG W 访问器（0x833f04c..0x833f0e6；本 TU 提供强定义）
// ============================================================================

// ORIG IsSave @ 0x833f04c：movzbl +0x40。
#include "CCreatureMgr/IsSave.inc"

// ORIG GetStomach @ 0x083384ee：(int)CStomach::GetStomachValue(+0x3c,+0x4c)
// （fnstcw/fldcw RC=11 截断，即 C++ float→int 截断转换）。
#include "CCreatureMgr/GetStomach.inc"

// ORIG GetExp @ 0x833f08e：+0x30。
#include "CCreatureMgr/GetExp.inc"

// ORIG IsNoCharge @ 0x833f0a6：movzbl +0x14。
#include "CCreatureMgr/IsNoCharge.inc"

// ORIG SetNoCharge @ 0x833f0b2：bool 直存（mov %dl,0x14），无三目规范化。
#include "CCreatureMgr/SetNoCharge.inc"

// ORIG GetCreatureId @ 0x833f0da：+0x48。
#include "CCreatureMgr/GetCreatureId_1.inc"

// ORIG GetOverSkillTime @ 0x833f0e6：返回 this+0x50。
#include "CCreatureMgr/GetOverSkillTime.inc"

// ORIG UseFeed(CUser*, int&) T @ 0x083373d4：
// 仅 +0x4c==1 可喂；死亡则 MakeNotipacketRevivalCreature + SendAll；
// IncrementValue(30.0f) 截断写入 out；MakeNotipacketCreatureState(uid,out)
// 后 Send()，返回其值。
// [b28d-23 N·N82] 帧/槽差（79/81）⇒ N。ORIG 0x083373d4
// [b28d-solo-CCreatureMgr N] bool 规范化：`return pkt.Send()`（int→bool）4.1 直透 `mov %eax,%ebx` vs 4.4 test/setne；P183 去 value 槽与 GetUid 真调用已落；ORIG 0x83373d4
#include "CCreatureMgr/UseFeed.inc"

// ORIG ~CCreatureMgr @ 0x08339226（~CCreatureMgr.md）：
// 成员逆序析构（先 m_field30 map<int,int>，再 m_creatureItems
// map<int,CCreatureItem*>）；指针/标量成员无析构。用户定义使 D1/D2 为 T。
#include "CCreatureMgr/dtor_CCreatureMgr.inc"

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
#include "CCreatureMgr/Reset_1.inc"

#include "CCreatureMgr/SetUser.inc"

// ORIG UseItem @ 0x833a5bc：feed 判定用 Inven_Item+2（int，即 m_addInfo/item idx）。
#include "CCreatureMgr/UseItem.inc"

// ORIG CCreatureMgr::UseFeed T @ 0x08339dba（UseFeed.md）：
// 装备宠为空 → 0；CCreature::UseFeed(m_user, out) 失败 → 0；
// 成功后 CDBMsgSender(0x8a, m_user) 发 UseFeedCreature(uid, out)：
// 写库失败打 cMyTrace("...UseFeed()",0xab5,5) 返回 0；成功 sender.Send() 返回 1。
// [b28d-solo-CCreatureMgr AE] 翻正三旋钮：①`if (!sender.UseFeedCreature(...))` 内联判定
// + 早返（禁具名 ok，4.4 槽化 cmpb 反劣）；②cMyTrace 全表达式临时链（P227 vararg 跨 ctor
// 保活 %ebx）；③GetUid 内联实参（P183）。84=84 逐条全等。ORIG 0x08339dba
#include "CCreatureMgr/UseFeed_1.inc"

// ORIG GainExp @ 0x8339b06。
#include "CCreatureMgr/GainExp_1.inc"

#include "CCreatureMgr/IsEquippedCreature.inc"

// ORIG IsGrowCreature_Equipped_Creature @ 0x833debc。
#include "CCreatureMgr/IsGrowCreature_Equipped_Creature.inc"

// ORIG IsGrowCreature @ 0x833f0ca：movzbl +0x4c 后 xor $1 —— 即 +0x4c==0 返回
// true、==1 返回 false（与 Reset 初值相反；GainExp/UseFeed/vaildItemExp 用裸字段判断，
// 本访问器供包序列化/查询路径使用，语义以汇编为准）。
// 定义点挪至 CCreatureMgr::IsGrowCreature_Equipped_Creature 之后：4.4 对同 TU 可见
// callee 收缩 caller 帧（sub $0x4），ORIG 0x833debf 为 sub $0x18（4.1 宽松帧）。
#include "CCreatureMgr/IsGrowCreature.inc"

// ORIG setTempExpireTime W 0x833f10c：m_tempExpireDelta(+0x2c) = time。
#include "CCreatureMgr/setTempExpireTime.inc"
// ORIG CCreatureMgr::InsertCreatureItem @ 0x833b204（InsertCreatureItem.md）：
// 槽位/类型过滤后 CDBMsgSender(0x87, m_user)，size=m_creatureItems.size()，
// sender.InsertCreatureItem(m_user,item,slot,reason,size,days,expireTick)：
// 成功 → sender.Send() + 到期时间（days==0 且 expireTick!=0 取
// expireTick-GetDateTimeTick()，否则 days*0x15180）写 m_tempExpireDelta 返回 1；
// 失败 → cMyTrace("...InsertCreatureItem(...)",0xcaf,5) 返回 0。
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833b204
// [b28d-solo-CCreatureMgr AE] P248 裸 register tick=expireTick 令值升格 callee-saved %ebx 跨 GetDateTimeTick call 保活（非绑定，P0 合规）；ORIG 0x833b204
#include "CCreatureMgr/InsertCreatureItem.inc"

// ORIG SendCreatureItemList @ 0x833bdd2（41 指令，SendCreatureItemList.md）：
// CPacketRespondent(m_user,0,0) → MakeNotipacketCreatureItemList(&m_creatureItems,-1)
// → Send()（透传 CUser::Send 返回值）→ 析构。
#include "CCreatureMgr/SendCreatureItemList.inc"

}  // namespace user_creature

namespace user_creature
{

// ---- 简单访问器（ORIG 证据见各注释；宠物侧字段未建模部分显式标注）----
// ORIG GetCreatureItemId @ 0x833a638：m_equippedCreature 为空返回 0，
// 否则 CCreatureItem::GetItemId（W 0x833efa4，镜像同址转换调用）。
#include "CCreatureMgr/GetCreatureItemId.inc"

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
#include "CCreatureMgr/GetCreatureItem.inc"

// ORIG GetCreatureName @ 0x833a688：空返回静态空串（DAT_08c2d760），
// 否则 CCreature::GetName()。
#include "CCreatureMgr/GetCreatureName.inc"

// ORIG GetName @ 0x833f058：返回 this+0x1c。
// 定义点挪至 CCreatureMgr::GetCreatureName 之后：同 TU 可见 callee 使 4.4 caller
// 帧塌缩为 sub $0x4，ORIG 0x833a68b sub $0x18 要求 callee 在调用点不可见。
#include "CCreatureMgr/GetName.inc"

// ORIG IsDieCreature @ 0x833a660：空返回 1，否则委托 CCreature::IsDieCreature。
#include "CCreatureMgr/IsDieCreature_1.inc"

// ORIG GetEquipedCreatureLevel @ 0x833a6b0。
#include "CCreatureMgr/GetEquipedCreatureLevel.inc"

// ORIG GetLevel @ 0x833f09a：+0x2c。
// 定义点挪至 CCreatureMgr::GetEquipedCreatureLevel 之后（同 GetName 帧塌缩原因）。
#include "CCreatureMgr/GetLevel.inc"


// ORIG _ZN13user_creature12CCreatureMgr11GetArtifactEi T 0x833be56（GetArtifact.md）：
// idx=slot-0x17，仅上界检查（>3 → 0）；返回 &m_artifacts[idx]
// （ORIG this+(idx+8)*4 与镜像 m_artifacts 基址 +0x20 一致）。
#include "CCreatureMgr/GetArtifact.inc"


// ============================================================================
// 本批补齐（U3）：IsCreatureEquipmentScope / RecvMail / RegistTempPeriod
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr24IsCreatureEquipmentScopeEi T 0x0833926c。
#include "CCreatureMgr/IsCreatureEquipmentScope.inc"

// ORIG _ZN13user_creature12CCreatureMgr16RegistTempPeriodEiii T 0x0833d7de。
// 汇编实证 key=a(0xc)、value=b(0x10)（0x833d7ed find(&a)、0x833d83b/0x833d842
// make_pair(&a,&b)）；形参 c(0x14) 未引用。查中 → false；插入后 true。
#include "CCreatureMgr/RegistTempPeriod.inc"

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
#include "CCreatureMgr/GetRemainDate.inc"

// ORIG CCreatureMgr::GetRemainDate(int, int) const @0x833d78e：
//   m_tempExpireDelta（+0x2c）!= -1 → 直接返回它；
//   否则 FindCreatureItem(uid)：空 → -1；否则 → item->GetRemainDate(nowSec)。
#include "CCreatureMgr/GetRemainDate_1.inc"

// ORIG _ZN13user_creature12CCreatureMgr8RecvMailEP10Inven_Itemii T 0x0833ab9a
// （docs/class_func_reports/user_creature__CCreatureMgr/RecvMail.md）。
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833ab9a
// [b28d-solo-CCreatureMgr AE] P183 去 characNo 具名槽（实参直传）+ P221 全表达式临时链（trace 槽 -0x2c）；ORIG 0x833ab9a
#include "CCreatureMgr/RecvMail.inc"

// ORIG _ZN13user_creature12CCreatureMgr8SendMailEP10Inven_Itemii T 0x0833aa32
// （docs/class_func_reports/user_creature__CCreatureMgr/SendMail.md）。
// 逻辑：非宠物装备槽 → 1；item 类型非 5（CREATURE_EQUIP）→ 1；
// CDBMsgSender sender(0x8a,m_user) → mapSize=m_creatureItems.size()，
// characNo=m_user->getCurCharacNo()；SendMail 写库失败 → cMyTrace 并返回 0；
// 成功 → sender.Send() 后 DeleteCreatureItem(item->m_addInfo2, reason)。
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833aa32
// [b28d-solo-CCreatureMgr AE] P194 尾判 `if(!DeleteCreatureItem())return false;return true;` + P221 临时链 + P183 去 characNo 槽；ORIG 0x833aa32
#include "CCreatureMgr/SendMail.inc"

// ORIG _ZN13user_creature12CCreatureMgr18DeleteCreatureItemEii T 0x0833b606。
// 仅委托 UnregisterCreatureItem(param_2)（param_1 调用方传 item->m_addInfo2，本函数未使用）。
// 返回 void 的 ORIG 反编译，但 eax 继承 UnregisterCreatureItem 的返回值，
// 故以 bool 返回其值（匹配 SendMail 调用处的分支判断）。
#include "CCreatureMgr/DeleteCreatureItem.inc"

// ORIG _ZN13user_creature12CCreatureMgr22UnregisterCreatureItemEi T 0x0833a854。
// 在 m_creatureItems 中查 uid：命中则经 CCreatureItemFactory 释放并 erase，返回 true；
// 未命中 → cMyTrace("UnregisterCreatureItem failed. uid(%d), key(%d)\n") 并返回 false。
// 注：CCreatureItemFactory::FreeCreatureItem 未建模（CCreature.cpp 未入 CMake），
// 与 Reset() 一致采用 BLOCKED 标注；erase 照常执行。
// [b28d-solo-CCreatureMgr AE] 翻正两旋钮：①块序改 `if (it == end) { fail; return false; }`
// 早返（失败块 fall-through、成功块跳转目标，与 ORIG 一致）；②`it->second` 存具名局部
// （P0 槽往返：ORIG `mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp)`）；68=68 逐条全等。
// ORIG 0x0833a854
#include "CCreatureMgr/UnregisterCreatureItem.inc"

// ============================================================================
// user_creature::CDBMsgSender（ORIG 域内类，本 TU 提供最小真实实现）
// ============================================================================

// ORIG C1 @ 0x833f8ae：仅清 m_stream（offset 0）后委托 Init；m_user 由 Init 写入。
#include "CCreatureMgr/CDBMsgSender.inc"

// Init 定义已迁移至 CCreature.cpp（ORIG W 0x833f7a8 逐条转写；本 TU 仅保留
// C1 委托调用，避免两 TU 强符号冲突；全局 ::ENUM_INTERNALPACKET 见 CacheCharacterType.h）。

// ORIG D1/D2 @ 0x833f8d8：按 stream 的 poolType 归还对象池后置空。
#include "CCreatureMgr/dtor_CDBMsgSender.inc"

// ORIG Send @ 0x833f950：投递 DB_Q 后释放持有并返回 true。
#include "CCreatureMgr/Send.inc"

// ORIG _ZN13user_creature12CDBMsgSender8RecvMailEP10Inven_Itemiiii W 0x833ff34：
// GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 → setCopy → 逐字段填充。
// [b28d-23 N·P30/N4] 临时 this 取址：ORIG `lea 0x4(%eax),%edx` vs 4.4 `add $0x4,%eax`（+1 mov）⇒ N。ORIG 0x0833ff34
// [b28d-solo-CCreatureMgr AE] P235 m_itemView.setCopy 出 ORIG lea（N107 cast 形恒 add 的边界）；ORIG 0x833ff34
#include "CCreatureMgr/RecvMail_1.inc"

// ORIG _ZN13user_creature12CDBMsgSender8SendMailEP10Inven_Itemiiii W 0x833fe8c：
// 与 RecvMail 同构，差异：m_hdr |0x100（or $0x1,%dh）、m_field53=1、m_field64=3、
// 不写 slot（+0x2c）。slot 形参在汇编中未使用。
// [b28d-23 N·P30/N4] 同 RecvMail：lea-vs-add 不可翻 ⇒ N。ORIG 0x0833fe8c
// [b28d-solo-CCreatureMgr AE] P235 m_itemView.setCopy 出 ORIG lea（同 RecvMail）；ORIG 0x833fe8c
#include "CCreatureMgr/SendMail_1.inc"

// ORIG _ZN13user_creature12CDBMsgSender15UseFeedCreatureEii W 0x0833fc5a：
// m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
// m_hdr|=2；+0x30(m_addInfo2)=uid；+0x44(m_stomachValue)=饱食度；返回 1。
#include "CCreatureMgr/UseFeedCreature.inc"

// ORIG _ZN13user_creature12CDBMsgSender12UseKeepRightEii W 0x083401ec
// （objdump 逐条转写，SetKeepRightItem 落库用）：m_stream 空 → 0；
// GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 → m_hdr|=0x80；
// +0x30(m_addInfo2)=uid；+0x52(m_keepRight)=flag 低字节；返回 1。
#include "CCreatureMgr/UseKeepRight.inc"

// ORIG _ZN13user_creature12CDBMsgSender15EvoluteCreatureEii W 0x833fd74
// （objdump 逐条转写）：m_stream 空 → 0；buf 清 0x74 后 m_hdr|=0x40；
// compat.m_itemIdx(+0x08)=新宠物物品 idx；m_addInfo2(+0x30)=uid；返回 1。
// [b28d-23 NEAR·N76] 同 HatchCreature：+0x8 store 装载序不可翻。ORIG 0x0833fd74
// [b28d-solo-CCreatureMgr AE] P235 具名视角（同 HatchCreature）；ORIG 0x833fd74
#include "CCreatureMgr/EvoluteCreature.inc"

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
#include "CCreatureMgr/InsertCreatureItem_1.inc"

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
#include "CCreatureMgr/SetEggItem.inc"


// ============================================================================
// 进化资格校验链（RestoreUserQuestStubs：Dispatcher_VerifyCreatureQuest 依赖）
// ============================================================================

// ORIG _ZNK13user_creature9CCreature13IsAbleEvoluteEP5CUser T 0x083380fc：
// script==0 / GetEvolutionLevel()==0 / m_level<进化等级 → 0；
// 无进化任务 → 1；有进化任务 → 给 user 补发任务列表包（get_quest_info）后 0。
// [b28d-solo-CCreatureMgr P248档①] ORIG 调用点 0x8337d11 `test %al,%al` ⇒ bool。
#include "CCreatureMgr/IsAbleEvolute.inc"

// ORIG _ZN13user_creature9CCreature19IsAbleSelectEvoluteEP5CUser T 0x08337d58：
// 同 IsAbleEvolute，但等级门槛取 GetTypeSelectEvolutionLevel()（-1 视为不可）。
#include "CCreatureMgr/IsAbleSelectEvolute.inc"

// ORIG _ZNK13user_creature12CCreatureMgr26VerifyEquipedCreatureQuestEv
// T 0x0833d6fa：无装备宠物 → 0；成长宠走 IsAbleSelectEvolute，否则 IsAbleEvolute。
#include "CCreatureMgr/VerifyEquipedCreatureQuest.inc"

// ============================================================================
// SwapCreatureItem 调用链（Prod_DB_Misc 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreatureMgr/ 与 user_creature__CCreature/
// 的 ## 汇编 + ## 反编译 C 转写，符号与 ORIG mangled 一致）
// ============================================================================

// ORIG _ZN13user_creature13CCreatureItem9SetSlotNoEi T 0x08337050：写 +0x10 槽号。
#include "CCreatureMgr/SetSlotNo.inc"

// ORIG _ZN13user_creature13CCreatureItem18GetCreatureItemTypeEv W 0x0833ef98
//（本批改为外联定义：SwapCreature 链按 ORIG 外联调用形态取类型）。
#include "CCreatureMgr/GetCreatureItemType.inc"

// ORIG _ZN13user_creature14COverSkillTime9checkTimeEv T 0x0833d65e：
// (float)(getCurSec()-m_time) ∈ (0.0, 20.0)（0x8c2f468 处常量 = 20.0f）→ 1。
// [b28d-23 N·N85] x87 比较 lowering（两处 fucompp/fnstsw）⇒ N。ORIG 0x0833d65e
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering；ORIG 0x833d65e
#include "CCreatureMgr/checkTime.inc"

// ORIG _ZN13user_creature9CCreature5EquipEP5CUser T 0x08337330：
// 战斗中 → SetState(1)，否则 SetState(3)；恒返回 1。
#include "CCreatureMgr/Equip.inc"

// ORIG _ZN13user_creature9CCreature9DismantleEP5CUser T 0x08337382：
// SetState(2) 后恒返回 1。
#include "CCreatureMgr/Dismantle.inc"

// ORIG _ZN13user_creature12CDBMsgSender16SwapCreatureItemEP5CUserP10Inven_ItemiS4_i
// W 0x0833fa0c（objdump 逐条转写）：m_stream 空 → 0；
// GetInBuffer<SIG_SWAP_CREATURE_ITEM> → memset 0x74；
// +0x04=slot1、+0x00=item1->get_add_info()、item1 空(+0x02==0) → +0x08=1；
// +0x10=slot2、+0x0c=item2->get_add_info()、item2 空 → +0x14=1；返回 1。
#include "CCreatureMgr/SwapCreatureItem.inc"

// ORIG _ZN13user_creature12CCreatureMgr19IsArtifactEquipSlotEi T 0x083393fc：
// 槽 0x17/0x18/0x19 为神器装备槽。
#include "CCreatureMgr/IsArtifactEquipSlot.inc"

// ORIG _ZN13user_creature12CCreatureMgr11EquipArtifactEiP10Inven_Item T 0x083396c8：
// GetArtifact 为空 → 0；否则委托 CArtifact::EquipArtifact 并按其结果返回。
#include "CCreatureMgr/EquipArtifact.inc"

// ORIG _ZN13user_creature12CCreatureMgr17DismantleArtifactEiP10Inven_Item T 0x08339718：
// GetArtifact 为空 → 0；否则 CArtifact::Reset 后返回 1。
#include "CCreatureMgr/DismantleArtifact.inc"

// ORIG _ZN13user_creature12CCreatureMgr5EquipEPNS_9CCreatureE T 0x083392f6：
// 仅种族类型 1 可装备；CCreature::Equip 成功后记录为已装备宠。
#include "CCreatureMgr/Equip_1.inc"

// ORIG _ZN13user_creature12CCreatureMgr9DismantleEPNS_9CCreatureE T 0x0833934c：
// 种族类型 1 才可卸下；成功后清空已装备记录。
#include "CCreatureMgr/Dismantle_1.inc"

// ORIG _ZN13user_creature12CCreatureMgr17EquipCreatureItemEPNS_9CCreatureES2_
// T 0x083393a2：非空旧宠先 Dismantle，非空新宠再 Equip，任一失败返回 0。
#include "CCreatureMgr/EquipCreatureItem.inc"

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
#include "CCreatureMgr/SetKeepRightItem.inc"

// ORIG _ZN13user_creature12CCreatureMgr9CheckItemEP5CUserP10Inven_ItemRb
// T 0x0833c240（CheckItem.md）：默认 outFlag=true；仅当物品为堆叠宠物蛋类
// （m_field1==7 ITEM_TYPE_CREATURE_STACK 且 m_addInfo==0x1a）时置 outFlag=
// false 并委托 SetKeepRightItem 清占用（其返回值即函数返回）；否则返 true。
#include "CCreatureMgr/CheckItem.inc"

// ORIG _ZNK13user_creature12CCreatureMgr12GetExtraGoldEi T 0x0833c31a：
// 无装备宠/已死亡 → 0；按宠物种族 id 给额外金币比例（*5 或 *3 再 /100）。
// 两个独立 if（非 ||）：ORIG 字节序两条独立 0 返回路径（je +0xfc / jmp +0x101）。
// [b28d-solo-CCreatureMgr AE] 既有（P0 具名 int 槽 + goto 块序）；compare_tu AE；ORIG 0x833c31a
// [b28d-solo-CCreatureMgr N112] game_func_report 口径多 1 条 4.4 对齐 nop（jmp 后
// mov$0 岛前）；compare_tu 已 AE（-falign-jumps 不可控，N112 剥离口径）。
#include "CCreatureMgr/GetExtraGold.inc"

// ORIG _ZNK13user_creature12CCreatureMgr21GetOverSkillExtraGoldEi T 0x0833c6f6：
// 装备宠存活且 itemId==0xf68a 且超技能计时有效时给 *5/100，否则 0。
// [b28d-solo-CCreatureMgr AE] 既有（裸比较 + goto 钉块序）；compare_tu AE；ORIG 0x833c6f6
// [b28d-solo-CCreatureMgr N112] 同上：多 1 条 4.4 对齐 nop，compare_tu 口径已 AE。
#include "CCreatureMgr/GetOverSkillExtraGold.inc"

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
#include "CCreatureMgr/SwapCreature.inc"

// ORIG _ZN13user_creature12CCreatureMgr12SwapArtifactEP10Inven_ItemiiS2_ii
// T 0x08339752：任一侧计数为 0 时按神器槽位拆解/装配，随后清空源物品首字节。
#include "CCreatureMgr/SwapArtifact.inc"

// ORIG _ZN13user_creature12CCreatureMgr16SwapCreatureItemEP10Inven_ItemiiS2_ii
// T 0x083399fa：按物品类型分流到 SwapCreature/SwapArtifact，最后交换两物品内容。
#include "CCreatureMgr/SwapCreatureItem_1.inc"

// ORIG _ZN13user_creature12CCreatureMgr16ResponseCreatureEv T 0x0833a6d8：
// 构造 CPacketRespondent(user,0,0) → 填充 ResponseCreature 通知包 →
// 以 (村, 区域) 广播（门区由 Broadcast 内部直发本人），透传其返回值。
#include "CCreatureMgr/ResponseCreature.inc"

// ORIG _ZN13user_creature12CCreatureMgr20RegisterCreatureItemEPNS_13CCreatureItemEi
// T 0x0833a77e：uid 查重后插入 m_creatureItems；重复则 cMyTrace 报错返回 false。
// [b28d-23 N·N18a/N76] 查重物化后 uid/get_unique_id 装载序 ⇒ N。ORIG 0x0833a77e
// [b28d-solo-CCreatureMgr AE] P227 全表达式临时链令变参在 ctor 前求值并 callee-saved 保活（删具名 uniqBx）；ORIG 0x833a77e
#include "CCreatureMgr/RegisterCreatureItem.inc"

// ============================================================================
//   m_hdr|=0x200；+0x30(m_addInfo2)=uid；name 拷入 +0x34（m_pad34 名字区，截 0xc）；
//   返回 1。hdr 位/name 位置依同族（UseFeedCreature|0x2 等）推断，无独立报告。
//   （ORIG asm 833fbe9..833fc1a 无 buf 判空、无 name 判空。）
#include "CCreatureMgr/RenameCreatureItem.inc"

// ============================================================================
// CDBMsgSender::HatchCreature @ 0x0833fcc2（W，objdump 逐条转写）
//   m_stream 空 → 0；GetInBuffer<SIG_CREATURE_ITEM> → memset 0x74 →
//   m_hdr|=0x400；+0x30(m_addInfo2)=uid；+0x50(m_flag50)=flag 低字节；
//   +0x08(m_item 兼容区 m_itemIdx)=itemIdx；返回 1。hdr 位推断同族。
// ============================================================================
// [b28d-23 NEAR·N76] +0x8 store 装载序：ORIG 0x833fd0f RHS-first vs 4.4 LHS-first（槽基址形）；探针 /tmp/p23_probe.cpp k2 证仅形参基址 RHS-first，槽基址恒 LHS-first ⇒ N76。ORIG 0x0833fcc2
// [b28d-solo-CCreatureMgr AE] P235 SIG_CREATURE_ITEM::m_item 匿名 union 具名视角 m_itemView 复现 +0x08 store RHS-first；ORIG 0x833fcc2
#include "CCreatureMgr/HatchCreature.inc"
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
#include "CCreatureMgr/RenameCreature.inc"

}  // namespace user_creature

// ============================================================================
// Inven_Item_Compatible（全局域，ORIG 0x833f38e/0x833f418；U3Creature 批次实现）
// ============================================================================

// ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e。
// [b28d-23 NEAR·N76] 语句序照 ORIG 0x833f38e 重排 + m_amp0Int 具名视角（54→47 条）；残余 1 = +0x11 装载序 ⇒ N76。ORIG 0x0833f38e
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f38e
#include "CCreatureMgr/setCopy.inc"

// ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418。
// [b28c-30] N76 封顶：末段 `*reinterpret_cast<int*>(&item.m_amp) = m_amp0;` ORIG
// 为 `mov %edx,0x11(%eax)` disp 直写；取址形反产 `add $0x11,%eax; mov %edx,(%eax)`
//（+1 条）。具名 int 视角可去 add 但装载序变 RHS-first（ORIG LHS-first），净差 1
// ⇒ 封顶（前两段 guid 结构赋值 + 两字段同型化已 AE）。
// [b28d-23 NEAR·N76] m_amp0Int 具名视角消 add $0x11（54→47 条）；残余 1 = +0x11 store 装载序（ORIG LHS-first vs union 视角 RHS-first，int/unsigned 同异型试尽）⇒ N76。ORIG 0x0833f418
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f418
#include "CCreatureMgr/makeInvenItem.inc"

// ============================================================================
// CCreatureItemFactory（ORIG ctor 0x0833f67a / dtor 0x0833f6d0 / guard 单例）
//   两 DynamicPool：+0x00 CEgg 池、+0x5c CCreature 池，各 0x3e8（1000）基数。
//   AcquireCreatureItem(type)：type==1 → CCreature 池，type==0 → CEgg 池；
//   Acquire 后经 vtable+0 调 C1 构造。失败 cMyTrace 报错并返回 0。
//   FreeCreatureItem(item)：按池归还。
// ============================================================================
// [AnonLane3 R14 · P154] 删除发明死常量 (anonymous namespace)::kCreativeFactoryTrace
//   （.data 4B 指针 + 所指数串；本 TU 零使用点——AcquireCreatureItem.inc 已按 P139 直用
//   __PRETTY_FUNCTION__；ORIG 全库无该 mangled 名，nm 零命中）⇒ 仅NEW 噪音，删定义。

#include "CCreatureMgr/CCreatureItemFactory.inc"

#include "CCreatureMgr/dtor_CCreatureItemFactory.inc"

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
#include "CCreatureMgr/AcquireCreatureItem.inc"

// ORIG 0x833f618：GetCreatureItemType()==1 → 归还 m_creaturePool(+0x5c)；
// ==0 → 归还 m_eggPool(+0x00)；否则返回 0（ORIG 无 null 检查，忠实转写）。
#include "CCreatureMgr/FreeCreatureItem.inc"

// ORIG guard 单例（guard var 0x943c070 / instance 0x943c080）：__cxa_guard
// 双检锁 + atexit 注册。
#include "CCreatureMgr/GetInstanceCreatureItemFactory.inc"

// ============================================================================
// CEgg::Hatch @ 0x0833912a（objdump 逐条转写）：蛋物品 find_item 非空且
//   非 stackable → *out = CEquipItem::get_hatch_item_id() 返回 1；否则 0
//   （ORIG 0x8339181 为外联 call 0x833eeda；nm -S 核验
//   _ZNK10CEquipItem18get_hatch_item_idEv W 0x0833eeda size 0xe）。
// ============================================================================
#include "CCreatureMgr/Hatch.inc"

// ============================================================================
// CCreature::SetStomach @ 0x0833f070：调 CStomach::SetStomachValue(this+0x34, (float)stomachValue)，
// ORIG 即 call SetStomachValue(写入 +0x4 m_value)。
// ============================================================================
#include "CCreatureMgr/SetStomach.inc"

// ============================================================================
// CCreatureItem::Set @ 0x0833ef20：深拷贝 src 各字段（蛋 → 宠物项复制）。
// ============================================================================
// [b28d-solo-CCreatureMgr 检查点14 头改动回归·已报 Main] 37 vs 39：CInventory.h 把
//   m_noCharge 改 bool 而 GetNoCharge() 仍返 unsigned char ⇒ 本函数 `m_noCharge =
//   src->GetNoCharge();` 4.4 插 `test %al,%al; setne %dl`（+2 条）。ORIG 0x833ef86
//   `call GetNoCharge; mov 0x8(%ebp),%edx; mov %al,0x14(%edx)` 直存；getter 体
//   0x833efd6 `movzbl 0x14(%eax),%eax; ret`（P315/P136）⇒ GetNoCharge 应为 **bool**
//   （返回类型不入 mangled）。等待 Main 落头+定义同步；本 TU 不改头。
#include "CCreatureMgr/Set.inc"

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
#include "CCreatureMgr/Hatch_1.inc"

// ORIG _ZN13user_creature20CCreatureItemFactory7GetSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f546：type==1 → m_creaturePool.Size()；type==0 → m_eggPool.Size()；
// 其余返回 0。
#include "CCreatureMgr/GetSize.inc"

// ORIG _ZN13user_creature20CCreatureItemFactory13GetRemainSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f510：同 GetSize 分派，调 DynamicPool::Remain()。
#include "CCreatureMgr/GetRemainSize.inc"

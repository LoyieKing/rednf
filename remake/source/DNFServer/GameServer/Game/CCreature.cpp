// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreature/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <algorithm>   // 与 CCreatureMgr.cpp 同前缀，保持共享头解析环境一致

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CCreatureScriptMgr.h"  // GetInstanceCreatureScriptMgr / FindCreatureScript
#include "CPacketRespondent.h"
#include "CCreatureScript.h"     // CCreatureScript 访问器（GetMaxLevel 等）
#include "TimerQueue.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "GameTypes.h"           // cMyTrace / ENUM_ITEMSPACE


namespace user_creature
{

// ORIG _ZN13user_creature8CStomach13IsSaveStomachENS_14CREATURE_STATEE T 0x8336dee
//（IsSaveStomach.md 全汇编转写）：默认可保存；state==1（战斗）时饱食度<=0
// → 不可保存；state∈[2,3] 时饱食度与 .rodata 0x8c2f450（100.0f）相等
//（fucompp+sahf，含 NaN 判定）→ 不可保存；其余 state 恒真。
#include "CCreature/IsSaveStomach.inc"
{
    // b28d-37 [N85 封顶，ORIG 0x8336dee]：x87 比较两向（ORIG fucompp/fnstsw/sahf/sete
    // vs 4.4 fucomip/fstp/setae；P230 换 -march=i486 可消，本波不改路由）。
    // ORIG 决策树 `cmp $1,%eax; je; cmp $1,%eax; jl; cmp $3,%eax; jg; jmp` 为
    // 形参载入寄存器比较且无槽往返（P251/P286：无槽多路比较 ⇒ switch；if 链会被
    // 4.4 折成内存直比并多出 `int s` 槽），故按 ORIG 还原 switch(state) 形。
    bool saveOk = true;
    switch (state)
    {
    case CREATURE_STATE_1:
        if (m_value <= 0.0f)   // ORIG test $0x5（C0|C2）：<=0 不可保存；NaN 走 unordered 保持可保存
            saveOk = false;
        break;
    case CREATURE_STATE_2:
    case CREATURE_STATE_3:
        if (m_value == 100.0f)
            saveOk = false;
        break;
    default:
        break;
    }
    return saveOk;
}

// ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime = 当前秒。
#include "CCreature/Resume.inc"
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
}

// CCreature ctor/dtor/GetOverSkillTime 定义于 CCreatureMgr.cpp（去重保留）。

// ORIG _ZN13user_creature9CCreature3DieEP5CUser T 0x8338536（Die.md）：
// CPacketRespondent(user,0,0).MakeNotipacketDiedCreature() + SendAll()，恒返 1。
#include "CCreature/Die.inc"
{
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketDiedCreature();
    pkt.SendAll();
    return true;
}

// ORIG _ZN13user_creature9CCreature17CheckDiedCreatureEP5CUser T 0x83384b8
//（CheckDiedCreature.md）：IsDieCreature 为真时先 Die(user)，返回判定结果。
#include "CCreature/CheckDiedCreature.inc"
{
    // ORIG: call IsDieCreature; test %al,%al; je 返 0 岛（bool callee 裸形，
    // 无落槽局部）；Die 后 mov$1/jmp/mov$0 常量岛。
    if (IsDieCreature())
    {
        Die(user);
        return 1;
    }
    return 0;
}

// ============================================================================
// Save/Load 落库链（TranslateCCreatureMgrTU 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreature/ 的 ## 汇编 + ## 反编译 C）
// ============================================================================

// ORIG _ZN13user_creature9CCreature16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x83387f6（SaveCreatureItem.md 全汇编转写）。param user 未读（保留形参以匹配
// ORIG 签名）；item 仅被写 idx 字段：*(int*)(item+2) = GetItemId()。
// 注意顺序：先写饱食度/经验/uid，再经 IsSaveStomach 刷新 m_saveFlag(+0x40)。
#include "CCreature/SaveCreatureItem.inc"
                                 SIG_CREATURE_ITEM_DETAIL_INFO* info,
                                 CUser* user)
{
    (void)user;   // ORIG 未读该实参
    info->m_stomachValue = (int)m_stomach.GetStomachValue(
        (CREATURE_STATE)m_state, m_isGrow != 0);   // fnstcw/fldcw RC 截断
    info->m_exp = m_exp;                           // ← +0x30
    info->m_uid = m_uid;                           // ← +0x0c
    m_saveFlag = m_stomach.IsSaveStomach((CREATURE_STATE)m_state) ? 1 : 0;
    item->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    info->m_slotNo = reinterpret_cast<CCreatureItem*>(this)->GetSlotNo();
    info->m_creatureItemType = (unsigned char)m_creatureItemType;  // 低字节 +0x08
    strncpy(info->m_name, m_name, 0xc);            // ← +0x1c，12B（ORIG call strncpy PLT）
    info->m_noCharge = m_noCharge;                 // +0x14 byte → +0x26
    info->m_stat = m_stat;                         // +0x15 byte → +0x27
    return true;
}

// ORIG _ZN13user_creature9CCreature16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
// T 0x83385aa（LoadCreatureItem.md 全汇编转写）：DB 载入侧逆操作。
// +0x27 非 0 → Inven_Item::reset(item) 并返回 0（条目已删除）；成功路径把
// 成长宠/满级标志写回条目 +0x30/+0x31；slotNo==0xee 视作装备中并调 Equip；
// 注册后回填 CREATURE 区槽物 add_info 并按 IsSaveStomach 重算 m_saveFlag。
// [N96 清绑 2026-09-09] 原 `register unsigned char isNotGrow asm("eax")` 全域禁用
// （P0②/N96/P248 尾条），还原 P168a④ 具名 uchar 局部自然形（`v = 字段; v ^= 1; if (v)`）。
// 4.4 对该两语句形发 `movzbl; mov %al,-N; xorb $1,-N; cmpb $0,-N; je`（栈槽物化），
// ORIG 为 `movzbl 0x4c(%eax),%eax; xor $1,%eax; test %al,%al; je`（寄存器物化）⇒
// 自然形不可达，残差 = N96+N18a 封顶（P168a④ 补充，四形实测同）。
// 读数：清绑前 our_insns=178（TU 口径 DIFF）；清绑后 our_insns=179 / orig=177
//（TU 口径 DIFF）。
#include "CCreature/LoadCreatureItem.inc"
                                 SIG_CREATURE_ITEM_DETAIL_INFO* info,
                                 CUser* user)
{
    if (info->m_stat != 0)
    {
        item->reset();
        return false;
    }

    m_exp = info->m_exp;                           // → +0x30
    SetItemId(item->m_addInfo);                    // *(int*)(item+2) 驱动脚本解析
    m_uid = info->m_uid;                           // → +0x0c
    m_slotNo = info->m_slotNo;                     // → +0x10
    // round1 R19：ORIG 833862f 段 = `movzbl 0x24(%eax),%eax; movsbl %al,%edx;
    // mov 0x8(%ebp),%eax; mov %edx,0x8(%eax)`（RHS 值先 + disp store）。
    // int& 视图（同型）在 4.4 产 LHS 地址先物化（`add $0x8,%eax; mov %edx,(%eax)`）；
    // 按 P211 档③ 用**异型 RHS**（signed char → enum 字段）触发转换中转翻装载序。
    m_creatureItemType =
        static_cast<CREATURE_ITME_TYPE>(static_cast<signed char>(info->m_creatureItemType));
    m_stomach.SetStomachValue((float)info->m_stomachValue);
    strncpy(m_name, info->m_name, 0xc);            // → +0x1c，12B（ORIG call strncpy PLT）
    // ORIG 8338657：`movzbl 0x4c(%eax),%eax; xor $0x1,%eax; test %al,%al; je`
    // ⇒ P248 档②/P292 字段 bool 直证：m_isGrow 本身已是 bool（CGameManager.h:367）
    // ⇒ 删具名 uchar 局部（4.4 恒落栈槽 `mov %al,-0xd; xorb $1,-0xd; cmpb $0,-0xd`
    // 并顶掉后续全部槽 4B）、写裸 `if (!m_isGrow)`。
    // 高 16 位提取走 union 拆半（P197①：整字落槽 -0x10 → movzwl -0xe + cwtl 直传）。
    if (!m_isGrow)
    {
        union { int all; unsigned short w[2]; } u;
        u.all = m_exp;
        m_level = GetExpLevel((int)(short)u.w[1]);
        info->m_growExpFlag = 0;
    }
    else
    {
        m_level = GetExpLevel(m_exp);
        info->m_growExpFlag = 1;
    }
    info->m_isMaxLevel = isMaxLevel();
    m_noCharge = (info->m_noCharge != 0) ? 1 : 0;
    m_stat = info->m_stat;
    m_expireTime = info->m_expireTime;
    if (m_isGrow != 0)
        m_stomach.Resume();
    SetState(user, CREATURE_STATE_2);
    m_saveFlag = 1;
    // ORIG：两独立守卫共享失败出口（cmp;jne L + call;xor;test;je L，P133 独立守卫形；
    // 合并 && 会致 4.4 布尔汇聚 mov$1/jmp/mov$0 岛）。
    if (info->m_slotNo == 0xee)                        // 0xee=装备槽编码（0xd8+0x16）
    {
        if (!mgr->Equip(this))
            return false;
    }
    if (!mgr->RegisterCreatureItem(reinterpret_cast<CCreatureItem*>(this),
                                   info->m_uid))
        return false;
    // P197③双槽置 0：ORIG 块入口 `movl $0,-0xc` 预清 + 后续独立赋值两段式
    //（`T* p=0;` + `p=f()` 合并形缺 init 行）。
    Inven_Item* slotItem = 0;
    slotItem =
        user->getCurCharacInvenW()->GetInvenRef(
            3, reinterpret_cast<CCreatureItem*>(this)->GetSlotNo());
    slotItem->set_add_info(m_uid);
    // 残差 N76：ORIG `mov %al,0x31(%edx)` RHS 值先载；4.4 恒 LHS 地址先载
    //（mov %eax,%edx 中转 + mov 0x14(%ebp),%eax），probe 全试不可翻。
    m_saveFlag = m_stomach.IsSaveStomach((CREATURE_STATE)m_state) ? 1 : 0;
    return true;
}

// ============================================================================
// 以下为待头声明落地后的追加实现（TranslateCCreatureTU；已按各 .md 转写完毕）
// ============================================================================

// ORIG _ZN13user_creature9CCreature14RenameCreatureEPKc T 0x83373aa：
// strncpy(m_name, name, 0xc) 后恒返 1。
#include "CCreature/RenameCreature.inc"
{
    strncpy(m_name, name, 0xc);
    return true;
}

// ORIG _ZN13user_creature9CCreature19IsEvolutionCreatureEi T 0x83388ec。
// [N96 清绑 2026-09-09] 原 `register bool eq asm("al")` 全域禁用（P0②/N96/P248 尾条），
// 还原 P168a② 具名 bool 局部自然形（`bool eq = (call()==arg); if (eq) return 1;`）。
// 4.4 发 `sete %al; mov %al,-N; cmpb $0,-N; je`（栈槽物化），ORIG 为
// `sete %al; test %al,%al; je`（寄存器物化）⇒ 自然形不可达，残差 = N96+N18a 封顶。
// 读数：清绑前 our_insns=20（TU 口径 AE）；清绑后 our_insns=22 / orig=20（TU 口径 DIFF）。
#include "CCreature/IsEvolutionCreature.inc"
{
    // round1 R19：ORIG 第二守卫 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`
    // （寄存器物化、无槽）⇒ 按 P225 推广拆嵌套 if 复现 setcc 物化；具名 bool 局部
    // 在 4.4 恒落栈槽（`mov %al,-0x9; cmpb $0,-0x9`，帧 0x28 vs ORIG 0x18），
    // `&&` 合并则被 4.4 折成直跳。
    if (m_script != 0)
    {
        if (m_script->GetEvolutionCreatureId() == creatureId)
            return 1;
    }
    return 0;
}

// ORIG _ZN13user_creature9CCreature24IsEvolutionCreatureMultyEi T 0x8338922。
#include "CCreature/IsEvolutionCreatureMulty.inc"
{
    // ORIG `test; je 返false岛`：call 作 fall-through、return false 作岛（P196 极性）。
    if (m_script != 0)
        return m_script->isEvoluationCreature(creatureId);
    return false;
}

// ORIG _ZN13user_creature9CCreature24IsEventEvolutionCreatureEi T 0x8338bfa。
#include "CCreature/IsEventEvolutionCreature.inc"
{
    // ORIG `test; je 返false岛`：call 作 fall-through、return false 作岛（P196 极性）。
    if (m_script != 0)
        return m_script->validEventEvlouteCreature(creatureId);
    return false;
}

// ORIG _ZN13user_creature9CCreature22GetLearnOverSkillLevelEv T 0x8338c78。
#include "CCreature/GetLearnOverSkillLevel.inc"
{
    if (m_script == 0)
        return -1;
    return m_script->GetLearnOverSkillLevel();
}

// ORIG _ZN13user_creature9CCreature24GetOverSkillRecoveryTimeEv T 0x8338c50。
#include "CCreature/GetOverSkillRecoveryTime.inc"
{
    if (m_script == 0)
        return -1;
    return m_script->GetOverSkillRecoveryTime();
}

// ORIG _ZN13user_creature9CCreature20GetSkillRecoveryTimeEv T 0x8338c28。
#include "CCreature/GetSkillRecoveryTime.inc"
{
    if (m_script == 0)
        return -1;
    return m_script->GetSkillRecoveryTime();
}

// ORIG _ZN13user_creature9CCreature12EvoluteMultyEP5CUseri T 0x8338950
//（EvoluteMulty.md 全转写）：进化目标由调用方给定 creatureId（isEvoluationCreature 校验）。
#include "CCreature/EvoluteMulty.inc"
{
    // [b28d-37 复核 2026-09-09] DIFF→AE：补 P0 具名局部 evolutionCreatureId(-0x18)/
    // nextItemId(-0x10)、P197③ slotItem 预清零、P227 全表达式临时链 trace；
    // 179/180（+1 尾部对齐 nop，§6.1 豁免）。
    if (m_script == 0)
    {
        // P227：ORIG 在 cMyTrace ctor 前右起求值 varargs 并跨 call 保活
        // %ebx/%esi（N80 反面），故用全表达式临时链而非具名 trace 局部。
        cMyTrace(__PRETTY_FUNCTION__, 0x843, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    if (!m_script->isEvoluationCreature(creatureId))
        return false;
    // ORIG 83389eb：mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)（P0 具名局部），
    // 后续 ==0 判定与 FindCreatureScript/包通知均读该槽。
    int evolutionCreatureId = creatureId;
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // ORIG 8338a33：GetItemId() 结果先落 -0x10 槽再回读传 SetItemId（P0）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // P197③：ORIG 8338a48 `movl $0x0,-0xc(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x866, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(evolutionCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

// ORIG _ZN13user_creature9CCreature11ItemEvoluteEP5CUseri T 0x8338ca0
//（ItemEvolute.md 全转写）：目标取 GetItemEvolutionCreatureId(itemIdx)（0x833cad8）。
// 返回 int：ORIG 尾 `mov %ebx,%eax` 直通 + caller CCreatureMgr::ItemEvolute
// 0x833d6c2 call 后无 movzbl 归一（P159/P9 bool→int 判据）。
#include "CCreature/ItemEvolute.inc"
{
    // [b28d-37 复核 2026-09-09] DIFF→AE：补 nextItemId(-0x10) 具名局部 + P197③
    // slotItem 预清零 + P227 trace；173/174（+1 尾部对齐 nop）。
    if (m_script == 0)
    {
        // P227 全表达式临时链（同 EvoluteMulty 失败路径）。
        cMyTrace(__PRETTY_FUNCTION__, 0x89b, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int evolutionCreatureId = m_script->GetItemEvolutionCreatureId(itemIdx);
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // ORIG 8338d6f：GetItemId() 结果先落 -0x10 槽再回读传 SetItemId（P0）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // P197③：ORIG 8338d84 `movl $0x0,-0xc(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8bb, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(evolutionCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

// ORIG _ZN13user_creature9CCreature12EventEvoluteEP5CUserii T 0x833db2e
//（EventEvolute.md 全转写）：活动进化。成长门（xor 链实证）：仅 next 脚本
// IsItemGrowCreature()==0 且 自身 m_isGrow!=1（即成长宠）进入经验折算；
// newExp = get_creature_level_exp((short)(oldExp>>16)/step + 1)，step 取
// GetGrowItemCountforLevelUp()（0→0x19）。包通知携带 param_3(slot)。
#include "CCreature/EventEvolute.inc"
{
    // [b28d-37 复核 2026-09-09] DIFF→AE，含一处真 bug：GetGrowItemCountforLevelUp()
    // 的 receiver ORIG 为 this->m_script(+0x44)（Ghidra C 与 0x833dc30 实证），原实现
    // 误用 nextScript；另补 isGrow(-0x21)/商(-0xc) 具名局部、P197③ 预清零、P227 trace；
    // 246/247（+1 尾部对齐 nop）。
    if (m_script == 0)
    {
        // P227 全表达式临时链（ORIG ctor 前求值实参并保活 %ebx/%esi）。
        cMyTrace(__PRETTY_FUNCTION__, 0x10cc, 5)(
            "cannot event evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int baseCreatureId = GetCreatureId(creatureId);   // +0x85 call
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(baseCreatureId);
    if (nextScript == 0)
        return false;
    int nextItemId = nextScript->GetItemId();
    // ORIG 833dbf0：m_isGrow 先落字节槽 -0x21（P0 具名 bool 局部），
    // 守卫读槽（movzbl -0x21; xor $1; test）而非直读字段。
    bool isGrow = m_isGrow;

    // ORIG：连用两段 xor$1/test 守卫链物化 bool 后 if（P198 ①）。
    // exp 高 16 位→(int)(short)：先用全宽槽保存 m_exp，再读槽+2 高半字
    // movzwl+cwtl（P197 ① union 拆半）。
    if (!nextScript->IsItemGrowCreature() && !isGrow)
    {
        union { int all; unsigned short w[2]; } expSplit;
        expSplit.all = m_exp;
        // ORIG 833dc30：receiver 是 this->m_script(+0x44)，**非** nextScript
        // （Ghidra C 亦为 GetGrowItemCountforLevelUp(*(CCreatureScript**)(this+0x44))）。
        int step = m_script->GetGrowItemCountforLevelUp();     // thunk 0x833f356
        if (step == 0)
            step = 0x19;
        int level = (int)(short)expSplit.w[1] / step;          // ORIG -0xc 商槽
        m_exp = G_CDataManager()->get_creature_level_exp(level + 1);
    }
    SetItemId(nextItemId);
    CItem* item = G_CDataManager()->find_item(nextItemId);
    if (item == 0)
        return false;
    int usablePeriod = item->getUsablePeriod();       // thunk 0x08110c60
    int expirationDate = item->getExpirationDate();   // thunk 0x080f1306
    if (usablePeriod == 0 && expirationDate != 0)
        m_expireTime = expirationDate;                // +0x18 ← 过期日
    else if (usablePeriod == 0 && expirationDate == 0)
        m_expireTime = 0;

    // P197③：ORIG 833dcfd `movl $0x0,-0x14(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EventEvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId(), m_exp))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1104, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(slot);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

}  // namespace user_creature
// ORIG _ZN13user_creature12CDBMsgSender20EventEvoluteCreatureEiii W 0x833fddc
//（objdump 逐条转写）：stream 空→0；buf memset 0x74（ORIG 无判空，如实保留）；
// m_hdr|=0x248；compat.m_itemIdx(+0x08)=itemIdx；m_addInfo2(+0x30)=uid；
// exp 写 +0x4c；find_item(itemIdx)（ORIG 不判空）→ m_days(+0x54)=
// getUsablePeriod()、m_expireTick(+0x58)=getExpirationDate()；返 1。

#include "CCreature/EventEvoluteCreature.inc"
{
    // b28d-37 [N76 封顶复核，ORIG 0x833fddc]：P248 档③字段侧旋钮已试（m_itemIdx int→unsigned 后 store 仍 LHS-first，与 D23 回退实测一致），源码侧直赋/语句表达式亦不可翻；ORIG RHS-first 3 条形保留 N76。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x248;
    buf->m_addInfo2 = uid;                            // +0x30（先写，ORIG 序）
    // ORIG 833fe39：`mov 0x10(%ebp),%edx; mov -0x10(%ebp),%eax; mov %edx,0x8(%eax)`
    // = RHS-first 3 条形。旧判词按 N76 封顶，根因其实是**视图 cast + 同型字段**：
    // P235 直取完整类型具名视角（`buf->m_itemView.m_itemIdx`）配合 P211 档③
    // **异型化**（m_itemIdx 为 unsigned int，RHS 为 int 形参）即翻回 ORIG 序。
    buf->m_itemView.m_itemIdx = itemIdx;              // +0x08
    buf->m_exp = exp;                                 // +0x4c（SigTypes 已具名 m_exp）
    CItem* item = G_CDataManager()->find_item(itemIdx);
    buf->m_days = item->getUsablePeriod();            // +0x54
    buf->m_expireTick = item->getExpirationDate();    // +0x58
    return true;
}
 
 // ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8

// ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8
//（objdump 逐条转写）：type==0x89 走 BigStreamPool::Acquire("Creature.cpp",0xa7)，
// 其余走 StreamPool::Acquire("Creature.cpp",0xa9)；stream 空→false；
// m_user=user；CStreamGuard(m_stream,false) 后经 operator*()（de 0x80c8c46）链式
// 依次 << (int)type、<< (int)GetUID()；return true 落在 guard 作用域内
//（ORIG：mov $1,%ebx 先物化 → guard D1 → mov %ebx,%eax 共享出口）。
// 池记录名取 ORIG .rodata 0x8c2d76a 字面量 "Creature.cpp"（AE 口径归 <A>，语义等价）。
#include "CCreature/Init.inc"
{
    if ((int)type == ENUM_INTERNALPACKET_0x89)
        m_stream = GlobalData::s_big_stream_pool->Acquire("Creature.cpp", 0xa7);
    else
        m_stream = GlobalData::s_stream_pool->Acquire("Creature.cpp", 0xa9);
    if (m_stream == 0)
        return false;
    m_user = user;
    CStreamGuard guard(m_stream, false);
    (*guard)->operator<<((int)type);
    (*guard)->operator<<((int)m_user->GetUID());
    return true;
}

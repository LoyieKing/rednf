// ============================================================================
// df_game_r 还原 —— user_creature::CCreature 专用实现 TU（TranslateCCreatureTU）。
//
// 承载（类声明权威宿主均为 CGameManager.h 的 user_creature 域）：
//   - user_creature::CStomach      （ORIG 0x8336bd0 域；5 方法，报告
//     docs/class_func_reports/user_creature__CStomach/）
//   - user_creature::COverSkillTime（ORIG checkTime 0x833d65e，报告
//     docs/class_func_reports/user_creature__COverSkillTime/）
//   - user_creature::CCreature     （ORIG 38 方法 + 访问器，报告
//     docs/class_func_reports/user_creature__CCreature/ 各 *.md）
//
// 字段全部具名访问（无裸偏移、无固定返回、无 asm 标签）；签名与 ORIG
// mangled 一致。Save/Load/Die/CheckDied/stomach 链此前由 CCreatureMgr.cpp
// 代管，本 TU 建立后为其唯一权威定义宿主；接入构建时需从 CCreatureMgr.cpp
// 移除同名定义（清单见 /tmp/df_batches/TranslateCCreatureTU.md）。
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
bool CStomach::IsSaveStomach(CREATURE_STATE state)
{
    // b28d-37 [N85+N82 封顶，ORIG 0x83?]：x87 比较两向（ORIG fucompp/fnstsw/sahf/sete vs 4.4 fucomip/fstp/setae；P230 换 -march=i486 可消，本波不改路由）+ 槽残差；38/34。
    bool saveOk = true;
    int s = (int)state;
    if (s == 1)
    {
        if (m_value <= 0.0f)   // ORIG test $0x5（C0|C2）：<=0 不可保存；NaN 走 unordered 保持可保存
            saveOk = false;
    }
    else if (s > 0 && s < 4)
    {
        if (m_value == 100.0f)
            saveOk = false;
    }
    return saveOk;
}

// ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime = 当前秒。
void CStomach::Resume()
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
}

// CCreature ctor/dtor/GetOverSkillTime 定义于 CCreatureMgr.cpp（去重保留）。

// ORIG _ZN13user_creature9CCreature3DieEP5CUser T 0x8338536（Die.md）：
// CPacketRespondent(user,0,0).MakeNotipacketDiedCreature() + SendAll()，恒返 1。
bool CCreature::Die(CUser* user)
{
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketDiedCreature();
    pkt.SendAll();
    return true;
}

// ORIG _ZN13user_creature9CCreature17CheckDiedCreatureEP5CUser T 0x83384b8
//（CheckDiedCreature.md）：IsDieCreature 为真时先 Die(user)，返回判定结果。
bool CCreature::CheckDiedCreature(CUser* user)
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
bool CCreature::SaveCreatureItem(Inven_Item* item,
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
bool CCreature::LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
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
    // enum 字段直接 cast 赋值使 4.4 movsbl 拆两段（+%eax 中转）；int& 视图同宽
    // 直存复现 ORIG `movsbl %al,%edx; mov %edx,0x8(%eax)` 单段形。
    reinterpret_cast<int&>(m_creatureItemType) = (int)(char)info->m_creatureItemType;  // movsbl
    m_stomach.SetStomachValue((float)info->m_stomachValue);
    strncpy(m_name, info->m_name, 0xc);            // → +0x1c，12B（ORIG call strncpy PLT）
    // ORIG 8338657：xor $0x1 取反形（X1：bool 字段 register 取反，movzbl+test+je
    // 共享 else 块在后）；高 16 位提取走 union 拆半（P197①：整字落槽 -0x10 →
    // movzwl -0xe + cwtl 符号扩展直传 GetExpLevel）。
    unsigned char isNotGrow = m_isGrow;
    isNotGrow ^= 1;
    if (isNotGrow)
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
bool CCreature::RenameCreature(const char* name)
{
    strncpy(m_name, name, 0xc);
    return true;
}

// ORIG _ZN13user_creature9CCreature19IsEvolutionCreatureEi T 0x83388ec。
bool CCreature::IsEvolutionCreature(int creatureId)
{
    // ORIG: test m_script; je 返 0 岛；body 内 sete %al → test %al → je 共享
    // 返 0 块（X 族 bool 物化，register-bool 落 al 不落槽）。
    if (m_script != 0)
    {
        bool eq = (m_script->GetEvolutionCreatureId() == creatureId);
        if (eq)
            return 1;
    }
    return 0;
}

// ORIG _ZN13user_creature9CCreature24IsEvolutionCreatureMultyEi T 0x8338922。
bool CCreature::IsEvolutionCreatureMulty(int creatureId)
{
    // ORIG `test; je 返false岛`：call 作 fall-through、return false 作岛（P196 极性）。
    if (m_script != 0)
        return m_script->isEvoluationCreature(creatureId);
    return false;
}

// ORIG _ZN13user_creature9CCreature24IsEventEvolutionCreatureEi T 0x8338bfa。
bool CCreature::IsEventEvolutionCreature(int creatureId)
{
    // ORIG `test; je 返false岛`：call 作 fall-through、return false 作岛（P196 极性）。
    if (m_script != 0)
        return m_script->validEventEvlouteCreature(creatureId);
    return false;
}

// ORIG _ZN13user_creature9CCreature22GetLearnOverSkillLevelEv T 0x8338c78。
int CCreature::GetLearnOverSkillLevel()
{
    if (m_script == 0)
        return -1;
    return m_script->GetLearnOverSkillLevel();
}

// ORIG _ZN13user_creature9CCreature24GetOverSkillRecoveryTimeEv T 0x8338c50。
int CCreature::GetOverSkillRecoveryTime()
{
    if (m_script == 0)
        return -1;
    return m_script->GetOverSkillRecoveryTime();
}

// ORIG _ZN13user_creature9CCreature20GetSkillRecoveryTimeEv T 0x8338c28。
int CCreature::GetSkillRecoveryTime()
{
    if (m_script == 0)
        return -1;
    return m_script->GetSkillRecoveryTime();
}

// ORIG _ZN13user_creature9CCreature12EvoluteMultyEP5CUseri T 0x8338950
//（EvoluteMulty.md 全转写）：进化目标由调用方给定 creatureId（isEvoluationCreature 校验）。
bool CCreature::EvoluteMulty(CUser* user, int creatureId)
{
    // b28d-37 [N82 封顶，ORIG 0x83?]：帧/槽残差（179/174）。
    if (m_script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x843, 5);
        trace("cannot evolute because it haven't creature script uid(%d), "
              "creature id(%d) \n", user->get_unique_id() & 0xffff,
              reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    if (!m_script->isEvoluationCreature(creatureId))
        return false;
    if (creatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(creatureId);
    if (nextScript == 0)
        return false;
    SetItemId(nextScript->GetItemId());
    Inven_Item* slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x866, 5);
        trace("cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(creatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}

// ORIG _ZN13user_creature9CCreature11ItemEvoluteEP5CUseri T 0x8338ca0
//（ItemEvolute.md 全转写）：目标取 GetItemEvolutionCreatureId(itemIdx)（0x833cad8）。
// 返回 int：ORIG 尾 `mov %ebx,%eax` 直通 + caller CCreatureMgr::ItemEvolute
// 0x833d6c2 call 后无 movzbl 归一（P159/P9 bool→int 判据）。
int CCreature::ItemEvolute(CUser* user, int itemIdx)
{
    // b28d-37 [N82 封顶，ORIG 0x83?]：帧/槽残差（173/170）。
    if (m_script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x89b, 5);
        trace("cannot evolute because it haven't creature script uid(%d), "
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
    SetItemId(nextScript->GetItemId());
    Inven_Item* slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8bb, 5);
        trace("cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
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
bool CCreature::EventEvolute(CUser* user, int creatureId, int slot)
{
    // b28d-37 [N82 封顶，ORIG 0x83?]：帧/槽残差（246/244）。
    if (m_script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x10cc, 5);
        trace("cannot event evolute because it haven't creature script uid(%d), "
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

    // ORIG：连用两段 xor$1/test 守卫链物化 bool 后 if（P198 ①；不落槽直测）。
    // exp 高 16 位→(int)(short)：先用全宽槽保存 m_exp，再读槽+2 高半字
    // movzwl+cwtl（P197 ① union 拆半）。
    if (!nextScript->IsItemGrowCreature() && m_isGrow != 1)
    {
        union { int all; unsigned short w[2]; } expSplit;
        expSplit.all = m_exp;
        int progress = (int)(short)expSplit.w[1];
        int step = nextScript->GetGrowItemCountforLevelUp();   // thunk 0x833f356
        if (step == 0)
            step = 0x19;
        int level = (int)progress / step + 1;                  // idivl 后 +1
        m_exp = G_CDataManager()->get_creature_level_exp(level);
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

    Inven_Item* slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EventEvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId(), m_exp))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1104, 5);
        trace("cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
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

bool user_creature::CDBMsgSender::EventEvoluteCreature(int uid, int itemIdx, int exp)
{
    // b28d-37 [N76 封顶复核，ORIG 0x833fddc]：P248 档③字段侧旋钮已试（m_itemIdx int→unsigned 后 store 仍 LHS-first，与 D23 回退实测一致），源码侧直赋/语句表达式亦不可翻；ORIG RHS-first 3 条形保留 N76。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x248;
    buf->m_addInfo2 = uid;                            // +0x30（先写，ORIG 序）
    // ORIG `mov %edx,0x8(%eax)` buf 基址 disp8 直存（具名视图，探针 bl8_probe6 f5）。
    // [b28-BM7 判词] NEAR 2 = N76：ORIG 本 store 为 RHS-first 3 条形（先
    // `mov 0x10(%ebp),%edx` 后载 this），相邻 +0x4c exp store 却 LHS-first；
    // 同函数两向并存排除类型律——int/unsigned 字段×直赋×语句表达式 4 形探针
    // 恒 LHS-first（/tmp/bm7/p2-p4.o），源码层不可复现。
    reinterpret_cast<SigCreatureItemIdxView*>(buf)->m_itemIdx = itemIdx;  // +0x08
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
bool user_creature::CDBMsgSender::Init(ENUM_INTERNALPACKET type, CUser* user)
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

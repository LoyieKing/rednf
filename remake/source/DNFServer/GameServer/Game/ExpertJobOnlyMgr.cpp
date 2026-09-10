// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobOnlyMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CExpertJob.h"

#include "CBattle_Field.h"     // CBattle_Field::SetAssignNPCByPotion（UseDelilah）
#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript / find_item
#include "CParty.h"            // GetParty/get_user（+0xb24 内嵌战场区）
#include "CPrivateStoreMgr.h"  // private_store 单例 / IsBusyPrivateStore
#include "CStackableItem.h"    // m_int2d0（APC 药水持续秒）
#include "CSystemTime.h"       // CSystemTime::getCurSec
#include "InstanceRentalSystem.h"  // CheckRentableItem（OnEnchantByBead 租赁双查）
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // s_systemTime_
#include "PacketGuard.h"
#include "SkillSlot.h"      // SkillSlot::delete_skill
#include "GameWorld.h"      // G_GameWorld / send_all

// CExpertJobList.cpp 定义的专家炼金信息表（B 0x094f7778，8 字节；
// [1] 即 ORIG 高半区 _DAT_094f777c）。
extern int g_alchemistInfo[2];

namespace expert_job
{

namespace
{

// 自身扩展数据槽（charac_expand 槽 0）快捷读取。b28d-solo：P49 必须用**宏**——
// -O0 下 static inline helper 会发射真实 call 符号（ORIG 无该符号，属发明符号），
// 且每个使用点须独立重发 GetCharacExpandData 调用（P48，ORIG 逐处 call 实证）。
#define GetSelfExpand(user) \
    reinterpret_cast<CCharacExpertJob*>((user)->GetCharacExpandData( \
        (ENUM_CHARAC_EXPAND_TYPE)0))

}  // namespace

// ===========================================================================
// expert_job::CAlchemist
// ===========================================================================

#include "ExpertJobOnlyMgr/CDollController.inc"
{
}

// ---- UseDelilah（报告 UseDelilah.md，ORIG T 0x085d18ae sz 0x33）----
#include "ExpertJobOnlyMgr/UseDelilah.inc"
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->m_battleField.SetAssignNPCByPotion(
            true);
    }
}

// ---- UseRevivalItem（报告 UseRevivalItem.md，ORIG T 0x085d1694 sz 0x7d）----
// b28d-solo-ExpertJobOnlyMgr：AE（near 1→0，47 件 identical/AE）。
// ORIG 0x085d1694 多实参 call 首值 callee-saved 角色 = %esi（GetRevivalTime）/
// %ebx（GetUID）；4.4.7 对裸内联实参恒反向（首值 %ebx）。修法 = P231 裸
// `register int` 局部（非 asm 绑定，P0 合规）：两值以 register 提示升格
// callee-saved 且不落槽，分配偏好翻正为 ORIG 的 esi/ebx 序。
#include "ExpertJobOnlyMgr/UseRevivalItem.inc"
{
    // b28d-solo 试形：P231 裸 register（非绑定，P0 合规）复现 ORIG 的
    // GetRevivalTime→%esi / GetUID→%ebx 分配偏好（4.4.7 恒反向）。
    user->SetRevivalTime(GlobalData::s_systemTime_.getCurSec() + 0x4b0);
    register int revivalTime = user->GetRevivalTime();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)0, uid, (TIMER_MESSAGE)0x82, 0x4b0,
        revivalTime, 0);
}

// ---- GetQuestItemDropRate（报告，ORIG T 0x085d12ee sz 0x71）----
#include "ExpertJobOnlyMgr/GetQuestItemDropRate.inc"
{
    // ORIG 0x085d12ee：A || B 短路合并（jne→mov$1 / je→mov$0 + test，boosted 不落槽）
    if (IsExistContinuousEffectItem(user, 0x27ac52) ||
        IsExistContinuousEffectItem(user, 0x1f8e))
    {
        return (g_alchemistInfo[1] + 10000) * rate / 10000;
    }
    return rate;
}

// ---- CheckUsableStackableItem（报告，ORIG T 0x085d18e2 sz 0x96）----
// [IG1 2026-09-10 S2 授权] bool 化：ORIG 调用点 xor $1/test/je 直证返回 bool
// （P248 档①/P292；返回类型不入 mangled、不改布局；定义侧机器码/尺寸 0x97 实测不变）。
#include "ExpertJobOnlyMgr/CheckUsableStackableItem.inc"
{
    // ORIG：CStackableItem::get_sub_type()==2（APC 强力药水）时检查全队
    // GetUseAPCPotionTime 未到者存在则禁用；其余放行。
    if (static_cast<const CStackableItem*>(item)->get_sub_type() == 2)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member != 0)
                {
                    // b28：清 register asm 绑定（N96/P0 全域禁），直测内联。
                    if (member->GetUseAPCPotionTime() >
                        GlobalData::s_systemTime_.getCurSec())
                        return false;
                }
            }
        }
    }
    return true;
}

// ---- GetAssaultPrevent（报告，ORIG T 0x085d137c sz 0xa7）----
// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x085d140a/0x085d140d）后 0 差。
#include "ExpertJobOnlyMgr/GetAssaultPrevent.inc"
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (IsExistContinuousEffectItem(user, 0x27ac4a))
    {
        return 1;
    }
    if (party != 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            CUser* member = party->get_user(i);
            // b28c-1：P223 多岛形（ORIG 0x085d140a member==0 岛 / 0x085d140d
            //  member==user 岛），清绑后裸条件 + goto 到相邻尾标签。
            if (member == 0)
                goto NEXT1;
            if (member == user)
                goto NEXT2;
            // b28：清 register asm（N96/P0），裸 if 直测。
            if (IsExistContinuousEffectItem(member, 0x27ac4a))
            {
                if (peerFlag != 0)
                {
                    *peerFlag = true;
                }
                return 1;
            }
        NEXT1:
            ;
        NEXT2:
            ;
        }
    }
    return 0;
}

// ---- OnUseStackableItem（报告，ORIG T 0x085d1712 sz 0x19b）----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyMgr/OnUseStackableItem.inc"
                                   CItem const* item)
{
    if (item->is_stackable())
    {
        // vtbl+0xc = GetItemType()==3 且对象 +0x199 字节非零 → 可学 recipe。
        if (((const CStackableItem*)item)->GetItemType() == 3 &&
            // [b23S3C] §4.3 字段化：+0x199 = CStackableItem::m_recipe174(+0x174)
            // .m_field25(+0x25)（CStackableItem.h RecipeInfo 布局注释实证），
            // 裸偏移 `add $0x199 + movzbl` 形 ⇒ 具名成员直读（ORIG movzbl 0x199）。
            ((const CStackableItem*)item)->m_recipe174.m_field25 != 0)
        {
            STExpertJobScript* script =
                (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
                    user->GetCurCharacExpertJobType());
            if (script != 0)
            {
                // [b23S3C] §4.1 反发明符号 + P0/P48 去缓存：ORIG 0x085d17a4/0xb6
                // 两次直调 CUser::GetCharacExpandData(0)（无本域 helper），
                // exp/level/learnRecipe 结果全程 %eax/%ebx 无槽（帧 sub $0x24，
                // 仅 -0xc 一个 script 局部）⇒ 实参右起内联（exp→level→expand→
                // uid/script 落槽，与 ORIG call 序一致）。
                // [b23S3C] P191 极性（learnRecipe 返回 bool 后复查）：ORIG 0x85d17d5
                // `test %al,%al; je +0xeb`（false 跳转到 return 0xd 岛、成功路径
                // fall-through）⇒ 正向 if + else(return) 形（负形 if(!x) 使 4.4 产
                // xor $1 折叠，多 1 条）。
                if (((CCharacExpertJob*)user->GetCharacExpandData(
                         (ENUM_CHARAC_EXPAND_TYPE)0))
                        ->learnRecipe(
                            itemUniqueId, script,
                            user->GetCurExpertJobLevel(
                                user->GetCurCharacExpertJobExp())))
                {
                    ((CCharacExpertJob*)user->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)0))
                        ->SendExpertJobInfo(user);
                }
                else
                {
                    return 0xd;
                }
            }
        }

        // ORIG 0x085d17e2：sub_type()==2 的 APC 药水——记录使用截止时间
        // SetUseAPCPotionTime(curSec + m_int2d0/1000)。
        // 反汇编铁证：imul $0x10624dd3 + sar $0x6 = 除以 1000（毫秒→秒），
        // 非 1e6（0x431bde83+sar 0x12 才是 /1e6；旧注释误判魔数）。
        if (((const CStackableItem*)item)->get_sub_type() == 2)
        {
            user->SetUseAPCPotionTime(
                GlobalData::s_systemTime_.getCurSec() +
                ((const CStackableItem*)item)->m_int2d0 / 1000);
        }
    }

    // 固定道具唯一 id 分派（分支序照 ORIG 汇编比较树）：
    //   0x28e7cf / 0x27ac54 / 0x29f287 / 0x29f2ed → UseRevivalItem
    //   0x27acda → UseDelilah；其余不处理。
    // [b23S3C] ORIG 0x085d185c 起 `mov 0xc(%ebp),%eax` 一次载入后以
    // 0x28e7cf 为中枢二分（cmp/je、cmp/ja、cmp/je、cmp/je、jmp、cmp/je、cmp/jne）
    // ⇒ 原码是 switch(itemUniqueId)（或然链形会逐次 cmp 内存实参且无线性载入）。
    switch (itemUniqueId)
    {
    case 0x28e7cfUL:
    case 0x27ac54UL:
    case 0x29f287UL:
    case 0x29f2edUL:
        UseRevivalItem(user);
        break;
    case 0x27acdaUL:
        UseDelilah(user);
        break;
    default:
        break;
    }
    return 0;
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointer（分解术士三接口）
// ===========================================================================

namespace expert_job
{

// ---- OnEnterDisjointStore（报告，ORIG T 0x085d309a sz 0x101）----
// 包 0xf5：byte(1) byte(等级+1) int(修理费) int(耐久)。
#include "ExpertJobOnlyMgr/OnEnterDisjointStore.inc"
{
    PacketGuard packet;
    packet.put_header(1, 0xf5);
    packet.put_byte(1);
    packet.put_byte(get_disjoint_machine_grade() + 1);
    packet.put_int(get_disjoint_cost());
    // ORIG：owner/expand 无槽，链式内联（P52 单语句）。
    packet.put_int(((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)0))
                       ->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnRepairDisjointMachine（报告，ORIG T 0x085d35ce sz 0x218）----
// 分支 A：现价修理；分支 B（钱不够）：GetDisjointMachineRepair 以全部现金
// 换算可修值并回写现价。新耐久 <1 → 错误 0x16。
#include "ExpertJobOnlyMgr/OnRepairDisjointMachine.inc"
{
    int newEndurance = 0;
    int endurance =
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance();
    int grade =
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_grade();
    int currentPrice =
        GetScript()->GetDisjointMachineRepairCost(grade + 1, endurance);

    if (user->getCurCharacMoney() < currentPrice)
    {
        // 现金不足：以全部金钱换算，返回基础值 + 当前耐久（出参回写现价）。
        newEndurance =
            GetScript()->GetDisjointMachineRepair(
                grade + 1, user->getCurCharacMoney(), currentPrice) +
            endurance;
    }
    else
    {
        stEnduranceRepairCost* cost =
            GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
        if (cost != 0)
        {
            newEndurance = cost->m_field4;   // 满值槽
        }
    }

    if (newEndurance < 1)
    {
        return 0x16;
    }

    user->getCurCharacInvenW()->use_money(currentPrice, (eMoneySubReason)0x20, true);
    ((CCharacExpertJob*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
        ->set_disjoint_machine_endurance(newEndurance);

    PacketGuard packet;
    packet.put_header(1, 0xf0);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacMoney());
    packet.put_int(
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnUpgradeDisjointMachine（报告，ORIG T 0x085d37e8 sz 0x2e2）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（块序 N93附/N121 类）+ 真语义修复。
// 修复：①P49 宏化 GetSelfExpand（原 static inline 发射发明符号
//   _ZN10expert_job12_GLOBAL__N_113GetSelfExpandEP5CUser，ORIG 无）⇒ 每使用点
//   独立重发 GetCharacExpandData 调用（P48）；②**真 bug** put_int(grade + 1)
//   （ORIG 0x085d3a2e `mov -0x18(%ebp),%eax; add $0x1,%eax`，原实现漏 +1）；
//   ③keyByte unsigned char + find 直传其地址（无 cast 临时）+ 两次独立
//   GetEtcScript() + 非 const iterator（P48/P96/P270）；④upgradeCost<0||==0
//   双独立守卫（P133①）；⑤比较方向 `it->second > level`（ORIG 先算 RHS）。
// 残差：it==end 的 0x13 错误块物理位置（ORIG 0x085d38ee 紧跟 0xe 块后，4.4 恒
// 置于函数尾 fall-through；202 vs 201，差 1 条 jmp）。else-if/正向包裹两形互试
// ⇒ 正向包裹最优（identical 20 vs 19）。p167 调用点 38/38 CLEAR、指令集合与
// 槽位集全等 ⇒ 语义完成、块序 N 封顶。
#include "ExpertJobOnlyMgr/OnUpgradeDisjointMachine.inc"
{
    int grade = GetSelfExpand(user)->get_disjoint_machine_grade();

    if (!(grade + 1 < GetScript()->GetDisjointMachineMaxGrade()))
    {
        return 0x13;
    }
    int expertLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    if (grade + 1 >= expertLevel)
    {
        return 0x13;
    }

    // Etc 脚本 [expertjob level limit] 表：key=grade+2 → 所需角色等级。
    // b28d-solo 修法（P48/P96/P270）：ORIG 0x085d386c keyByte 为 unsigned char
    // 且 find 直传其地址（无 cast 临时）；0x085d386f/0x085d388f 两次独立
    // GetEtcScript()（禁缓存 map 引用局部）；迭代器为非 const（ORIG 仅发射
    // _Rb_tree_iterator 实例，const_iterator 会多转换构造）。
    unsigned char keyByte = (unsigned char)(grade + 2);
    std::map<unsigned char, short>::iterator it =
        GetEtcScript()->m_levelMap.find(keyByte);
    // b28d-solo（P222 正向包裹）：ORIG 把 it==end 的 0x13 错误块放在 0xe 块
    // **之后**（0x085d38ee vs 0x085d38e4）；实测 else-if 链劣化（identical
    // 20→19），正向包裹为最优形（残差仅块序，见函数头判词）。
    // [2026-09-10 复测] 显式 `else { return 0x13; }` 形实测无效（仍 202/201，
    // 4.4 把 else 出口排到函数尾 landing pad 之后）⇒ 块序属 N93附/N121 族。
    if (it != GetEtcScript()->m_levelMap.end())
    {
        // ORIG 0x085d38c9 先取 it->second（movzwl+movswl→%ebx）再调
        // get_charac_level（→%eax）、`cmp %eax,%ebx; setg` ⇒ 比较方向写
        // `it->second > level`（等价改写，先算 RHS）。
        if (it->second > user->get_charac_level())
        {
            return 0xe;
        }

        ++grade;   // 升级
        int upgradeCost = GetScript()->GetUpgradeCost((unsigned)grade);
        // ORIG 0x085d3913/0x085d3919 两条**独立**比较（js <0 / jne !=0 共落
        // 0x137 错误块）⇒ 拆两条独立守卫（P133①）；money 检查独立成句。
        if (upgradeCost < 0 || upgradeCost == 0)
        {
            return 0x16;
        }
        if (user->getCurCharacMoney() < upgradeCost)
        {
            return 0x16;
        }

        user->getCurCharacInvenW()->use_money(upgradeCost,
                                              (eMoneySubReason)0x20, true);
        GetSelfExpand(user)->set_disjoint_machine_grade(grade);

        // 新等级满值耐久若存在则一并重置。
        stEnduranceRepairCost* full =
            GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
        if (full != 0)
        {
            GetSelfExpand(user)->set_disjoint_machine_endurance(full->m_field4);
        }

        PacketGuard packet;
        packet.put_header(1, 0xf4);
        packet.put_byte(1);
        packet.put_int(user->getCurCharacMoney());
        // b28d-solo 真语义修正：ORIG 0x085d3a2e `mov -0x18(%ebp),%eax;
        // add $0x1,%eax` ⇒ put_int(grade + 1)（原实现少 +1）。
        packet.put_int(grade + 1);
        packet.put_int(GetSelfExpand(user)->get_disjoint_machine_endurance());
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    return 0x13;
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDollController —— 抽象派生（ORIG vtable @0x8cc08e0：4 业务槽
// 全 __cxa_pure_virtual + D1/D0）。ctor 仅基类构造+vptr 置位，dtor 链基类析构，
// 由类定义自然生成；完整类布局建议见 decl 文件。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/dtor_CDollController.inc"
{
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CExpertJobMgr
// ===========================================================================

namespace expert_job
{

// ---- OnLeaveUser（报告，ORIG T 0x0849e214 sz 0x41）----
// b28c-1 N7: 虚调用 callee %ecx↔%edx 互换。
#include "ExpertJobOnlyMgr/OnLeaveUser.inc"
{
    // ORIG 0x0849e214：getter 两次独立调用、返回值不出具名槽（P0 律）；
    // 虚调用实参用第二次返回值。虚调用 vptr+4 = 业务槽 1 OnCloseMission。
    // [b22判词·N-REG] 非空判断 ORIG 为 `test;setne;test;je` bool 惯用法且
    // 虚调用指针走 %edx/%ecx 暂存，gcc 新版直接 `cmpl $0`+单寄存器复用；
    // 寄存器/惯用法选择为编译器结构，不可源码复现，语义等价。
    if (user->GetCurCharacExpertJob())
        user->GetCurCharacExpertJob()->OnCloseMission(
            user, ENUM_CLOSE_MISSION_TYPE_1);
}

// ---- IsAvailableRequestPeer（报告，ORIG T 0x0849e55e sz 0xad）----
#include "ExpertJobOnlyMgr/IsAvailableRequestPeer.inc"
                                                   CUser* target,
                                                   ENUM_PEER_REQUEST_TYPE type,
                                                   PacketGuard& packet)
{
    (void)type;
    (void)packet;
    // b28d-31: ORIG 0x0849e564 为 `||` 短路物化（je P; jne Q; P:mov$1; Q:mov$0;
    // test;je）——负极性守卫 return 3 直落；ExpertJob 两守卫为独立形
    // （各发 test;setne;test;je，P133① 独立守卫）。
    if (target->getCurCharacR() == 0 || requester->getCurCharacR() == 0)
        return 3;
    if (requester->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (target->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(target))
        return 0x3d;
    if (target->IsCurCharacGhost())
        return 0x13;
    return 0;
}

// ---- OnRequestPeer（报告，ORIG T 0x0849e256 sz 0x10b）----
// b28c-1 N-88: 槽比零 setne 物化（具名 bool 实测 2→11 回退）。
#include "ExpertJobOnlyMgr/OnRequestPeer.inc"
                                  ENUM_PEER_REQUEST_TYPE type,
                                  PacketGuard& packet)
{
    unsigned int err = 0;
    err = IsAvailableRequestPeer(requester, target, type, packet);
    // b28：清 register asm（N96/P0 全域禁）——直测短路条件。
    // b28d-31：P225 while+break 复现 4.1 槽比零 setne 物化
    //（ORIG 0x0849e26b `cmpl $0,-0xc(%ebp); setne %al; test %al,%al; je`）。
    while (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)10, type,
                                          (unsigned char)err, packet);
        return;
    }
    packet.clear();
    packet.put_header(0, 7);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int((int)(target->get_unique_id() & 0xffffffff));
    packet.finalize(true);
    target->Send(packet);
}

// ---- OnResponsePeer（报告，ORIG T 0x0849e362 sz 0x1fb）----
// b28d-solo-ExpertJobOnlyMgr：AE（DIFF→AE，48 件 identical/AE）。
// 两处修法：①**真语义 bug**——首包 put_header 前多一次 packet.clear()，
// ORIG 全函数仅 0x0849e4ce 一次 clear（首包 Send 之后）⇒ 删除（P171 照抄）；
// ②P277 派生 cast 前置到调用点
//（`static_cast<CEnchanter*>(enchanter)->SetPeerRequestType(type)`）复现 ORIG
// 0x0849e43b 段 this-first 装载（`mov -0xc,%eax; mov 0x20,%edx; …`）；
// 虚调用 OnStartMission 保持基类 CExpertJob* 形（槽 0）。
#include "ExpertJobOnlyMgr/OnResponsePeer.inc"
                                   CUser* jobOwnerA, CUser* jobOwnerB,
                                   int cookie, ENUM_PEER_REQUEST_TYPE type,
                                   PacketGuard& packet)
{
    // ORIG 0x849e368：err 预清槽（movl $0x0,-0x10）+ 独立赋值（P197-③ 两句式）；
    // 0x849e39d setne %al 判定物化（4.4 恒折槽比零，P197-④/N-88）。
    unsigned int err = 0;
    err = IsAvailableRequestPeer(requester, peerTarget, type, packet);
    // b28d-31：P225 while+break 复现 4.1 槽比零 setne 物化（ORIG 0x0849e39d）。
    while (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type,
                                          (unsigned char)err, packet);
        return;
    }

    CExpertJob* enchanter = AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)1);
    if (enchanter == 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type, 0x16, packet);
        return;
    }

    // b28d-solo 试形（P277）：派生 cast 前置到调用点 ⇒ 复现 ORIG
    // 0x0849e43b 段 this-first 装载（`mov -0xc,%eax; mov 0x20,%edx; …`）。
    enchanter->OnStartMission(jobOwnerA, jobOwnerB);   // ORIG vptr+0 业务槽 0
    static_cast<CEnchanter*>(enchanter)->SetPeerRequestType(type);

    // b28d-solo 真语义修正（P171 照抄）：ORIG 0x0849e44d 首包前**无** clear，
    // 唯一 clear 在首包 Send 之后（0x0849e4ce）⇒ 删除多余的一次 clear()。
    packet.put_header(0, 8);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int(cookie & 0xffffffff);
    packet.finalize(true);
    peerTarget->Send(packet);

    packet.clear();
    packet.put_header(1, 0xb);
    packet.put_byte(1);
    packet.put_short((int)(peerTarget->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.finalize(true);
    requester->Send(packet);
}

// ---- OnEnchantByBead（报告，ORIG T 0x0849ed1a sz 0x474）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/N115/N7）+ 真语义修复。
// 修复：①**真 bug**——统计调用漏第二次 `masterItem->get_index()`
//   （ORIG 0x0849f076 为第二次独立调用，P48；p167 修前 MISSING 1 → 修后 46/46
//   CLEAR）；②m_fieldd 写入时机提前到发包前（ORIG 0x0849f288）且 EnchantItem
//   第 3 参**重读** ringRef->m_fieldd（无 replacedCardIdx 具名局部，P0/P48）；
//   ③cMyTrace 改 `__PRETTY_FUNCTION__` + 全表达式临时链（P118/P139/P227）。
// 残差：帧 0x60 vs ORIG 0x6c（4.4 带 call 帧恒 16k 量化，N115/P190②不可凑）+
// 缺 push %edi（ORIG 用 %edi 存 &user->m_historyLog，4.4 用 %esi）⇒ 全帧槽号
// 整体差 0x10；callee-saved 角色互换（N7/N82附）。指令集合/调用点全等。
#include "ExpertJobOnlyMgr/OnEnchantByBead.inc"
                                   int param4, int ringSlot)
{
    (void)param2;
    (void)param4;

    Inven_Item* beadRef = user->getCurCharacInvenW()->GetInvenRef(1, beadSlot);
    if (beadRef == 0)
    {
        return 0x11;
    }
    Inven_Item* ringRef = user->getCurCharacInvenW()->GetInvenRef(1, ringSlot);
    if (ringRef == 0)
    {
        return 0x11;
    }

    // 租赁两连查（ORIG 0x82702dc 实证：查的是 ringRef 的 m_addInfo，kind=1/2）
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            1, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            2, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }

    // P48：ORIG G_CDataManager() 每用点 fresh（无 dm 槽）
    CItem* beadItem = G_CDataManager()->find_item(beadRef->m_addInfo);
    if (beadItem == 0)
    {
        return 0x11;
    }
    CItem* ringItem = G_CDataManager()->find_item(ringRef->m_addInfo);
    if (ringItem == 0)
    {
        return 0x11;
    }
    if (!beadItem->is_stackable())
    {
        return 0x11;
    }

    // 怪物卡主表链（ORIG 序）：cardId→masterItem→EnableWorld 校验
    int cardId = ((CStackableItem*)beadItem)->GetMonsterCardId();
    CItem* masterItem = G_CDataManager()->find_item(cardId);
    if (masterItem == 0)
    {
        return 0x11;
    }
    if (EnableWorld(ringItem, beadItem) != 1)
    {
        return 0x13;
    }
    // ORIG 0x849eeb3：ringItem->GetItemType() 结果作实参调
    // masterItem->getUsableItemType()（虚槽 vptr+0xc / vptr+0x20），失败返回 0x17。
    if (!masterItem->getUsableItemType(ringItem->GetItemType()))
    {
        return 0x17;
    }

    bool deleted =
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, beadSlot, 1,
                                                (eItemDelReason)8, true);
    if (!deleted)
    {
        // b28d-solo（P118/P139/P227）：ORIG 0x0849ef5f ctor 实参为
        // __PRETTY_FUNCTION__ 局部静态符号，且 getCurCharacNo() 在 ctor **前**
        // 求值并入 %ebx 跨 ctor 保活（全表达式临时链）⇒ 不用具名 trace 对象。
        cMyTrace(__PRETTY_FUNCTION__, 0x6cd, 5)(
            "CExpertJobMgr::OnEnchantByBead, getCurCharacInvenW()->delete_item "
            "failed , %d %d", user->getCurCharacNo(), beadSlot);
        return 0x11;
    }

    int savedCardIdx = ringRef->m_fieldd;
    // b28d-solo：ORIG 0x0849f076 先写 ringRef->m_fieldd = masterItem->get_index()
    //（第一次调用），无 replacedCardIdx 具名局部（P0 无槽往返）。
    ringRef->m_fieldd = masterItem->get_index();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(2);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, ringSlot, packet);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, beadSlot, packet);
    packet.finalize(true);
    user->Send(packet);

    // 统计（ORIG 0x860f940/0x860f9d4：increaseItemCount((ENUM_EXPERT_JOB_TYPE)1,
    // masterIndex, 1)）——ORIG 0x0849f076 为**第二次独立** get_index 调用（P48）。
    GetInstanceExpertJobStatistic()->increaseItemCount(
        (ENUM_EXPERT_JOB_TYPE)1, (unsigned long)masterItem->get_index(), 1);

    // 历史（ORIG 0x8684d74：EnchantItem(ringRef->m_addInfo, savedCardIdx,
    // ringRef->m_fieldd 重读, true, acc_name, characName)）
    user->m_historyLog.EnchantItem(ringRef->m_addInfo, savedCardIdx,
                                   ringRef->m_fieldd, true, user->get_acc_name(),
                                   user->getCurCharacName());

    packet.clear();
    packet.put_header(1, 0x113);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ---- OnGiveup（报告，ORIG T 0x0849e60c sz 0x70d）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/P75 求值序 + 帧槽差）+ 真结构还原。
// 还原：①删除 TU-local SendGiveupError helper（**发明符号**）⇒ 6 处
//   `throw (ENUM_ERROR)<code>` + `catch (ENUM_ERROR err){ SendCmdErrorPacket
//   (0xf3,(uchar)err); }`（P6/P229；p167 修前 MISSING __cxa_throw×6/
//   __cxa_begin_catch/__cxa_end_catch/__cxa_allocate_exception×6/GetCurCharac
//   ExpertJobType/getCurCharacQuestR/vector<pair>::operator[]/… + EXTRA
//   SendGiveupError×5 → 修后 95/95 CLEAR）；②补 jobType==0 独立守卫（ORIG
//   0x0849e6d0，第二次独立调用取 GetExpertJobScript 实参）；③get_quest_info
//   receiver 改 getCurCharacQuestR（ORIG 实证，非 QuestW）；④m_growSkillVec
//   每轮**两次** operator[] + 两次 getCurCharacSkillW（P48）；⑤删具名 inven
//   局部（P0，getCurCharacInvenW() 结果不落槽）；⑥两循环改 P283 goto 尾标签形
//   （ORIG begin/end 后无条件 jmp 底部条件块，for 恒条件置顶）。
// 残差：4.4 实参右→左求值序 vs ORIG 左→右（skillIdx 先于 get_charac_job，
// P75/N82附）、帧 0x50 vs ORIG 0x60、PacketGuard 槽 -0x34 vs -0x30；
// 指令数 460 vs 458，调用点 95/95 CLEAR，语义完成。
#include "ExpertJobOnlyMgr/OnGiveup.inc"
{
    try
    {
        if (user->getCurCharacR() == 0)
        {
            throw (ENUM_ERROR)3;
        }
        // ORIG 0x0849e6d0：jobType==0 单独守卫（第二次独立调用取实参）。
        if (user->GetCurCharacExpertJobType() == 0)
        {
            throw (ENUM_ERROR)8;
        }
        STExpertJobScript* script =
            (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
                user->GetCurCharacExpertJobType());
        if (script == 0)
        {
            throw (ENUM_ERROR)8;
        }

        unsigned int giveupCount =
            GetSelfExpand(user)->get_giveup_count() & 0xffff;
        if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
        {
            throw (ENUM_ERROR)1;
        }
        int giveupCost = script->m_giveupCost.at(giveupCount);
        if (user->getCurCharacMoney() < giveupCost)
        {
            throw (ENUM_ERROR)0x15;
        }

        // 前置校验：[clear quest] 清单全部已达标，任一未清 → 错误 8。
        // b28d-solo（P283）：ORIG begin/end 求值后**无条件 jmp 底部条件块**，
        // 用 goto 尾标签复现（4.4 对 for 恒条件置顶）；两循环复用同一对迭代器槽
        // （ORIG -0x20/-0x24 实证）。
        std::vector<int>::iterator it = script->m_clearQuests.begin();
        std::vector<int>::iterator itEnd = script->m_clearQuests.end();
        goto clearCheckCond;
    clearCheckBody:
        if (user->getCurCharacQuestRefR()->isClearQuest(*it) != true)
        {
            throw (ENUM_ERROR)8;
        }
        ++it;
    clearCheckCond:
        if (it != itEnd)
        {
            goto clearCheckBody;
        }

        it = script->m_clearQuests.begin();
        itEnd = script->m_clearQuests.end();
        goto clearResetCond;
    clearResetBody:
        user->getCurCharacQuestW()->resetClearQuest(*it);
        ++it;
    clearResetCond:
        if (it != itEnd)
        {
            goto clearResetBody;
        }

        PacketGuard packet;
        // ORIG 0x0849e915：getCurCharacInvenW() 结果**不落槽**（P0 无具名局部），
        // 直接作 receiver 调 use_money（具名 inven 会多占一槽致全帧下移 4）。
        user->getCurCharacInvenW()->use_money(giveupCost, (eMoneySubReason)0x1e,
                                              true);

        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte(0);
        packet.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, packet);
        packet.finalize(true);
        user->Send(packet);

        // 删除双树技能（[skill] 表 first = skillIdx）。ORIG 每轮**两次**
        // operator[] + 两次 getCurCharacSkillW（P48 逐点重取，无具名 skillIdx）。
        for (unsigned int i = 0; i < (unsigned int)script->m_growSkillVec.size();
             ++i)
        {
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                script->m_growSkillVec[i].first, user->get_charac_job(),
                (ENUM_SKILL_TREE_KIND)0);
            ((SkillSlot*)user->getCurCharacSkillW())->delete_skill(
                script->m_growSkillVec[i].first, user->get_charac_job(),
                (ENUM_SKILL_TREE_KIND)1);
        }
        user->send_skill_info();

        user->LogHistory("ExpertJob-,%d,%d,%d", user->get_charac_level(),
                         user->GetCurCharacExpertJobType(), (int)giveupCount);
        user->SetCurCharacExpertJobType(0);
        user->SetCurCharacExpertJobExp(-1);

        // ORIG 0x0849ead5 用 getCurCharacQuestR（非 QuestW）调 get_quest_info。
        user->getCurCharacQuestR()->get_quest_info(
            reinterpret_cast<char*>(&packet));   // 复用包缓冲
        user->Send(packet);

        ++giveupCount;
        if ((unsigned int)script->m_giveupCost.size() <= giveupCount)
        {
            giveupCount = script->m_giveupCost.size() - 1;
        }
        ((charac_expand::CData*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0))->reset();
        GetSelfExpand(user)->set_giveup_count((unsigned short)giveupCount);

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);

        packet.clear();
        packet.put_header(1, 0xf3);
        packet.put_byte(1);
        packet.put_int(user->getCurCharacMoney());
        packet.put_byte((int)giveupCount);
        packet.finalize(true);
        user->Send(packet);
    }
    catch (ENUM_ERROR err)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf3, (unsigned char)err);
    }
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CCharacExpertJob 补充
// ===========================================================================

namespace expert_job
{

// ---- SendExpertJobInfo（报告，ORIG T 0x0832b454 sz 0x201）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（P197④/N-88 族，语义等价）。
// ORIG 0x0832b572 `cmpl $0,-0xc; sete %al; test %al,%al; jne <continue 岛>` 系
// 4.1.2 槽比零 setcc 物化；4.4.7 对 if/while 条件恒折直跳 `je`（-3 条）。
// 本波实测旋钮：①P194 具名 bool（落槽 -0x9 ⇒ 全帧重排，劣化）；
// ②P225 while+break（产镜像 setne/je 且少 1 岛，劣化）；③P226 语句表达式
//（回退基线 je）；④内联比较 = 双调用真 bug（P197④ 禁）。其余逐条全等：
// switch(1,2,3,4) 决策树、三 continue 岛（nop;jmp;nop;jmp;nop）、槽位集
//（-0x18/-0x28/-0x14/-0xc/-0x10）与全部调用点。唯一残差 = 该 3 条 setcc 物化。
#include "ExpertJobOnlyMgr/SendExpertJobInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xcd);
    packet.put_byte(m_giveupCount);
    // [b23S3C] P48 去缓存 + P0 律：ORIG 0x0832b44d/0x61/0xbb 三处独立 call
    // GetCurCharacExpertJobType（put_byte 实参、switch 判定、GetExpertJobScript
    // 实参），判定比较一律走 %eax 无 -off 槽 ⇒ 原码无 jobType 具名局部；
    // 决策树 `cmp $3;je / cmp $3;jg / cmp $1;jl / cmp $4;jne` 为 switch(1,2,3,4)
    // 分派形（旧 `>=1 && <=4` 范围测形不同）。
    packet.put_byte(user->GetCurCharacExpertJobType());
    switch (user->GetCurCharacExpertJobType())
    {
    case 1:
    case 2:
    case 4:
    {
        int count = 0;
        int countIndex = packet.get_index();
        packet.put_byte(count);   // 先占位写 0（ORIG 0xa3 读 -0x18 槽，非立即数）
        // [b23S3C] 第三处独立重取（ORIG 0x0832b4bb→0xc7 结果入 %ebx 作第 2 实参）。
        STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()
                                        ->GetExpertJobScript(
                                            user->GetCurCharacExpertJobType());
        if (script != 0)
        {
            for (int i = 0; i < 0xf0; ++i)
            {
                // b28：清 register asm（N96/P0 全域禁）。ORIG `xor $1/test %al`
                // 为 4.1 !int 直测 lowering（4.4 恒折 test/je），值域 0/1 等价改写。
                // b28c-1：P168a X2 实测占槽（帧 0x30→0x40，ndiff 7→78）⇒ 不可翻。
                if (!get_recipe(i))
                    continue;
                // [b23S3C] P197③/P198①：ORIG 0x0832b556 先 `movl $0x0,-0xc(%ebp)`
                // 再 GetRecipeInfo ⇒ 两句式。
                stRecipeInfo* info = 0;
                info = script->GetRecipeInfo(i);
                if (info == 0)
                    continue;
                // b28d-solo [R2 适配]：STExpertJobScript.h 将 m_pad 拆为真实字段
                // m_field0(+0x00, ulong)；本 TU 按 ORIG 0x0832b57d/0x586 的
                // `mov (%eax),%eax; test %eax,%eax` 4 字节读语义直取该字段。
                if (info->m_field0 == 0)
                    continue;
                packet.put_int(static_cast<int>(info->m_field0));
                ++count;
            }
        }
        packet.put_byte(countIndex, count);   // 回填 recipe 计数
        break;
    }
    case 3:
        packet.put_int(m_enchantCount + 1);
        packet.put_int(m_endurance);
        break;
    default:
        break;
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---- set_disjoint_machine_grade【objdump】----
// ORIG W 0x85d3e9a sz 0x1b：call charac_expand::CData::alter 后写 +0x0c。
#include "ExpertJobOnlyMgr/set_disjoint_machine_grade.inc"
{
    alter();
    m_enchantCount = value;   // +0x0c
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointMachine::include【objdump】
// ORIG W 0x85d3de6 sz 0x20：this+0x04 与 other+0x04 两矩形转调
// collision_detection::Rectangle::include（W 0x85cb492）。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/include.inc"
{
    return m_rect.include(
        reinterpret_cast<const CDisjointMachine&>(other).m_rect);
}

}  // namespace expert_job

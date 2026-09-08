// ============================================================================
// CompoundDeathLCB.cpp — WongWork::CCompoundAvatar 合成核心四方法 +
//                        WongWork::CDeathTowerRanking 记录 packet（LinkClosureB）
//
// 全部实现逐段转写自 docs/class_func_reports/{WongWork__CCompoundAvatar,
// WongWork__CDeathTowerRanking}/ 报告汇编+反编译 C：
//   WongWork::CCompoundAvatar
//     _MakeCompoundInfo            T 0x08331efe（sret 返回 tagCompoundInfo）
//     _CheckCondition              T 0x0833260c（const）
//     _ProcCompoundCore            T 0x08332ee8
//     _SendDisjointAvatarStatistic T 0x08333ecc
//     _generateIPGNo               T 0x08333df6（const；闭链 helper，报告同目录）
//   WongWork::CDeathTowerRanking
//     makeMyBestRecordPacket       T 0x08468b8e
//     makeTopRankerRecordPacket    T 0x08468c68
//     _makeRecordPacket            T 0x08468cb6（前两者共用的序列化 helper）
// 同 TU 附加直接依赖 producer（报告完整、当前全源无定义）：
//   getItemIndex(const std::vector<std::pair<int,int>>&, int) T 0x08332e4e
//   Packet_Avater_Disjoint_Statistic C1 W 0x08110bae（内嵌 PacketHeader W 0x08110b70；
//     权威布局已归口 Packet_Avater_Disjoint_Statistic.h，本 TU 仅 include）
//   DB_UpdateAvatarHistory::makeRequest(unsigned int,int,char*) T 0x0844536a
// ============================================================================

#include "CCompoundAvatar.h"
#include "CDeathTowerRanking.h"
#include "CInventory.h"         // CInventory 完整类型 + OS_API::snprintf
#include "PacketGuard.h"        // PacketGuard 完整类型（CDeathTowerRanking.h 仅前向）
#include "CAvatarItemMgr.h"      // IsTempKey/GetRemainDate/GetExpireDate/UnRegistItem
#include "CUser.h"               // CUserCharacInfo getter/getCurCharacInven*
#include "CEnvironment.h"        // G_CEnvironment()/m_serverEnvir（_generateIPGNo 通道号）
#include "GlobalData.h"          // s_systemTime_/s_statistic_proxy_mgr/s_stream_pool/s_msgq_mgr
#include "CSystemTime.h"
#include "MsgQueueMgr.h"        // MsgQueueMgr::put / QUEUE_IDX
#include "CServerProxyMgr.h"     // CServerProxyMgr<CStatisticServerProxy> 完整模板
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "GameTypes.h"          // cMyTrace
#include "CDataManager.h"        // G_CDataManager()/find_item
#include "PvPTypes.h"            // CMTRand（m_rand 完整类型）
#include "GameRand.h"            // get_rand_int
#include "DB_UpdateAvatarHistory.h"
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（本 TU 原重复定义已迁除）
#include <cstring>
#include <ctime>
#include <utility>
#include <vector>


// ---------------------------------------------------------------------------
// getItemIndex —— ORIG T 0x08332e4e _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi。
// 全局函数；唯一定义已在 HeroMissionLink4.cpp:257（签名/mangled 与 ORIG 一致），
// 此处仅声明复用，避免重复符号。语义：返回向量中第一个 second > bound 的
// 元素 first；遍历完仍无 → 0。
// ---------------------------------------------------------------------------
int getItemIndex(const std::vector<std::pair<int, int> >& vec, int bound);

// ---------------------------------------------------------------------------
// DB_UpdateAvatarHistory —— ORIG T 0x0844536a（makeRequest.md 反编译转写）。
// 流头 0x2b0 + -1；正文 characNo(uint)/avatarUid(int)/ipgNo 二进制 0x18 字节；
// 入 MsgQueue 队列 2。
// ---------------------------------------------------------------------------
void DB_UpdateAvatarHistory::makeRequest(unsigned int characNo, int avatarUid,
                                         char* ipgNo)
{
    // P0：Acquire 返回值右值内联进 ctor（ORIG %eax 直通，无 stream 栈槽）。
    // 每个 operator<< 前有独立 operator*(deEv)；put_binary 前是 operator->(ptEv)
    // ——仓内惯用显式代理链（user-defined operator* 返回指针，*guard << v 不可编译）。
    // characNo 为 uint → 选中 lsEj（0x080e2292）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac3b), true);
    guard.operator*()->operator<<((int)0x2b0);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(characNo);       // lsEj
    guard.operator*()->operator<<(avatarUid);      // lsEi
    guard.operator->()->put_binary(ipgNo, 0x18);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

namespace WongWork {

// ---------------------------------------------------------------------------
// ORIG CItem 虚表槽直调 helper。
// ORIG CItem vtable(.rodata 0x08c96da0) 槽序实证：D1/D2、D0、纯虚区；
//   +0x0c=子类型索引（派生 _ZNK10CEquipItem12get_sub_typeEv）、
//   +0x10=IsAvatarItem(bool)、+0x24=getUsableItemType(int)(bool)。
// 当前 CItem.h 虚函数声明序与该权威槽序的审计差异尚未收敛（AuditCItemVtable
// 域负责）；为保证本 TU 与 ORIG 虚分派逐指令等价，经槽偏移显式调用，
// 全局虚表收敛后应替换回语义虚调用。
//
// 必须是**宏**而非 static helper：ORIG 的虚取址序列
//   mov (obj),%eax ; add $slot,%eax ; mov (%eax),%edx ; mov obj,%eax ;
//   mov %eax,(%esp) ; call *%edx
// （7 insn/处）位于调用者帧内；提成 helper 后调用者只剩 2 insn，本 TU
// _CheckCondition 实测 10 处虚调用 ⇒ ~50 insn 缺口（P183 同类物化口径，
// P167 差集 ORIG indirect=10 / ours=0 实证）。
// ---------------------------------------------------------------------------
#define CI_VCALL0(ret, slot, obj)                                        \
    reinterpret_cast<ret (*)(const void*)>(                              \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj)
#define CI_VCALL1(ret, slot, obj, a1)                                    \
    reinterpret_cast<ret (*)(const void*, int)>(                         \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj, (a1))

#define CI_SubType(item)       CI_VCALL0(int, 0x0c, item)
#define CI_IsAvatarItem(item)  CI_VCALL0(bool, 0x10, item)
#define CI_UsableType1(item)   CI_VCALL1(bool, 0x24, item, 1)

// ============================ _MakeCompoundInfo ============================

// ORIG 0x08331efe（sret 返回 tagCompoundInfo）。步骤：
//   1. tagCompoundInfo 构造（0x08334344）+ clear()（0x0833424a）；
//   2. 回填 msg 定长字段（MSG_COMPOUND_AVATAR.h 权威布局，具名字段读取）；
//   3. **三次独立** getCurCharacInvenR → GetInvenSlot(1|2, (short)msg+0xd/0xf/0x15)
//      → 临时槽（-0x88）→ 15×movl + 1×movb 逐字段拷进 info 的
//      m_avatar(+0x92)/m_materialA(+0x0a)/m_materialB(+0x4a)；
//   4. find_item 两脚本指针（各自重取 G_CDataManager）；
//   5. 双脚本有效且 msg 两 itemIdx 与槽内 addInfo 一致时，job 0..10 匹配循环
//      定 info.m_field00；再按表长遍历 rec，对 msg->m_type 选中的 **那条**
//      tail 向量在 case 分支内各自完整展开一遍 begin/end/!=/++/-> 循环
//      （ORIG 物理展开 4 组，非提公共指针后单循环 —— N-C/P165），
//      命中 first==m_avatar.addInfo 则 found=idx 并终止双层。
//   6. 落 info.m_field04(+0x6cd)/m_type(+0x6d9)。
//
// 【batch-12 验收标注（§6.2 超大函数口径）】o=523 n=537（gap 114 → 14）。
//   ① 调用点差集（P167）：ORIG direct 40 ↔ ours direct 40，**归零**（残
//      missing=2/extra=2 仅为 tagCompoundInfo C2/D2 ↔ 4.4 发射 C1/D1 的
//      同体符号名对称差）；
//   ② 补全的三形：
//      a) `info.x = inv->GetInvenSlot(...)` 折叠 sret 目标 ⇒ 改「Inven_Item
//         临时 + 赋值」两步形，恢复 3×（getCurCharacInvenR + 15×movl+1×movb）；
//      b) 遍历提公共 `opts` 指针 ⇒ 只发 1 组迭代器调用，ORIG 是 4 组物理展开
//         （begin/end/operator!=/operator++/operator-> 各 ×4）⇒ case 内各自成组，
//         且迭代器**声明即初始化**（ORIG 该区无 normal_iterator 默认构造）；
//      c) found 经 `cand` 变量在 while 体内传递（ORIG 用条件里的 `uVar8 = idx`
//         逗号项），避免第二次 operator!= 调用；
//      d) job 匹配循环改「相等判定 + 命名 bool bothUsable」以复现 4.1 的
//         `mov$1/mov$0/test` 汇聚岛（三条独立 continue 少 6 insn）。
//   ③ 余 14 insn 逐块核对为：每字段拷贝前 ORIG 重发 `mov %ebx,%eax`（this 重载，
//      4.4 寄存器缓存不重发，6 处）、job 段 `jmp/nop` 中继岛、末尾对齐 nop
//      （§6.1 豁免）⇒ 判**语义完成、byte 层 N76/N82/N18a 封顶**，勿再耗轮次。
CCompoundAvatar::tagCompoundInfo CCompoundAvatar::_MakeCompoundInfo(
    const CUser* user, const MSG_COMPOUND_AVATAR* msg) const
{
    tagCompoundInfo info;                        // ORIG C1 0x08334344
    info.clear();                                // ORIG clear 0x0833424a

    // msg 字段读取（MSG_COMPOUND_AVATAR.h；偏移/宽度与 ORIG movzwl/mov/movzbl 一致）：
    //   m_optionCheckFlag→+0xd6 / m_optionIdx→+0xd0 / m_slot→+0xd4 /
    //   m_avatarSlot→+0x90 / m_materialSlotA→+0x08 / m_materialSlotB→+0x48 /
    //   m_materialItemIdA/B→find_item / m_type→+0xe8
    info.m_fieldD6 = (msg->m_optionCheckFlag == 1);
    info.m_fieldD0 = msg->m_optionIdx;
    info.m_slot = msg->m_slot;
    info.m_field90 = msg->m_avatarSlot;

    // 三步形（0x08331f64..0x08332043）：临时槽 + 逐字段拷贝；每次重取
    // getCurCharacInvenR（P183/P48 禁缓存 inv）。
    Inven_Item avatarSlot = user->getCurCharacInvenR()->GetInvenSlot(
        1, static_cast<int>(static_cast<short>(msg->m_avatarSlot)));
    info.m_avatar = avatarSlot;

    info.m_field08 = msg->m_materialSlotA;
    Inven_Item materialSlotA = user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotA)));
    info.m_materialA = materialSlotA;

    info.m_field48 = msg->m_materialSlotB;
    Inven_Item materialSlotB = user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotB)));
    info.m_materialB = materialSlotB;

    info.m_scriptA = G_CDataManager()->find_item(
        static_cast<int>(msg->m_materialItemIdA));
    info.m_scriptB = G_CDataManager()->find_item(
        static_cast<int>(msg->m_materialItemIdB));

    if (info.m_scriptA == 0 || info.m_scriptB == 0)
    {
        info.m_errorFlag = true;                 // ORIG +0xd7
        return info;
    }
    // ORIG 0x08332133/0x0833215c：msg 两 itemIdx 必须与槽内物品 addInfo
    //（Inven_Item+0x02，即 info+0x0c / info+0x4c）一致，任一不符 → errorFlag。
    if (static_cast<int>(msg->m_materialItemIdA) !=
            static_cast<int>(info.m_materialA.m_addInfo) ||
        static_cast<int>(msg->m_materialItemIdB) !=
            static_cast<int>(info.m_materialB.m_addInfo))
    {
        info.m_errorFlag = true;
        return info;
    }
    // ORIG 0x08332153：job 0..10 匹配循环 —— 两脚本同 job 且均可用时
    // m_field00=job；无匹配保持 clear 哨兵 0xb → 直接返回（不设 error）。
    for (int job = 0; job <= 10; ++job)
    {
        // ORIG 0x083322xx 发形：先 `check_job_type(A) == check_job_type(B)`，
        // 命中后再各调一次并做 **bool 汇聚岛**（`test; je 岛; mov$1/mov$0;
        // test%al; je`）⇒ 两条件必须写成命名 bool（P84/P133）；三条独立
        // continue 会让 4.4 直接 cmp/jcc（实测少 6 insn）。
        if (info.m_scriptA->check_job_type(static_cast<char>(job)) ==
            info.m_scriptB->check_job_type(static_cast<char>(job)))
        {
            const bool bothUsable =
                (info.m_scriptA->check_job_type(static_cast<char>(job)) != 0 &&
                 info.m_scriptB->check_job_type(static_cast<char>(job)) != 0);
            if (bothUsable)
            {
                info.m_field00 = job;        // ORIG +0x00
                break;
            }
        }
    }
    if (info.m_field00 == 0xb)                   // ORIG 0x0833221b：无匹配
        return info;

    const tagCompoundAvatarTable& table = m_tables[info.m_field00];
    unsigned int found = 0;                      // -0x2c：匹配到的 rec 下标
    for (unsigned int idx = 1; idx < table.m_avatars.size(); ++idx)  // -0x24
    {
        const tagCompoundAvatar* rec = table.getCompoundAvatarInfo(
            static_cast<int>(idx));
        if (rec == 0)
            continue;

        // ORIG 4 个类型分支**各自完整展开**一遍迭代器遍历（begin/end/operator!=/operator++/operator-> 各 4 组，槽 -0x30/-0x34…-0x48/-0x4c），
        // 不是提公共 vector 指针后单循环（P165 物理展开 / N-C）。
        switch (msg->m_type)
        {
            case 0:
            {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailA.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailA.end();
                unsigned int cand = found;
                while (it != last)
                {
                    cand = idx;
                    if (static_cast<int>(info.m_avatar.m_addInfo) == it->first)
                    {
                        break;
                    }
                    ++it;
                }
                found = cand;
                break;
            }
            case 1:
            {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailB.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailB.end();
                unsigned int cand = found;
                while (it != last)
                {
                    cand = idx;
                    if (static_cast<int>(info.m_avatar.m_addInfo) == it->first)
                    {
                        break;
                    }
                    ++it;
                }
                found = cand;
                break;
            }
            case 3:
            {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailC.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailC.end();
                unsigned int cand = found;
                while (it != last)
                {
                    cand = idx;
                    if (static_cast<int>(info.m_avatar.m_addInfo) == it->first)
                    {
                        break;
                    }
                    ++it;
                }
                found = cand;
                break;
            }
            case 4:
            {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_cube.m_vec.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_cube.m_vec.end();
                unsigned int cand = found;
                while (it != last)
                {
                    cand = idx;
                    if (static_cast<int>(info.m_avatar.m_addInfo) == it->first)
                    {
                        break;
                    }
                    ++it;
                }
                found = cand;
                break;
            }
            default:
                continue;                    // 其它类型跳过本轮
        }
        if (found != 0)
            break;
    }
    info.m_field04 = static_cast<unsigned int>(found);  // ORIG +0x6cd
    info.m_type = msg->m_type;                   // ORIG +0x6d9
    return info;
}

// ============================== _CheckCondition =============================

// ORIG 0x0833260c（const）。错误码：1 材料异常/条目缺失、0x11 通用条件失败、
// 0x13 有效期类别不一致、0x16 缺事件材料、0x17 组合约束失败、0x18 剩余天数过短；
// 0 通过。
//
// 【batch-11 验收标注（§6.2 口径）】orig 611 / ours 616（gap 5）。
//   ① 调用点差集：ORIG direct 52 + indirect 10 ↔ ours direct 52 + indirect 10，
//      **归零**（仅 __normal_iterator C2×2 ↔ 4.4 发射 C1×2 的同体符号名对称差）；
//   ② 逐块补齐：CI_* 由 static helper 改宏内联（虚取址 7 insn 回到本帧，
//      indirect 0→10）、补 ORIG 0x08332792 的 `m_field04 == 0 → 0x17` 漏判、
//      getCurCharacInvenR ×7 / GetAvatarItemMgrR ×3 / get_grade ×6 /
//      get_charac_job ×2 全部改为逐点重取、迭代器改「默认构造 + 分支内
//      begin/end + 循环后 ==」形、opts 下标不再命名局部（gap 156 → 5）；
//   ③ 残余 5 insn 为 4.1 的 `cmp $imm; sete; test` 布尔物化岛与统一出口
//      jmp/nop 布局（N3/N76/setcc-fold 族）⇒ 判**语义完成、byte 层封顶**。
// 物化口径（P183/P167 实证）：ORIG 对每个判据都重发完整取数链 ——
//   getCurCharacInvenR ×7（0x08332c27/0c6a/0cad/0cee + 日期段 0x0723/0x0752/0x0781/0x07ab）
//   GetAvatarItemMgrR ×3、get_grade ×6、get_charac_job ×2、
//   两个 vector 迭代器各显式构造（normal_iterator C2 ×2）+ begin/end ×2 + 循环后 ==。
// 重建端禁缓存（一处缓存即整条链消失，本函数 gap 156 的主体即此）。
int CCompoundAvatar::_CheckCondition(const CUser* user,
                                     const tagCompoundInfo& info) const
{
    // 分离模式(type4)仅允许指定物品 0x2898c4。
    if (info.m_type == 4 && info.m_avatar.m_addInfo != 0x2898c4)
    {
        return 0x11;
    }
    if (info.m_slot > 99)                                       // u16 cmp $0x63
    {
        return 0x11;
    }
    if (info.m_errorFlag != 0)
    {
        return 0x11;
    }
    if (info.m_field00 == 0xb)                                  // 表索引无效
    {
        return 0x17;
    }
    // ORIG 两个独立守卫（0x08332687 / 0x08332691 各 je/jne 直跳统一出口，P133）。
    if (info.m_materialA.m_addInfo == 0)
    {
        return 0x11;
    }
    if (info.m_materialB.m_addInfo == 0)
    {
        return 0x11;
    }

    // 任一材料的 uid 是临时 key → 1（0x083326a8 / 0x083326ba 两次实调 +
    // mov$1/mov$0 汇聚岛）。
    if (CAvatarItemMgr::IsTempKey(info.m_materialA.m_addInfo2) ||
        CAvatarItemMgr::IsTempKey(info.m_materialB.m_addInfo2))
    {
        return 1;
    }

    if (!CI_IsAvatarItem(info.m_scriptA) ||
        !CI_IsAvatarItem(info.m_scriptB))
    {
        return 0x11;
    }
    if (CI_SubType(info.m_scriptA) != CI_SubType(info.m_scriptB))
    {
        return 0x17;
    }
    if (!(CI_UsableType1(info.m_scriptA) && CI_UsableType1(info.m_scriptB)))
    {
        throw static_cast<ENUM_ERROR>(0x17);                    // __cxa_throw
    }
    // ORIG 0x08332792：recipe 下标为 0 → 0x17（位于 throw 之后、取表之前）。
    if (info.m_field04 == 0)
    {
        return 0x17;
    }

    const tagCompoundAvatarTable& table =
        m_tables[info.m_field00];
    const tagCompoundAvatar* rec =
        table.getCompoundAvatarInfo(info.m_field04);            // -0x1c

    if (!(info.m_scriptA->get_rarity() < 2 &&
          info.m_scriptB->get_rarity() < 2))
    {
        return 0x17;
    }
    if (info.m_scriptA->get_grade() != info.m_scriptB->get_grade())
    {
        return 0x17;
    }

    // 非“加攻标记”时才检查选项池里存在用户请求的 option 对：
    // grade == rec->m_idx 用 m_optsC[sub]（+0x194），否则 grade 必须 ==
    // rec->m_flag2dc（+0x2dc）再用 m_optsB[sub]（+0xf4）。
    if (info.m_fieldD6 != 1)
    {
        // ORIG 两个迭代器先默认构造（-0x20/-0x24，normal_iterator C2 ×2），
        // 再在两分支内各自 begin()/end() 赋值；循环用 != 条件 + 循环后 ==。
        // ORIG 每个 begin()/end() 前都重发一次 CI_SubType 虚调用（0x0833292f
        // 与 0x0833297b 各 12/13 insn；else 支 0x083329f3/0x08332a3f 同）⇒
        // 下标不命名局部（P183/P36）。
        std::vector<std::pair<int, int> >::const_iterator it;
        std::vector<std::pair<int, int> >::const_iterator last;
        if (info.m_scriptA->get_grade() == rec->m_idx)
        {
            it = rec->m_optsC[CI_SubType(info.m_scriptA)].begin();
            last = rec->m_optsC[CI_SubType(info.m_scriptA)].end();
        }
        else
        {
            if (info.m_scriptA->get_grade() != rec->m_flag2dc)
            {
                return 1;
            }
            it = rec->m_optsB[CI_SubType(info.m_scriptA)].begin();
            last = rec->m_optsB[CI_SubType(info.m_scriptA)].end();
        }
        while (it != last && it->first != info.m_fieldD0)
        {
            ++it;
        }
        if (it == last)
        {
            return 1;
        }
    }

    // grade 必须都 >= 所选 recipe 下标，且不超出表内上限 flag2dc。
    if (info.m_scriptA->get_grade() < info.m_field04 ||
        info.m_scriptB->get_grade() < info.m_field04)
    {
        return 0x17;
    }
    if (static_cast<int>(rec->m_flag2dc) < info.m_scriptA->get_grade() ||
        static_cast<int>(rec->m_flag2dc) < info.m_scriptB->get_grade())
    {
        return 0x17;
    }

    // 角色职业两件均可用（两臂各重取 job，0x08332ba8 / 0x08332bd2）。
    if (!(info.m_scriptA->check_job_type(
              static_cast<char>(user->get_charac_job())) == 1 &&
          info.m_scriptB->check_job_type(
              static_cast<char>(user->get_charac_job())) == 1))
    {
        return 0x17;
    }

    // 按类型验证事件材料消耗单（ORIG if/else-if 链，每分支重取
    // getCurCharacInvenR，0x08332c27/0c6a/0cad/0cee）。
    if (info.m_type == 0)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailA) != 1)
        {
            return 0x16;                                        // +0x2b8
        }
    }
    else if (info.m_type == 1)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailB) != 1)
        {
            return 0x16;                                        // +0x2c4
        }
    }
    else if (info.m_type == 3)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailC) != 1)
        {
            return 0x16;                                        // +0x2d0
        }
    }
    else if (info.m_type == 4)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_cube.m_vec)
                != 1)
        {
            return 0x16;                                        // +0x20c
        }
    }

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const int remainA =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetRemainDate(info.m_materialA.m_addInfo2, curSec);
    const int remainB =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetRemainDate(info.m_materialB.m_addInfo2, curSec);
    const bool noExpireA =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetExpireDate(info.m_materialA.m_addInfo2) == 0;
    const bool noExpireB =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetExpireDate(info.m_materialB.m_addInfo2) == 0;

    if (noExpireA != noExpireB)
    {
        return 0x13;
    }
    if (noExpireA || (remainA > 5 && remainB > 5))
    {
        // 分离升级位标记禁拆者不允许操作。
        if (info.m_materialA.m_upgradeSep.IsTradeRestriction() ||
            info.m_materialB.m_upgradeSep.IsTradeRestriction())
        {
            return 0x17;
        }
        return 0;
    }
    return 0x18;
}

// ============================== _ProcCompoundCore ===========================

// ORIG 0x08332ee8。结果选择（按 rec 内三组权重池抽样）→ 删除两件材料与事件
// 材料 → 注销两 uid 并 push 结果三元组 → AddAvatarItem 入包 → 回填 DB 历史。
//
// 【batch-16 验收标注（§6.2 口径）】orig 653 / ours 635（gap 77 → 18）。
//   ① 调用点差集 **归零**：direct 61 ↔ 61、indirect 2 ↔ 2（残 missing=1/extra=1
//      仅 cMyTrace C2 ↔ C1 同体符号名对称差）；
//   ② 补齐的真实缺口（P167 差集定位）：
//      a) 事件材料整单删除由「switch 提公共 materials 指针 + 单调用」改为
//         4 个 if-else 分支**各自完整展开**（ORIG 每支独立 delete_event_items +
//         vector::size + 独立 0x16 出口）⇒ 恢复 delete_event_items ×3、size ×2；
//         type4 的 size 比较对象仍是 tailC(+0x2d0)【ORIG 怪癖，已保留】；
//      b) 两段 GetRemainDate 各自重取 getCurCharacInvenR → GetAvatarItemMgrR
//         （不缓存 mgr）；c) AddAvatarItem 走重取的 getCurCharacInvenW。
//   ③ 余 18 为 N82 帧号重排下的块序对齐伪影（difflib 显示 -46/-32/-5 与总量
//      -18 自相矛盾，而调用点差集已归零）⇒ 判语义完成、byte 层封顶。
int CCompoundAvatar::_ProcCompoundCore(CUser* user, tagCompoundInfo& info,
                                       tagCompoundAvatarTable* table)
{
    CInventory* inv = user->getCurCharacInvenW();
    const tagCompoundAvatar* rec =
        table->getCompoundAvatarInfo(info.m_field04);
    const int subType = CI_SubType(info.m_scriptA);
    int resultIdx = 0;                                         // -0x48

    if (info.m_fieldD6 == 0 || info.m_type == 4)
    {
        if (info.m_type == 4)
        {
            const int r = get_rand_int(
                rec->m_cube.m_field0c[subType]);               // +0x218 权重计数
            resultIdx = getItemIndex(rec->m_cube.m_vecs[subType], r);  // +0x240
        }
        else
        {
            resultIdx = info.m_fieldD0;                        // 直接选定结果
        }
    }
    else
    {
        // 输出权重池空则失败。
        if (rec->m_b[subType] == 0 || rec->m_optsA[subType].empty())
            return 1;

        int weight = 0;                                        // -0x38
        if (info.m_scriptA->get_grade() == 1 &&
            info.m_scriptB->get_grade() == 1)
        {
            weight = rec->m_d[subType];                        // +0x16c
            if (m_rand->randInt(
                    static_cast<unsigned long>(10000)) < 5000) // setbe 0x1387
            {
                const int r = m_rand->randInt(
                    static_cast<unsigned long>(weight));
                resultIdx = getItemIndex(rec->m_optsC[subType], r);  // +0x194
            }
            if (resultIdx < 1)
                weight = rec->m_a[subType];                    // +0x004 兜底权重
        }
        else
        {
            weight = rec->m_e[subType];                        // +0x2e0
        }

        if (resultIdx < 1)
        {
            // 服务端全局修正（datamgr+0x4a54 加成 / +0x4a58 千分比衰减；
            // CDataManager 域具名化后改为成员访问）。
            if (info.m_type == 1)
            {
                // datamgr+0x4a54 = m_serverParameter(+0x68)+0x49ec
                //（importServerParameterScript "[upper bind cube rate bonus]" 写点）；
                // ORIG 0x08333170 `mov 0x4a54(%eax),%eax` = 4 字节读 → int 成员。
                weight += G_CDataManager()->m_serverParameter.m_49ec;
            }
            if (info.m_type == 3)
            {
                // datamgr+0x4a58 = m_serverParameter+0x49f0
                //（"[lower bind cube rate bonus]"；ORIG 0x0833318c `mov 0x4a58(%eax),%eax`）。
                weight -= (G_CDataManager()->m_serverParameter.m_49f0 * weight) /
                          10000;
            }
            if (m_rand->randInt(static_cast<unsigned long>(10000)) < weight)
            {
                const int r = get_rand_int(rec->m_b[subType]);
                resultIdx = getItemIndex(rec->m_optsA[subType], r);  // +0x7c
            }
        }
    }

    if (resultIdx == 0)
    {
        const int r = get_rand_int(rec->m_c[subType]);         // +0x054 权重计数
        resultIdx = getItemIndex(rec->m_optsB[subType], r);    // +0xf4
    }

    if (resultIdx != 0 && G_CDataManager()->find_item(resultIdx) != 0)
    {
        // ---- 成功路径：删两件材料 ----
        CItem* item = G_CDataManager()->find_item(resultIdx);
        if (!item->IsCreatureItem())          // ORIG vtable+0x10（IsCreatureItem）
            return 1;
        // ORIG 0x083330ec：槽位由目标 item 的 avatar option size 随机重选，
        // 且 >0x63(99) 时清零（覆盖客户端传入 slot）。
        info.m_slot = static_cast<unsigned short>(get_rand_int(
            reinterpret_cast<CEquipItem*>(item)->getAvatarOptionSize()));
        if (info.m_slot > 99)
            info.m_slot = 0;
        if (inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field08), 1,
                             static_cast<eItemDelReason>(3), true) == 1 &&
            inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field48), 1,
                             static_cast<eItemDelReason>(3), true) == 1)
        {
            bool bFoundAll = false;                            // -0x8d
            // 【batch-16 物理展开】ORIG 0x0833321x..0x083332bx 是 4 个 if-else 分支
            // **各自完整调用一次** delete_event_items + vector::size + 比较 + 独立
            // `return 0x16` 出口（调用点差集实测：提公共 materials 指针后单调用会
            // 少 3 个 delete_event_items 与 2 个 size）。type4 的 size 比较对象
            // 仍是 tailC（ORIG 0x083332b7 读 +0x2d0）【怪癖保留】。
            if (info.m_type == 0)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailA, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailA.size()))
                    return 0x16;
            }
            else if (info.m_type == 1)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailB, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailB.size()))
                    return 0x16;
            }
            else if (info.m_type == 3)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailC, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return 0x16;
            }
            else if (info.m_type == 4)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_cube.m_vec, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return 0x16;
            }

            const int curSec = GlobalData::s_systemTime_.getCurSec();
            // 两段 remain 计算各自重取 invenR → GetAvatarItemMgrR（ORIG
            // 0x0833330a / 0x08333345，不缓存 mgr）。
            const int remainA =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialA.m_addInfo2),
                                    curSec);
            const int remainB =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialB.m_addInfo2),
                                    curSec);

            // 注销并广播移除事件。
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialA.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialA.m_addInfo2),
                                 1, static_cast<int>(info.m_field08) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组①
            info.m_results.push_back(static_cast<int>(info.m_field08));
            info.m_results.push_back(1);
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialB.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialB.m_addInfo2),
                                 1, static_cast<int>(info.m_field48) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组②
            info.m_results.push_back(static_cast<int>(info.m_field48));
            info.m_results.push_back(1);

            char ipgNoBuf[0x14];                               // -0xa5
            _generateIPGNo(user, curSec, ipgNoBuf);

            // AddAvatarItem 走**重取**的 invenW（ORIG 0x083333xx getCurCharacInvenW）。
            info.m_resultSlot = static_cast<unsigned short>(
                user->getCurCharacInvenW()->AddAvatarItem(
                    resultIdx, remainA + remainB, false,
                                   static_cast<char>(info.m_slot),
                                   static_cast<char>(-1), ipgNoBuf,
                                   static_cast<eAvatarItemAddReason>(6), true,
                                   0));
            if (static_cast<short>(info.m_resultSlot) == -1)
            {
                cMyTrace trace(
                    "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                    "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                    "stCompoundAvatarTable*)",
                    0x361, 5);
                trace("[%s][%d] AddAvatarItem Fail",
                      "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                      "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                      "stCompoundAvatarTable*)",
                      0x361);
                return 0x11;
            }
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialA.m_addInfo2), ipgNoBuf);
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialB.m_addInfo2), ipgNoBuf);
            return 0;
        }
        return 0x16;                                           // delete_item 失败
    }
    return 1;                                                  // 结果不可实例化
}

// =========================== _SendDisjointAvatarStatistic ===================

// ORIG 0x08333ecc。统计服上报本次分离的两件头像分级；注意 B 组 subType 也读
// scriptA 的 vtbl+0xc（ORIG 如此）。
// 【batch-12 标注】o=79 n=78（gap 11 → 1）：调用点差集归零；余 1 = ORIG 多一条
//   callee-saved `push`（帧/寄存器分配，N82）+ 尾部对齐 nop（§6.1 豁免）⇒ N 封顶。
void CCompoundAvatar::_SendDisjointAvatarStatistic(tagCompoundInfo& info)
{
    Packet_Avater_Disjoint_Statistic packet;

    const CItem* scriptA = info.m_scriptA;
    const CItem* scriptB = info.m_scriptB;

    packet.m_field0a = 2;                                      // -0x36/local_36
    // ORIG 0x08333ef6..0x08333f33：先 `movl $0,-0xc` 建分级**中间槽**，
    // 命中分支 `movl $2,-0xc` + jmp，未命中分支 get_grade 后落同槽，
    // 末尾统一 `mov -0xc,%eax` 写包字段 ⇒ 三目直写包成员会丢整组槽
    // （实测每组少 5-6 insn，两组共 11）。
    int gradeIdxA = 0;
    if (scriptA->get_rarity() == 2)
    {
        gradeIdxA = 2;
    }
    else
    {
        gradeIdxA = scriptA->get_grade() - 1;
    }
    packet.m_gradeIdxA = gradeIdxA;
    packet.m_subTypeA = CI_SubType(scriptA);
    packet.m_flagA = 0;
    int gradeIdxB = 0;
    if (scriptB->get_rarity() == 2)
    {
        gradeIdxB = 2;
    }
    else
    {
        gradeIdxB = scriptB->get_grade() - 1;
    }
    packet.m_gradeIdxB = gradeIdxB;
    packet.m_subTypeB = CI_SubType(scriptA);                   // 【ORIG 怪癖】保留
    packet.m_flagB = 0;

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    proxy->SendPacket(reinterpret_cast<char*>(&packet),
                      static_cast<int>(sizeof(packet)));       // 0x28
}

// ================================ _generateIPGNo ============================

// ORIG T 0x08333df6（const）。"C%02d%02d%02d%02d%010d" 五元组。
char* CCompoundAvatar::_generateIPGNo(const CUser* user, long curSec,
                                      char* buf) const
{
    time_t t = static_cast<time_t>(curSec);
    struct tm lt;
    localtime_r(&t, &lt);
    const int characNo = user->getCurCharacNo();
    // ORIG 0x08333e63 `mov 0x1b0(%eax),%eax`（4 字节读）= CEnvironment+0x198
    //（m_serverEnvir）+0x18 → Server_Envir::m_channelNo（通道号；同
    // CDataManager.cpp:1461 具名口径）。
    const int serverId = G_CEnvironment()->m_serverEnvir.m_channelNo;
    OS_API::snprintf(buf, 0x14, "C%02d%02d%02d%02d%010d",
                     serverId,
                     (lt.tm_year + 1900) % 2000,
                     lt.tm_mon + 1,
                     lt.tm_mday,
                     characNo);
    return buf;
}

}  // namespace WongWork

// ======================== CDeathTowerRanking packets ========================

namespace WongWork {

// ORIG T 0x08468cb6。每名成员：strlen 名字(int)+名字串、两个槽位 byte
// （stBestRecord_t +0x80/+0x84 即 m_pad7c[4+i]/m_pad7c[8+i]）、层号 short、
// 通关耗时 int、名次 int。
// 【batch-12 标注】o=89 n=88（gap 10 → 1）：strlen 两次独立调用已补齐（P48）；余 1
//   为 ORIG 尾 `call leave ret` 前的 mov + 对齐 nop（§6.1 豁免）⇒ N 封顶。
void CDeathTowerRanking::_makeRecordPacket(const stBestRecord_t& record,
                                           PacketGuard& packet)
{
    for (int i = 0; i < 4; ++i)
    {
        // ORIG 0x08468cd6/0x08468d03：strlen **两次独立调用**（每次完整重发
        // record+i*0x1e+4 两级寻址 + call），put_str 用第二次结果（P48/P183）。
        packet.put_int(
            static_cast<int>(strlen(record.m_memberData[i])));
        // ORIG 0x8468d37 调 const 重载 put_str(PKci)@0x0822b770（非 Pci 版）。
        packet.put_str(
            record.m_memberData[i],
            static_cast<int>(strlen(record.m_memberData[i])));
        packet.put_byte(record.m_pad7c[1][i]);                // +0x80..+0x83
        packet.put_byte(record.m_pad7c[2][i]);                // +0x84..+0x87
    }
    packet.put_short(static_cast<int>(record.m_stage));        // +0x88
    packet.put_int(static_cast<int>(record.m_playTime));       // +0x8c
    packet.put_int(record.m_field94);                          // +0x94
}

// ORIG T 0x08468b8e。查 best 表：命中 put_byte(1)+纪录；否则 put_byte(0)。
// 【切片转写】0x08468b9e `call _getBestRecordTable`（不内联算址）；find/end 结果
// 各经 _Hashtable_iterator → _Hashtable_const_iterator **转换构造 2 次**
// （0x08468bc5/0x08468bfb），比较用 const_iterator::operator==、取值用
// const_iterator::operator-> ⇒ 必须声明 const_iterator 并写 `==`（iterator +
// `!=` 会走 operator!= 并省掉两次转换 ctor，实测差 5 call / 9 insn）。
// 【batch-12 标注】o=63 n=61（gap 9 → 2）：调用点差集 10/10 归零（残差仅
//   _Hashtable_const_iterator 转换 ctor 的 C2↔C1 符号名）；余 2 = 尾部 nop/装载
//   步序（§6.1/N82）⇒ N 封顶。
void CDeathTowerRanking::makeMyBestRecordPacket(unsigned int towerIdx,
                                                unsigned int key,
                                                PacketGuard& packet)
{
    // [b27 封顶] 残差（63 vs 61）：hash_map const_iterator 转换构造/迭代器
    // 槽布局跨版本差（N86 族：4.1 临时槽+拷贝 vs 4.4 直构；find→const_iterator
    // 转换与 it->second 槽化）——调用链/判形/序列化序全等。
    BestMap* best = _getBestRecordTable(towerIdx);
    const BestMap::const_iterator it = best->find(key);
    const BestMap::const_iterator last = best->end();
    if (it == last)
    {
        packet.put_byte(0);
    }
    else
    {
        packet.put_byte(1);
        _makeRecordPacket(it->second, packet);
    }
}

// ORIG T 0x08468c68。该塔 5 个榜首槽顺序序列化（基址 tower*0x390，stride 0x98）。
void CDeathTowerRanking::makeTopRankerRecordPacket(unsigned int towerIdx,
                                                   PacketGuard& packet)
{
    // ORIG 0x08468c68：i 从 1 起（movl $1,-0xc → cmpl $5,setle ⇒ i<=5），
    // 槽址 = m_towers + towerIdx*0x390 + i*0x98（双 imul 直用 m_towers 基址，
    // 不经 m_slots 具名 +0x98 偏移；具名 slots 局部会引入 add $0x90/+0x8
    // 常量折叠与 -0x10 基址槽）。
    // [b27] 残差：ORIG 先载 0xc(towerIdx)/-0xc(i) 后两 imul、我们 eax 预载
    // m_towers（mov $0,%eax 常量形）且 ecx 中转——N18a 族寄存器序不可定向
    //（探针 43 全局数组形亦多 1 条），语义/循环界/槽序已全等。
    for (int i = 1; i <= 5; ++i)
        _makeRecordPacket(
            *reinterpret_cast<stBestRecord_t*>(
                reinterpret_cast<char*>(m_towers) + (i * 0x98 + towerIdx * 0x390)),
            packet);
}

}  // namespace WongWork

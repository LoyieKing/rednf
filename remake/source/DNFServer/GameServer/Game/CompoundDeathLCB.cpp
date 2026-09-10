// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CompoundDeathLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CompoundDeathLCB/getItemIndex.inc"

// ---------------------------------------------------------------------------
// DB_UpdateAvatarHistory —— ORIG T 0x0844536a（makeRequest.md 反编译转写）。
// 流头 0x2b0 + -1；正文 characNo(uint)/avatarUid(int)/ipgNo 二进制 0x18 字节；
// 入 MsgQueue 队列 2。
// ---------------------------------------------------------------------------
#include "CompoundDeathLCB/makeRequest.inc"
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
// 【b28d-solo-CompoundDeathLCB 定案】达成 **AE**：523/523 逐条全等（clean
//   519=519，仅 .o 重定位标签伪差，P264）。本轮修法：①三步 GetInvenSlot 临时改
//   `*&expr` 匿名临时 ⇒ 4.4 复用同一槽（帧 0x11c→0x8c=ORIG，槽位集全等）；
//   ②`m_tables[f00]` 每迭代重取（P183 禁缓存）；③循环内 `found=idx; break;` 直赋
//   （删 cand×4）；④job 段 `!=` 否定早返 + P226 语句表达式（消 bothUsable 槽）；
//   ⑤switch→if-else 链（P17）；⑥每分支尾各一份 `if(found)break`（ORIG 各一份）；
//   ⑦删冗余 (int) cast（P41/P254）＋索引 `static_cast<unsigned int>` 触发 4.4
//   中转（P211 异型化，消最后 2 条）。
#include "CompoundDeathLCB/_MakeCompoundInfo.inc"
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
    // [b28d-solo-CompoundDeathLCB] ORIG 三步**复用同一 -0x88 临时槽**（0x08331fxx/
    // 0x083320xx 三次 GetInvenSlot 的 sret 目标同址；帧 0x8c）⇒ 用块作用域同名
    // 局部让 4.4 复用槽（三个不同名局部会各占 0x3d 字节，帧 0x11c，实测 +0x90）。
    info.m_avatar = *&user->getCurCharacInvenR()->GetInvenSlot(
        1, static_cast<int>(static_cast<short>(msg->m_avatarSlot)));

    info.m_field08 = msg->m_materialSlotA;
    info.m_materialA = *&user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotA)));

    info.m_field48 = msg->m_materialSlotB;
    info.m_materialB = *&user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotB)));

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
    // [b28d-solo-CompoundDeathLCB] 两侧同为 unsigned int ⇒ 裸比较（ORIG
    // 0x08332201 `mov 0x11(%eax),%edx` 单段直读）；冗余 `(int)` cast 令 4.4
    // 多一条 `mov %eax,%edx` 中转（P41/P254 变体）。
    if (msg->m_materialItemIdA != info.m_materialA.m_addInfo ||
        msg->m_materialItemIdB != info.m_materialB.m_addInfo)
    {
        info.m_errorFlag = true;
        return info;
    }
    // ORIG 0x08332153：job 0..10 匹配循环 —— 两脚本同 job 且均可用时
    // m_field00=job；无匹配保持 clear 哨兵 0xb → 直接返回（不设 error）。
    for (int job = 0; job <= 10; ++job)
    {
        // [b28d-solo-CompoundDeathLCB] ORIG 0x08332269 首段为 `cmp %al,%dl;
        // setne %al; test; jne <continue>` ⇒ 源码是否定早返 `if (A != B)
        // continue;`（P196/P133 极性判据；写 `==` 会产 sete/je 反形）。
        // 第二段 `mov$1/mov$0; test %al,%al; je` 是**无槽**寄存器汇聚岛（P0）
        // ⇒ 用 P226 语句表达式包住短路条件（具名 bool 会多 store+cmpb 两条）。
        if (info.m_scriptA->check_job_type(static_cast<char>(job)) !=
            info.m_scriptB->check_job_type(static_cast<char>(job)))
        {
            continue;
        }
        if (({ info.m_scriptA->check_job_type(static_cast<char>(job)) != 0 &&
               info.m_scriptB->check_job_type(static_cast<char>(job)) != 0; }))
        {
            info.m_field00 = job;        // ORIG +0x00
            break;
        }
    }
    if (info.m_field00 == 0xb)                   // ORIG 0x0833221b：无匹配
        return info;

    unsigned int found = 0;                      // -0x2c：匹配到的 rec 下标
    // [b28d-solo-CompoundDeathLCB] ORIG 0x083322fb 每次迭代重发 `m_tables[f00]`
    // 完整取址（f00*0xc + this，P183 禁缓存；引用局部会占槽并把帧撑大）。
    for (unsigned int idx = 1; idx < m_tables[static_cast<unsigned int>(info.m_field00)].m_avatars.size();
         ++idx)  // -0x24
    {
        const tagCompoundAvatar* rec =
            m_tables[static_cast<unsigned int>(info.m_field00)].getCompoundAvatarInfo(
                static_cast<int>(idx));
        if (rec == 0)
            continue;

        // ORIG 4 个类型分支**各自完整展开**一遍迭代器遍历（begin/end/operator!=/operator++/operator-> 各 4 组，槽 -0x30/-0x34…-0x48/-0x4c），
        // 不是提公共 vector 指针后单循环（P165 物理展开 / N-C）。
        // [b28d-solo-CompoundDeathLCB] ORIG 0x08332329 为线性比较链
        //（`test %al,%al; jne` / `cmp $0x1,%al; jne` / `cmp $0x3,%al; jne`）
        // ⇒ 原码是 if-else-if 链而非 switch（P17；switch 令 4.4 发决策树
        // cmp/jg/je 共 +10 条）。
        if (msg->m_type == 0)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailA.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailA.end();
                // [b28d-solo-CompoundDeathLCB] ORIG 0x08332370..0x083323b7 循环体形：
                // 命中直接 `found = idx; break;`（无 cand 中转槽；四个 case 的 cand
                // 各占 4 字节 ⇒ 帧多 0x10）。
                while (it != last)
                {
                    // ORIG 0x08332370 `mov 0x94(%eax),%esi` 单段直读 ⇒ 两侧同为
                    // unsigned（`(int)` cast 会令 4.4 多一条 eax→esi 中转）。
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                // ORIG 0x083323b7：每个分支循环后各有一份 `cmpl $0,-0x28(%ebp);
                // je <下一 else-if>; jmp <跳出>`（= if (found != 0) break;）
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 1)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailB.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailB.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 3)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailC.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailC.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 4)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_cube.m_vec.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_cube.m_vec.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else
        {
            continue;                        // 其它类型跳过本轮
        }
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
// 【b28e-M1S2 本波复核】读数 orig 611 / ours 609（残余 2，较旧记录 5 已再降）。
//   逐 hunk 定位残余：① `call __normal_iterator::operator->` 后 ORIG 2 条
//   （`mov (%eax),%eax; mov %eax,%edx`）被 4.4 copy-prop 折成 1 条
//   `mov (%eax),%edx`；② 同族另一处（共 2 条）。二者皆 4.4 折叠产物。
//   新钥匙逐条核过：P339/P331（本件差 2 条、非「整体平移/槽互换」⇒ 不属该域）、
//   P226/P231（CI_* 宏属 TU 级实体禁改；18 处间接虚调用指令数已对齐，仅
//   ORIG 函数指针→%edx / 4.4→%eax 的 N7 寄存器差）、P216/§6.6.2（无 landing 差）
//   ⇒ 判 N7+copy-prop 封顶，无语义缺口（p167 CLEAR 52/52 direct + 10/10 indirect）。
// 物化口径（P183/P167 实证）：ORIG 对每个判据都重发完整取数链 ——
//   getCurCharacInvenR ×7（0x08332c27/0c6a/0cad/0cee + 日期段 0x0723/0x0752/0x0781/0x07ab）
//   GetAvatarItemMgrR ×3、get_grade ×6、get_charac_job ×2、
//   两个 vector 迭代器各显式构造（normal_iterator C2 ×2）+ begin/end ×2 + 循环后 ==。
// 重建端禁缓存（一处缓存即整条链消失，本函数 gap 156 的主体即此）。
#include "CompoundDeathLCB/_CheckCondition.inc"
                                     const tagCompoundInfo& info) const
{
    // 【b28d-solo-CompoundDeathLCB 判词】DIFF 封顶（§6.2）：ORIG 611 / ours 615、
    // p167 CLEAR(52/52)。本轮修法：table 引用内联（P183）、m_field04 检查前移至
    // CI_UsableType1 之前、while+break 拆 &&（P225）、尾段正极性包裹（P214①）。
    // 残余 = 间接虚调用目标寄存器 N7 ×18（4.4.7 恒 %eax / ORIG %edx；本 TU 换
    // 4.4.6 后端实测 n7 50→10 但 TU 整体 ae/near/diff 变差 ⇒ 不路由）+ 尾段
    // 块序/槽号（N82/N93 族）。
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
    // [b28e] ORIG 0x08332687/0x08332691 为 `test a; je L11` + `test b; jne NEXT`
    // —— **两跳同落同一个 `mov $0x11` 共享块 L11**（P272 双跳同落 then 形），
    // 两个独立 if-return 会各发一份 `mov $0x11` 岛。
    if (info.m_materialA.m_addInfo == 0 || info.m_materialB.m_addInfo == 0)
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
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08332792 的 `m_field04 == 0 → 0x17`
    // 位于 CI_UsableType1 虚检查（0x0833292f）**之前**（历史注释位置有误）。
    if (info.m_field04 == 0)
    {
        return 0x17;
    }
    if (!(CI_UsableType1(info.m_scriptA) && CI_UsableType1(info.m_scriptB)))
    {
        throw static_cast<ENUM_ERROR>(0x17);                    // __cxa_throw
    }

    // [b28d-solo-CompoundDeathLCB] ORIG 0x083327a5 直接把 `m_tables[m_field00]`
    // 取址内联进调用点（`mov 0x4(%eax),%ecx` 先物化实参、后算表地址）；
    // 具名 table 引用会占槽（P183 禁缓存）。
    const tagCompoundAvatar* rec =
        m_tables[info.m_field00].getCompoundAvatarInfo(info.m_field04);  // -0x1c

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
            // [b28e] ORIG 0x083329cc 段为 `cmp; sete %al; test %al,%al; je
            // <0x08332a8d: mov $1; jmp>`（相等 → 直落 begin/end，不等 → 跳出到
            // 返回 1 岛）⇒ 原码是 **正极性 if(==){…} else {return 1;}** 的
            // if/else 形，而非 `if(!=) return 1;` 早返（早返令 4.4 产
            // `setne; test; je 越过 mov$1` 的镜像块序）。
            if (info.m_scriptA->get_grade() == rec->m_flag2dc)
            {
                it = rec->m_optsB[CI_SubType(info.m_scriptA)].begin();
                last = rec->m_optsB[CI_SubType(info.m_scriptA)].end();
            }
            else
            {
                return 1;
            }
        }
        // [b28d-solo-CompoundDeathLCB] ORIG 循环体为 `cmp; sete %al; test %al,%al;
        // jne <找到出口>` 的 bool 物化形（P225）⇒ 拆成 while(条件) + if break，
        // 合并 && 会令 4.4 直产 je/jne 直跳（无 setcc）。
        while (it != last)
        {
            if (it->first == info.m_fieldD0)
            {
                break;
            }
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
    // [b28e] ORIG 0x08332b58/0x08332b6a 先 `call get_grade` 再装
    // `rec->m_flag2dc`（`mov -0x1c(%ebp),%edx; mov 0x2dc(%edx),%edx; cmp
    // %edx,%eax; jg/jle`）= 左操作数是 get_grade() 的 `>` 书写形；写
    // `flag2dc < get_grade()` 会令 4.4 先把 flag2dc 缓存进 %ebx 再 call（求值序反）。
    if (info.m_scriptA->get_grade() > rec->m_flag2dc ||
        info.m_scriptB->get_grade() > rec->m_flag2dc)
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
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08332dcx 尾段为 `jg <继续>` +
    // `mov $0x18,%eax; jmp`（早返岛）⇒ 正极性包裹改写（P214①；负极性
    // `if(cond){...} return 0x18;` 会让 4.4 产 jle 反形）。
    if (!(noExpireA || (remainA > 5 && remainB > 5)))
    {
        return 0x18;
    }
    // 分离升级位标记禁拆者不允许操作。
    if (info.m_materialA.m_upgradeSep.IsTradeRestriction() ||
        info.m_materialB.m_upgradeSep.IsTradeRestriction())
    {
        return 0x17;
    }
    return 0;
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
// 【b28e-M1S2 本波】落地「外层条件取反 + A/B 块互换」（德摩根等价，见体内注释）：
//   ORIG 0x08332f4x/0x08332f5x 的**正条件双 je** 与 then-物理在后布局已逐条对齐
//   （`movzbl 0xd6(%eax),%eax; test %al,%al; je <then>` + `movzbl 0xe8(%eax),%eax;
//   cmp $0x4,%al; je <then>`，两跳同落 +0x334），旧形的 `xor $0x1` 反条件与多余
//   m_type 重载消失。读数 orig 653 / ours 635→**638**（gap 18→15，TU 四数
//   32/50/1/3/8 逐值不变）。剩余 15 = 帧 0xcc/0xc0 + 槽整体平移（ORIG
//   -0x54/-0x50/-0x4c/-0x48 vs 我方 -0x4c/-0x48/-0x44/-0x40）+ ORIG 独有
//   callee-saved %edi + N7 间接调用寄存器 + `const int r` 栈缓存（P0 形态：
//   ORIG 用 `mov %eax,%ecx` 直传无槽；但删具名局部使 gap 18→更负 ⇒ 按闸门不落地）
//   ⇒ N82/N7 封顶。
#include "CompoundDeathLCB/_ProcCompoundCore.inc"
                                       stCompoundAvatarTable* table)
{
    // 【b28d-solo-CompoundDeathLCB 判词】DIFF 封顶（§6.2）：ORIG 653 / ours 638、
    // p167 CLEAR(61/61)、无缺失调用。极性/块序已在 b28e 修正（外层条件取反 +
    // A/B 互换）；残余 = 槽整体平移（ORIG 深 8：-0x54/-0x50/-0x4c/-0x48 vs
    // 我方 -0x4c/-0x48/-0x44/-0x40）+ 帧 0xcc vs 0xc0（ORIG 多 callee-saved
    // %edi）+ 间接虚调用 N7 + 权重抽样段 `const int r` 栈缓存（N82/N7 族）。
    CInventory* inv = user->getCurCharacInvenW();
    const tagCompoundAvatar* rec =
        table->getCompoundAvatarInfo(info.m_field04);
    const int subType = CI_SubType(info.m_scriptA);
    int resultIdx = 0;                                         // -0x48

    // [b28e-CompoundDeathLCB] ORIG 0x08332f4x/0x08332f5x 判据为**正条件双 je**
    // （`movzbl 0xd6(%eax),%eax; test %al,%al; je <then>` +
    //  `movzbl 0xe8(%eax),%eax; cmp $0x4,%al; je <then>`，两跳同落 +0x334 的
    //  then 块 —— then 物理在**后**）。旧形（then 在前 + 4.4 `xor $0x1` 反条件）
    // 另多发一次 m_type 重载与重排。改写为德摩根等价：条件取反 + A/B 块互换
    //（语义不变，仅改块物理序）。
    if (info.m_fieldD6 != 0 && info.m_type != 4)
    {
        // 输出权重池空则失败。
        if (rec->m_b[subType] == 0 || rec->m_optsA[subType].empty())
            return static_cast<ENUM_ERROR>(1);

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
            // [轴 B 2026-09-10 保留] ORIG 0x083331d1 `cmp -0x38(%ebp),%eax; setl %al`
            // （**有符号序**，槽 -0x38 = weight；上下文同构：`call randInt(const&)`
            // 后即刻比较，与 ORIG 0x083331cc/0x083331d1 逐条对应）⇒ 原码比较为有符号；
            // 旧文 `unsigned long < int` 按 C++ 规则转无符号 ⇒ 4.4 产 `setb`。
            // 加 `(int)` 复现有符号序（randInt(10000) ∈ [0,9999]，值域内无分叉）。
            // 实测：our 636→635；汇编 diff 段 992→988 行（minus 364→361）⇒ ndiff 严格改善；
            // TU 四数 32/50/1/3/8 逐值不变（不劣）⇒ 保留。
            // 同件另一行（`_ProcCompoundCore` ORIG 0x08333157 `cmpl $0x0,-0x48; jg`）
            // 经地址自校为**表内配对假阳性**：我方 0x1280 同为 `jg`（有符号）⇒ 不改。
            if ((int)m_rand->randInt(static_cast<unsigned long>(10000)) < weight)
            {
                const int r = get_rand_int(rec->m_b[subType]);
                resultIdx = getItemIndex(rec->m_optsA[subType], r);  // +0x7c
            }
        }
    }
    else
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
            return static_cast<ENUM_ERROR>(1);
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
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 1)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailB, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailB.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 3)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailC, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 4)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_cube.m_vec, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                // ORIG 怪癖保留：type4 分支的 size 比较对象是 tailC(+0x2d0)
                //（ORIG 0x083332b7 读 +0x2d0 实证；不得改为 m_cube 或 m_tailD）。
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return static_cast<ENUM_ERROR>(0x16);
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
                // [b28e-M1S2 本波·§6.5/P139 盲区] ORIG 0x08333842/0x0833384a 两处
                // 函数名实参是**符号**（`movl $&_ZZN8WongWork15CCompoundAvatar17_
                // ProcCompoundCoreEP5CUserRNS0_15tagCompoundInfoEP22tagCompoundAvatar
                // TableE19__PRETTY_FUNCTION__,0x8(%esp)`），即 `__PRETTY_FUNCTION__`
                // 的函数局部静态（@0x08c2c540，**size 0x8a = 138B**）。ORIG 该串经
                // `objdump -s -j .rodata` 逐字节实录：
                //   `ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*,
                //    WongWork::CCompoundAvatar::stCompoundAvatarInfo&, stCompoundAvatarTable*)`
                // 依据（探针 /tmp/probe4.s，4.4.7 + 本仓旗标）：GCC 的 `__PRETTY_FUNCTION__`
                // 展开 = `<返回类型> <全限定函数>(<参数>)`；**嵌套** typedef 用全限定名
                // （`WongWork::CCompoundAvatar::stCompoundAvatarInfo&`），**全局** typedef
                // 用裸名 ⇒ ORIG 第 3 参 `stCompoundAvatarTable*` 是全局作用域 typedef。
                // [M1Free76to90 2026-09-11 已落地] 头改（返回类型 `ENUM_ERROR` + 类内
                // typedef `stCompoundAvatarInfo`）后本处改回 `__PRETTY_FUNCTION__`（P139），
                // 发射 ORIG 同名局部静态；本 TU 实测 **size 0x8b**。
                // 【残余 1 字符】我方第 3 参拼写 `tagCompoundAvatarTable*`（22 字符）vs
                // ORIG `stCompoundAvatarTable*`（21）⇒ 还需**全局** typedef
                // `typedef tagCompoundAvatarTable stCompoundAvatarTable;`（放类内或
                // `namespace WongWork` 内都会带限定 ⇒ 不符）。已将该头改动**再报**父泳道。
                // 当前串除该 1 字符外与 ORIG 逐字节相同。
                // mangled 不含返回类型/typedef ⇒ 该头改动零 ABI 影响。
                cMyTrace trace(__PRETTY_FUNCTION__, 0x361, 5);
                trace("[%s][%d] AddAvatarItem Fail", __PRETTY_FUNCTION__, 0x361);
                return static_cast<ENUM_ERROR>(0x11);
            }
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialA.m_addInfo2), ipgNoBuf);
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialB.m_addInfo2), ipgNoBuf);
            return ENUM_ERROR_NONE;
        }
        return static_cast<ENUM_ERROR>(0x16);                  // delete_item 失败
    }
    return static_cast<ENUM_ERROR>(1);                         // 结果不可实例化
}

// =========================== _SendDisjointAvatarStatistic ===================

// ORIG 0x08333ecc。统计服上报本次分离的两件头像分级；注意 B 组 subType 也读
// scriptA 的 vtbl+0xc（ORIG 如此）。
// 【b28d-solo-CompoundDeathLCB 判词】NEAR（79/79 逐条全等；帧 `push %ebx;
//   sub $0x54`、全部槽号、`lea -0x3c(%ebp),%ebx` 与 ORIG 全等）。修法：
//   ①两组分级复用同一 gradeIdx 槽（P160，两个独立局部会各占一槽 +4 帧）；
//   ②proxy 具名局部改单语句链式调用（P52，消槽且 &packet 升格 ebx）。
//   残余 = CI_SubType 虚调用目标寄存器 N7 ×2（4.4.7 恒 %eax / ORIG %edx）。
// 【b28e-M1S2 本波复核】复测 orig 79 / ours 79（NEAR 不变；帧 push %ebx +
//   sub $0x54、`movl $0x2,-0x32`、`movb $0x0,-0x26/-0x19` 槽号全等）。
//   语义轴：本件无字符串字面量（无 §6.5/§6.5.1 盲区）；B 组 CI_SubType 亦读
//   scriptA（ORIG 0x08333f8e 用 -0x14(%ebp)）已一致。唯一残余 = N7 ×2
//   （ORIG `mov (%eax),%edx; mov -0x14(%ebp),%eax; mov %eax,(%esp); call *%edx`
//    / 我方 `mov (%eax),%eax; mov -0x14(%ebp),%edx; mov %edx,(%esp); call *%eax`）。
#include "CompoundDeathLCB/_SendDisjointAvatarStatistic.inc"
{
    Packet_Avater_Disjoint_Statistic packet;

    const CItem* scriptA = info.m_scriptA;
    const CItem* scriptB = info.m_scriptB;

    packet.m_field0a = 2;                                      // -0x36/local_36
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333ef6/0x08333f52 两组分级**复用同一
    // -0xc 槽**（各以 `movl $0,-0xc` 重新初始化）⇒ 源码必须是同一个具名局部
    // 两次使用（P160 复用槽；两个独立局部会各占一槽，实测 +4 帧）。
    int gradeIdx = 0;
    if (scriptA->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptA->get_grade() - 1;
    }
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = CI_SubType(scriptA);
    packet.m_flagA = 0;
    gradeIdx = 0;
    if (scriptB->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptB->get_grade() - 1;
    }
    packet.m_gradeIdxB = gradeIdx;
    packet.m_subTypeB = CI_SubType(scriptA);                   // 【ORIG 怪癖】保留
    packet.m_flagB = 0;

    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333fa7：`lea -0x3c(%ebp),%ebx` 先把
    // &packet 升格 callee-saved，再求 receiver（GetServerProxy）⇒ 单语句链式
    // 表达式（P52）；具名 proxy 局部会多一个槽且 &packet 退化为实参处现算。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet),
                      static_cast<int>(sizeof(packet)));       // 0x28
}

// ================================ _generateIPGNo ============================

// ORIG T 0x08333df6（const）。"C%02d%02d%02d%02d%010d" 五元组。
#include "CompoundDeathLCB/_generateIPGNo.inc"
                                      char* buf) const
{
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333df6 直取形参地址（无 time_t t
    // 局部槽）且把 localtime_r 返回值存具名指针槽 -0x1c 后经 p-> 读字段
    //（P43/P0）；五个 snprintf 实参全内联、右起求值（characNo→mday→mon→
    // year%2000→serverId），characNo 升格 %esi 跨 G_CEnvironment 保活
    //（P226/P227 形）。
    // 【定案】达成 **AE**：56/56 逐条全等（仅字符串立即数重定位标签伪差）。
    struct tm lt;
    struct tm* p = localtime_r(&curSec, &lt);
    // ORIG 0x08333e63 `mov 0x1b0(%eax),%eax`（4 字节读）= CEnvironment+0x198
    //（m_serverEnvir）+0x18 → Server_Envir::m_channelNo（通道号；同
    // CDataManager.cpp:1461 具名口径）。
    OS_API::snprintf(buf, 0x14, "C%02d%02d%02d%02d%010d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (p->tm_year + 1900) % 2000,
                     p->tm_mon + 1,
                     p->tm_mday,
                     user->getCurCharacNo());
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
#include "CompoundDeathLCB/_makeRecordPacket.inc"
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
#include "CompoundDeathLCB/makeMyBestRecordPacket.inc"
                                                unsigned int key,
                                                PacketGuard& packet)
{
    // 【b28d-solo-CompoundDeathLCB 判词】NEAR（63/63 助记符逐条全等；仅 find/end
    // 两组的 sret 临时与迭代器槽号互换：ORIG find 临时 -0x28/it -0x30、
    // last -0x20/end 临时 -0x18；我们 it -0x28/find 临时 -0x20、end 临时 -0x18/
    // last -0x30）。修法：补具名引用局部 `const stBestRecord_t& rec = it->second;`
    // 复现 ORIG ptEv 后 `add $0x4` + 槽回读（63→63）。试过「先声明后赋值」形
    // ⇒ EXTRA `_Hashtable_const_iterator::C1Ev` ×2（P50 负例，已回退）；指针/引用
    // best、const/非 const 变量四形槽号不变 ⇒ N86/N93 封顶。
    BestMap* best = _getBestRecordTable(towerIdx);
    // [b28f-SweepGroup17 判据⑩ 实测负例] 试过交换为「先 end/last、后 find/it」以复现
    //   ORIG 0x08468b8e 槽阶梯（end 临时 -0x18 < last -0x20 < find 临时 -0x28 <
    //   it -0x30）⇒ NEAR→**DIFF**（near 2→1 / diff 3→4）反劣，已回滚。
    //   维持原序（find→end）封顶 N86/N93。
    const BestMap::const_iterator it = best->find(key);
    // [b28f-SweepGroup17 判据⑩ 配方①（L29）] ORIG 不给 end() 具名槽 ⇒ 内联 end()
    //   调用（不落 `last` 对象槽），只保留 it 具名 + end() 的 sret 临时。
    if (it == best->end())
    {
        packet.put_byte(0);
    }
    else
    {
        packet.put_byte(1);
        // ORIG 0x08468c42..c4a：ptEv 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)`，
        // 再于实参处 `mov -0xc(%ebp),%eax` 回读 ⇒ 原码有具名引用局部（P0 槽往返）。
        const stBestRecord_t& rec = it->second;
        _makeRecordPacket(rec, packet);
    }
}

// ORIG T 0x08468c68。该塔 5 个榜首槽顺序序列化（基址 tower*0x390，stride 0x98）。
#include "CompoundDeathLCB/makeTopRankerRecordPacket.inc"
                                                   PacketGuard& packet)
{
    // ORIG 0x08468c68：i 从 1 起（movl $1,-0xc → cmpl $5,setle ⇒ i<=5），
    // 槽址 = m_towers + towerIdx*0x390 + i*0x98（双 imul 直用 m_towers 基址，
    // 不经 m_slots 具名 +0x98 偏移；具名 slots 局部会引入 add $0x90/+0x8
    // 常量折叠与 -0x10 基址槽）。
    // [b27] 残差：ORIG 先载 0xc(towerIdx)/-0xc(i) 后两 imul、我们 eax 预载
    // m_towers 且 lea 合并——N18a 族寄存器序不可定向，语义/循环界/槽序已全等。
    // 【b28d-solo-CompoundDeathLCB 定案】达成 **AE**：24/24 逐条全等。修法 =
    // 真二维下标 `m_towers[towerIdx][i]`（两级索引令 4.4 发
    // `mov 0xc(%ebp),%eax; mov -0xc,%edx; imul 0x98; imul 0x390; lea; add
    // 0x8(%ebp),%eax`，与 ORIG 全等；旧 char* 算术形 25 条且 this-first 不可翻）。
    // [b28d-Main·检查点14] stTowerBlock 包装类删除 ⇒ 成员访问改二维下标（同形）。
    for (int i = 1; i <= 5; ++i)
        _makeRecordPacket(m_towers[towerIdx][i], packet);
}

}  // namespace WongWork

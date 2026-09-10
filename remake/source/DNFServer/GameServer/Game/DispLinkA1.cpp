// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispLinkA1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

#include <utility>
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"         // stStackableBoosterItemInfo_t / getBoosterItem
#include "CDataManager.h"           // G_CDataManager / find_item / m_disJoint (+0x1c)
#include "DisJoint.h"               // DisJoint（m_disJoint 指向对象）
#include "CInventory.h"             // MakeItemPacket / GetInvenSlot
#include "CUser.h"                  // Send / get_state / GetParty
#include "CUserCharacInfo.h"        // getCurCharacInvenR / getCurCharacR / GetCurCharacExpertJob*
#include "PacketGuard.h"            // PacketGuard（含 PacketHeader 传递可见）
#include "CompoundEmblemDeps.h"     // LineFunc
#include "CStatisticServerProxy.h"  // SendPacket(char*, int)
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy
#include "GlobalData.h"             // GlobalData::s_statistic_proxy_mgr
// [b28c-32] P104 合并：匿名 namespace 内的 Packet_Emblem_Create_Statistic /
// Packet_Avater_Disjoint_Statistic 镜像类删除，统一 include 权威头
// （Avater 局部版仅 0x17 字节 ⇒ 帧 0x34 vs ORIG 0x44、对象落 -0x23 vs -0x34）。
#include "Packet_Emblem_Create_Statistic.h"
#include "Packet_Avater_Disjoint_Statistic.h"

// ============================================================================
// 1. Dispatcher_Avatar_Disjoint_Random_Reward::read  ORIG 0x081de9da
//    LineFunc(0x5041, pretty, 0, 0)
// ============================================================================
#include "DispLinkA1/read.inc"
{
    (void)packet;
    (void)msg;
    double framePad; (void)&framePad;   // P190: ORIG 帧 0x28 = 4.4 outgoing 0x18 + double 8 对齐撑帧
    return LineFunc(0x5041,
                    "virtual int Dispatcher_Avatar_Disjoint_Random_Reward::read"
                    "(PacketBuf&, MSG_BASE&)",
                    0, 0);
}

// ============================================================================
// 2. Dispatcher_Avatar_Disjoint_Random_Reward::send  ORIG 0x081dec56
//    拷贝 stAvatarDisjointRandomReward（含 vector 拷贝），按结果码分支发包。
// [b28d-solo-DispLinkA1] verdict=AE（compare_tu clean；p167 25/25 CLEAR）。
//   修法：P96 迭代器改非 const iterator；P13+ORIG 多一次 size() 并以非零包裹
//   循环（setne 物化）；P28 it++（ORIG operator++(int)）。
// ============================================================================
#include "DispLinkA1/send.inc"
{
    // ORIG C1 @0x0823516e 为拷贝构造；ParamBase 实际类型即本 Param。
    stAvatarDisjointRandomReward result(
        static_cast<stAvatarDisjointRandomReward&>(param));

    PacketGuard packet;
    packet.put_header(1, 0x1c2);
    if (result.m_4 == 0) {
        packet.put_byte(1);
        packet.put_short(result.m_8);
        packet.put_byte((int)result.m_rewards.size());
        // [b28d-solo-DispLinkA1 P96/P13] ORIG 迭代器为非 const iterator
        // （p167: operator-><pair<int,int>*> / ne<pair*,pair*>），且 size() 被
        // 调用两次：一次作 put_byte 实参、一次以其非零包裹循环（setne 物化）。
        if (result.m_rewards.size() != 0) {
            for (std::vector<std::pair<int, int> >::iterator it =
                     result.m_rewards.begin();
                 it != result.m_rewards.end(); ++it) {
                packet.put_int(it->first);
                packet.put_int(it->second);
            }
        }
    }
    else {
        packet.put_byte(0);
        packet.put_byte(result.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// 3. Dispatcher_CallGuildCreateRight::send_packet_guildcreateright
//    ORIG 0x081ffbde：clear + header(0,0xe) + byte(0) + short(1) +
//    MakeItemPacket(INVEN 1, slot 0) + finalize + Send
// ============================================================================
#include "DispLinkA1/send_packet_guildcreateright.inc"
    CUser* user, unsigned char errCode)
{
    (void)errCode;  // ORIG 未读取该参数（栈上落 -0x1c 后未使用）
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, packet);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// 4. Dispatcher_CallGuildInvite::send_packet_guild_invite  ORIG 0x08207852
//    header(1,0x9a)；inviteType==0 → byte(1)，否则 byte(0)+byte(inviteType)
// ============================================================================
#include "DispLinkA1/send_packet_guild_invite.inc"
    CUser* user, unsigned char inviteType)
{
    PacketGuard packet;
    packet.put_header(1, 0x9a);
    if (inviteType == 0) {
        packet.put_byte(1);
    }
    else {
        packet.put_byte(0);
        packet.put_byte((int)inviteType);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// 5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup  ORIG 0x08200126
//    同 4 型：header(1,0x4c)
// [b28d-solo-DispLinkA1] verdict=IDENTICAL（58=58，纯重定位标签差）。
// ============================================================================
#include "DispLinkA1/send_packet_guild_levelup.inc"
    CUser* user, unsigned char errCode)
{
    PacketGuard packet;
    packet.put_header(1, 0x4c);
    if (errCode == 0) {
        packet.put_byte(1);
    }
    else {
        packet.put_byte(0);
        packet.put_byte((int)errCode);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// 6. Dispatcher_Client_Spec_Statistic::check_error  ORIG 0x081ce75a
// ============================================================================
#include "DispLinkA1/check_error.inc"
{
    if (user == 0) {  // ORIG 判 user@+0xc（0x081ce760 cmp param_1,0）；我们误判 msg ref@+0x10
        (void)msg;
        return LineFunc(
            0x27fd,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}

// ============================================================================
// 7. Dispatcher_Client_Spec_Statistic::read  ORIG 0x081ce5fe
//    msg+0xd/+0xe/+0xf 三字节，+0xf 为条数；条目步长 0xc：
//    +0x12 byte、+0x16 dword、+0x1a dword（ORIG 裸偏移直读 MSG 体）。
//    b28c-34 判词（identical）：ORIG 寻址为 (p + i*0xc) + K（先 add 基址、后 add $K，
//    0x081ce65e..0x081ce6a5）。4.4 前端会把纯指针形 `p + i*0xc + K` 折叠成
//    `p + (i*0xc + K)`（先 add 常量、后 add 基址，后两条再多 mov %ecx/lea）；
//    经 unsigned long 中转 `(unsigned long)(p + i*0xc) + K` 阻止该折叠，逐条复现 ORIG。
// ============================================================================
#include "DispLinkA1/read_1.inc"
{
    unsigned char* const p = reinterpret_cast<unsigned char*>(&msg);
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xe));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xf));
    for (int i = 0; i < (int)reinterpret_cast<unsigned char (&)[0x10]>(*p)[0xf]; ++i) {  // ORIG movzbl 0xf(%eax)
        packet.get_byte(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x12));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x16));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x1a));
    }
    return 0;
}

// ============================================================================
// 8. Dispatcher_CompatibilityIndex::check_error  ORIG 0x081e23e0（纯 return 0）
// ============================================================================
#include "DispLinkA1/check_error_1.inc"
                                               ParamBase& param)
{
    (void)user;
    (void)msg;
    (void)param;
    return 0;
}

// ============================================================================
// 9. Dispatcher_Compound_ExtreamItem::_read_packet  ORIG 0x08225806
// [b28d-solo-DispLinkA1] verdict=AE（172=172，ndiff=0；p167 12/12 CLEAR）。
//   修法：P0 删具名 slot 局部（ORIG 值留 %edi 跨 GetInvenSlot）；P211 档③
//   字段侧旋钮——m_itemRefs 改 4 字节整型存储（Dispatchers.h），复现 ORIG
//   RHS-first + `mov %eax,%edx` 中转（const CItem*/const void* 两形实测均同型）。
// ============================================================================
#include "DispLinkA1/_read_packet.inc"
{
    if (m_user == 0)
        return 0;

    _reset();

    // +0x04：目标槽
    packet.get_short(*reinterpret_cast<unsigned short*>(&m_field4));

    for (int i = 0; i < 2; ++i) {
        // +0x08 + i*2：材料槽位
        packet.get_short(
            *reinterpret_cast<unsigned short*>(&m_slots[i]));
        // [b28d-solo-DispLinkA1 P0] ORIG 无具名 slot 槽（值留 %edi 跨
        // GetInvenSlot 调用），实参内联直传 m_slots[i]。
        m_materials[i] =
            m_user->getCurCharacInvenR()->GetInvenSlot(1, m_slots[i]);
        if (!m_materials[i].isEquipableItemType())
            return 0;
        // find_item(m_materials[i].m_addInfo)（ORIG 读 this+i*0x3d+0xe int）；
        // [P211 档③] 字段为整型存储（异型 RHS）⇒ 复现 ORIG RHS-first+edx 中转。
        m_itemRefs[i] = reinterpret_cast<unsigned int>(
            G_CDataManager()->find_item(m_materials[i].m_addInfo));
    }

    // +0xcd：选择字节（≤2 合法）
    packet.get_byte(*reinterpret_cast<char*>(&m_select));
    if (m_select > 2)
        return 0;

    // 目标物品：槽 m_field4；按 item idx 判定 +0xd4 目标类别
    const Inven_Item target =
        m_user->getCurCharacInvenR()->GetInvenSlot(1, m_field4);
    if (target.isEmpty())
        return 0;
    if (target.m_addInfo == 0x4e3)
        m_targetKind = 0;
    else if (target.m_addInfo == 0x4e4)
        m_targetKind = 1;
    else if (target.m_addInfo == 0x28becf)
        m_targetKind = 3;
    else
        m_targetKind = 2;
    return 1;
}

// ============================================================================
// 10. Dispatcher_CreateDisjointStore::check_error  ORIG 0x081cf8d4
// ============================================================================
#include "DispLinkA1/check_error_2.inc"
                                                ParamBase& param)
{
    (void)msg;
    (void)param;
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;
    if (user->GetCurCharacExpertJobType() != 3)
        return -2;
    if (user->get_state() != 3)
        return 0x13;
    if (user->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (user->GetParty() != 0)
        return 0x13;
    return 0;
}

// ============================================================================
// 11. Dispatcher_DisJointAvatar::SendCreateEmblemInfo  ORIG 0x08218a36
//     条数<6 才发；每对 (itemIdx, value)：find_item 失败即整体放弃；
//     slots[i+1]=素材 grade、slots[i+6]=附加值；末写 slots[0]=条数。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（98 vs 97，ndiff=13；p167 14/14 CLEAR）。
//   已修：P28 it++（operator++(int)）、P167 内联 GetServerProxy（删匿名 helper）、
//   P0 具名 itemIdx/value 槽、P13 end 缓存、P196/P218 负条件 setne/jne、
//   P239 begin→end→count=0 发射序、P311 具名数组成员视图（m_grades 已归正）。
//   残余 N：①N7 实参先升格 %ebx 跨 receiver call（P226 语句表达式/裸形均不翻，
//   4.4 恒先 call 后装载）；②N94 m_values 常量拆分（ORIG add $8+disp -0x58，
//   4.4 lea 0x4+disp -0x48，地址等价）；③N18b 尾比较（ORIG mov count,%ebx;
//   call size; cmp %eax,%ebx，4.4 恒 cmp mem,%eax 内存直比）。
// ============================================================================
#include "DispLinkA1/SendCreateEmblemInfo.inc"
    std::vector<std::pair<int, int> > const& results)
{
    Packet_Emblem_Create_Statistic packet;
    if (results.size() >= 6)
        return;

    std::vector<std::pair<int, int> >::const_iterator it = results.begin();
    std::vector<std::pair<int, int> >::const_iterator end = results.end();
    // [b28d-solo-DispLinkA1 P239] ORIG 发射序：begin→end→count=0。
    int count = 0;
    while (it != end) {
        // [b28d-solo-DispLinkA1 P0] ORIG 具名槽 -0x14/-0x10（各一次 operator->）。
        const int itemIdx = it->first;
        const int value = it->second;
        // [b28d-solo-DispLinkA1 N] ORIG 实参先升格 %ebx 跨 receiver call
        // （G_CDataManager）保活；P226 语句表达式/裸形均不翻（4.4 恒先 call 后装载）。
        const CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
            return;
        packet.m_grades[count] = item->get_grade();
        packet.m_values[count] = value;
        ++count;
        if (count > 5)
            break;
        it++;   // [b28d-solo-DispLinkA1 P28] ORIG 调 operator++(int)（sret -0x1c）
    }
    // [b28d-solo-DispLinkA1 P196/P218] ORIG 为负条件（setne/jne）。
    if (count != (int)results.size())
        return;
    packet.m_count = count;
    // [b28d-solo-DispLinkA1 P167] ORIG 直调 GetServerProxy(0)（无匿名 helper）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}

// ============================================================================
// 12. Dispatcher_DisJointAvatar::SendDisjointAvatarInfo  ORIG 0x08218996
// ============================================================================
// [b28c-32] verdict=NEAR：P104 合并镜像类为权威头（0x17→0x28）；余 N7（47=47）。
#include "DispLinkA1/SendDisjointAvatarInfo.inc"
{
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    // ORIG 0x08218996：gradeIdx 具名临时 + if/else 两段（sete 物化 + 槽往返
    // 后写回 +0x0e）；m_field0a=1 先行、m_flag=1 后行。
    int gradeIdx = 0;
    if (pItem->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = pItem->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = pItem->GetItemType();
    packet.m_flagA = 1;
    // 伪 helper 归一（战术四式）：ORIG 0x08218996 内联
    // GetServerProxy(s_statistic_proxy_mgr, 0) → SendPacket(buf, 0x28)，
    // 不引入统计代理伪 helper。
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
}

// ============================================================================
// 13. Dispatcher_DisJointAvatar::_getResultItems (const)  ORIG 0x0821873c
//     DisJoint 表选素材：jewelTypeIndex==-1 → [grade][type] 区（+0x444 起），
//     否则按 job 遍历定位（+0xd4 起）；素材须为 stackable(0x10)，收集
//     getBoosterItem 条目 (m_itemIdx, m_addInfo)。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（186 vs 190，ndiff=52；p167 23/23 CLEAR）。
//   已修：P315/P167 字段 m_itemIdx int→unsigned int（CStackableItem.h）⇒
//   make_pair<unsigned int const&,unsigned short const&> 与
//   pair<int,int>::pair<unsigned,unsigned short> 实例、2 次 iterator→const_iterator
//   转换构造、ne<const*,const*> 全部归位；P47/P222 if/else 反转（je 落表查找块）；
//   P0 去 dj/grade/type 具名槽；P231 纯 register dj（非 asm 绑定）消槽。
//   残余 N：4.4 算术 lowering/寄存器角色差（N86/N87/N7 族）——ORIG dj→%esi 且
//   job 先于 get_grade 装载、grade*5 用 edx 累加、`add $0x34; mov 0x4(%esi,%eax,4)`
//   单条 scaled（成员数组视图缺 +4 表建模）；4.4 恒 dj→%ebx、job 后装、指针下标
//   `[i+K]` 折 shl+lea。P226/裸 register/基址+4 三形实测均不翻。
// ============================================================================
#include "DispLinkA1/_getResultItems.inc"
    CUser* user, CItem const* pItem, int jewelTypeIndex,
    std::vector<std::pair<int, int> >& results) const
{
    (void)user;
    int materialIdx = 0;
    // [b28d-solo-DispLinkA1 P47/P222] ORIG 布局：`je` 跳到函数后部的表查找块，
    // job 遍历块在 fall-through ⇒ 源码条件取反、两臂互换。
    if (jewelTypeIndex != -1) {
        char job = '\xb';
        for (int i = 0; i < 0xb; ++i) {
            if (pItem->check_job_type((char)i))
                job = (char)i;
        }
        if (job == '\xb')
            return 0x16;
        // [b28d-solo-DispLinkA1 P0] ORIG 无具名 dj/grade 槽（dj→%esi、job→%ebx
        // 跨调用保活），全部内联表达式。
        // §4.3 例外（P156 表寻址 intAtConst）：ORIG 把 DisJoint 对象首部起
        // +4 处的跨成员大表当 int 数组访问（dj+4+idx*4），DisjointScript 现有
        // 成员建模（vector 族）无法线性表达该视图，保留地址形式并注明。
        // [b28d-solo-DispLinkA1 P231] ORIG 无 dj 栈槽（`mov 0x1c(%eax),%esi`
        // 单段落 callee-saved）⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
        register DisJoint* const dj = G_CDataManager()->m_disJoint;
        materialIdx = reinterpret_cast<int*>(dj)
            [pItem->get_grade() * 5 + (int)job * 0x14 + jewelTypeIndex + 0x35];
    }
    else {
        register DisJoint* const dj = G_CDataManager()->m_disJoint;
        materialIdx = reinterpret_cast<int*>(dj)
            [pItem->get_grade() * 10 + pItem->GetItemType() + 0x111];
    }

    if (materialIdx == 0)
        return 0x16;

    CStackableItem* const mat =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(materialIdx));
    if (mat == 0 || mat->GetItemType() != 0x10)
        return 0x16;

    std::vector<stStackableBoosterItemInfo_t> boosters;
    mat->getBoosterItem(boosters);
    // [b28d-solo-DispLinkA1 P167/P13] ORIG begin()/end() 各求值一次并各触发一次
    // iterator→const_iterator 转换构造，循环条件用 ne<const*,const*>。
    std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
        boosters.begin();
    std::vector<stStackableBoosterItemInfo_t>::const_iterator end =
        boosters.end();
    for (; it != end; ++it) {
        results.push_back(std::make_pair(it->m_itemIdx, it->m_addInfo));
    }
    return 0;
}

// ============================================================================
// DispLinkA1.cpp — Dispatcher 链接核销批次 A1（独占 TU）
//
// 逐符号忠实转写，报告依据 docs/class_func_reports/：
//   1. Dispatcher_Avatar_Disjoint_Random_Reward::read        0x081de9da (read.md)
//   2. Dispatcher_Avatar_Disjoint_Random_Reward::send        0x081dec56 (send.md)
//   3. Dispatcher_CallGuildCreateRight::send_packet_guildcreateright 0x081ffbde
//   4. Dispatcher_CallGuildInvite::send_packet_guild_invite  0x08207852
//   5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup 0x08200126
//   6. Dispatcher_Client_Spec_Statistic::check_error         0x081ce75a
//   7. Dispatcher_Client_Spec_Statistic::read                0x081ce5fe
//   8. Dispatcher_CompatibilityIndex::check_error            0x081e23e0
//   9. Dispatcher_Compound_ExtreamItem::_read_packet         0x08225806
//  10. Dispatcher_CreateDisjointStore::check_error           0x081cf8d4
//  11. Dispatcher_DisJointAvatar::SendCreateEmblemInfo       0x08218a36
//  12. Dispatcher_DisJointAvatar::SendDisjointAvatarInfo     0x08218996
//  13. Dispatcher_DisJointAvatar::_getResultItems (const)    0x0821873c
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

namespace
{

inline CStatisticServerProxy* statisticProxyGroup0()
{
    return GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0);
}

}  // namespace

// ============================================================================
// 1. Dispatcher_Avatar_Disjoint_Random_Reward::read  ORIG 0x081de9da
//    LineFunc(0x5041, pretty, 0, 0)
// ============================================================================
int Dispatcher_Avatar_Disjoint_Random_Reward::read(PacketBuf& packet, MSG_BASE& msg)
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
// ============================================================================
void Dispatcher_Avatar_Disjoint_Random_Reward::send(CUser* user, ParamBase& param)
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
        for (std::vector<std::pair<int, int> >::const_iterator it =
                 result.m_rewards.begin();
             it != result.m_rewards.end(); ++it) {
            packet.put_int(it->first);
            packet.put_int(it->second);
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
void Dispatcher_CallGuildCreateRight::send_packet_guildcreateright(
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
void Dispatcher_CallGuildInvite::send_packet_guild_invite(
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
// ============================================================================
void Dispatcher_CallGuildLevelUp::send_packet_guild_levelup(
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
int Dispatcher_Client_Spec_Statistic::check_error(CUser* user, MSG_BASE& msg)
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
int Dispatcher_Client_Spec_Statistic::read(PacketBuf& packet, MSG_BASE& msg)
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
int Dispatcher_CompatibilityIndex::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)user;
    (void)msg;
    (void)param;
    return 0;
}

// ============================================================================
// 9. Dispatcher_Compound_ExtreamItem::_read_packet  ORIG 0x08225806
// ============================================================================
int Dispatcher_Compound_ExtreamItem::_read_packet(PacketBuf& packet)
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
        const int slot = m_slots[i];
        m_materials[i] =
            m_user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        if (!m_materials[i].isEquipableItemType())
            return 0;
        // find_item(m_materials[i].m_addInfo)（ORIG 读 this+i*0x3d+0xe int）
        m_itemRefs[i] = G_CDataManager()->find_item(m_materials[i].m_addInfo);
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
int Dispatcher_CreateDisjointStore::check_error(CUser* user, MSG_BASE& msg,
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
// ============================================================================
void Dispatcher_DisJointAvatar::SendCreateEmblemInfo(
    std::vector<std::pair<int, int> > const& results)
{
    Packet_Emblem_Create_Statistic packet;
    if (results.size() >= 6)
        return;

    int count = 0;
    std::vector<std::pair<int, int> >::const_iterator it = results.begin();
    while (it != results.end()) {
        const CItem* item = G_CDataManager()->find_item(it->first);
        if (item == 0)
            return;
        packet.m_slots[count + 1] = item->get_grade();
        packet.m_slots[count + 6] = it->second;
        ++count;
        if (count > 5)
            break;
        ++it;
    }
    if (count == (int)results.size()) {
        packet.m_slots[0] = count;
        statisticProxyGroup0()->SendPacket(reinterpret_cast<char*>(&packet),
                                           0x36);
    }
}

// ============================================================================
// 12. Dispatcher_DisJointAvatar::SendDisjointAvatarInfo  ORIG 0x08218996
// ============================================================================
// [b28c-32] verdict=NEAR：P104 合并镜像类为权威头（0x17→0x28）；余 N7（47=47）。
void Dispatcher_DisJointAvatar::SendDisjointAvatarInfo(CItem const* pItem)
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
// ============================================================================
int Dispatcher_DisJointAvatar::_getResultItems(
    CUser* user, CItem const* pItem, int jewelTypeIndex,
    std::vector<std::pair<int, int> >& results) const
{
    (void)user;
    int materialIdx = 0;
    if (jewelTypeIndex == -1) {
        // ORIG：*(int*)(G_CDataManager()+0x1c) → DisJoint；表基 +4。
        DisJoint* const dj = G_CDataManager()->m_disJoint;
        const int grade = pItem->get_grade();
        const int type = pItem->GetItemType();
        materialIdx = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(dj) + 4 +
            (grade * 10 + type + 0x110) * 4);
    }
    else {
        char job = '\xb';
        for (int i = 0; i < 0xb; ++i) {
            if (pItem->check_job_type((char)i))
                job = (char)i;
        }
        if (job == '\xb')
            return 0x16;
        DisJoint* const dj = G_CDataManager()->m_disJoint;
        const int grade = pItem->get_grade();
        materialIdx = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(dj) + 4 +
            (grade * 5 + job * 0x14 + jewelTypeIndex + 0x34) * 4);
    }

    if (materialIdx == 0)
        return 0x16;

    CStackableItem* const mat =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(materialIdx));
    if (mat == 0 || mat->GetItemType() != 0x10)
        return 0x16;

    std::vector<stStackableBoosterItemInfo_t> boosters;
    mat->getBoosterItem(boosters);
    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
             boosters.begin();
         it != boosters.end(); ++it) {
        results.push_back(std::make_pair(it->m_itemIdx, it->m_addInfo));
    }
    return 0;
}

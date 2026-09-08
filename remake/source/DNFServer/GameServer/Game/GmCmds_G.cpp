// GmCmds_G.cpp — game_master GM 命令批次 G（13 个命令类）。
//
// 对照 docs/class_func_reports/game_master__{CTeleportCmd,CTestDisjointCmd,
// CTestTradeRestrict,CTitleBookAdjust,CTitleBookNoClear,CTitleBookReset,
// CUpgradeCardDebug,CUserPremiumDelete,CViewTutorialFlagCmd,
// CVillageAttackedFightCmd,CVillageAttackedStartCmd,CVillageAttackedStopCmd,
// CDropItem}/ 逐函数转写（objdump 汇编 + Ghidra C）。
//
// 注：GameMasterCmd.h 本批禁止编辑，头中占位字段 m_str0(std::string@+0x08)
// 在 ORIG 中部分实为 int（atoi 解析结果），按布局以 reinterpret_cast 直写，
// 使用点均注明。
#include "GameMasterCmd.h"
#include "CDataManager.h"           // G_CDataManager/find_item
#include "CGameManager.h"           // G_CGameManager()->GetParty()
#include "DB_TestTradeRestrict.h"   // DB_TestTradeRestrict::makeRequest（static 真成员）
#include "CHandlePremium.h"         // WongWork::CHandlePremium
#include "CItem.h"                  // CItem 完整类型
#include "CMonitorServerProxy.h"    // CMonitorServerProxy::SendTcpPacket
#include "CParty.h"                 // CParty::set_single_play/OnFightVillageMonster
#include "CStreamGuard.h"           // CStreamGuard(Stream*, bool)/operator<<
#include "CStackableItem.h"         // CStackableItem::IsReinforceTicket 等真成员
#include "CTitleBook.h"             // CTitleBook::_gm_clear/sendList
#include "CAchievement.h"           // CAchievement::_gm_init/adjust/sendList
#include "CExpertJob.h"             // expert_job::CAlchemist::OnTeleportCharacter（static 真成员）
#include "CUser.h"                  // CUser/CUserCharacInfo/CAccountCargo/CCargo
#include "DisJoint.h"               // DisJointResult
#include "GameRand.h"               // get_rand_int
#include "GameWorld.h"              // make_packet_chat_msg
#include "GlobalData.h"             // s_msgq_mgr/s_stream_pool/s_monitor_proxy_mgr/
                                    // s_systemTime_/g_SPremiumInfo
#include "InterfacePacketBuf.h"     // put_*/set_index/get/finalize
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "PacketHeader.h"           // PacketHeader(ushort, ushort)
#include "SPremiumInfo.h"           // SPremiumInfo（g_SPremiumInfo 元素）
#include "StreamPool.h"
#include "CEnvironment.h"           // G_CEnvironment()->m_serverEnvir.m_serverGroup
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy
#include "OS_API.h"                 // OS_API::WriteLog（第三方，定义 LinkMisc4_Data.cpp）

#include <cstdio>
#include <cstring>
#include <vector>

// ============================================================================
// ORIG 外部符号桥接（共享头未声明；asm 标签保证链接名一致）。
// ============================================================================

struct Inven_Item;

// expert_job::CDisjointer::get_disjoint_result_
//   (CItem*, Inven_Item*, int, DisJointResult&, bool, CUser*)
// ORIG T 0x085d234a（CTestDisjointCmd::execute 调用）。保留自由函数桥：
// 头内为非 static 成员，但 ORIG 调用点（0x084a9c0c+0x102）仅压 6 实参
// 无 this 槽 ⇒ static 语义实证；转真成员调用会多压 this，破坏字节等价，
// 故以 thiscall 首参显式桥接。
extern void CDisjointer_get_disjoint_result_(
    void* disjointer, Inven_Item* item, int grade, DisJointResult& result,
    bool flag, CUser* user)
    asm("_ZN10expert_job11CDisjointer20get_disjoint_result_EP5CItemP10Inven_Item"
        "iR14DisJointResultbP5CUser");

// CStackableItem 非虚查询族（ORIG W：0x84b4222/0x84b4232/0x84b4242/0x84b4252；
// 真成员已落地（CStackableItem.h + ItemUpgradeVtableB2.cpp），按真成员直调：
//   IsReinforceTicket()/GetReinforceRate()/IsAmplifyReinforceTicket()/
//   GetAmplifyReinforceRate()。调用点对 CItem* 经 static_cast<CStackableItem*>
// 上转（单继承 +0x00，同 this，字节等价）。

// ============================================================================
// CVillageAttackedStartCmd 专用监控包（ORIG 全局类；
// ctor W 0x0822c088：PacketHeader(this,0x1779,0x1a) 后清 +0x0a/+0x0e）。
// 总长 0x1a：+0x00 PacketHeader(10B) / +0x0a uid / +0x0e characNo /
// +0x12 vill(atoi(args)) / +0x16 恒 0。
// 注意须位于全局命名空间（ORIG callee 为 _ZN31Packet_VillageAttackedGMCommandC1Ev）。
// ============================================================================
struct Packet_VillageAttackedGMCommand : public PacketHeader
{
    Packet_VillageAttackedGMCommand()
        : PacketHeader(0x1779, 0x1a)
    {
        m_uid = 0;         // ORIG movl $0x0,+0xa
        m_characNo = 0;    // ORIG movl $0x0,+0xe
    }

    unsigned int m_uid;       // +0x0a
    unsigned int m_characNo;  // +0x0e
    unsigned int m_vill;      // +0x12（parse 结果）
    unsigned int m_field16;   // +0x16（execute 置 0）
} __attribute__((packed));

namespace game_master
{

namespace { struct IntSlotView { void* m_vptr; void* m_parser; int m_slot; }; }

// ============================================================================
// CViewTutorialFlagCmd — ORIG ctor 0x082a80d0（sizeof=0x8）/ execute 0x084a927e
// ============================================================================

// 拼 "tutorial flag : " + 各置位教程位 "(%d) "，组包(0,0xc) 回发本人。
void CViewTutorialFlagCmd::execute()
{
    std::string text("tutorial flag : ");
    char buf[255] = { 0 };
    for (int i = 0; i <= 0x7f; ++i)
    {
        if ((char)GetUser()->getCurCharacEscaladeTutorialFlag((unsigned int)i))
        {
            sprintf(buf, "(%d) ", i);
            text += buf;
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)text.size());
    packet.put_str(text.c_str(), (int)text.size());
    packet.finalize(true);
    GetUser()->Send(packet);
}

// ============================================================================
// CTeleportCmd — ORIG ctor 0x082a814e（sizeof=0x8）/ execute 0x084a986e
// ============================================================================

// 调炼金术师 OnTeleportCharacter(本人, 当前村庄, 0,0,0, true) 回城传送。
void CTeleportCmd::execute()
{
    expert_job::CAlchemist::OnTeleportCharacter(
        GetUser(),
        (int)(signed char)GetUser()->getCurCharacVill(),
        0, 0, 0, true);
}

// ============================================================================
// CVillageAttackedStartCmd — ORIG ctor 0x082a8266（sizeof=0xc，int@+0x08）/
// parse 0x084aa10c / execute 0x084aa144
// ============================================================================

// trim(args)；atoi → +0x08。
void CVillageAttackedStartCmd::parse()
{
    trim(m_parser->m_args);
    reinterpret_cast<IntSlotView*>(this)->m_slot = atoi(m_parser->m_args.c_str());
}

// 组 Packet_VillageAttackedGMCommand 经 monitor proxy 发往本人所属服务器组。
void CVillageAttackedStartCmd::execute()
{
    Packet_VillageAttackedGMCommand pkt;
    pkt.m_uid = GetUser()->GetUID();
    pkt.m_characNo = (unsigned int)GetUser()->get_charac_no(-1);
    pkt.m_vill = (unsigned int)reinterpret_cast<IntSlotView*>(this)->m_slot;
    pkt.m_field16 = 0;

    GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetUser()->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x1a);
}

// ============================================================================
// CVillageAttackedStopCmd — ORIG ctor 0x082a8282（sizeof=0x8）/
// execute 0x084aa298（纯空）
// ============================================================================

void CVillageAttackedStopCmd::execute()
{
}

// ============================================================================
// CVillageAttackedFightCmd — ORIG ctor 0x082a829e（sizeof=0x8）/
// execute 0x084aa1dc
// ============================================================================

// 本人无队伍则借 GameManager 大厅队伍并设单人；随后发起村战入场，
// 失败码经 SendCmdErrorPacket(0x10b, code) 回发。
void CVillageAttackedFightCmd::execute()
{
    CParty* party = static_cast<CParty*>(GetUser()->GetParty());
    if (party == 0)
    {
        party = G_CGameManager()->GetParty();
        if (party != 0)
            party->set_single_play(GetUser());
    }
    if (party != 0)
    {
        int ret = 0;
        ret = party->OnFightVillageMonster(GetUser(), 0);
        // 4.1 bool 物化+je 形：while(cond){body;break} 使 4.4 保留
        // cmpl/setne/test/je 四段（探针 /tmp/bl5/probe7.cpp m1 实证）。
        while (ret != 0)
        {
            GetUser()->SendCmdErrorPacket((ENUM_CMDPACKET)0x10b,
                                          (unsigned char)ret);
            break;
        }
    }
}

// ============================================================================
// CTestDisjointCmd — ORIG ctor 0x082a81f6（sizeof=0x8）/ execute 0x084a9c0c
// ============================================================================

// 对 6 个样本道具 × 9 个品级 × 1000 次跑分解结果并把六元组刷入日志
// （disjoint_result："rarity;grade;f6;f0;f7;f2\n"，压栈序依 objdump 实测）。
void CTestDisjointCmd::execute()
{
    CItem* samples[6];
    samples[0] = G_CDataManager()->find_item(0x795a);
    samples[1] = G_CDataManager()->find_item(0x795b);
    samples[2] = G_CDataManager()->find_item(0x797a);
    samples[3] = G_CDataManager()->find_item(0x795e);
    samples[4] = G_CDataManager()->find_item(0x7977);
    samples[5] = G_CDataManager()->find_item(620000);
    int runs = 1000;                       // -0x28

    DisJointResult result;                 // -0x68（ctor 清零 0x28）
    for (int s = 0; s <= 5; ++s)           // -0x24
    {
        for (int grade = 0; grade <= 8; ++grade)   // -0x20
        {
            for (int i = 0; i < runs; ++i)         // -0x1c
            {
                CDisjointer_get_disjoint_result_(
                    samples[s], (Inven_Item*)0, grade, result, false,
                    (CUser*)0);
                // 参数序（ORIG result@-0x68 下位移实测 +0x14/+0x00/+0x18/+0x04）：
                // f[5] / f[0] / f[6] / f[1]
                OS_API::WriteLog("disjoint_result",
                                 "%d;%d;%d;%d;%d;%d\n",
                                 samples[s]->get_rarity(), grade,
                                 result.m_f[5], result.m_f[0],
                                 result.m_f[6], result.m_f[1]);
            }
        }
    }
}

// ============================================================================
// CUserPremiumDelete — ORIG ctor 0x082a8522（sizeof=0x8）/ execute 0x084abf4e
// ============================================================================

// 收集全部生效 premium 项：逐项发 DB_Q(0x54) 流、解除并重挂效果、通知剩余时长。
// [b28d-19] N：N82 —— 帧/槽序（大件 GM 命令）；ORIG 0x084abf4e。
void CUserPremiumDelete::execute()
{
    CUser* user = GetUser();
    WongWork::CUserPremium* premium = user->GetPremiumInfo();

    std::vector<WongWork::SUserPremiumInfo> list;
    for (int type = 1; type <= 0x69; ++type)
    {
        WongWork::SUserPremiumInfo* info = premium->GetPremiumInfo(type);
        if (info->m_type != 0)
            list.push_back(*info);
    }

    long curSec = (long)GlobalData::s_systemTime_.getCurSec();
    long nextSec = curSec + 1;

    for (std::vector<WongWork::SUserPremiumInfo>::iterator it = list.begin();
         it != list.end(); ++it)
    {
        unsigned int serverCode = 0;
        if (g_SPremiumInfo[it->m_type].m_useTargetServer == 1)
            serverCode = (unsigned int)
                G_CEnvironment()->m_serverEnvir.m_serverGroup;   // +0x378

        Stream* stream =
            GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x989);
        CStreamGuard guard(stream, true);
        // ORIG operator<< 重载序：int / uint / uint / int / long / long /
        // int / uint（0x54 头 + uid + accId + type + curSec + nextSec +
        // flag + serverCode）。
        guard << 0x54;
        guard << user->GetUID();
        guard << user->get_acc_id();
        guard << it->m_type;
        guard << curSec;
        guard << nextSec;
        guard << it->m_flag;
        guard << serverCode;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)it->m_type, curSec, nextSec,
            it->m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(
            user, (ENUM_PREMIUM_TYPE)it->m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, it->m_type, (int)(nextSec - curSec));
    }
}

// ============================================================================
// CDropItem — ORIG ctor 0x082a85ae（sizeof=0x8）/ execute 0x084aca8c
// ============================================================================

// 取 0x32 号分发器，手组掉落测试包（finalize(false)+set_index(0xf)）后
// 经其虚表 [0] 入口 (this, user, PacketBuf*) 投递。
void CDropItem::execute()
{
    IDispatcher* dispatcher = CGameMasterMgr::GetDispatcher(0x32);
    if (dispatcher == 0)
        return;

    PacketGuard packet;
    packet.put_header(1, 0x32);
    packet.put_short(1);
    packet.put_short(1);
    packet.put_byte(0);
    packet.put_short(8);
    packet.put_int(1);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.finalize(false);
    packet.set_index(0xf);

    typedef int (*DispatchFn)(void*, CUser*, PacketBuf*);
    // ORIG 返回值经 bool 物化后丢弃（test/setne/test 无消费者）：
    // `while (call != 0) break;` 形复现（探针 /tmp/bl5/probe6.cpp q1 实证）。
    while ((*reinterpret_cast<DispatchFn*>(*(void**)dispatcher))(
               dispatcher, GetUser(), packet.get()) != 0)
        break;
}

// ============================================================================
// CTitleBookReset — ORIG ctor 0x082a86c6（sizeof=0x8）/ execute 0x084b063c
// ============================================================================

// 称号簿(扩展数据 0xe)：_gm_clear 后重发列表（ORIG 每次 GetUser 重取）。
void CTitleBookReset::execute()
{
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
}

// ============================================================================
// CTitleBookAdjust — ORIG ctor 0x082a86e2（sizeof=0x8）/ execute 0x084b068a
// ============================================================================

// 称号簿清空 + 成就(0xf)_gm_init/adjust + 双列表重发（顺序照 ORIG）。
void CTitleBookAdjust::execute()
{
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_init();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->adjust();
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->sendList();
}

// ============================================================================
// CTitleBookNoClear — ORIG ctor 0x082a86fe（sizeof=0x8）/ execute 0x084b0742
// ============================================================================

namespace { struct AchFlagView { char m_pad[0x25]; unsigned char m_flag; }; }

// 置成就扩展数据(+0x25 字节，未具名字段) 为 1（ORIG 单表达式直写）。
void CTitleBookNoClear::execute()
{
    reinterpret_cast<AchFlagView*>(GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->m_flag = 1;
}

}  // namespace game_master

// ============================================================================
// CUpgradeCardDebug — ORIG ctor 0x082a883e（sizeof=0xc，int@+0x08）/
// parse 0x084b1b5e / execute 0x084b1b96 / SendChatMsg 0x084b1e8a
namespace game_master
{
void CUpgradeCardDebug::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG SendChatMsg(CUser*, char*) @0x084b1e8a：strlen 后经
// GameWorld::make_packet_chat_msg(type=2, NOTI=0xc) 发给本人。
// 头已按 ORIG 两参签名声明（GameMasterCmd.h）；以真成员定义承载 ORIG 符号。
void CUpgradeCardDebug::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, (int)strlen(msg), 2, (ENUM_NOTIPACKET)0xc);
    user->Send(packet);
}

// 万次蒙特卡洛统计强化券成功率并向 GM 回播聊天消息。
// [b28d-19] N：N82 —— 槽序/帧（大件 GM 命令）；ORIG 0x084b1b96。
void CUpgradeCardDebug::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    int itemIdx = reinterpret_cast<const IntSlotView*>(this)->m_slot;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return;

    // ORIG：call is_stackable 后 xor $0x1/test 判 _Bool 假值即出函数。
    if (item->is_stackable() == false)
        return;

    // ORIG 入口对三块栈缓冲整块清零（0x400/0x1e/0x400 内联展开 +
    // 95000 大缓冲 memset call）；除 sprintf 目标外均为死存储。
    char msg[1024] = { 0 };
    unsigned char deadLarge[95000];
    memset(deadLarge, 0, sizeof(deadLarge));
    unsigned short dead400[512] = { 0 };
    unsigned char dead1e[30] = { 0 };

    int rate = 0;                                   // -0x34
    switch (item->get_index())
    {
    case 0x1d29:
    case 0x1c69:
    case 0x1c6a:
    case 0x1c6b:
    case 0x1e3f:
    case 0x1e40:
    case 0x1e86:
        rate = 100;
        break;
    default:
        break;
    }

    if (static_cast<CStackableItem*>(item)->IsReinforceTicket())
    {
        signed char value =
            (signed char)static_cast<CStackableItem*>(item)->GetReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }
    else if (static_cast<CStackableItem*>(item)->IsAmplifyReinforceTicket())
    {
        signed char value =
            (signed char)static_cast<CStackableItem*>(item)->GetAmplifyReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }

    int success = 0;                                // -0x30
    int failure = 0;                                // -0x2c
    for (int i = 0; i <= 9999; ++i)                 // -0x1c
    {
        if (get_rand_int(100) < rate)
            ++success;
        else
            ++failure;
    }

    sprintf(msg,
            "아이템:%s(%d) 성공확률:%d  (%d번 성공, %d번 실패)\n",
            item->GetItemName(), item->get_index(), rate, success, failure);
    SendChatMsg(GetUser(), msg);
}

// ============================================================================
// CTestTradeRestrict — ORIG ctor 0x082a88ca（sizeof=0x8）/
// parse 0x084b263a（空）/ execute 0x084b2640
// ============================================================================

void CTestTradeRestrict::parse()
{
}

// 对背包/仓库/账号货物全部在库且 flag==1 的道具槽打上 0x20 交易限制标志，
// 最后按当前角色号发起 DB_TestTradeRestrict 请求。
// 槽位记录步长 0x3d：+0x01 flag 字节、+0x02 itemIndex dword、+0x33 限制字节。
// [b28d-19] N：N82 —— 槽序/帧（bool 物化 + 局部槽平移）；ORIG 0x084b2640。
void CTestTradeRestrict::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    // ---- 循环 1：m_pEquipSlot 数组（0x39 槽）----
    for (int i = 0; i < 0x39; ++i)
    {
        // ORIG 每用点 fresh 取 getter/基址（P48），右值表达式寻址（P0 无具名）
        if (user->getCurCharacInvenR()->m_pEquipSlot[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_pEquipSlot[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_pEquipSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 2：inventory 对象内嵌数组（0x1a 槽，m_invenItem）----
    for (int i = 0; i < 0x1a; ++i)
    {
        if (user->getCurCharacInvenR()->m_invenItem[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_invenItem[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_invenItem[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 3：m_pAvatarSlot 数组（0x69 槽，仅判 itemIndex）----
    for (int i = 0; i < 0x69; ++i)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo != 0)
            user->getCurCharacInvenW()->m_pAvatarSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 4：角色仓库 CCargo（0x98 槽）----
    Inven_Item* cargoStart =
        const_cast<Inven_Item*>(user->getCurCharacCargoW()->get_cargo_start());
    for (int i = 0; i < 0x98; ++i)
    {
        if (cargoStart[i].m_addInfo != 0 &&
            cargoStart[i].m_field1 == 1)            // ITEM_TYPE_EQUIP
        {
            cargoStart[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
    }

    // ---- 账号货物（GetSlotRef(0)，0x38 槽；引用为空则跳过）----
    Inven_Item* accSlot = user->GetAccountCargo()->GetSlotRef(0);
    int i = 0;
    while (i < 0x38 && accSlot != 0)
    {
        if (accSlot[i].m_addInfo != 0 &&
            accSlot[i].m_field1 == 1)               // ITEM_TYPE_EQUIP
        {
            accSlot[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
        ++i;
    }

    DB_TestTradeRestrict::makeRequest((unsigned int)user->getCurCharacNo());
}

}  // namespace game_master

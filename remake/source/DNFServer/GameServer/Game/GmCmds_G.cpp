// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_G/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

// [b28d-Main 代落·S258] 原 asm 自由函数桥已删：CExpertJob.h:310 已把
// expert_job::CDisjointer::get_disjoint_result_ 改为 static（ORIG 0x085d234a
// 体首参 0x8(%ebp)=item、无 this）⇒ 直调即可，6 实参槽与 ORIG 一致。

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

#include "GmCmds_G/execute.inc"


// ============================================================================
// CTeleportCmd — ORIG ctor 0x082a814e（sizeof=0x8）/ execute 0x084a986e
// ============================================================================

#include "GmCmds_G/execute_1.inc"


// ============================================================================
// CVillageAttackedStartCmd — ORIG ctor 0x082a8266（sizeof=0xc，int@+0x08）/
// parse 0x084aa10c / execute 0x084aa144
// ============================================================================

#include "GmCmds_G/parse.inc"


#include "GmCmds_G/execute_2.inc"


// ============================================================================
// CVillageAttackedStopCmd — ORIG ctor 0x082a8282（sizeof=0x8）/
// execute 0x084aa298（纯空）
// ============================================================================

#include "GmCmds_G/execute_3.inc"


// ============================================================================
// CVillageAttackedFightCmd — ORIG ctor 0x082a829e（sizeof=0x8）/
// execute 0x084aa1dc
// ============================================================================

#include "GmCmds_G/execute_4.inc"


// ============================================================================
// CTestDisjointCmd — ORIG ctor 0x082a81f6（sizeof=0x8）/ execute 0x084a9c0c
// ============================================================================

#include "GmCmds_G/execute_5.inc"


// ============================================================================
// CUserPremiumDelete — ORIG ctor 0x082a8522（sizeof=0x8）/ execute 0x084abf4e
// ============================================================================

#include "GmCmds_G/execute_6.inc"


// ============================================================================
// CDropItem — ORIG ctor 0x082a85ae（sizeof=0x8）/ execute 0x084aca8c
// ============================================================================

#include "GmCmds_G/execute_7.inc"
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

#include "GmCmds_G/execute_8.inc"


// ============================================================================
// CTitleBookAdjust — ORIG ctor 0x082a86e2（sizeof=0x8）/ execute 0x084b068a
// ============================================================================

#include "GmCmds_G/execute_9.inc"


// ============================================================================
// CTitleBookNoClear — ORIG ctor 0x082a86fe（sizeof=0x8）/ execute 0x084b0742
// ============================================================================

namespace { struct AchFlagView { char m_pad[0x25]; unsigned char m_flag; }; }

#include "GmCmds_G/execute_10.inc"
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
#include "GmCmds_G/parse_1.inc"


#include "GmCmds_G/SendChatMsg.inc"


#include "GmCmds_G/execute_11.inc"


// ============================================================================
// CTestTradeRestrict — ORIG ctor 0x082a88ca（sizeof=0x8）/
// parse 0x084b263a（空）/ execute 0x084b2640
// ============================================================================

#include "GmCmds_G/parse_2.inc"


#include "GmCmds_G/execute_12.inc"


}  // namespace game_master

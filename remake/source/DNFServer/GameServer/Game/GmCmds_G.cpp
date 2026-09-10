// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCmds_G/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// 拼 "tutorial flag : " + 各置位教程位 "(%d) "，组包(0,0xc) 回发本人。
#include "GmCmds_G/execute.inc"
}

// ============================================================================
// CTeleportCmd — ORIG ctor 0x082a814e（sizeof=0x8）/ execute 0x084a986e
// ============================================================================

// 调炼金术师 OnTeleportCharacter(本人, 当前村庄, 0,0,0, true) 回城传送。
#include "GmCmds_G/execute_1.inc"
}

// ============================================================================
// CVillageAttackedStartCmd — ORIG ctor 0x082a8266（sizeof=0xc，int@+0x08）/
// parse 0x084aa10c / execute 0x084aa144
// ============================================================================

// trim(args)；atoi → +0x08。
#include "GmCmds_G/parse.inc"
}

// 组 Packet_VillageAttackedGMCommand 经 monitor proxy 发往本人所属服务器组。
#include "GmCmds_G/execute_2.inc"
}

// ============================================================================
// CVillageAttackedStopCmd — ORIG ctor 0x082a8282（sizeof=0x8）/
// execute 0x084aa298（纯空）
// ============================================================================

#include "GmCmds_G/execute_3.inc"
}

// ============================================================================
// CVillageAttackedFightCmd — ORIG ctor 0x082a829e（sizeof=0x8）/
// execute 0x084aa1dc
// ============================================================================

// 本人无队伍则借 GameManager 大厅队伍并设单人；随后发起村战入场，
// 失败码经 SendCmdErrorPacket(0x10b, code) 回发。
#include "GmCmds_G/execute_4.inc"
}

// ============================================================================
// CTestDisjointCmd — ORIG ctor 0x082a81f6（sizeof=0x8）/ execute 0x084a9c0c
// ============================================================================

// 对 6 个样本道具 × 9 个品级 × 1000 次跑分解结果并把六元组刷入日志
// （disjoint_result："rarity;grade;f6;f0;f7;f2\n"，压栈序依 objdump 实测）。
#include "GmCmds_G/execute_5.inc"
}

// ============================================================================
// CUserPremiumDelete — ORIG ctor 0x082a8522（sizeof=0x8）/ execute 0x084abf4e
// ============================================================================

// 收集全部生效 premium 项：逐项发 DB_Q(0x54) 流、解除并重挂效果、通知剩余时长。
// [b28d-19 / b28c-R26 修复] verdict=DIFF(routed) [N82 残余] ORIG 0x084abf4e
//   orig/ours=242/243。本轮修掉三处 p167 真缺口（修复前 OUR calls=39 / ORIG=48）：
//   ① 8 处 `guard << x` → `guard.operator*()->operator<<(x)`：ORIG 每处先
//      `call CStreamGuard::operator*`（0x080c8c46）再 `call CStreamGuard::operator<<`
//      （0x080c8c56 / 0x080e2276 / 0x080e2292）⇒ P103/P132 链不可省（+8 call）。
//   ② 循环内 GetPremiumInfo 具名局部 → 两次独立调用：ORIG 0x084abf93（判条件）
//      与 0x084abfb0（push_back）各一次（P48：O0 无 CSE）。
//   ③ serverCode `unsigned int` → `int`：ORIG 用 `CStreamGuard::operator<<(int)`
//      （lsEi 0x080c8c56），unsigned 实参选 lsEj 0x080e2292 = 重载选错。
//   修复后 p167 CLEAR（48=48；残余 C1/C2、D1/D2 为 P93 同址别名伪差）。
//   残余 DIFF = 槽/帧序跨版本差（N82）：ORIG guard -0x3c/vector -0x30/it -0x24 一族。
#include "GmCmds_G/execute_6.inc"
}

// ============================================================================
// CDropItem — ORIG ctor 0x082a85ae（sizeof=0x8）/ execute 0x084aca8c
// ============================================================================

// 取 0x32 号分发器，手组掉落测试包（finalize(false)+set_index(0xf)）后
// 经其虚表 [0] 入口 (this, user, PacketBuf*) 投递。
#include "GmCmds_G/execute_7.inc"
}

// ============================================================================
// CTitleBookReset — ORIG ctor 0x082a86c6（sizeof=0x8）/ execute 0x084b063c
// ============================================================================

// 称号簿(扩展数据 0xe)：_gm_clear 后重发列表（ORIG 每次 GetUser 重取）。
#include "GmCmds_G/execute_8.inc"
}

// ============================================================================
// CTitleBookAdjust — ORIG ctor 0x082a86e2（sizeof=0x8）/ execute 0x084b068a
// ============================================================================

// 称号簿清空 + 成就(0xf)_gm_init/adjust + 双列表重发（顺序照 ORIG）。
#include "GmCmds_G/execute_9.inc"
}

// ============================================================================
// CTitleBookNoClear — ORIG ctor 0x082a86fe（sizeof=0x8）/ execute 0x084b0742
// ============================================================================

namespace { struct AchFlagView { char m_pad[0x25]; unsigned char m_flag; }; }

// 置成就扩展数据(+0x25 字节，未具名字段) 为 1（ORIG 单表达式直写）。
#include "GmCmds_G/execute_10.inc"
}

}  // namespace game_master

// ============================================================================
// CUpgradeCardDebug — ORIG ctor 0x082a883e（sizeof=0xc，int@+0x08）/
// parse 0x084b1b5e / execute 0x084b1b96 / SendChatMsg 0x084b1e8a
namespace game_master
{
#include "GmCmds_G/parse_1.inc"
}

// ORIG SendChatMsg(CUser*, char*) @0x084b1e8a：strlen 后经
// GameWorld::make_packet_chat_msg(type=2, NOTI=0xc) 发给本人。
// 头已按 ORIG 两参签名声明（GameMasterCmd.h）；以真成员定义承载 ORIG 符号。
#include "GmCmds_G/SendChatMsg.inc"
}

// 万次蒙特卡洛统计强化券成功率并向 GM 回播聊天消息。
// [b28d-19 / b28c-R26 复验] verdict=DIFF(routed) [N126 + 待解 memset-edx] ORIG 0x084b1b96
//   orig/ours=233/229（gap +4）。本轮改动：
//   ① 删具名 itemIdx 局部（P0/P183）：ORIG 0x084b1bba `mov 0x8(%eax),%ebx` 把
//      m_slot 装 callee-saved 直传 find_item；具名局部会多占 -0x34 槽。
//   ② 死缓冲声明序改为 dead1e → dead400（ORIG 物理序 0x084b1bf4(0x400) →
//      0x084b1c3e(memset) → 0x084b1c5d(0x1e) → 0x084b1caf(0x400)）。
//   ③ 试 `memset((char*)p,0,N)`（P68）复现 ORIG 0x084b1c3e `lea→%eax;
//      mov $0x17318,%edx; mov %edx,0x8(%esp)` 的 edx 中转形 —— 实测无效
//      （4.4 仍 `movl $imm,0x8(%esp)`）⇒ 并入「待解：memset 首参 edx 中转」
//      （与 CAchievementReport::execute 同现象）封顶。
//   残余 = N126 版本首槽基址常数：我方全帧浅 8（user -0x34 vs ORIG -0x3c；
//   deadLarge `lea -0x17b6a` vs ORIG -0x17b72；msg -0x452 vs -0x45a）⇒ 整帧平移不可消。
#include "GmCmds_G/execute_11.inc"
}

// ============================================================================
// CTestTradeRestrict — ORIG ctor 0x082a88ca（sizeof=0x8）/
// parse 0x084b263a（空）/ execute 0x084b2640
// ============================================================================

#include "GmCmds_G/parse_2.inc"
}

// 对背包/仓库/账号货物全部在库且 flag==1 的道具槽打上 0x20 交易限制标志，
// 最后按当前角色号发起 DB_TestTradeRestrict 请求。
// 槽位记录步长 0x3d：+0x01 flag 字节、+0x02 itemIndex dword、+0x33 限制字节。
// [b28d-19 / b28c-R26 复验] verdict=DIFF(routed) [N82/P223 边界] ORIG 0x084b2640
//   orig/ours=214/215（gap −1）。p167 CLEAR（15=15）。残差三处（均 4.1 独有形）：
//   ① 循环 3 ORIG 0x084b27a9 `test %eax,%eax; sete %al; test %al,%al; jne <skip>`
//      （"==0" 的 4.1 bool 物化）vs 我方 `je <skip>` 直跳（P225 只给出 "!=0" 的
//      setne 形；负向 sete 形探针不可复现）。
//   ② 循环 4 ORIG 每处 if-join 出 `jmp <tail>; nop` 双岛（P223 正例），
//      4.4 对 `if(a&&b)` 恒 jump-thread 消岛。
//   ③ 循环 5 ORIG 判空 `cmpl $0,-0x24(%ebp); je` 位于条件块；我方把
//      `i<0x38 && p!=0` 物化为 `jg` + `mov $1/$0` 岛（P223 岛数恒=条件数或 0）。
#include "GmCmds_G/execute_12.inc"
}

}  // namespace game_master

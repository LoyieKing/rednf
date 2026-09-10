// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCommunityServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CCommunityServerProxy（独占 OrigCommPx 批）。
// 依据 docs/class_func_reports/CCommunityServerProxy/*.md 与 objdump 转写
// （OnTcpRecv/SendTcpPacket 报告缺失，按 §3.2 nm 定址后 objdump 区间转写）。
#include <errno.h>
#include <string.h>   // memcpy / strerror

#include "GameTypes.h"   // cMyTrace
#include "CCommunityServerProxy.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "CUser.h"          // CUser::get_acc_id/GetUID/GetServerGroup/getSex
#include "Packet_Community_Login.h"
#include "Packet_Community_Logout.h"
#include "Packet_Request_Add_PvP_Buddy.h"
#include "Packet_Response_Add_PvP_Buddy.h"
#include "Packet_Request_Remove_PvP_Buddy.h"

// ORIG T 0x0846c5c8：基类构造 + string(ip)；m_tcpPort/m_udpPort 在函数体赋值
//（报告汇编：string 构造后先写 +0x30e00 再清 +0x30df8）。
#include "CCommunityServerProxy/CCommunityServerProxy.inc"
}

// ORIG T 0x0846c660：纯转发 InitTcp。
#include "CCommunityServerProxy/Init.inc"
}

// ORIG T 0x0846c674：ip/port 有效且 socket 未连时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0（P15 赋值即条件 + P37 临时链式 trace）。
#include "CCommunityServerProxy/InitTcp.inc"
}

// ORIG T 0x0846c770：连接有效（ip 非空且端口非 0）时 disconnect + 清 udpPort。
#include "CCommunityServerProxy/Disconnect.inc"
}

// ORIG T 0x0846c7c4：连接态(+0x30df8)==2 时经 CNetwork::send_packet 下发；
// 否则 cMyTrace 报 disconnected（字符串 .rodata 直读转写）。
#include "CCommunityServerProxy/SendTcpPacket.inc"
}

// ORIG T 0x0846c82c（报告缺失，objdump 转写）：
// recv_packet()>0 → Parsing 失败则 Disconnect 返 0；<0 → logFormat 后 Disconnect 返 0。
#include "CCommunityServerProxy/OnTcpRecv.inc"
}

// ORIG T 0x0846c8c4：send_packet()<0 → 报错+Disconnect 返 -1；否则返 m_recvOffset。
#include "CCommunityServerProxy/OnTcpSend.inc"
}

// ORIG T 0x0846c962：errno!=0 → 报错返 0；否则返 1。
#include "CCommunityServerProxy/OnTcpError.inc"
}

// ORIG T 0x0846c9ec：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
#include "CCommunityServerProxy/Parsing.inc"
}

// ORIG T 0x0846ca06：Packet_Community_Login 填用户信息 + PvP 好友载荷后下发。
// 字段/调用序逐条对齐报告汇编（m_id←acc_id, server_id←GetServerGroup,
// channel_no←G_CEnvironment()->get_channel_no, m_uid←GetUID,
// charac_no←getCurCharacNo, growType/level/job/sex, name memcpy 0x1d,
// buddyCount←sig[0], buddies memcpy 0x560）。
#include "CCommunityServerProxy/SendLogin.inc"
}

// ORIG T 0x0846cb44：Packet_Community_Logout(m_id=accId, m_characNo) 下发。
#include "CCommunityServerProxy/SendLogout.inc"
}

// ---- StubS3A_deps 批：Send*PvPBuddy（ORIG 逐指令转写）----
// 通用形态：栈上构造对应包（ctor 预填 header+len+清零），填
// +0x0a accId / +0x0e characNo / +0x12 flag，memcpy(name, nameLen) 到 +0x13，
// SendTcpPacket(pkt, *(ushort*)(pkt+2)=packetSize)。

// ORIG T 0x0846cbbc _ZN21CCommunityServerProxy18SendReqAddPvPBuddyEjjcPci
#include "CCommunityServerProxy/SendReqAddPvPBuddy.inc"
}

// ORIG T 0x0846cc22 _ZN21CCommunityServerProxy18SendResAddPvPBuddyEjjcPci
#include "CCommunityServerProxy/SendResAddPvPBuddy.inc"
}

// ORIG T 0x0846cc88 _ZN21CCommunityServerProxy18SendRemovePvPBuddyEjjcPci
#include "CCommunityServerProxy/SendRemovePvPBuddy.inc"
}
